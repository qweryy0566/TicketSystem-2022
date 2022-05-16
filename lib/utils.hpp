//
// Created by 韦捷 on 2022/1/14.
//

#ifndef TICKETSYSTEM_UTILS_HPP_
#define TICKETSYSTEM_UTILS_HPP_

#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "unordered_map.hpp"
#include "vector.hpp"
using std::cin;
using std::cout;
using std::string;

class TokenScanner {
 private:
  // char delimiter_ = ' '; //分隔符
  size_t current_ = 0;  //所在的位置
  std::string buffer_;  //储存的字符串缓冲区
 public:
  TokenScanner() = default;
  TokenScanner(std::string &Str) : current_{0}, buffer_{Str} {}
  TokenScanner(const TokenScanner &obj)
      : current_{obj.current_}, buffer_{obj.buffer_} {}

  std::string NextToken(char delimiter_ = ' ') {
    if (current_ >= buffer_.size()) return "";
    while (current_ < buffer_.size() && buffer_[current_] == delimiter_)
      current_++;
    size_t h = current_;
    while (current_ < buffer_.size() && buffer_[current_] != delimiter_)
      current_++;
    return buffer_.substr(h, current_ - h);
  }

  std::string NextToken(char delimiter_head_, char delimiter_tail_,
                        bool ck = 0) {
    if (current_ == buffer_.size()) return "";
    while (current_ < buffer_.size() && buffer_[current_] != delimiter_head_)
      ++current_;
    size_t h = current_ + 1 - ck;
    while (current_ < buffer_.size() && buffer_[current_] != delimiter_tail_)
      ++current_;
    return buffer_.substr(h, current_++ - h);  // 修改：++
  }
  void Clear() {
    buffer_.clear();
    current_ = 0;
  }

  bool If_left(char delimiter_ = ' ') {
    if (current_ == buffer_.size()) return true;
    size_t h = current_;
    while (h < buffer_.size() && buffer_[h] == delimiter_) h++;
    return h == buffer_.size();
  }

  std::string Getleft(char delimiter_ = ' ') {
    if (current_ == buffer_.size()) return "";
    while (current_ < buffer_.size() && buffer_[current_] == delimiter_)
      current_++;
    if (current_ == buffer_.size()) return "";
    size_t t = buffer_.size() - 1;
    while (t >= current_ && buffer_[t] == delimiter_) t--;
    return buffer_.substr(current_, t + 1 - current_);
  }
};

// 为文件读写而写的定长字符串类。
template <int len>
class FixedStr {
  char str[len + 1]{};

 public:
  FixedStr() {}
  FixedStr(const string &obj) { strcpy(str, obj.c_str()); }

  string ToString() const { return str; }

  char &operator[](const int &index) { return str[index]; }
  const char operator[](const int &index) const { return str[index]; }

  friend bool operator<(const FixedStr<len> &lhs, const FixedStr<len> &rhs) {
    for (int i = 0; i < len; ++i) {
      if (lhs[i] < rhs[i]) return 1;
      if (lhs[i] > rhs[i]) return 0;
    }
    return 0;
  }
  friend bool operator<=(const FixedStr<len> &lhs, const FixedStr<len> &rhs) {
    return !(rhs < lhs);
  }
  friend bool operator>(const FixedStr<len> &lhs, const FixedStr<len> &rhs) {
    return rhs < lhs;
  }
  friend bool operator>=(const FixedStr<len> &lhs, const FixedStr<len> &rhs) {
    return !(lhs < rhs);
  }
  friend bool operator==(const FixedStr<len> &lhs, const FixedStr<len> &rhs) {
    for (int i = 0; i < len && (lhs[i] || rhs[i]); ++i)
      if (lhs[i] != rhs[i]) return 0;
    return 1;
  }
  friend bool operator!=(const FixedStr<len> &lhs, const FixedStr<len> &rhs) {
    return !(lhs == rhs);
  }

  friend std::ostream &operator<<(std::ostream &lhs, const FixedStr<len> &rhs) {
    return lhs << rhs.str;
  }
};

namespace fqj {}  // namespace fqj

#endif  // TICKETSYSTEM_UTILS_HPP_