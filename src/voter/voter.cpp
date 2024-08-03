#include "voter.h"
#include "utils.h"
#include <algorithm>
#include <fcntl.h>
#include <map>
#include <unistd.h>

#define MAX_LENGTH 100
#define COORD_DELIMITER_CHARACTER '_'

void Voter::run(char **argv) {
  // Name of named pipe shared between this process and ensemble classifier
  char *ensembleNamedPipeName = argv[1];

  char linearNamedPipeName[MAX_LENGTH];
  char datasetLengthStr[MAX_LENGTH];
  char numOfClassifierStr[MAX_LENGTH];

  int fd = open(ensembleNamedPipeName, O_RDONLY);
  read(fd, linearNamedPipeName, MAX_LENGTH);
  read(fd, datasetLengthStr, MAX_LENGTH);
  read(fd, numOfClassifierStr, MAX_LENGTH);
  close(fd);

  int datasetLength = atoi(datasetLengthStr);
  int numOfClassifier = atoi(numOfClassifierStr);

  std::vector<int> row(numOfClassifier);
  std::vector<std::vector<int>> table(datasetLength, row);

  int numOfReadData = 0;
  int numOfTotalData = numOfClassifier * datasetLength;

  fd = open(linearNamedPipeName, O_RDONLY);
  char readBuffer[MAX_LENGTH];
  while (numOfReadData < numOfTotalData) {
    read(fd, readBuffer, MAX_LENGTH);
    save(table, readBuffer);
    numOfReadData++;
  }
  close(fd);

  sendToEnsembleClassifier(table, ensembleNamedPipeName);
}
void Voter::sendToEnsembleClassifier(const std::vector<std::vector<int>> &votes,
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
int Voter::collectMajorityVote(const std::vector<int> &votes) {
  std::map<int, int> repeats;
  int maxRepeats = 0;
  int majorityVote;

  for (int vote : votes) {
    if (++repeats[vote] > maxRepeats) {
      maxRepeats = repeats[vote];
      majorityVote = vote;
    } else if (repeats[vote] == maxRepeats && vote < majorityVote) {
      majorityVote = vote;
    }
  }

  return majorityVote;
}

void Voter::save(std::vector<std::vector<int>> &table, char *readData) {
  std::vector<std::string> partsOfReadData =
      parse_csv(readData, COORD_DELIMITER_CHARACTER);
  int x = stoi(partsOfReadData[0]);
  int y = stoi(partsOfReadData[1]);
  int numOfClass = stoi(partsOfReadData[2]);
  table[y][x] = numOfClass;
}