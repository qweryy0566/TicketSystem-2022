#ifndef TICKETSYSTEM_USERS_HPP_
#define TICKETSYSTEM_USERS_HPP_

#include "../lib/bplustree.hpp"
#include "../lib/utils.hpp"

constexpr int kUserName = 21, kPassword = 31, kName = 16, kMail = 31;

class User {
  char username[kUserName];
  char password[kPassword];
  char name[kName];
  char mail_addr[kMail];
  int privilege;

 public:
  User(){};
  User(const string &username_, const string &password_, const string &name_,
       const string &mail_, const int &privilege_) {
    strcpy(username, username_.c_str());
    strcpy(password, password_.c_str());
    strcpy(name, name_.c_str());
    strcpy(mail_addr, mail_.c_str());
    privilege = privilege_;
  }

  const string Username() const { return username; }
  const string Password() const { return password; }
  const string Name() const { return name; }
  const string MailAddr() const { return mail_addr; }
  const int Privilege() const { return privilege; }

  void ChangePassword(const string &password_) {
    fqj::ModifyString(password, password_);
  }
  void ChangeName(const string &name_) {
    fqj::ModifyString(name, name_);
  }
  void ChangeMailAddr(const string &mail_) {
    fqj::ModifyString(mail_addr, mail_);
  }
  void ChangePrivilege(const int &privilege_) {
    privilege = privilege_;
  }
};

class UserManagement {
  BPlusTree<string, int, User> users;
  fqj::unordered_map<string, bool> is_login;

 public:
  UserManagement() : users{"users_index.bin", "users.bin"} {}

  string AddUser(TokenScanner &token) {
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
    if (users.Size() && is_login.find(cur_username) == is_login.end())
      return "-1";
    User cur = users.Get(cur_username, 0);
    if (cur.Privilege() <= privilege || users.Exist(username))
      return "-1";
    privilege = users.Size() ? privilege : 10;
    User new_user{username, password, name, mail_addr, privilege};
    users.Insert(username, 0, new_user);
    return "0";
  }
};

#endif  // TICKETSYSTEM_USERS_HPP_