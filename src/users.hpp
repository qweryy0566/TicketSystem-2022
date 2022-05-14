#ifndef TICKETSYSTEM_USERS_HPP_
#define TICKETSYSTEM_USERS_HPP_

const int kUserName = 21, kPassWord = 31, kName = 16, kMail = 31;

class User {
  char username[kUserName];
  char password[kPassWord];
  char name[kName];
  char mail_addr[kMail];
  int privilege;

 public:
  User() {};

};

class UserManagement {

};

#endif  // TICKETSYSTEM_USERS_HPP_