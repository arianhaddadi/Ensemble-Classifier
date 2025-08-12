#ifndef LINEAR_CLASSIFIER_H
#define LINEAR_CLASSIFIER_H

#include <string>
#include <vector>

class LinearClassifier {
public:
  /**
   * Runs the linear classifier process.
   *
   * @param[in] datasetAddress: Location of dataset on disk
   * @param[in] namedPipeFilename: Name of the FIFO pipe shared between this
   * process and its parent process
   * @param[in] weightsFileAddress: Location of the weights file for this
   * classifier
   * @param[in] index: Index of this classifier among all linear classifiers
   */
  static void run(const std::string &datasetAddress,
                  const std::string &namedPipeFilename,
                  const std::string &weightsFileAddress, int index);

private:
  /**
   * Reads the weights from a CSV file and returns them as a 2D vector of
   * strings.
   *
   * @param[in] filename: Path to the weights CSV file.
   * @returns A 2D vector containing the weights read from the file.
   */
  static std::vector<std::vector<std::string>>
  getWeights(const std::string &filename);

  /**
   * Classifies the dataset using the provided weights and writes the results
   * to a named pipe.
   *
   * @param[in] weights: A 2D vector containing the weights for classification.
   * @param[in] datasetFilename: Path to the dataset CSV file.
   * @param[in] namedPipeFilename: The named pipe used for communication.
   * @param[in] index: The index of the classifier.
   */
  static void
  classifyDataset(const std::vector<std::vector<std::string>> &weights,
                  const std::string &datasetFilename,
                  const std::string &namedPipeFilename, int index);

  /**
   * Calculates the dot product of two vectors of strings, where each string
   * represents a numeric value.
   *
   * @param[in] v1: The first vector.
   * @param[in] v2: The second vector.
   * @returns The dot product of the two vectors as a float.
   */
  static float dotProduct(const std::vector<std::string> &v1,
                          const std::vector<std::string> &v2);
};

#endif // LINEAR_CLASSIFIER_H
