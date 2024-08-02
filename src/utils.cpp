#include "headers.hpp"

void clear(char *chr) {
    for (int i = 0; i < MAX_LENGTH; i++) {
        chr[i] = '\0';
    }
}

std::vector<std::string> parse(const std::string &line, char delimiter) {
    std::vector<std::string> parts;
    std::string part;
    int i = 0;
    while (true) {
        part = "";
        while (i < line.size() && line[i] != delimiter) {
            part += line[i];
            i++;
        }
        parts.push_back(part);
        if (i >= line.size()) break;
        i++;
    }
    return parts;
}

float dotProduct(const std::vector<std::string> &v1,
                 const std::vector<std::string> &v2) {
    float product = 0;
    for (int i = 0; i < v2.size(); i++) {
        product += (stof(v1[i]) * stof(v2[i]));
    }
    return product + stof(v1[v1.size() - 1]);
}

void prepareMsg(char *msg, char *index, const std::string &lineNum,
                const std::string &classNum) {
    clear(msg);
    strcat(msg, index);
    strcat(msg, COORD_DELIMITER_STRING);
    strcat(msg, lineNum.c_str());
    strcat(msg, COORD_DELIMITER_STRING);
    strcat(msg, classNum.c_str());
}

void save(std::vector<std::vector<int>> &table, char *readData) {
    std::vector<std::string> partsOfReadData =
            parse(readData, COORD_DELIMITER_CHARACTER);
    int x = stoi(partsOfReadData[0]);
    int y = stoi(partsOfReadData[1]);
    int numOfClass = stoi(partsOfReadData[2]);
    table[y][x] = numOfClass;
}

int maximum(const std::vector<int> &row) {
    std::map<int, int> repeat;
    std::vector<int> keys;
    int maxValue = 0, maxKey;
    for (int i = 0; i < row.size(); i++) {
        repeat[row[i]]++;
        if (repeat[row[i]] == 1) {
            keys.push_back(row[i]);
        }
    }
    sort(keys.begin(), keys.end());
    for (int i = 0; i < keys.size(); i++) {
        if (repeat[keys[i]] > maxValue) {
            maxValue = repeat[keys[i]];
            maxKey = keys[i];
        }
    }
    return maxKey;
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

void sendToEnsembleClassifier(const std::vector<std::vector<int>> &table,
                              char *VoterPipeFilename) {
    std::vector<int> votes;
    for (int i = 0; i < table.size(); i++) {
        votes.push_back(maximum(table[i]));
    }

    int fd = open(VoterPipeFilename, O_WRONLY);
    for (int i = 0; i < table.size(); i++) {
        write(fd, std::to_string(votes[i]).c_str(), MAX_LENGTH);
    }
    close(fd);
}


void classifyDataset(const std::vector<std::vector<std::string>> &weightVectors,
                     char *datasetFilename, char *namedPipeFilename,
                     char *index) {
    std::ifstream dataset;
    dataset.open(datasetFilename);

    int fd = open(namedPipeFilename, O_WRONLY);

    char *msg = (char *) malloc(MAX_LENGTH * sizeof(char));
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
        prepareMsg(msg, index, std::to_string(lineNum),
                   std::to_string(indexOfMax));
        write(fd, msg, MAX_LENGTH);
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
                          const std::string &voterPipeFilename,
                          const std::string &namedPipeFilename,
                          int numOfClassifiers) {

    // send data to voter
    int fd = open(voterPipeFilename.c_str(), O_WRONLY);
    write(fd, namedPipeFilename.c_str(), MAX_LENGTH);
    write(fd, std::to_string(labelsVector.size()).c_str(), MAX_LENGTH);
    write(fd, std::to_string(numOfClassifiers).c_str(), MAX_LENGTH);
    close(fd);

    // receive data from voter
    fd = open(voterPipeFilename.c_str(), O_RDONLY);
    std::vector<int> estimatedLabels;
    char *readLabel = (char *) malloc(MAX_LENGTH * sizeof(char));
    for (int i = 0; i < labelsVector.size(); i++) {
        clear(readLabel);
        read(fd, readLabel, MAX_LENGTH);
        estimatedLabels.push_back(atoi(readLabel));
    }
    close(fd);

    calAccuracy(estimatedLabels, labelsVector);
}