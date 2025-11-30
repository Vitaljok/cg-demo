#include "utils.hpp"
#include <format>
#include <fstream>
#include <sstream>

std::vector<uint8_t> readFile(const std::string &fileName) {
  std::ifstream file(fileName, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error(std::format("Failed to open file {}", fileName));
  }

  size_t fileSize = (size_t)file.tellg();
  std::vector<uint8_t> buffer(fileSize);
  file.seekg(0);
  file.read((char *)buffer.data(), fileSize);
  file.close();
  return buffer;
}


std::string readTextFile(const std::string &fileName) {
  std::ifstream file(fileName);
  if (!file.is_open()) {
    throw std::runtime_error(std::format("Failed to open file {}", fileName));
  }

  std::stringstream ss;
  ss << file.rdbuf();
  return ss.str();
}
