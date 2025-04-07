#include "linear_classifier.h"
#include "utils.h"
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <unistd.h>

void LinearClassifier::run(char **argv) {
  char datasetAddress[MAX_LENGTH] = {0};
  char namedPipeFilename[MAX_LENGTH] = {0};
  char weightsFileAddress[MAX_LENGTH] = {0};
  char classifierNum[MAX_LENGTH] = {0};

  /* Get file descriptor of the read end of the pipe shared by this process
   * and its parent process */
  const int pipefd = std::stoi(argv[1]);
  read(pipefd, datasetAddress, MAX_LENGTH);
  read(pipefd, namedPipeFilename, MAX_LENGTH);
  read(pipefd, weightsFileAddress, MAX_LENGTH);
  read(pipefd, classifierNum, MAX_LENGTH);
  close(pipefd);

  const std::vector<std::vector<std::string>> weights =
      getWeights(weightsFileAddress);

  const int index = std::stoi(std::string(classifierNum));
  classifyDataset(weights, datasetAddress, namedPipeFilename, index);
}

std::vector<std::vector<std::string>>
LinearClassifier::getWeights(char *filename) {
  std::vector<std::vector<std::string>> weights;

  std::ifstream weightsFile;
  weightsFile.open(filename);

  std::vector<std::string> csvLine;
  std::string line;
  getline(weightsFile, line);
  while (getline(weightsFile, line)) {
    csvLine.clear();
    csvLine = parse_line(line, CSV_DELIMITER);
    weights.push_back(csvLine);
  }

  weightsFile.close();
  return weights;
}

void LinearClassifier::classifyDataset(
    const std::vector<std::vector<std::string>> &weights, char *datasetFilename,
    char *namedPipeFilename, const int index) {
  std::ifstream dataset;
  dataset.open(datasetFilename);

  int fd = open(namedPipeFilename, O_WRONLY);

  std::string line;
  std::vector<std::string> csvLine;
  getline(dataset, line);
  int indexOfMax, lineNum = 0;
  float max, product;

  while (getline(dataset, line)) {
    csvLine.clear();
    csvLine = parse_line(line, CSV_DELIMITER);
    max = 0;
    for (int i = 0; i < weights.size(); i++) {
      product = dotProduct(weights[i], csvLine);
      if (product > max) {
        max = product;
        indexOfMax = i;
      }
    }

    std::stringstream message;
    message << index << COORD_DELIMITER_CHAR << lineNum << COORD_DELIMITER_CHAR
            << indexOfMax;

    write(fd, message.str().c_str(), MAX_LENGTH);
    lineNum++;
  }

  close(fd);
  dataset.close();
}

float LinearClassifier::dotProduct(const std::vector<std::string> &v1,
                                   const std::vector<std::string> &v2) {
  float product = 0;
  for (int i = 0; i < v2.size(); i++) {
    product += (stof(v1[i]) * stof(v2[i]));
  }
  return product + stof(v1[v1.size() - 1]); // Add bias term.
}
