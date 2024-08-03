#ifndef LINEAR_CLASSIFIER_H
#define LINEAR_CLASSIFIER_H

#include <string>
#include <vector>

class LinearClassifier {
public:
  static void getWeights(std::vector<std::vector<std::string>> &weightVectors,
                         char *weightVectorFilename);
  static void
  classifyDataset(const std::vector<std::vector<std::string>> &weightVectors,
                  char *datasetFilename, char *namedPipeFilename, char *index);

  static float dotProduct(const std::vector<std::string> &v1,
                          const std::vector<std::string> &v2);

  static void run(char **argv);
};

#endif // LINEAR_CLASSIFIER_H
