#ifndef ENSEMBLE_CLASSIFIER_H
#define ENSEMBLE_CLASSIFIER_H

#include <string>
#include <vector>

class EnsembleClassifier {
public:
  static void getLabels(std::vector<int> &labelsVector,
                        const std::string &filename);

  static int getNumOfClassifiers(const std::string &classifiersDirectory);

  static void
  communicateWithVoter(const std::vector<int> &labelsVector,
                       const std::string &voterNamedPipeName,
                       const std::string &linearClassifierNamedPipeName,
                       int numOfClassifiers);

  static void calAccuracy(const std::vector<int> &estimate,
                          const std::vector<int> &real);

  static void run(char **argv);
};

#endif // ENSEMBLE_CLASSIFIER_H
