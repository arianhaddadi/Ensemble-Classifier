#include "headers.hpp"


int main(int argc, char *argv[]) {
    // Name of named pipe shared between this process and ensemble classifier
    char *ensembleNamedPipeName = argv[1];

    char linearNamedPipeName[MAX_LENGTH];
    char datasetLengthStr[MAX_LENGTH];
    char numOfClassifierStr[MAX_LENGTH];

    int fd = open(ensembleNamedPipeName, O_RDONLY);
    read(fd, linearNamedPipeName, MAX_LENGTH);
    read(fd, datasetLengthStr, MAX_LENGTH);
    read(fd, numOfClassifierStr, MAX_LENGTH);
    close(fd);

    int datasetLength = atoi(datasetLengthStr);
    int numOfClassifier = atoi(numOfClassifierStr);

    std::vector<int> row(numOfClassifier);
    std::vector<std::vector<int>> table(datasetLength, row);

    int numOfReadData = 0;
    int numOfTotalData = numOfClassifier * datasetLength;

    fd = open(linearNamedPipeName, O_RDONLY);
    char readBuffer[MAX_LENGTH];
    while (numOfReadData < numOfTotalData) {
        read(fd, readBuffer, MAX_LENGTH);
        save(table, readBuffer);
        numOfReadData++;
    }
    close(fd);

    sendToEnsembleClassifier(table, ensembleNamedPipeName);

    return 0;
}