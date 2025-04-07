#include "ensemble_classifier.h"
#include "utils.h"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void EnsembleClassifier::run(char **argv) {
  const std::string validationDirectory = argv[1];
  const std::string weightsDirectory = argv[2];

  std::string datasetFileAddress = validationDirectory + "/" + DATASET_FILENAME;
  std::string labelsFileAddress = validationDirectory + "/" + LABELS_FILENAME;

  const std::vector<int> labels = getLabels(labelsFileAddress);

  const int numOfClassifiers = getNumOfClassifiers(weightsDirectory);

  // Creating the shared pipes (FIFOs)
  mkfifo(LINEAR_CLASSIFIER_FIFO_FILE, 0666);
  mkfifo(VOTER_FIFO_FILE, 0666);

  int p[numOfClassifiers][2];

  for (int i = 0; i < numOfClassifiers; i++) {
    pipe(p[i]);
    if (fork() == 0) {
      // Child Process
      close(p[i][1]); // close the write end of the pipe
      const std::string pipeReadFdStr = std::to_string(p[i][0]);
      char *args[] = {(char *)LINEAR_CLASSIFIER_EXECUTABLE_NAME,
                      (char *)pipeReadFdStr.c_str(), nullptr};
      execvp(args[0], args);
      perror("execvp failed");
      exit(-1);
    }

    // Only parent process comes here.
    close(p[i][0]); // close the read end of the pipe
    std::string classifierNumStr = std::to_string(i);
    std::stringstream weightsFileAddress;
    weightsFileAddress << weightsDirectory << "/" << CLASSIFIER_FILENAME_PREFIX
                       << classifierNumStr << CSV_FILE_EXTENSION;

    write(p[i][1], datasetFileAddress.c_str(), MAX_LENGTH);
    write(p[i][1], LINEAR_CLASSIFIER_FIFO_FILE, MAX_LENGTH);
    write(p[i][1], weightsFileAddress.str().c_str(), MAX_LENGTH);
    write(p[i][1], classifierNumStr.c_str(), MAX_LENGTH);
  }

  if (fork() == 0) {
    // Child Process
    char *args[] = {(char *)VOTER_EXECUTABLE_NAME, (char *)VOTER_FIFO_FILE,
                    nullptr};
    execvp(args[0], args);
    perror("execvp failed");
    exit(-1);
  }

  // Only parent process comes here.
  communicateWithVoter(labels, VOTER_FIFO_FILE, LINEAR_CLASSIFIER_FIFO_FILE,
                       numOfClassifiers);

  for (int i = 0; i < numOfClassifiers; i++) {
    close(p[i][1]);
    wait(nullptr);
  }

  wait(nullptr);

  // Removing the shared pipes (FIFOs)
  unlink(LINEAR_CLASSIFIER_FIFO_FILE);
  unlink(VOTER_FIFO_FILE);
}

std::vector<int> EnsembleClassifier::getLabels(const std::string &filename) {
  std::vector<int> labels;
  std::ifstream labelsFile;
  labelsFile.open(filename);
  std::string line;
  getline(labelsFile, line);
  while (getline(labelsFile, line)) {
    labels.push_back(stoi(line));
  }
  labelsFile.close();
  return labels;
}

int EnsembleClassifier::getNumOfClassifiers(
    const std::string &classifiersDirectory) {
  int counter = 0;
  while (true) {
    std::stringstream fileAddress;
    fileAddress << classifiersDirectory << "/" << CLASSIFIER_FILENAME_PREFIX
                << std::to_string(counter) << CSV_FILE_EXTENSION;

    const int fd = open(fileAddress.str().c_str(), O_RDONLY);
    if (fd < 0) {
      // File doesn't exist. Counting is over.
      break;
    }
    counter++;
    close(fd);
  }
  return counter;
}

void EnsembleClassifier::communicateWithVoter(
    const std::vector<int> &labels, const std::string &voterNamedPipeName,
    const std::string &linearClassifierNamedPipeName,
    const int numOfClassifiers) {

  // send data to voter
  int fd = open(voterNamedPipeName.c_str(), O_WRONLY);
  write(fd, linearClassifierNamedPipeName.c_str(), MAX_LENGTH);
  write(fd, std::to_string(labels.size()).c_str(), MAX_LENGTH);
  write(fd, std::to_string(numOfClassifiers).c_str(), MAX_LENGTH);
  close(fd);

  // receive data from voter
  fd = open(voterNamedPipeName.c_str(), O_RDONLY);
  std::vector<int> estimatedLabels;
  for (int i = 0; i < labels.size(); i++) {
    char readBuffer[MAX_LENGTH];
    read(fd, readBuffer, MAX_LENGTH);
    estimatedLabels.push_back(atoi(readBuffer));
  }
  close(fd);

  calAccuracy(estimatedLabels, labels);
}

void EnsembleClassifier::calAccuracy(const std::vector<int> &estimate,
                                     const std::vector<int> &real) {
  int correctEstimate = 0;
  for (int i = 0; i < estimate.size(); i++) {
    if (estimate[i] == real[i]) {
      correctEstimate++;
    }
  }
  std::cout << "Accuracy: " << ((float)correctEstimate / estimate.size()) * 100
            << "%" << std::endl;
}
