#ifndef VOTER_H
#define VOTER_H

#include <string>
#include <vector>

class Voter {
public:
  static void run(char **argv);

  static void
  sendToEnsembleClassifier(const std::vector<std::vector<int>> &votes,
                           char *ensembleNamedPipeName);

  static int collectMajorityVote(const std::vector<int> &votes);

  static void save(std::vector<std::vector<int>> &table, char *readData);
};

#endif // VOTER_H
