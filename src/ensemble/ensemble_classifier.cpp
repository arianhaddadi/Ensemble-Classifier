#include "ensemble_classifier.h"
#include "utils.h"
#include <algorithm>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LENGTH 100
#define PIPE_FLAG 0666
#define LABELS_FILENAME "labels.csv"
#define LINEAR_CLASSIFIER_FIFO_FILE "npf"
#define VOTER_FIFO_FILE "vpf"
#define VOTER_EXECUTABLE_NAME "./voter"
#define LINEAR_CLASSIFIER_EXECUTABLE_NAME "./linear"
#define DATASET_FILENAME "dataset.csv"
#define CLASSIFIER_FILENAME_PREFIX "classifier_"
#define CSV_FILE_FORMAT ".csv"

void EnsembleClassifier::run(char **argv) {
  std::string validationDirectory = argv[1];
  std::string weightsDirectory = argv[2];

  std::string datasetFileAddress = validationDirectory + "/" + DATASET_FILENAME;
  std::string labelsFileAddress = validationDirectory + "/" + LABELS_FILENAME;

  std::vector<int> labelsVector;
  getLabels(labelsVector, labelsFileAddress);

  int numOfClassifiers = getNumOfClassifiers(weightsDirectory);

  mkfifo(LINEAR_CLASSIFIER_FIFO_FILE, PIPE_FLAG);
  mkfifo(VOTER_FIFO_FILE, PIPE_FLAG);

  int p[numOfClassifiers][2];

  for (int i = 0; i < numOfClassifiers; i++) {
    pipe(p[i]);
    int pid = fork();
    if (pid == 0) {
      close(p[i][1]); // close the write end of the pipe
      std::string pipeReadFdStr = std::to_string(p[i][0]);
      char *args[] = {(char *)LINEAR_CLASSIFIER_EXECUTABLE_NAME,
                      (char *)pipeReadFdStr.c_str(), nullptr};
      execvp(args[0], args);
    } else {
      close(p[i][0]); // close the read end of the pipe
      std::string classifierNumStr = std::to_string(i);
      std::stringstream weightsFileAddress;
      weightsFileAddress << weightsDirectory << "/"
                         << CLASSIFIER_FILENAME_PREFIX << classifierNumStr
                         << CSV_FILE_FORMAT;

      write(p[i][1], datasetFileAddress.c_str(), MAX_LENGTH);
      write(p[i][1], LINEAR_CLASSIFIER_FIFO_FILE, MAX_LENGTH);
      write(p[i][1], weightsFileAddress.str().c_str(), MAX_LENGTH);
      write(p[i][1], classifierNumStr.c_str(), MAX_LENGTH);
    }
  }

  if (fork() == 0) {
    // child process
    char *args[] = {(char *)VOTER_EXECUTABLE_NAME, (char *)VOTER_FIFO_FILE,
                    nullptr};
    execvp(args[0], args);
  } else {
    // parent process
    communicateWithVoter(labelsVector, VOTER_FIFO_FILE,
                         LINEAR_CLASSIFIER_FIFO_FILE, numOfClassifiers);
  }

  for (int i = 0; i < numOfClassifiers; i++) {
    close(p[i][1]);
    wait(nullptr);
  }

  wait(nullptr);
  unlink(LINEAR_CLASSIFIER_FIFO_FILE);
  unlink(VOTER_FIFO_FILE);
}

void EnsembleClassifier::getLabels(std::vector<int> &labelsVector,
                                   const std::string &filename) {
  std::ifstream labels;
  labels.open(filename);
  std::string line;
  getline(labels, line);
  while (getline(labels, line)) {
    labelsVector.push_back(stoi(line));
  }
  labels.close();
}

int EnsembleClassifier::getNumOfClassifiers(
    const std::string &classifiersDirectory) {
  int counter = 0;
  while (true) {
    std::stringstream fileAddress;
    fileAddress << classifiersDirectory << "/" << CLASSIFIER_FILENAME_PREFIX
                << std::to_string(counter) << CSV_FILE_FORMAT;

    int fd = open(fileAddress.str().c_str(), O_RDONLY);
    if (fd < 0)
      break;
    counter++;
    close(fd);
  }
  return counter;
}

void EnsembleClassifier::communicateWithVoter(
    const std::vector<int> &labelsVector, const std::string &voterNamedPipeName,
    const std::string &linearClassifierNamedPipeName, int numOfClassifiers) {

  // send data to voter
  int fd = open(voterNamedPipeName.c_str(), O_WRONLY);
  write(fd, linearClassifierNamedPipeName.c_str(), MAX_LENGTH);
  write(fd, std::to_string(labelsVector.size()).c_str(), MAX_LENGTH);
  write(fd, std::to_string(numOfClassifiers).c_str(), MAX_LENGTH);
  close(fd);

  // receive data from voter
  fd = open(voterNamedPipeName.c_str(), O_RDONLY);
  std::vector<int> estimatedLabels;
  char readBuffer[MAX_LENGTH];
  for (int i = 0; i < labelsVector.size(); i++) {
    read(fd, readBuffer, MAX_LENGTH);
    estimatedLabels.push_back(atoi(readBuffer));
  }
  close(fd);

  calAccuracy(estimatedLabels, labelsVector);
}
void EnsembleClassifier::calAccuracy(const std::vector<int> &estimate,
                                     const std::vector<int> &real) {
  int correctEstimate = 0;
  for (int i = 0; i < estimate.size(); i++) {
    if (estimate[i] == real[i]) {
      correctEstimate++;
    }
  }
  std::cout << "Accuracy: "
            << ((float)correctEstimate / (float)estimate.size()) * 100 << "%"
            << std::endl;
}
