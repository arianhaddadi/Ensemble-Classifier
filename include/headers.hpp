#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <vector>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <map>
#include <algorithm>
#include <iomanip>


#define MAX_LENGTH 100
#define PIPE_FLAG 0666
#define CSV_DELIMITER ','
#define LABELS_FILENAME "labels.csv"
#define LINEAR_CLASSIFIER_FIFO_FILE "npf"
#define VOTER_FIFO_FILE "vpf"
#define VOTER_EXECUTABLE_NAME "./voter"
#define LINEAR_CLASSIFIER_EXECUTABLE_NAME "./linear_classifier"
#define DATASET_FILENAME "dataset.csv"
#define CLASSIFIER_FILENAME_PREFIX "classifier_"
#define CSV_FILE_FORMAT ".csv"
#define COORD_DELIMITER_CHARACTER '_'
#define COORD_DELIMITER_STRING "_"

std::vector<std::string> parse(const std::string &, char);

float dotProduct(const std::vector<std::string> &,
                 const std::vector<std::string> &);

void save(std::vector<std::vector<int>> &, char *);

int collectMajorityVote(const std::vector<int> &row);

void getLabels(std::vector<int> &, const std::string &);

void calAccuracy(const std::vector<int> &, const std::vector<int> &);

void sendToEnsembleClassifier(const std::vector<std::vector<int>> &, char *);

void communicateWithVoter(const std::vector<int> &, const std::string &,
                          const std::string &, int);

void getWeights(std::vector<std::vector<std::string>> &, char *);

void classifyDataset(const std::vector<std::vector<std::string>> &, char *,
                     char *, char *);

int getNumOfClassifiers(const std::string &);