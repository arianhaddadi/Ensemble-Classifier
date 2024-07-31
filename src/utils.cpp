#include "headers.hpp"

void clear(char* chr) {
    for (int i = 0 ; i < MAX_LENGTH ; i++) {
        chr[i] = '\0';
    }
}

vector<string> parse(string line, char delimiter) {
    vector<string> parts;
    string part;
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

double dotProduct(vector<string>& v1, vector<string>& v2) {
    double product = 0;
    for (int i = 0 ; i < v2.size(); i++) {
        product += (stod(v1[i]) * stod(v2[i]));
    }
    return product + stod(v1[v1.size()-1]);
}

void prepareMsg(char* msg, char* index, string lineNum, string classNum) {
    clear(msg);
    strcat(msg, index);
    strcat(msg, COORD_DELIMITER_STRING);
    strcat(msg, lineNum.c_str());
    strcat(msg, COORD_DELIMITER_STRING);
    strcat(msg, classNum.c_str());
}

void save(vector<vector<double>>&table, char* readData) {
    vector<string> partsOfReadData = parse(readData, COORD_DELIMITER_CHARACTER);
    double x = stod(partsOfReadData[0]), y = stod(partsOfReadData[1]), numOfClass = stod(partsOfReadData[2]);
    table[y][x] = numOfClass;
}

bool contains(double num, vector<double>& container) {
    for (int i = 0; i < container.size(); i++) {
        if (container[i] == num) {
            return true;
        }
    }
    return false;
}

double maximum(vector<double>& row) {
    map<double, double> repeat;
    vector<double> keys;
    double maxValue = 0, maxKey;
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

void getLabels(vector<double> &labelsVector, string filename) {
    ifstream labels;
    labels.open(filename);
    string line;
    getline(labels, line);
    while(getline(labels, line)) {
        labelsVector.push_back(stod(line));
    }
    labels.close();
}

void calAccuracy(vector<double>& estimate, vector<double>& real) {
    int correctEstimate = 0;
    for (int i = 0; i < estimate.size(); i++) {
        if (estimate[i] == real[i]) {
            correctEstimate++;
        }
    }
    cout << "Accuracy:" << fixed << setprecision(2) << (double)correctEstimate/(double)estimate.size()*100 << "%" << endl;
}

void sendToEnsembleClassifier(vector<vector<double>>& table, char* VoterPipeFilename) {
    int datasetLength = table.size(); 
    vector<double> votes;
    for (int i = 0; i < datasetLength; i++) {
        votes.push_back(maximum(table[i]));
    }
    int fd = open(VoterPipeFilename, O_WRONLY);
    for (int i = 0; i < datasetLength; i++) {
        write(fd, to_string(votes[i]).c_str(), MAX_LENGTH);
    }

    close(fd);
}


void classifyDataset(vector<vector<string>>& weightVectors, char* datasetFilename, char* namedPipeFilename, char* index) {
    ifstream dataset;
    dataset.open(datasetFilename);

    int fd = open(namedPipeFilename, O_WRONLY);

    char* msg = (char*) malloc(MAX_LENGTH * sizeof(char));
    string line;
    vector<string> csvLine;
    getline(dataset, line);
    int indexOfMax, lineNum = 0;
    double max, product;
    
    while(getline(dataset, line)) {
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
        prepareMsg(msg, index, to_string(lineNum), to_string(indexOfMax));
        write(fd, msg, MAX_LENGTH);
        lineNum++;
    }

    close(fd);
    dataset.close();
}

void getWeights(vector<vector<string>>& weightVectors, char* weightVectorFilename) {
    ifstream weights;
    vector<string> csvLine;
    weights.open(weightVectorFilename);
    string line;

    getline(weights, line);
    while (getline(weights, line)) {
        csvLine.clear();
        csvLine = parse(line, CSV_DELIMITER);
        weightVectors.push_back(csvLine);
    }

    weights.close();
}

int getNumOfClassifiers(string classifierNamePrefix, string classifierNameSuffix) {
    int counter = 0, fd;
    while(true) {
        fd = open((classifierNamePrefix + to_string(counter) + classifierNameSuffix).c_str(), O_RDONLY);
        if (fd < 0) break;
        counter++;
        close(fd);
    }
    return counter;
}

void communicateWithVoter(vector<double>& labelsVector, char* voterPipeFilename, char* namedPipeFilename, int numOfClassifiers) {
    
    //send data to voter
    int fd = open(voterPipeFilename, O_WRONLY), datasetLength = labelsVector.size();
    write(fd, namedPipeFilename, MAX_LENGTH);
    write(fd, to_string(datasetLength).c_str(), MAX_LENGTH);
    write(fd, to_string(numOfClassifiers).c_str(), MAX_LENGTH);
    close(fd);

    //receive data from voter
    fd = open(voterPipeFilename, O_RDONLY);
    vector<double> estimatedLabels;
    char* readLabel = (char*)malloc(MAX_LENGTH * sizeof(char));
    for (int i = 0; i < datasetLength; i++) {
        clear(readLabel);
        read(fd, readLabel, MAX_LENGTH);
        estimatedLabels.push_back(atof(readLabel));
    }
    close(fd);

    //calculate Accuracy
    calAccuracy(estimatedLabels, labelsVector);
}