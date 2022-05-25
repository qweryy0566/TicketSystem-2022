#ifndef TICKETSYSTEM_TRAINS_HPP_
#define TICKETSYSTEM_TRAINS_HPP_

#include "../lib/bplustree.hpp"
#include "../lib/utils.hpp"

static StrHash<20> TrainIdHash;
static StrHash<30> StationHash;

constexpr int kStNum = 101;  // 0-base

struct Train {
  FixedStr<20> train_id;
  int seat_num, station_num;
  FixedStr<30> stations[kStNum];
  int prices[kStNum]{};      // 累计价格。
  int arr_times[kStNum]{};   // 到达累计时间。
  int dept_times[kStNum]{};  // 出发累计时间。
  Time start_time;
  Date begin_date, end_date;
  char type;
  bool is_release{0};
};

class TrainManagement {
  // trainid -> train
  BPlusTree<size_t, int, Train> trains;
  // 接下来的 BPT 均是在 release 时更改。
  // (stationid, trainid) -> trainid
  BPlusTree<size_t, size_t, size_t> station_trains;

 public:
  TrainManagement()
      : trains{"trains_index.bin", "trains.bin"},
        station_trains{"station_trains_index.bin", "station_trains.bin"} {}
  bool AddTrain(TokenScanner &token) {
    string key;
    Train new_train;
    size_t trainid;
    while (!token.If_left()) {
      key = token.NextToken();
      if (key == "-i") {
        new_train.train_id = token.NextToken();
        trainid = TrainIdHash(new_train.train_id);
        if (trains.Exist(trainid, 0)) return 0;
      } else if (key == "-n") {
        new_train.station_num = std::stoi(token.NextToken());
      } else if (key == "-m") {
        new_train.seat_num = std::stoi(token.NextToken());
      } else if (key == "-s") {
        TokenScanner tmp{token.NextToken()};
        for (int i = 0; !tmp.If_left(); ++i)
          new_train.stations[i] = tmp.NextToken('|');
      } else if (key == "-p") {
        TokenScanner tmp{token.NextToken()};
        for (int i = 1; !tmp.If_left(); ++i)
          new_train.prices[i] = new_train.prices[i - 1] + std::stoi(tmp.NextToken('|'));
      } else if (key == "-x") {
        new_train.start_time = token.NextToken();
      } else if (key == "-t") {
        // 暂时先存在 arr_times 中。
        TokenScanner tmp{token.NextToken()};
        for (int i = 1; !tmp.If_left(); ++i)
          new_train.arr_times[i] = std::stoi(tmp.NextToken('|'));
      } else if (key == "-o") {
        // 暂时先存在 dept_times 中。
        TokenScanner tmp{token.NextToken()};
        for (int i = 1; !tmp.If_left(); ++i) {
          key = tmp.NextToken('|');
          if (key != "_") new_train.dept_times[i] = std::stoi(key);
        }
      } else if (key == "-d") {
        TokenScanner tmp{token.NextToken()};
        new_train.begin_date = tmp.NextToken();
        new_train.end_date = tmp.NextToken();
      } else if (key == "-y") {
        new_train.type = token.NextToken().front();
      } else {
        throw Exception{"Invaild Argument!"};
      }
      // 接下来需要计算 arr_times 和 dept_times。
      for (int i = 1; i < new_train.station_num; ++i)
        new_train.arr_times[i] += new_train.dept_times[i - 1],
        new_train.dept_times[i] += new_train.arr_times[i];

      trains.Insert(trainid, 0, new_train);
    }
    return 1;
  }

  bool DeleteTrain(TokenScanner &token) {
    string key;
    size_t trainid;
    while (!token.If_left()) {
      key = token.NextToken();
      if (key == "-i")
        trainid = TrainIdHash(token.NextToken());
      else
        throw Exception{"Invaild Argument!"};
    }
    if (!trains.Exist(trainid) || trains.Get(trainid, 0).is_release) return 0;
    return trains.Delete(trainid, 0), 1;
  }

  bool ReleaseTrain(TokenScanner &token) {
    string key;
    size_t trainid;
    Train target_train;
    while (!token.If_left()) {
      key = token.NextToken();
      if (key == "-i")
        trainid = TrainIdHash(token.NextToken());
      else
        throw Exception{"Invaild Argument!"};
    }
    if (!trains.Exist(trainid)) return 0;
    target_train = trains.Get(trainid, 0);
    if (target_train.is_release) return 0;
    target_train.is_release = 1;
    trains.Modify(trainid, 0, target_train);
    // TODO : other BPT
    return 0;
  }
};

#endif  // TICKETSYSTEM_TRAINS_HPP_