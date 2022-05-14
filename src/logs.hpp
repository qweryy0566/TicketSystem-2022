#ifndef TICKETSYSTEM_LOGS_HPP_
#define TICKETSYSTEM_LOGS_HPP_

#include "../lib/bplustree.hpp"

class LogManagement {
  // 按时间戳存命令。
  BPlusTree<int, int, std::string> logs;

 public:
  LogManagement() : logs{"logs_index.bin", "logs.bin"} {}
  void AddLog(const int &timestamp, const std::string &cmd) {
    
  }
};

#endif  // TICKETSYSTEM_LOGS_HPP_