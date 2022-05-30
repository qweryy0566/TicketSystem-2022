
#ifndef TICKETSYSTEM_UTILS_HPP_
#define TICKETSYSTEM_UTILS_HPP_

#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "unordered_map.hpp"
#include "vector.hpp"
using fqj::unordered_map;
using fqj::vector;
using std::cin;
using std::cout;
using std::string;

namespace fqj {
string To2Str(const int &x) {
  return x < 10 ? "0" + std::to_string(x) : std::to_string(x);
}

template <class Iter, class Compare = std::less<typename Iter::value_type>>
void Qsort(Iter first, Iter last, Compare Cmp = Compare{}) {
  if (first == last) return;
  Iter li{first}, ri{last};
  std::swap(*li, *(li + rand() % (ri - li)));
  typename Iter::value_type x{*li};
  for (--ri; li != ri;) {
    while (li != ri && !Cmp(*ri, x)) --ri;
    *li = *ri;
    while (li != ri && !Cmp(x, *li)) ++li;
    *ri = *li;
  }
  *li = x;
  Qsort(first, li, Cmp);
  Qsort(++li, last, Cmp);
}
}  // namespace fqj

using fqj::To2Str;

//
// Created by 韦捷 on 2022/1/14.
//
class TokenScanner {
 private:
  // char delimiter_ = ' '; //分隔符
  size_t current_ = 0;  //所在的位置
  std::string buffer_;  //储存的字符串缓冲区
 public:
  TokenScanner() = default;
  TokenScanner(const std::string &Str) : current_{0}, buffer_{Str} {}
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

  explicit operator string() const { return str; }

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

template <int len>
struct StrHash {
  size_t operator()(const FixedStr<len> &obj) const {
    size_t ret{0};
    for (int i = 0; i < len && obj[i]; ++i) ret = ret * 257 + obj[i];
    return ret;
  }
};

StrHash<20> UserNameHash, TrainIdHash;
StrHash<30> StationHash;

struct Time {
  int day{0}, hour, min;

  Time() {}
  Time(const string &obj) {  // hh-mm
    hour = (obj[0] - '0') * 10 + obj[1] - '0';
    min = (obj[3] - '0') * 10 + obj[4] - '0';
  }
  explicit operator int() const { return (day * 24 + hour) * 60 + min; }
  explicit operator string() const { return To2Str(hour) + ":" + To2Str(min); }

  Time operator+(const int &rhs) const {
    Time ret;
    int total_time = (day * 24 + hour) * 60 + min + rhs;
    ret.min = total_time % 60, total_time /= 60;
    ret.hour = total_time % 24, total_time /= 24;
    ret.day = total_time;
    return ret;
  }
  friend bool operator<(const Time &lhs, const Time &rhs) {
    if (lhs.hour == rhs.hour) return lhs.min < rhs.min;
    return lhs.hour < rhs.hour;
  }
};

static constexpr int kDay[]{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static constexpr int kSumDay[]{0,   31,  59,  90,  120, 151, 181,
                               212, 243, 273, 304, 334, 365};
// 仅支持平年内的日期少量加减。
struct Date {
  int month, day;

  Date() {}
  Date(const int &m, const int &d) : month{m}, day{d} {}
  Date(const string &obj) {  // hh-mm
    month = (obj[0] - '0') * 10 + obj[1] - '0';
    day = (obj[3] - '0') * 10 + obj[4] - '0';
  }
  explicit operator int() const { return day + kSumDay[month - 1]; }
  explicit operator string() const { return To2Str(month) + "-" + To2Str(day); }

  Date operator+(const int &rhs) const {
    Date ret{month, day + rhs};
    if (ret.day > kDay[ret.month]) ret.day -= kDay[ret.month++];
    return ret;
  }
  Date &operator++() { return *this = *this + 1; }
  Date operator-(const int &rhs) const {
    Date ret{month, day - rhs};
    if (ret.day < 1) ret.day += kDay[--ret.month];
    return ret;
  }
  friend int operator-(const Date &lhs, const Date &rhs) {
    return int(lhs) - int(rhs);
  }
  friend bool operator<(const Date &lhs, const Date &rhs) {
    if (lhs.month == rhs.month) return lhs.day < rhs.day;
    return lhs.month < rhs.month;
  }
  friend bool operator<=(const Date &lhs, const Date &rhs) {
    return !(rhs < lhs);
  }
  friend bool operator>(const Date &lhs, const Date &rhs) { return rhs < lhs; }
  friend bool operator>=(const Date &lhs, const Date &rhs) {
    return !(lhs < rhs);
  }
};

struct DateTime {
  Date date;
  Time time;

  DateTime() {}
  DateTime(const Date &d, const Time &t) : date{d}, time{t} {
    date = time.day > 0 ? date + time.day : date - time.day, time.day = 0;
  }
  explicit operator string() const { return string(date) + " " + string(time); }
  DateTime operator+(const int &rhs) const {
    DateTime ret{date, time + rhs};
    ret.date = ret.date + ret.time.day, ret.time.day = 0;
    return ret;
  }
  friend int operator-(const DateTime &lhs, const DateTime &rhs) {
    return (lhs.date - rhs.date) * 1440 + (int(lhs.time) - int(rhs.time));
  }
  friend std::ostream &operator<<(std::ostream &lhs, const DateTime &rhs) {
    return lhs << string(rhs);
  }
};

#endif  // TICKETSYSTEM_UTILS_HPP_