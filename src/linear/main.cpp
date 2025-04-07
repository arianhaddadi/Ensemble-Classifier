#include "iostream"
#include "linear_classifier.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Wrong Number of arguments. Required num: 2, Given: " << argc;
    return -1;
  }
  LinearClassifier::run(argv);
  return 0;
}
