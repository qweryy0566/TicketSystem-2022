#ifndef TICKETSYSTEM_TRAINS_HPP_
#define TICKETSYSTEM_TRAINS_HPP_

#ifndef NO_BPT_
#include "../lib/bplustree.hpp"
#else
#include "../lib/test_logic.hpp"
#endif
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

struct StationTrain {
  FixedStr<20> train_id;
  Time arr_time, dept_time;
  int price;
};
class TicketTrain {
  // 维护每个站区间内的票数。
  int station_num, tr[1 << 8], ltg[1 << 8];
  static int ql, qr, qv;
  void pushdown(int o) {
    tr[o << 1] += ltg[o], ltg[o << 1] += ltg[o];
    tr[o << 1 | 1] += ltg[o], ltg[o << 1 | 1] += ltg[o];
    ltg[o] = 0;
  }
  void pushup(int o) { tr[o] = std::min(tr[o << 1], tr[o << 1 | 1]); }
  void build(int o, int l, int r) {
    if (l == r) {
      tr[o] = qv;
      return;
    }
    int mid{l + r >> 1};
    build(o << 1, l, mid), build(o << 1 | 1, mid + 1, r);
    pushup(o);
  }
  void update(int o, int l, int r) {
    if (ql <= l && r <= qr) {
      tr[o] += qv, ltg[o] += qv;
      return;
    }
    pushdown(o);
    int mid{l + r >> 1};
    if (ql <= mid) update(o << 1, l, mid);
    if (qr > mid) update(o << 1 | 1, mid + 1, r);
    pushup(o);
  }
  int query(int o, int l, int r) {
    if (ql <= l && r <= qr) return tr[o];
    pushdown(o);
    int mid{l + r >> 1}, ansl{INT32_MAX}, ansr{INT32_MAX};
    if (ql <= mid) ansl = query(o << 1, l, mid);
    if (qr > mid) ansr = query(o << 1 | 1, mid + 1, r);
    return std::min(ansl, ansr);
  }

 public:
  TicketTrain() = default;
  TicketTrain(const int &num, const int &seat) : station_num{num} {
    qv = seat, build(1, 0, station_num - 2);  // 管该站到下一站的票数。
  }
  int QueryTicket(int l, int r) {
    return ql = l, qr = r, query(1, 0, station_num - 2);
  }
  void BuyATicket(int l, int r, int v = -1) {
    ql = l, qr = r, qv = v, update(1, 0, station_num - 2);
  }
};

int TicketTrain::ql, TicketTrain::qr, TicketTrain::qv;

class TrainManagement {
  // trainid -> train
  BPlusTree<size_t, int, Train> trains;
  // 接下来的 BPT 均是在 release 时更改。
  // (stationid, trainid) -> station_train
  BPlusTree<size_t, size_t, StationTrain> station_trains;
  // (trainid, date) -> ticket_of_train
  BPlusTree<size_t, Date, TicketTrain> ticket_trains;

 public:
  TrainManagement()
      : trains{"trains_index.bin", "trains.bin"},
        station_trains{"station_trains_index.bin", "station_trains.bin"},
        ticket_trains{"ticket_trains_index.bin", "ticket_trains.bin"} {}
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
          new_train.prices[i] =
              new_train.prices[i - 1] + std::stoi(tmp.NextToken('|'));
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
        new_train.begin_date = tmp.NextToken('|');
        new_train.end_date = tmp.NextToken('|');
      } else if (key == "-y") {
        new_train.type = token.NextToken().front();
      } else {
        throw Exception{"Invaild Argument!"};
      }
    }
    // 接下来需要计算 arr_times 和 dept_times。
    for (int i = 1; i < new_train.station_num; ++i)
      new_train.arr_times[i] += new_train.dept_times[i - 1],
          new_train.dept_times[i] += new_train.arr_times[i];
    trains.Insert(trainid, 0, new_train);
    return 1;
  }

  bool DeleteTrain(const string &train_id) {
    size_t trainid{TrainIdHash(train_id)};
    if (!trains.Exist(trainid) || trains.Get(trainid, 0).is_release) return 0;
    return trains.Delete(trainid, 0), 1;
  }

  bool ReleaseTrain(const string &train_id) {
    size_t trainid{TrainIdHash(train_id)};
    if (!trains.Exist(trainid)) return 0;
    Train the_train{trains.Get(trainid, 0)};
    if (the_train.is_release) return 0;
    the_train.is_release = 1;
    trains.Modify(trainid, 0, the_train);
    // TODO : other BPT
    for (Date i = the_train.begin_date; i <= the_train.end_date; ++i)
      ticket_trains.Insert(
          trainid, i, TicketTrain{the_train.station_num, the_train.seat_num});
    for (int i = 0; i < the_train.station_num; ++i) {
      station_trains.Insert(
          StationHash(the_train.stations[i]), trainid,
          StationTrain{the_train.train_id,
                       the_train.start_time + the_train.arr_times[i],
                       the_train.start_time + the_train.dept_times[i],
                       the_train.prices[i]});
    }
    return 1;
  }
  string QueryTrain(const Date &date, const string &train_id) {
    string ret{"-1"};
    size_t trainid{TrainIdHash(train_id)};
    Train the_train;
    if (trains.Exist(trainid)) {
      the_train = trains.Get(trainid, 0);
      if (date < the_train.begin_date || date > the_train.end_date)
        return ret;  // 日期不在范围内。
      ret = train_id + ' ' + the_train.type + '\n';
      Time dept{the_train.start_time};
      TicketTrain ticket;
      if (the_train.is_release) ticket = ticket_trains.Get(trainid, date);
      for (int i = 0; i < the_train.station_num; ++i) {
        ret += string(the_train.stations[i]) + ' ';
        if (i)
          ret += DateTime{date, dept + the_train.arr_times[i]}, ret += ' ';
        else
          ret += "xx-xx xx:xx ";
        ret += "-> ";
        if (i + 1 < the_train.station_num)
          ret += DateTime{date, dept + the_train.dept_times[i]}, ret += ' ';
        else
          ret += "xx-xx xx:xx ";
        ret += std::to_string(the_train.prices[i]) + ' ';
        if (i + 1 == the_train.station_num)
          ret += 'x';
        else if (the_train.is_release)
          ret += std::to_string(ticket.QueryTicket(i, i)) + '\n';
        else
          ret += std::to_string(the_train.seat_num) + '\n';
      }
    }
    return ret;
  }
  // prior 默认为 0，表示 time，否则为 cost.
  string QueryTicket(const string &dept, const string &arr, const Date &date,
                     const bool &prior) {
    string ret{"0"};
    size_t deptid{StationHash(dept)}, arrid{StationHash(arr)};

  }
};

#endif  // TICKETSYSTEM_TRAINS_HPP_