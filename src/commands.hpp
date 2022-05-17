#ifndef TICKETSYSTEM_COMMANDS_HPP_
#define TICKETSYSTEM_COMMANDS_HPP_

#include "../lib/exceptions.hpp"
#include "../lib/utils.hpp"
#include "logs.hpp"
#include "trains.hpp"
#include "users.hpp"

class TicketSystem {
  UserManagement user_manager;
  TrainManagement train_manager;
  LogManagement log_manager;

 public:
  TicketSystem();

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
  string VisitBuyTicket(TokenScanner &);
  string VisitQueryOrder(TokenScanner &);
  string VisitRefundTicket(TokenScanner &);
  string VisitRollback(TokenScanner &);
  string VisitClean();
};

TicketSystem::TicketSystem() {
  // TODO
}

string TicketSystem::Interprete(TokenScanner token) {
  // TODO : timestamp
  string key;
  key = token.NextToken('[', ']');
  if (key.empty()) return 0;
  log_manager.AddLog(std::stoi(key), token.Getleft());

  key = token.NextToken();
  if (key == "exit") {
    return "bye";
  } else if (key == "add_user") {
    return VisitAddUser(token);
  } else if (key == "login") {
    return VisitLogin(token);
  } else if (key == "logout") {
    return VisitLogout(token);
  } else if (key == "query_profile") {
    return VisitQueryProfile(token);
  } else if (key == "modify_profile") {
    return VisitModifyProfile(token);
  } else if (key == "add_train") {
    return VisitAddTrain(token);
  } else if (key == "delete_train") {
    return VisitDeleteTrain(token);
  } else if (key == "release_train") {
    return VisitReleaseTrain(token);
  } else if (key == "query_train") {
    return VisitQueryTrain(token);
  } else if (key == "query_ticket") {
    return VisitQueryTicket(token);
  } else if (key == "query_transfer") {
    return VisitQueryTransfer(token);
  } else if (key == "buy_ticket") {
    return VisitBuyTicket(token);
  } else if (key == "query_order") {
    return VisitQueryOrder(token);
  } else if (key == "refund_ticket") {
    return VisitRefundTicket(token);
  } else if (key == "rollback") {
    return VisitRollback(token);
  } else if (key == "clean") {
    return VisitClean();
  } else {
    throw Exception{"Invalid command!"};
  }
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
      throw Exception{"Invaild Argument!"};
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
      throw Exception{"Invaild Argument!"};
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
      throw Exception{"Invaild Argument!"};
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
      throw Exception{"Invaild Argument!"};
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
      throw Exception{"Invaild Argument!"};
  }
  return user_manager.ModifyProfile(cur_username, username, password, name,
                                    mail_addr, privilege);
}
string TicketSystem::VisitAddTrain(TokenScanner &token) {
  return train_manager.AddTrain(token) ? "0" : "-1";
}
string TicketSystem::VisitDeleteTrain(TokenScanner &token) {}
string TicketSystem::VisitReleaseTrain(TokenScanner &token) {}
string TicketSystem::VisitQueryTrain(TokenScanner &token) {}
string TicketSystem::VisitQueryTicket(TokenScanner &token) {}
string TicketSystem::VisitQueryTransfer(TokenScanner &token) {}
string TicketSystem::VisitBuyTicket(TokenScanner &token) {}
string TicketSystem::VisitQueryOrder(TokenScanner &token) {}
string TicketSystem::VisitRefundTicket(TokenScanner &token) {}
string TicketSystem::VisitRollback(TokenScanner &token) {}
string TicketSystem::VisitClean() {}

#endif  // TICKETSYSTEM_COMMANDS_HPP_