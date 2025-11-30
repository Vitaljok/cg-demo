#pragma once

#include <vector>
#include <string>

/**
 * Read content of binary file to byte buffer.
 */
std::vector<uint8_t> readFile(const std::string &fileName);

/**
 * Read content of text file to string (null terminated).
 */
std::string readTextFile(const std::string &fileName);