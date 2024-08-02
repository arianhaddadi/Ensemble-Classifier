#include "headers.hpp"

int main(int argc, char* argv[]) {
    std::string validationDirectory = argv[1];
    std::string classifiersDirectory = argv[2];

    std::string datasetAddress = validationDirectory + DATASET_FILENAME;
    std::string labelsAddress = validationDirectory + LABELS_FILENAME;

    std::vector<int> labelsVector;
    getLabels(labelsVector, labelsAddress);

    int numOfClassifiers = getNumOfClassifiers(classifiersDirectory);
    
    std::vector<char*> readPipes(numOfClassifiers);

    mkfifo(LINEAR_CLASSIFIER_FIFO_FILE, PIPE_FLAG);
    mkfifo(VOTER_FIFO_FILE, PIPE_FLAG);

    int p[numOfClassifiers][2];

    for (int i = 0; i < numOfClassifiers; i++) {
        pipe(p[i]);
        int pid = fork();
        if (pid == 0) {
            readPipes[i] = (char*)malloc(MAX_LENGTH * sizeof(char));
            sprintf(readPipes[i], "%d", p[i][0]);
            char* args[] = {(char*)"./linear_classifier",
                            readPipes[i], nullptr};
            execvp(args[0], args);
        }
        else {
            std::string classifierNumStr = std::to_string(i);
            std::stringstream classifierAddress;
            classifierAddress << classifiersDirectory << "/"
                           << CLASSIFIER_FILENAME_PREFIX << classifierNumStr
                           << CSV_FILE_FORMAT;


            write(p[i][1], datasetAddress.c_str(), MAX_LENGTH);
            write(p[i][1], LINEAR_CLASSIFIER_FIFO_FILE, MAX_LENGTH);
            write(p[i][1], classifierAddress.str().c_str(), MAX_LENGTH);
            write(p[i][1], classifierNumStr.c_str(), MAX_LENGTH);
        }
    }

    if (fork() == 0) {
        char* args[] = {(char*)"./voter",
                        (char*)VOTER_FIFO_FILE, nullptr};
        execvp(args[0], args);
    }
    else {
        communicateWithVoter(labelsVector, VOTER_FIFO_FILE,
                             LINEAR_CLASSIFIER_FIFO_FILE, numOfClassifiers);
    }

    for (int i = 0 ; i < numOfClassifiers; i++) {
        close(p[i][0]);
        close(p[i][1]);
        wait(nullptr);
    }

    wait(nullptr);
    unlink(LINEAR_CLASSIFIER_FIFO_FILE);
    unlink(VOTER_FIFO_FILE);

    return 0;
}