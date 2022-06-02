#ifndef TICKETSYSTEM_COMMANDS_HPP_
#define TICKETSYSTEM_COMMANDS_HPP_

#include "../lib/exceptions.hpp"
#include "../lib/utils.hpp"
#include "trains.hpp"
#include "users.hpp"

class TicketSystem {
  UserManagement user_manager;
  TrainManagement train_manager;

 public:
  string Interprete(TokenScanner);
  string VisitAddUser(TokenScanner &);
  string VisitLogin(TokenScanner &);
  string VisitLogout(TokenScanner &);
  string VisitQueryProfile(TokenScanner &);
  string VisitModifyProfile(TokenScanner &);
  string VisitAddTrain(TokenScanner &);
  string VisitDeleteTrain(TokenScanner &);
  string VisitReleaseTrain(TokenScanner &);
  string VisitQueryTrain(TokenScanner &);
  string VisitQueryTicket(TokenScanner &);
  string VisitQueryTransfer(TokenScanner &);
  string VisitBuyTicket(const int &, TokenScanner &);
  string VisitQueryOrder(TokenScanner &);
  string VisitRefundTicket(TokenScanner &);
  string VisitRollback(TokenScanner &);
  string VisitClean();
};

string TicketSystem::Interprete(TokenScanner token) {
  string key, ret;
  key = token.NextToken('[', ']');
  if (!key.length()) return "";
  int timestamp{std::stoi(key)};
  // TODO : timestamp && rollback
  // log_manager.AddLog(timestamp, token.Getleft());
  ret = "[" + key + "] ";
  key = token.NextToken();
  if (key == "exit") {
    ret += "bye";
  } else if (key == "add_user") {
    ret += VisitAddUser(token);
  } else if (key == "login") {
    ret += VisitLogin(token);
  } else if (key == "logout") {
    ret += VisitLogout(token);
  } else if (key == "query_profile") {
    ret += VisitQueryProfile(token);
  } else if (key == "modify_profile") {
    ret += VisitModifyProfile(token);
  } else if (key == "add_train") {
    ret += VisitAddTrain(token);
  } else if (key == "delete_train") {
    ret += VisitDeleteTrain(token);
  } else if (key == "release_train") {
    ret += VisitReleaseTrain(token);
  } else if (key == "query_train") {
    ret += VisitQueryTrain(token);
  } else if (key == "query_ticket") {
    ret += VisitQueryTicket(token);
  } else if (key == "query_transfer") {
    ret += VisitQueryTransfer(token);
  } else if (key == "buy_ticket") {
    ret += VisitBuyTicket(timestamp, token);
  } else if (key == "query_order") {
    ret += VisitQueryOrder(token);
  } else if (key == "refund_ticket") {
    ret += VisitRefundTicket(token);
  } else if (key == "rollback") {
    ret += VisitRollback(token);
  } else if (key == "clean") {
    ret += VisitClean();
  } else {
    throw Exception{string{"Invalid command! "} + '"' + key + '"'};
  }
  return ret;
}

// [N] add_user
string TicketSystem::VisitAddUser(TokenScanner &token) {
  string key, cur_username, username, password, name, mail_addr;
  int privilege;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-c")
      cur_username = token.NextToken();
    else if (key == "-u")
      username = token.NextToken();
    else if (key == "-p")
      password = token.NextToken();
    else if (key == "-n")
      name = token.NextToken();
    else if (key == "-m")
      mail_addr = token.NextToken();
    else if (key == "-g")
      privilege = std::stoi(token.NextToken());
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  return user_manager.AddUser(cur_username, username, password, name, mail_addr,
                              privilege)
             ? "0"
             : "-1";
}
// [F] login
string TicketSystem::VisitLogin(TokenScanner &token) {
  string key, username, password;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-u")
      username = token.NextToken();
    else if (key == "-p")
      password = token.NextToken();
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  return user_manager.Login(username, password) ? "0" : "-1";
}
// [F] logout
string TicketSystem::VisitLogout(TokenScanner &token) {
  string key, username;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-u")
      username = token.NextToken();
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  return user_manager.Logout(username) ? "0" : "-1";
}
// [SF] query_profile
string TicketSystem::VisitQueryProfile(TokenScanner &token) {
  string key, cur_username, username;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-c")
      cur_username = token.NextToken();
    else if (key == "-u")
      username = token.NextToken();
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  return user_manager.QueryProfile(cur_username, username);
}
string TicketSystem::VisitModifyProfile(TokenScanner &token) {
  string key, cur_username, username, password{""}, name{""}, mail_addr{""};
  int privilege{-1};
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-c")
      cur_username = token.NextToken();
    else if (key == "-u")
      username = token.NextToken();
    else if (key == "-p")
      password = token.NextToken();
    else if (key == "-n")
      name = token.NextToken();
    else if (key == "-m")
      mail_addr = token.NextToken();
    else if (key == "-g")
      privilege = std::stoi(token.NextToken());
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  return user_manager.ModifyProfile(cur_username, username, password, name,
                                    mail_addr, privilege);
}

// 将 token 直接传入管理类。
string TicketSystem::VisitAddTrain(TokenScanner &token) {
  return train_manager.AddTrain(token) ? "0" : "-1";
}
string TicketSystem::VisitDeleteTrain(TokenScanner &token) {
  string key, train_id;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-i")
      train_id = token.NextToken();
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  return train_manager.DeleteTrain(train_id) ? "0" : "-1";
}
string TicketSystem::VisitReleaseTrain(TokenScanner &token) {
  string key, train_id;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-i")
      train_id = token.NextToken();
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  return train_manager.ReleaseTrain(train_id) ? "0" : "-1";
}
string TicketSystem::VisitQueryTrain(TokenScanner &token) {
  Date date;
  string key, train_id;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-i")
      train_id = token.NextToken();
    else if (key == "-d")
      date = token.NextToken();
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  return train_manager.QueryTrain(date, train_id);
}
string TicketSystem::VisitQueryTicket(TokenScanner &token) {
  bool prior{0};
  string key, dept, arr;
  Date date;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-s")
      dept = token.NextToken();
    else if (key == "-t")
      arr = token.NextToken();
    else if (key == "-d")
      date = token.NextToken();
    else if (key == "-p")
      prior = token.NextToken() == "cost";
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  return train_manager.QueryTicket(dept, arr, date, prior);
}
string TicketSystem::VisitQueryTransfer(TokenScanner &token) {
  bool prior{0};
  string key, dept, arr;
  Date date;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-s")
      dept = token.NextToken();
    else if (key == "-t")
      arr = token.NextToken();
    else if (key == "-d")
      date = token.NextToken();
    else if (key == "-p")
      prior = token.NextToken() == "cost";
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  return train_manager.QueryTransfer(dept, arr, date, prior);
}
string TicketSystem::VisitBuyTicket(const int &timestamp, TokenScanner &token) {
  bool will_wait{0};
  string key, username, train_id, dept, arr;
  Date date;
  int cnt;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-u")
      username = token.NextToken();
    else if (key == "-i")
      train_id = token.NextToken();
    else if (key == "-d")
      date = token.NextToken();
    else if (key == "-n")
      cnt = std::stoi(token.NextToken());
    else if (key == "-f")
      dept = token.NextToken();
    else if (key == "-t")
      arr = token.NextToken();
    else if (key == "-q")
      will_wait = token.NextToken() == "true";
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  if (!user_manager.IsLogin(username)) return "-1";
  return train_manager.BuyTicket(username, train_id, date, dept, arr, cnt,
                                 will_wait, timestamp);
}
string TicketSystem::VisitQueryOrder(TokenScanner &token) {
  string key, username;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-u")
      username = token.NextToken();
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  if (!user_manager.IsLogin(username)) return "-1";
  return train_manager.QueryOrder(username);
}
string TicketSystem::VisitRefundTicket(TokenScanner &token) {
  string key, username;
  unsigned rank{1};
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-u")
      username = token.NextToken();
    else if (key == "-n")
      rank = std::stoi(token.NextToken());
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  if (!user_manager.IsLogin(username)) return "-1";
  return train_manager.RefundTicket(username, rank) ? "0" : "-1";
}
string TicketSystem::VisitRollback(TokenScanner &token) {
  string key;
  int timestamp;
  while (!token.If_left()) {
    key = token.NextToken();
    if (key == "-t")
      timestamp = std::stoi(token.NextToken());
    else
      throw Exception{string{"Invaild Argument! "} + '"' + key + '"'};
  }
  // TODO rollback
  return "0";
}
string TicketSystem::VisitClean() {
  user_manager.Clean();
  train_manager.Clean();
  return "0";
}

#endif  // TICKETSYSTEM_COMMANDS_HPP_