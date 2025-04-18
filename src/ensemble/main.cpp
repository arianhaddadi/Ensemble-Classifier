#include "ensemble_classifier.h"
#include "iostream"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Wrong Number of arguments. Required num: 2, Given: " << argc;
    return -1;
  }
  EnsembleClassifier::run(argv);
  return 0;
}