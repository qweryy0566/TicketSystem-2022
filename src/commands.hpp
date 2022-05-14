#ifndef TICKETSYSTEM_COMMANDS_HPP_
#define TICKETSYSTEM_COMMANDS_HPP_

#include <iostream>

#include "../lib/utils.hpp"
#include "exceptions.hpp"
#include "logs.hpp"
#include "trains.hpp"
#include "users.hpp"

class TicketSystem {
  UserManagement user_manager;
  TrainManagement train_manager;

 public:
  TicketSystem();

  bool Interprete(TokenScanner);
  void VisitAddUser(TokenScanner &);
  void VisitLogin(TokenScanner &);
  void VisitLogout(TokenScanner &);
  void VisitQueryProfile(TokenScanner &);
  void VisitModifyProfile(TokenScanner &);
};

TicketSystem::TicketSystem() {
  // TODO
}

bool TicketSystem::Interprete(TokenScanner token) {
  // TODO : timestamp
  token.NextToken('[', ']');
}

void TicketSystem::VisitAddUser(TokenScanner &token) {}
void TicketSystem::VisitLogin(TokenScanner &token) {}
void TicketSystem::VisitLogout(TokenScanner &token) {}
void TicketSystem::VisitQueryProfile(TokenScanner &token) {}
void TicketSystem::VisitModifyProfile(TokenScanner &token) {}

#endif  // TICKETSYSTEM_COMMANDS_HPP_