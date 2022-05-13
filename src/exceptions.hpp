#ifndef TICKETSYSTEM_EXCEPTIONS_HPP_
#define TICKETSYSTEM_EXCEPTIONS_HPP_

#include <string>

class Exception {
  std::string info{""};

 public:
  Exception() {}
  Exception(const std::string &str) : info{str} {}
  virtual std::string what() const { return info; }
};

#endif  // TICKETSYSTEM_EXCEPTIONS_HPP_