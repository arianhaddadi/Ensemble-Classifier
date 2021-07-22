#include <iostream> 
#include <unistd.h> 
#include <string>
#include <string.h>
#include <vector>
#include <fcntl.h> 
#include <fstream>
#include <sys/stat.h>  
#include <sys/wait.h>
#include <map>
#include <algorithm>
#include <iomanip>


#define MAX_LENGTH 50
#define NUM_OF_VOTERS 1
#define PIPE_FLAG 0666
#define CSV_DELIMITER ','
#define LABELS_FILENAME "/labels.csv"
#define DATASET_FILENAME "/dataset.csv"
#define CLASSIFIER_FILENAME "/classifier_"
#define CSV_FILE_FORMAT ".csv"
#define COORD_DELIMITER_CHARACTER '_'
#define COORD_DELIMITER_STRING "_"


using namespace std;


void clear(char*);
vector<string> parse(string, char);
double dotProduct(vector<string>&, vector<string>&);
void prepareMsg(char*, char*, string, string);
void save(vector<vector<double>>&, char*);
bool contains(double, vector<double>&);
double maximum(vector<double>&);
void getLabels(vector<double> &, string);
void calAccuracy(vector<double>&, vector<double>&);
void sendToEnsembleClassifier(vector<vector<double>>&, char*);
void communicateWithVoter(vector<double>&, char*, char*, int);
void getWeights(vector<vector<string>>&, char*);
void classifyDataset(vector<vector<string>>&, char*, char*, char*);
int getNumOfClassifiers(string, string);