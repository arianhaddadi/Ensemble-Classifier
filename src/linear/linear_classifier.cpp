#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "linear_classifier.h"
#include "utils.h"

void LinearClassifier::run(const std::string &datasetAddress,
                           const std::string &namedPipeFilename,
                           const std::string &weightsFileAddress,
                           const int index) {

  const std::vector<std::vector<std::string>> weights =
      getWeights(weightsFileAddress);

  classifyDataset(weights, datasetAddress, namedPipeFilename, index);
}

std::vector<std::vector<std::string>>
LinearClassifier::getWeights(const std::string &filename) {
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
    const std::vector<std::vector<std::string>> &weights,
    const std::string &datasetFilename, const std::string &namedPipeFilename,
    const int index) {
  std::ifstream dataset;
  dataset.open(datasetFilename);

  const int fd = open(namedPipeFilename.c_str(), O_WRONLY);

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
