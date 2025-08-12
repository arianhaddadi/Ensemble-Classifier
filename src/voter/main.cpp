#include "iostream"
#include "utils.h"
#include "voter.h"
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Wrong Number of arguments. Required num: 2, Given: " << argc;
    return -1;
  }

  // Name of named pipe shared between this process and ensemble classifier
  const std::string ensembleNamedPipeName = argv[1];

  char linearNamedPipeName[MAX_LENGTH] = {0};
  char datasetLengthStr[MAX_LENGTH] = {0};
  char numOfClassifiersStr[MAX_LENGTH] = {0};

  const int fd = open(ensembleNamedPipeName.c_str(), O_RDONLY);
  read(fd, linearNamedPipeName, MAX_LENGTH);
  read(fd, datasetLengthStr, MAX_LENGTH);
  read(fd, numOfClassifiersStr, MAX_LENGTH);
  close(fd);

  const int datasetLength = std::stoi(datasetLengthStr);
  const int numOfClassifier = std::stoi(numOfClassifiersStr);

  Voter::run(linearNamedPipeName, ensembleNamedPipeName, datasetLength,
             numOfClassifier);
  return 0;
}