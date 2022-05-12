```c++
//
// Created by 韦捷 on 2022/1/14.
//

#ifndef UNTITLED4_TOKENSCANNER_H
#define UNTITLED4_TOKENSCANNER_H

#include <iostream>
#include <string>
#include <cstring>

class TokenScanner {
private:
    //char delimiter_ = ' '; //分隔符
    int current_ = 0; //所在的位置
    std::string buffer_; //储存的字符串缓冲区
public:
    TokenScanner() = default;

    TokenScanner(std::string &);

    std::string NextToken(char delimiter_ = ' ');

    std::string NextToken(char delimiter_head_, char delimiter_tail_, bool ck = false);

    void Clear();

    bool If_left(char delimiter_ = ' ');

    std::string Getleft(char delimiter_ = ' ');

};



#endif //UNTITLED4_TOKENSCANNER_H


//
// Created by 韦捷 on 2021/12/18.
//
#include "tokenscanner.h"
#include <string>

TokenScanner::TokenScanner(std::string & Str) {
    buffer_ = Str;
    current_ = 0;
}

std::string TokenScanner::NextToken(char delimiter_) {
    if (current_ >= buffer_.size()) return "";
    while (current_ < buffer_.size()&&buffer_[current_] == delimiter_) current_++;
    int h = current_;
    while (current_ < buffer_.size()&&buffer_[current_] != delimiter_) current_++;
    return buffer_.substr(h, current_ - h);
}

std::string TokenScanner::NextToken(char delimiter_head_,char delimiter_tail_,bool ck){
    if (current_ == buffer_.size()) return "";
    while (current_ < buffer_.size()&&buffer_[current_] != delimiter_head_) current_++;
    int h = current_+1;
    if(ck) h--;
    while (current_ < buffer_.size()&&buffer_[current_] != delimiter_tail_) current_++;
    return buffer_.substr(h, current_ - h);
}

void TokenScanner::Clear() {
    buffer_.clear();
    current_=0;
}

bool TokenScanner::If_left(char delimiter_){
    if(current_==buffer_.size()) return true;
    int h=current_;
    while(h<buffer_.size()&&buffer_[h]==delimiter_) h++;
    return h==buffer_.size();
}

std::string TokenScanner::Getleft(char delimiter_){
    if(current_==buffer_.size()) return "";
    while(current_<buffer_.size()&&buffer_[current_]==delimiter_) current_++;
    if(current_==buffer_.size()) return "";
    int t=buffer_.size()-1;
    while(t>=current_&&buffer_[t]==delimiter_) t--;
    return buffer_.substr(current_,t+1-current_);
}
```

