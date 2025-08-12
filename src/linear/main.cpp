#include "iostream"
#include "linear_classifier.h"
#include "utils.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Wrong Number of arguments. Required num: 2, Given: " << argc;
    return -1;
  }

  char datasetAddress[MAX_LENGTH] = {0};
  char namedPipeFilename[MAX_LENGTH] = {0};
  char weightsFileAddress[MAX_LENGTH] = {0};
  char classifierNum[MAX_LENGTH] = {0};

  /* Get file descriptor of the read end of the pipe shared by this process
   * and its parent process */
  const int pipe_fd = std::stoi(argv[1]);
  read(pipe_fd, datasetAddress, MAX_LENGTH);
  read(pipe_fd, namedPipeFilename, MAX_LENGTH);
  read(pipe_fd, weightsFileAddress, MAX_LENGTH);
  read(pipe_fd, classifierNum, MAX_LENGTH);
  close(pipe_fd);

  const int index = std::stoi(std::string(classifierNum));

  LinearClassifier::run(datasetAddress, namedPipeFilename, weightsFileAddress,
                        index);
  return 0;
}
