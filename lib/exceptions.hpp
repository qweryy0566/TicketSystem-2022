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

class index_out_of_bound : public Exception {
	/* __________________________ */
};

class runtime_error : public Exception {
	/* __________________________ */
};

class invalid_iterator : public Exception {
	/* __________________________ */
};

class container_is_empty : public Exception {
	/* __________________________ */
};

#endif  // TICKETSYSTEM_EXCEPTIONS_HPP_