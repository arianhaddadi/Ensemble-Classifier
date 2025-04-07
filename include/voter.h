#ifndef VOTER_H
#define VOTER_H

#include <string>
#include <vector>

class Voter {
public:
  /**
   * Runs the voter process.
   *
   * @param[in] argv: Command-line arguments passed to the voter process.
   */
  static void run(char **argv);

private:
  /**
   * Sends the majority votes to the ensemble classifier through a named pipe.
   *
   * @param[in] votes: A 2D vector containing the votes from all classifiers.
   * @param[in] ensembleNamedPipeName: The named pipe used for communication
   * with the ensemble classifier.
   */
  static void
  sendToEnsembleClassifier(const std::vector<std::vector<int>> &votes,
                           const std::string &ensembleNamedPipeName);

  /**
   * Collects the majority vote from a vector of votes, resolving ties by
   * choosing the smallest value.
   *
   * @param[in] votes: A vector containing votes for a single data point.
   * @returns The majority vote.
   */
  static int collectMajorityVote(const std::vector<int> &votes);

  /**
   * Saves the vote data into a 2D table based on the parsed coordinates and
   * class.
   *
   * @param[in,out] table: A 2D vector where the vote data will be stored.
   * @param[in] readData: A string containing the vote data in a delimited
   * format.
   */
  static void save(std::vector<std::vector<int>> &table,
                   const std::string &readData);
};

#endif // VOTER_H
