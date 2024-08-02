#include "headers.hpp"

std::vector<std::string> parse(const std::string &line, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}

float dotProduct(const std::vector<std::string> &v1,
                 const std::vector<std::string> &v2) {
    float product = 0;
    for (int i = 0; i < v2.size(); i++) {
        product += (stof(v1[i]) * stof(v2[i]));
    }
    return product + stof(v1[v1.size() - 1]);
}

void save(std::vector<std::vector<int>> &table, char *readData) {
    std::vector<std::string> partsOfReadData =
            parse(readData, COORD_DELIMITER_CHARACTER);
    int x = stoi(partsOfReadData[0]);
    int y = stoi(partsOfReadData[1]);
    int numOfClass = stoi(partsOfReadData[2]);
    table[y][x] = numOfClass;
}

int collectMajorityVote(const std::vector<int> &votes) {
    std::map<int, int> repeats;
    int maxRepeats = 0;
    int majorityVote;

    for (int vote: votes) {
        if (++repeats[vote] > maxRepeats) {
            maxRepeats = repeats[vote];
            majorityVote = vote;
        } else if (repeats[vote] == maxRepeats && vote < majorityVote) {
            majorityVote = vote;
        }
    }

    return majorityVote;
}

void getLabels(std::vector<int> &labelsVector, const std::string &filename) {
    std::ifstream labels;
    labels.open(filename);
    std::string line;
    getline(labels, line);
    while (getline(labels, line)) {
        labelsVector.push_back(stoi(line));
    }
    labels.close();
}

void calAccuracy(const std::vector<int> &estimate,
                 const std::vector<int> &real) {
    int correctEstimate = 0;
    for (int i = 0; i < estimate.size(); i++) {
        if (estimate[i] == real[i]) {
            correctEstimate++;
        }
    }
    std::cout << "Accuracy: "
              << ((float) correctEstimate / (float) estimate.size()) * 100
              << "%" << std::endl;
}

void sendToEnsembleClassifier(const std::vector<std::vector<int>> &votes,
                              char *ensembleNamedPipeName) {
    std::vector<int> majorityVotes(votes.size());
    for (int i = 0; i < votes.size(); i++) {
        majorityVotes[i] = collectMajorityVote(votes[i]);
    }

    int fd = open(ensembleNamedPipeName, O_WRONLY);
    for (int i = 0; i < votes.size(); i++) {
        write(fd, std::to_string(majorityVotes[i]).c_str(), MAX_LENGTH);
    }
    close(fd);
}


void classifyDataset(const std::vector<std::vector<std::string>> &weightVectors,
                     char *datasetFilename, char *namedPipeFilename,
                     char *index) {
    std::ifstream dataset;
    dataset.open(datasetFilename);

    int fd = open(namedPipeFilename, O_WRONLY);

    std::string line;
    std::vector<std::string> csvLine;
    getline(dataset, line);
    int indexOfMax, lineNum = 0;
    float max, product;

    while (getline(dataset, line)) {
        csvLine.clear();
        csvLine = parse(line, CSV_DELIMITER);
        max = 0;
        for (int i = 0; i < weightVectors.size(); i++) {
            product = dotProduct(weightVectors[i], csvLine);
            if (product > max) {
                max = product;
                indexOfMax = i;
            }
        }

        std::stringstream message;
        message << index << COORD_DELIMITER_STRING << lineNum
                << COORD_DELIMITER_STRING << indexOfMax;

        write(fd, message.str().c_str(), MAX_LENGTH);
        lineNum++;
    }

    close(fd);
    dataset.close();
}

void getWeights(std::vector<std::vector<std::string>> &weightVectors,
                char *weightVectorFilename) {
    std::ifstream weights;
    std::vector<std::string> csvLine;
    weights.open(weightVectorFilename);
    std::string line;

    getline(weights, line);
    while (getline(weights, line)) {
        csvLine.clear();
        csvLine = parse(line, CSV_DELIMITER);
        weightVectors.push_back(csvLine);
    }

    weights.close();
}

int getNumOfClassifiers(const std::string &classifiersDirectory) {
    int counter = 0;
    while (true) {
        std::stringstream fileAddress;
        fileAddress << classifiersDirectory << "/" << CLASSIFIER_FILENAME_PREFIX
                    << std::to_string(counter) << CSV_FILE_FORMAT;

        int fd = open(fileAddress.str().c_str(), O_RDONLY);
        if (fd < 0) break;
        counter++;
        close(fd);
    }
    return counter;
}

void communicateWithVoter(const std::vector<int> &labelsVector,
                          const std::string &voterNamedPipeName,
                          const std::string &linearClassifierNamedPipeName,
                          int numOfClassifiers) {

    // send data to voter
    int fd = open(voterNamedPipeName.c_str(), O_WRONLY);
    write(fd, linearClassifierNamedPipeName.c_str(), MAX_LENGTH);
    write(fd, std::to_string(labelsVector.size()).c_str(), MAX_LENGTH);
    write(fd, std::to_string(numOfClassifiers).c_str(), MAX_LENGTH);
    close(fd);

    // receive data from voter
    fd = open(voterNamedPipeName.c_str(), O_RDONLY);
    std::vector<int> estimatedLabels;
    char readBuffer[MAX_LENGTH];
    for (int i = 0; i < labelsVector.size(); i++) {
        read(fd, readBuffer, MAX_LENGTH);
        estimatedLabels.push_back(atoi(readBuffer));
    }
    close(fd);

    calAccuracy(estimatedLabels, labelsVector);
}