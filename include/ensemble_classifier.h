#ifndef ENSEMBLE_CLASSIFIER_H
#define ENSEMBLE_CLASSIFIER_H

#include <string>
#include <vector>

class EnsembleClassifier {
public:
  /**
   * Runs the ensemble classifier process.
   *
   * @param[in] validationDirectory: Directory that hosts validation files
   * @param[in] weightsDirectory: Directory that hosts weight files
   */
  static void run(const std::string &validationDirectory,
                  const std::string &weightsDirectory);

private:
  // Constants related to the ensemble classifier
  static constexpr char LABELS_FILENAME[] = "labels.csv";
  static constexpr char LINEAR_CLASSIFIER_FIFO_FILE[] = "npf";
  static constexpr char VOTER_FIFO_FILE[] = "vpf";
  static constexpr char VOTER_EXECUTABLE_NAME[] = "./voter";
  static constexpr char LINEAR_CLASSIFIER_EXECUTABLE_NAME[] = "./linear";
  static constexpr char DATASET_FILENAME[] = "dataset.csv";
  static constexpr char CLASSIFIER_FILENAME_PREFIX[] = "classifier_";

  /**
   * Reads the labels from a CSV file and returns them as a vector of integers.
   *
   * @param[in] filename: Path to the labels CSV file.
   * @returns A vector containing the labels read from the file.
   */
  static std::vector<int> getLabels(const std::string &filename);

  /**
   * Counts the number of classifier files in the specified directory.
   *
   * @param[in] classifiersDirectory: Path to the directory containing
   * classifier files.
   * @returns The number of classifier files found in the directory.
   */
  static int getNumOfClassifiers(const std::string &classifiersDirectory);

  /**
   * Communicates with the voter process by sending necessary data and receiving
   * the estimated labels. It also calculates the accuracy of the predictions.
   *
   * @param[in] labels: The ground truth labels for validation.
   * @param[in] voterNamedPipeName: The named pipe used for communication with
   * the voter.
   * @param[in] linearClassifierNamedPipeName: The named pipe used for
   * communication with linear classifiers.
   * @param[in] numOfClassifiers: The number of classifiers in the ensemble.
   */
  static void communicateWithVoter(
      const std::vector<int> &labels, const std::string &voterNamedPipeName,
      const std::string &linearClassifierNamedPipeName, int numOfClassifiers);

  /**
   * Calculates and prints the accuracy of the estimated labels compared to the
   * real labels.
   *
   * @param[in] estimate: The estimated labels predicted by the ensemble.
   * @param[in] real: The ground truth labels for validation.
   */
  static void calAccuracy(const std::vector<int> &estimate,
                          const std::vector<int> &real);
};

#endif // ENSEMBLE_CLASSIFIER_H
