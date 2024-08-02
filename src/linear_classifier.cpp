#include "headers.hpp"


int main(int argc, char* argv[]) {

    char* datasetFilename = (char*) malloc(MAX_LENGTH * sizeof(char)); 
    read(atoi(argv[1]), datasetFilename, MAX_LENGTH);
    
    char* namedPipeFilename = (char*) malloc(MAX_LENGTH * sizeof(char)); 
    read(atoi(argv[1]), namedPipeFilename, MAX_LENGTH);
    
    char* weightVectorFilename = (char*) malloc(MAX_LENGTH * sizeof(char)); 
    read(atoi(argv[1]), weightVectorFilename, MAX_LENGTH);

    char* index = (char*) malloc(MAX_LENGTH * sizeof(char));
    read(atoi(argv[1]), index, MAX_LENGTH);

    std::vector<std::vector<std::string>> weightVectors;
    getWeights(weightVectors, weightVectorFilename);

    classifyDataset(weightVectors, datasetFilename, namedPipeFilename, index);

    return 0;
}
