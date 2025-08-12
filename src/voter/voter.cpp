#include <fcntl.h>
#include <map>
#include <unistd.h>

#include "utils.h"
#include "voter.h"

void Voter::run(const std::string &linearNamedPipeName,
                const std::string &ensembleNamedPipeName,
                const int datasetLength, const int numOfClassifier) {

  std::vector<std::vector<int>> table(datasetLength,
                                      std::vector<int>(numOfClassifier));

  const int numOfTotalData = numOfClassifier * datasetLength;
  const int fd = open(linearNamedPipeName.c_str(), O_RDONLY);
  for (int i = 0; i < numOfTotalData; i++) {
    char readBuffer[MAX_LENGTH];
    read(fd, readBuffer, MAX_LENGTH);
    save(table, std::string(readBuffer));
  }
  close(fd);

  sendToEnsembleClassifier(table, ensembleNamedPipeName);
}

void Voter::sendToEnsembleClassifier(const std::vector<std::vector<int>> &votes,
                                     const std::string &ensembleNamedPipeName) {
  std::vector<int> majorityVotes(votes.size());
  for (int i = 0; i < votes.size(); i++) {
    majorityVotes[i] = collectMajorityVote(votes[i]);
  }

  const int fd = open(ensembleNamedPipeName.c_str(), O_WRONLY);
  for (int i = 0; i < votes.size(); i++) {
    write(fd, std::to_string(majorityVotes[i]).c_str(), MAX_LENGTH);
  }
  close(fd);
}

int Voter::collectMajorityVote(const std::vector<int> &votes) {
  std::map<int, int> repeats;
  int maxRepeats = 0;
  int majorityVote = 0;

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

void Voter::save(std::vector<std::vector<int>> &table,
                 const std::string &readData) {
  const std::vector<std::string> data =
      parse_line(readData, COORD_DELIMITER_CHAR);
  const int x = stoi(data[0]);
  const int y = stoi(data[1]);
  const int numOfClass = stoi(data[2]);
  table[y][x] = numOfClass;
}