#include "linear_classifier.h"
#include "utils.h"
#include <fcntl.h>
#include <fstream>
#include <unistd.h>

#define MAX_LENGTH 100
#define CSV_DELIMITER ','
#define COORD_DELIMITER_CHAR '_'

void LinearClassifier::run(char **argv) {
  char datasetAddress[MAX_LENGTH];
  char namedPipeFilename[MAX_LENGTH];
  char weightsFileAddress[MAX_LENGTH];
  char index[MAX_LENGTH];

  /* Get file descriptor of the read end of the pipe shared by this process
   * and its parent process */
  int pipefd = atoi(argv[1]);
  read(pipefd, datasetAddress, MAX_LENGTH);
  read(pipefd, namedPipeFilename, MAX_LENGTH);
  read(pipefd, weightsFileAddress, MAX_LENGTH);
  read(pipefd, index, MAX_LENGTH);
  close(pipefd);

  std::vector<std::vector<std::string>> weightVectors;
  getWeights(weightVectors, weightsFileAddress);

  classifyDataset(weightVectors, datasetAddress, namedPipeFilename, index);
}

void LinearClassifier::getWeights(
    std::vector<std::vector<std::string>> &weightVectors,
    char *weightVectorFilename) {
  std::ifstream weights;
  std::vector<std::string> csvLine;
  weights.open(weightVectorFilename);
  std::string line;

  getline(weights, line);
  while (getline(weights, line)) {
    csvLine.clear();
    csvLine = parse_csv(line, CSV_DELIMITER);
    weightVectors.push_back(csvLine);
  }

  weights.close();
}

void LinearClassifier::classifyDataset(
    const std::vector<std::vector<std::string>> &weightVectors,
    char *datasetFilename, char *namedPipeFilename, char *index) {
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
    csvLine = parse_csv(line, CSV_DELIMITER);
    max = 0;
    for (int i = 0; i < weightVectors.size(); i++) {
      product = dotProduct(weightVectors[i], csvLine);
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
  return product + stof(v1[v1.size() - 1]);
}
