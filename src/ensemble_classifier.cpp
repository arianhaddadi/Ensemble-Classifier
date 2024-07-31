#include "headers.hpp"

int main(int argc, char* argv[]) {
    string dataset = argv[1], classifierNamePrefix = argv[2], classifierNameSuffix = CSV_FILE_FORMAT, numberString, labels = argv[1];

    dataset += DATASET_FILENAME;
    labels += LABELS_FILENAME;
    classifierNamePrefix += CLASSIFIER_FILENAME;

    vector<double> labelsVector;
    getLabels(labelsVector, labels);
    int datasetLength = labelsVector.size();

    int numOfClassifiers = getNumOfClassifiers(classifierNamePrefix, classifierNameSuffix);
    
    vector<char*> readPipes(numOfClassifiers);
    char* namedPipeFilename = (char*)"npf", *voterPipeFilename = (char*)"vpf";
    mkfifo(namedPipeFilename, PIPE_FLAG);
    mkfifo(voterPipeFilename, PIPE_FLAG);
    

    int p[numOfClassifiers][2];

    for (int i = 0; i < numOfClassifiers; i++) {
        pipe(p[i]);
        if (fork() == 0) {
            readPipes[i] = (char*)malloc(MAX_LENGTH * sizeof(char));
            sprintf(readPipes[i], "%d", p[i][0]);
            char* args[] = {(char*)"./Linear.out", readPipes[i], NULL};
            execvp(args[0], args);
        }
        else {
            numberString = to_string(i);
            write(p[i][1], (dataset).c_str(), MAX_LENGTH);
            write(p[i][1], namedPipeFilename, MAX_LENGTH);
            if (i != numOfClassifiers) {
                write(p[i][1], (classifierNamePrefix + numberString + classifierNameSuffix).c_str(), MAX_LENGTH); 
                write(p[i][1], numberString.c_str(), MAX_LENGTH);
            }     
        }
    }

    if (fork() == 0) {
        char* args[] = {(char*)"./Voter.out", voterPipeFilename, NULL};
        execvp(args[0], args);
    }
    else {
        communicateWithVoter(labelsVector, voterPipeFilename, namedPipeFilename, numOfClassifiers);
    }

    for (int i = 0 ; i < numOfClassifiers; i++) {
        close(p[i][0]);
        close(p[i][1]);
        wait(NULL);
    }

    wait(NULL);
    unlink(namedPipeFilename);
    unlink(voterPipeFilename);

    return 0;
}