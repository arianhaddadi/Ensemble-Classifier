#include "headers.hpp"


int main(int argc, char *argv[]) {
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

    return 0;
}
