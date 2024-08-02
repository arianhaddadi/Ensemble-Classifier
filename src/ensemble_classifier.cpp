#include "headers.hpp"

int main(int argc, char *argv[]) {
    std::string validationDirectory = argv[1];
    std::string weightsDirectory = argv[2];

    std::string datasetFileAddress =
            validationDirectory + "/" + DATASET_FILENAME;
    std::string labelsFileAddress = validationDirectory + "/" + LABELS_FILENAME;

    std::vector<int> labelsVector;
    getLabels(labelsVector, labelsFileAddress);

    int numOfClassifiers = getNumOfClassifiers(weightsDirectory);

    mkfifo(LINEAR_CLASSIFIER_FIFO_FILE, PIPE_FLAG);
    mkfifo(VOTER_FIFO_FILE, PIPE_FLAG);

    int p[numOfClassifiers][2];

    for (int i = 0; i < numOfClassifiers; i++) {
        pipe(p[i]);
        int pid = fork();
        if (pid == 0) {
            close(p[i][1]); // close the write end of the pipe
            std::string pipeReadFdStr = std::to_string(p[i][0]);
            char *args[] = {(char *) LINEAR_CLASSIFIER_EXECUTABLE_NAME,
                            (char*) pipeReadFdStr.c_str(), nullptr};
            execvp(args[0], args);
        } else {
            close(p[i][0]); // close the read end of the pipe
            std::string classifierNumStr = std::to_string(i);
            std::stringstream weightsFileAddress;
            weightsFileAddress << weightsDirectory << "/"
                               << CLASSIFIER_FILENAME_PREFIX << classifierNumStr
                               << CSV_FILE_FORMAT;

            write(p[i][1], datasetFileAddress.c_str(), MAX_LENGTH);
            write(p[i][1], LINEAR_CLASSIFIER_FIFO_FILE, MAX_LENGTH);
            write(p[i][1], weightsFileAddress.str().c_str(), MAX_LENGTH);
            write(p[i][1], classifierNumStr.c_str(), MAX_LENGTH);
        }
    }

    if (fork() == 0) {
        // child process
        char *args[] = {(char *) VOTER_EXECUTABLE_NAME,
                        (char *) VOTER_FIFO_FILE, nullptr};
        execvp(args[0], args);
    } else {
        // parent process
        communicateWithVoter(labelsVector, VOTER_FIFO_FILE,
                             LINEAR_CLASSIFIER_FIFO_FILE, numOfClassifiers);
    }

    for (int i = 0; i < numOfClassifiers; i++) {
        close(p[i][1]);
        wait(nullptr);
    }

    wait(nullptr);
    unlink(LINEAR_CLASSIFIER_FIFO_FILE);
    unlink(VOTER_FIFO_FILE);

    return 0;
}