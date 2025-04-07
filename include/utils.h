#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

// General-purpose constants
constexpr int MAX_LENGTH = 100;
constexpr char COORD_DELIMITER_CHAR = '_';
constexpr char CSV_FILE_EXTENSION[] = ".csv";
constexpr char CSV_DELIMITER = ',';

/**
 * Parses a line that has multiple data separated by the given delimiter and
 * returns a vector of those data;
 * @param[in] line: Line to parse
 * @param[in] delimiter: Character used to separate data in the given line
 * @returns a vector of the data in the line.
 */
std::vector<std::string> parse_line(const std::string &line, char delimiter);

#endif // UTILS_H
