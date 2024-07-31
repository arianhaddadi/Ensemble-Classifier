#include "headers.hpp"
 

int main(int argc, char* argv[]) {

    char* voterPipeFilename = argv[1];
    char* LinearPipeFilename = (char*) malloc (MAX_LENGTH * sizeof(char));
    char* datasetLengthStr = (char*) malloc (MAX_LENGTH * sizeof(char));
    char* numOfClassifierStr = (char*) malloc (MAX_LENGTH * sizeof(char));

    int fd = open(voterPipeFilename, O_RDONLY);

    read(fd, LinearPipeFilename, MAX_LENGTH);
    read(fd, datasetLengthStr, MAX_LENGTH);
    read(fd, numOfClassifierStr, MAX_LENGTH);
    
    close(fd);

    int datasetLength = atoi(datasetLengthStr); 
    int numOfClassifier = atoi(numOfClassifierStr);

    vector<double> row(numOfClassifier);
    vector<vector<double>> table(datasetLength, row);

    int numOfReadData = 0, numOfTotalData = numOfClassifier * datasetLength;

    fd = open(LinearPipeFilename, O_RDONLY);
    char* readData = (char*) malloc(MAX_LENGTH * sizeof(char));

    while (numOfReadData < numOfTotalData) {
        clear(readData);
        read(fd, readData, MAX_LENGTH);
        save(table, readData);
        numOfReadData++;
    }

    close(fd);

    sendToEnsembleClassifier(table, voterPipeFilename);

    return 0;
}