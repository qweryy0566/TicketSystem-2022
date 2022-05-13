#ifndef TICKETSYSTEM_COMMANDS_HPP_
#define TICKETSYSTEM_COMMANDS_HPP_

#include <iostream>
#include "exceptions.hpp"
#include "logs.hpp"
#include "trains.hpp"
#include "users.hpp"
#include "utils.hpp"

class TicketSystem {
  UserManagement user_manager;
  TrainManagement train_manager;
  
 public:
  TicketSystem() {}

  bool Interprete(TokenScanner);
  void VisitAddUser(TokenScanner &);
  void VisitLogin(TokenScanner &);
  void VisitLogout(TokenScanner &);
  void VisitQueryProfile(TokenScanner &);
  void VisitModifyProfile(TokenScanner &);

};

bool TicketSystem::Interprete(TokenScanner token) {
  
}

void TicketSystem::VisitAddUser(TokenScanner &token) {

}
void TicketSystem::VisitLogin(TokenScanner &token) {

}
void TicketSystem::VisitLogout(TokenScanner &token) {

}
void TicketSystem::VisitQueryProfile(TokenScanner &token) {

}
void TicketSystem::VisitModifyProfile(TokenScanner &token) {

}

#endif  // TICKETSYSTEM_COMMANDS_HPP_