#ifndef TICKETSYSTEM_TRAINS_HPP_
#define TICKETSYSTEM_TRAINS_HPP_

#ifndef NO_BPT_
#include "../lib/bplustree.hpp"
#else
#include "../lib/test_logic.hpp"
#endif
#include "../lib/utils.hpp"

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
  size_t trainid;
  FixedStr<20> train_id;
  Time arr_time, dept_time;  // 里面存的是相对起点站发车的时间。
  int order, price;
};
class TicketTrain {
  // 维护每个站区间内的票数。
  int tr[1 << 8], ltg[1 << 8];
  static int ql, qr, qv;
  void pushdown(int o) {
    tr[o << 1] += ltg[o], ltg[o << 1] += ltg[o];
    tr[o << 1 | 1] += ltg[o], ltg[o << 1 | 1] += ltg[o];
    ltg[o] = 0;
  }
  void pushup(int o) { tr[o] = std::min(tr[o << 1], tr[o << 1 | 1]); }
  void build(int o, int l, int r) {
    if (l == r) {
      tr[o] = seat;
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
  int station_num, seat;
  TicketTrain() = default;
  TicketTrain(const int &num, const int &seat) : station_num{num}, seat{seat} {
    build(1, 0, station_num - 2);  // 管该站到下一站的票数。
  }
  int QueryTicket(int l, int r) {
    return ql = l, qr = r, query(1, 0, station_num - 2);
  }
  void BuyTickets(int l, int r, int v) {
    ql = l, qr = r, qv = v, update(1, 0, station_num - 2);
  }
};
int TicketTrain::ql, TicketTrain::qr, TicketTrain::qv;

struct TicketResult {
  FixedStr<20> train_id;
  DateTime dept_time, arr_time;
  int time, price, seat;

  static bool CmpTime(const TicketResult &lhs, const TicketResult &rhs) {
    if (lhs.time == rhs.time) return lhs.train_id < rhs.train_id;
    return lhs.time < rhs.time;
  }
  static bool CmpCost(const TicketResult &lhs, const TicketResult &rhs) {
    if (lhs.price == rhs.price) return lhs.train_id < rhs.train_id;
    return lhs.price < rhs.price;
  }
};

static bool (*ResultCmp[2])(const TicketResult &, const TicketResult &){
    TicketResult::CmpTime, TicketResult::CmpCost};

struct Order {
  enum Status { SUCCESS, PENDING, REFUNDED };
  Status status;
  int timestamp, price, cnt;
  FixedStr<20> username, train_id;
  FixedStr<30> dept, arr;
  DateTime dept_time, arr_time;

  Order() {}
  Order(const int &stamp, const int &price, const int &cnt,
        const string &username, const string &train_id, const string &dept,
        const string &arr, const DateTime &s_time, const DateTime &t_time)
      : timestamp{stamp},
        price{price},
        cnt{cnt},
        username{username},
        train_id{train_id},
        dept{dept},
        arr{arr},
        dept_time{s_time},
        arr_time{t_time} {}
  explicit operator string() const {
    string status_str;
    if (status == SUCCESS)
      status_str = "[success]";
    else if (status == PENDING)
      status_str = "[pending]";
    else
      status_str = "[refunded]";
    return status_str + ' ' + string(train_id) + ' ' + string(dept) + ' ' +
           string(dept_time) + " -> " + string(arr) + ' ' + string(arr_time) +
           ' ' + std::to_string(price) + ' ' + std::to_string(cnt);
  }
};

class TrainManagement {
  // trainid -> train
  BPlusTree<size_t, int, Train> trains;
  // 接下来的 BPT 均是在 release 时更改。
  // (stationid, trainid) -> station_train
  BPlusTree<size_t, size_t, StationTrain> station_trains;
  // (trainid, date) -> ticket_of_train
  BPlusTree<size_t, Date, TicketTrain> ticket_trains;
  // (userid, -timestamp) -> order
  BPlusTree<size_t, int, Order> orders;
  // (<trainid, date>, -timestamp) -> order
  BPlusTree<std::pair<size_t, Date>, int, Order> pending_orders;

 public:
  TrainManagement()
      : trains{"trains_index.bin", "trains.bin"},
        station_trains{"station_trains_index.bin", "station_trains.bin"},
        ticket_trains{"ticket_trains_index.bin", "ticket_trains.bin"},
        orders{"orders_index.bin", "orders.bin"},
        pending_orders{"pending_index.bin", "pending.bin"} {}
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
    for (Date i = the_train.begin_date; i <= the_train.end_date; ++i)
      ticket_trains.Insert(
          trainid, i, TicketTrain{the_train.station_num, the_train.seat_num});
    for (int i = 0; i < the_train.station_num; ++i) {
      station_trains.Insert(
          StationHash(the_train.stations[i]), trainid,
          StationTrain{trainid, the_train.train_id,
                       the_train.start_time + the_train.arr_times[i],
                       the_train.start_time + the_train.dept_times[i], i,
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
          ret += string(DateTime{date, dept + the_train.arr_times[i]}) + ' ';
        else
          ret += "xx-xx xx:xx ";
        ret += "-> ";
        if (i + 1 < the_train.station_num)
          ret += string(DateTime{date, dept + the_train.dept_times[i]}) + ' ';
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
    string ret;
    size_t deptid{StationHash(dept)}, arrid{StationHash(arr)}, trainid;
    vector<StationTrain> dept_trains{station_trains.Traverse(deptid)};
    vector<StationTrain> arr_trains{station_trains.Traverse(arrid)};
    vector<TicketResult> result;
    for (auto s_it = dept_trains.begin(), t_it = arr_trains.begin();
         s_it != dept_trains.end() && t_it != arr_trains.end(); ++s_it) {
      while (t_it != arr_trains.end() && t_it->trainid < s_it->trainid) ++t_it;
      if (s_it->train_id == t_it->train_id && s_it->order < t_it->order) {
        trainid = s_it->trainid;
        Date dept_date{date - s_it->dept_time.day};
        if (ticket_trains.Exist(trainid, dept_date)) {
          TicketTrain ticket{ticket_trains.Get(trainid, dept_date)};
          result.push_back({s_it->train_id,
                            DateTime{dept_date, s_it->dept_time},
                            DateTime{dept_date, t_it->arr_time},
                            int(t_it->arr_time) - int(s_it->dept_time),
                            t_it->price - s_it->price,
                            ticket.QueryTicket(s_it->order, t_it->order - 1)});
        }
      }
    }
    fqj::Qsort(result.begin(), result.end(), ResultCmp[prior]);
    ret = std::to_string(result.size());
    for (auto it = result.begin(); it != result.end(); ++it)
      ret += '\n' + string(it->train_id) + ' ' + dept + ' ' +
             string(it->dept_time) + " -> " + arr + ' ' + string(it->arr_time) +
             ' ' + std::to_string(it->price) + ' ' + std::to_string(it->seat);
    return ret;
  }
  string QueryTransfer(const string &dept, const string &arr, const Date &date,
                       const bool &prior) {
    string ret{"0"}, mid_str;
    Time s_time, t_time;
    Date s_dept, t_dept;
    DateTime mid_s, mid_t;
    size_t deptid{StationHash(dept)}, arrid{StationHash(arr)};
    vector<StationTrain> dept_trains{station_trains.Traverse(deptid)};
    vector<StationTrain> arr_trains{station_trains.Traverse(arrid)};
    TicketResult train1, train2;
    int the_time{INT32_MAX}, the_cost{INT32_MAX}, cur_cost, cur_time;

    for (auto s_it : dept_trains) {
      s_dept = date - s_it.dept_time.day;
      if (!ticket_trains.Exist(s_it.trainid, s_dept)) continue;
      Train s_train{trains.Get(s_it.trainid, 0)}, t_train;
      TicketTrain s_ticket{ticket_trains.Get(s_it.trainid, s_dept)};
      unordered_map<string, int> stat_order;
      for (int i = s_it.order + 1; i < s_train.station_num; ++i)
        stat_order[string(s_train.stations[i])] = i;
      for (auto t_it : arr_trains) {
        if (s_it.train_id == t_it.train_id) continue;
        t_train = trains.Get(t_it.trainid, 0);  // TODO : 可能会 TLE.
        for (int i = 0; i < t_it.order; ++i) {
          string trans{string(t_train.stations[i])};
          if (stat_order.find(trans) == stat_order.end()) continue;
          s_time = s_train.start_time + s_train.arr_times[stat_order[trans]];
          mid_s = {s_dept, s_time};
          // 接下来算出下一班车的理论最早发车日期。
          t_time = t_train.start_time + t_train.dept_times[i];
          t_dept = mid_s.date - t_time.day + (t_time < s_time);
          // 时间的小于号不看 day.
          t_dept = std::max(t_dept, t_train.begin_date);
          if (t_dept > t_train.end_date) continue;
          TicketTrain t_ticket{ticket_trains.Get(t_it.trainid, t_dept)};

          mid_t = {t_dept, t_time};
          cur_cost = s_train.prices[stat_order[trans]] - s_it.price +
                     t_it.price - t_train.prices[i];
          cur_time = s_train.arr_times[stat_order[trans]] -
                     s_train.dept_times[s_it.order] +
                     t_train.arr_times[t_it.order] - t_train.dept_times[i] +
                     (mid_t - mid_s);
          bool is_better{0};
          if (!prior) {
            if (cur_time == the_time)
              if (cur_cost == the_cost)
                if (train1.train_id == s_it.train_id)
                  is_better = t_it.train_id < train2.train_id;
                else
                  is_better = s_it.train_id < train1.train_id;
              else
                is_better = cur_cost < the_cost;
            else
              is_better = cur_time < the_time;
          } else {
            if (cur_cost == the_cost)
              if (cur_time == the_time)
                if (train1.train_id == s_it.train_id)
                  is_better = t_it.train_id < train2.train_id;
                else
                  is_better = s_it.train_id < train1.train_id;
              else
                is_better = cur_time < the_time;
            else
              is_better = cur_cost < the_cost;
          }

          if (is_better) {
            the_time = cur_time, the_cost = cur_cost;
            mid_str = trans;
            train1 = {s_it.train_id,
                      DateTime{s_dept, s_it.dept_time},
                      mid_s,
                      0,  // 没有必要计算时间。
                      s_train.prices[stat_order[trans]] - s_it.price,
                      s_ticket.QueryTicket(s_it.order, stat_order[trans] - 1)};
            train2 = {t_it.train_id,
                      mid_t,
                      DateTime{t_dept, t_it.arr_time},
                      0,  // 没有必要计算时间。
                      t_it.price - t_train.prices[i],
                      t_ticket.QueryTicket(i, t_it.order - 1)};
          }
        }
      }
    }

    if (mid_str.length()) {
      ret = string(train1.train_id) + ' ' + dept + ' ' +
            string(train1.dept_time) + " -> " + mid_str + ' ' +
            string(train1.arr_time) + ' ' + std::to_string(train1.price) + ' ' +
            std::to_string(train1.seat);
      ret += '\n' + string(train2.train_id) + ' ' + mid_str + ' ' +
             string(train2.dept_time) + " -> " + arr + ' ' +
             string(train2.arr_time) + ' ' + std::to_string(train2.price) +
             ' ' + std::to_string(train2.seat);
    }
    return ret;
  }
  // 用户已登录。
  string BuyTicket(const string &username, const string &train_id,
                   const Date &date, const string &dept, const string &arr,
                   const int &cnt, const bool &will_wait,
                   const int &timestamp) {
    string ret;
    size_t trainid{TrainIdHash(train_id)};
    size_t deptid{StationHash(dept)}, arrid{StationHash(arr)};
    StationTrain s_it, t_it;
    if (!station_trains.Exist(deptid, trainid) ||
        !station_trains.Exist(arrid, trainid))
      return "-1";  // 蕴含判断发布。
    s_it = station_trains.Get(deptid, trainid);
    t_it = station_trains.Get(arrid, trainid);
    if (s_it.order >= t_it.order) return "-1";
    Date dept_date{date};
    if (!ticket_trains.Exist(trainid, dept_date)) return "-1";
    TicketTrain ticket{ticket_trains.Get(trainid, dept_date)};
    if (ticket.seat < cnt) return "-1";
    int left{ticket.QueryTicket(s_it.order, t_it.order - 1)};
    if (left < cnt && !will_wait) return "-1";
    Order the_order{timestamp,
                    t_it.price - s_it.price,
                    cnt,
                    username,
                    train_id,
                    dept,
                    arr,
                    DateTime{dept_date, s_it.dept_time},
                    DateTime{dept_date, t_it.arr_time}};
    if (left >= cnt) {
      ticket.BuyTickets(s_it.order, t_it.order - 1, cnt);
      ticket_trains.Modify(trainid, dept_date, ticket);
      the_order.status = Order::SUCCESS;
      ret = std::to_string(1ll * cnt * (t_it.price - s_it.price));
    } else {
      // 注意：候补也要加入订单记录内。
      the_order.status = Order::PENDING;
      pending_orders.Insert({trainid, dept_date}, -timestamp, the_order);
      ret = "queue";
    }
    orders.Insert(UserNameHash(username), -timestamp, the_order);
    return ret;
  }
  string QueryOrder(const string &username) {
    string ret;
    vector<Order> user_orders{orders.Traverse(UserNameHash(username))};
    ret = std::to_string(user_orders.size());
    for (auto it : user_orders) ret += '\n' + string(it);
    return ret;
  }
  string RefundTicket(const string &username, const int &rank) {
    string ret;
    vector<Order> user_orders{orders.Traverse(UserNameHash(username))};
    
  }
};

#endif  // TICKETSYSTEM_TRAINS_HPP_