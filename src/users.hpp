#ifndef TICKETSYSTEM_USERS_HPP_
#define TICKETSYSTEM_USERS_HPP_

#include "../lib/bplustree.hpp"
#include "../lib/utils.hpp"

class User {
  FixedStr<20> username;  // 为了文件存储需要定长。
  FixedStr<30> password;
  FixedStr<15> name;
  FixedStr<30> mail_addr;
  int privilege;

 public:
  User(){};
  User(const string &username_, const string &password_, const string &name_,
       const string &mail_, const int &privilege_)
      : username{username_},
        password{password_},
        name{name_},
        mail_addr{mail_},
        privilege{privilege_} {}

  const string Username() const { return username.ToString(); }
  const string Password() const { return password.ToString(); }
  const string Name() const { return name.ToString(); }
  const string MailAddr() const { return mail_addr.ToString(); }
  const int Privilege() const { return privilege; }

  void ChangePassword(const string &password_) { password = password_; }
  void ChangeName(const string &name_) {name = name_; }
  void ChangeMailAddr(const string &mail_) { mail_addr = mail_; }
  void ChangePrivilege(const int &privilege_) { privilege = privilege_; }
};

class UserManagement {
  BPlusTree<FixedStr<20>, int, User> users;
  fqj::unordered_map<string, int> login;

 public:
  UserManagement() : users{"users_index.bin", "users.bin"} {}

  // 成功返回 1，失败返回 0.
  bool AddUser(const string &cur_username, const string &username,
               const string &password, const string &name,
               const string &mail_addr, int &privilege) {
    if (!users.Size())
      privilege = 10;
    else if (login.find(cur_username) == login.end() ||
             login[cur_username] <= privilege || users.Exist(username))
      return 0;
    User new_user{username, password, name, mail_addr, privilege};
    users.Insert(username, 0, new_user);
    return 1;
  }
  bool Login(const string &username, const string &password) {
    if (login.find(username) != login.end() || !users.Exist(username)) return 0;
    return login[username] = users.Get(username, 0).Privilege(), 1;
  }
  bool Logout(const string &username) { return login.erase(username); }
  string QueryProfile(const string &cur_username, const string &username) {
    if (login.find(cur_username) == login.end() || !users.Exist(username))
      return "-1";
    User target_user{users.Get(username, 0)};
    if (cur_username != username &&
        login[cur_username] <= target_user.Privilege())
      return "-1";
    return username + " " + target_user.Name() + " " + target_user.MailAddr() +
           " " + std::to_string(target_user.Privilege());
  }
  string ModifyProfile(const string &cur_username, const string &username,
                       const string &password, const string &name,
                       const string &mail_addr, int &privilege) {
    if (login.find(cur_username) == login.end() || !users.Exist(username))
      return "-1";
    User target_user{users.Get(username, 0)};
    if (cur_username != username &&
        login[cur_username] <= target_user.Privilege())
      return "-1";
    // 权限默认 -1.
    if (~privilege) {
      if (privilege >= login[cur_username]) return "-1";
      target_user.ChangePrivilege(privilege);
    }
    if (!password.empty()) target_user.ChangePassword(password);
    if (!name.empty()) target_user.ChangeName(name);
    if (!mail_addr.empty()) target_user.ChangeMailAddr(mail_addr);
    users.Modify(username, 0, target_user);
    return username + " " + target_user.Name() + " " + target_user.MailAddr() +
           " " + std::to_string(target_user.Privilege());
  }
};

#endif  // TICKETSYSTEM_USERS_HPP_