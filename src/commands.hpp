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
  string argv;
  argv = token.NextToken('[', ']');
  if (argv.empty()) return 0;
  log_manager.AddLog(std::stoi(argv), token.Getleft());

  argv = token.NextToken();
  if (argv == "exit") {
    return "bye";
  } else if (argv == "add_user") {
    return VisitAddUser(token);
  } else if (argv == "login") {
    return VisitLogin(token);
  } else if (argv == "logout") {
    return VisitLogout(token);
  } else if (argv == "query_profile") {
    return VisitQueryProfile(token);
  } else if (argv == "modify_profile") {
    return VisitModifyProfile(token);
  } else if (argv == "add_train") {
    return VisitAddTrain(token);
  } else if (argv == "delete_train") {
    return VisitDeleteTrain(token);
  } else if (argv == "release_train") {
    return VisitReleaseTrain(token);
  } else if (argv == "query_train") {
    return VisitQueryTrain(token);
  } else if (argv == "query_ticket") {
    return VisitQueryTicket(token);
  } else if (argv == "query_transfer") {
    return VisitQueryTransfer(token);
  } else if (argv == "buy_ticket") {
    return VisitBuyTicket(token);
  } else if (argv == "query_order") {
    return VisitQueryOrder(token);
  } else if (argv == "refund_ticket") {
    return VisitRefundTicket(token);
  } else if (argv == "rollback") {
    return VisitRollback(token);
  } else if (argv == "clean") {
    return VisitClean();
  } else {
    throw Exception{"Invalid command!"};
  }
}

// [N] add_user
string TicketSystem::VisitAddUser(TokenScanner &token) {
  string argv, cur_username, username, password, name, mail_addr;
  int privilege;
  while (!token.If_left()) {
    argv = token.NextToken();
    if (argv == "-c")
      cur_username = token.NextToken();
    else if (argv == "-u")
      username = token.NextToken();
    else if (argv == "-p")
      password = token.NextToken();
    else if (argv == "-n")
      name = token.NextToken();
    else if (argv == "-m")
      mail_addr = token.NextToken();
    else if (argv == "-g")
      privilege = std::stoi(token.NextToken());
    else
      throw Exception{"Invaild Argument!"};
  }
  return user_manager.AddUser(cur_username, username, password, name, mail_addr, privilege)
             ? "0"
             : "-1";
}
// [F] login
string TicketSystem::VisitLogin(TokenScanner &token) {
  string argv, username, password;
  while (!token.If_left()) {
    argv = token.NextToken();
    if (argv == "-u")
      username = token.NextToken();
    else if (argv == "-p")
      password = token.NextToken();
    else
      throw Exception{"Invaild Argument!"};
  }
  return user_manager.Login(username, password) ? "0" : "-1";
}
// [F] logout
string TicketSystem::VisitLogout(TokenScanner &token) {
  string argv, username;
  while (!token.If_left()) {
    argv = token.NextToken();
    if (argv == "-u")
      username = token.NextToken();
    else
      throw Exception{"Invaild Argument!"};
  }
  return user_manager.Logout(username) ? "0" : "-1";
}
// [SF] query_profile
string TicketSystem::VisitQueryProfile(TokenScanner &token) {
  string argv, cur_username, username;
  while (!token.If_left()) {
    argv = token.NextToken();
    if (argv == "-c")
      cur_username = token.NextToken();
    else if (argv == "-u")
      username = token.NextToken();
    else
      throw Exception{"Invaild Argument!"};
  }
  return user_manager.QueryProfile(cur_username, username);
}
string TicketSystem::VisitModifyProfile(TokenScanner &token) {
  string argv, cur_username, username, password{""}, name{""}, mail_addr{""};
  int privilege{-1};
  while (!token.If_left()) {
    argv = token.NextToken();
    if (argv == "-c")
      cur_username = token.NextToken();
    else if (argv == "-u")
      username = token.NextToken();
    else if (argv == "-p")
      password = token.NextToken();
    else if (argv == "-n")
      name = token.NextToken();
    else if (argv == "-m")
      mail_addr = token.NextToken();
    else if (argv == "-g")
      privilege = std::stoi(token.NextToken());
    else
      throw Exception{"Invaild Argument!"};
  }
  return user_manager.ModifyProfile(cur_username, username, password, name, mail_addr, privilege);
}
string TicketSystem::VisitAddTrain(TokenScanner &token) {
  
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