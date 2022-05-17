#ifndef TICKETSYSTEM_TRAINS_HPP_
#define TICKETSYSTEM_TRAINS_HPP_

#include "../lib/bplustree.hpp"
#include "../lib/utils.hpp"

static StrHash<20> TrainIdHash;
static StrHash<30> StationHash;

constexpr int kStNum = 101;

struct Train {
  FixedStr<20> train_id;
  int seat_num, station_num;
  FixedStr<30> stations[kStNum];
  int prices[kStNum];      // 累计价格。
  int arr_times[kStNum];   // 到达累计时间。
  int dept_times[kStNum];  // 出发累计时间。
  Time start_time;
  Date begin_date, end_date;
  char type;
  bool is_release;
};

class TrainManagement {
  BPlusTree<size_t, int, Train> trains;  // train_id -> train

 public:
  TrainManagement() : trains{"trains_index.bin", "trains.bin"} {}
  bool AddTrain(TokenScanner &token) {
    string key, train_id;
    while (!token.If_left()) {
      key = token.NextToken();
      if (key == "-i") {
        train_id = token.NextToken();
        
      } else if (key == "-n") {

      } else if (key == "-m") {
        
      } else if (key == "-s") {
        
      } else if (key == "-p") {
        
      } else if (key == "-x") {
        
      } else if (key == "-t") {
        
      } else if (key == "-o") {
        
      } else if (key == "-d") {
        
      } else if (key == "-y") {
        
      } else {
        throw Exception{"Invaild Argument!"};
      }
    }
  }
};

#endif  // TICKETSYSTEM_TRAINS_HPP_