#ifndef TICKETSYSTEM_USERS_HPP_
#define TICKETSYSTEM_USERS_HPP_

#ifndef NO_BPT_
#include "../lib/bplustree.hpp"
#else
#include "../lib/test_logic.hpp"
#endif
#include "../lib/utils.hpp"

class User {
  FixedStr<20> username;  // 为了文件存储需要定长。
  FixedStr<30> password;
  FixedStr<20> name;
  FixedStr<30> mail_addr;
  int privilege;

 public:
  User() : privilege{-1} {};
  User(const string &username_, const string &password_, const string &name_,
       const string &mail_, const int &privilege_)
      : username{username_},
        password{password_},
        name{name_},
        mail_addr{mail_},
        privilege{privilege_} {}

  const string Username() const { return string(username); }
  const string Password() const { return string(password); }
  const string Name() const { return string(name); }
  const string MailAddr() const { return string(mail_addr); }
  const int Privilege() const { return privilege; }

  void ChangePassword(const string &password_) { password = password_; }
  void ChangeName(const string &name_) { name = name_; }
  void ChangeMailAddr(const string &mail_) { mail_addr = mail_; }
  void ChangePrivilege(const int &privilege_) { privilege = privilege_; }
};

class UserManagement {
  BPlusTree<size_t, int, User, 339, 67> users;  // username_hash -> user
  unordered_map<string, int> login;

 public:
  UserManagement() : users{"users_index.bin", "users.bin"} {}

  bool IsLogin(const string &username) const {
    return login.find(username) != login.cend();
  }
  // 成功返回 1，失败返回 0.
  bool AddUser(const string &cur_username, const string &username,
               const string &password, const string &name,
               const string &mail_addr, int &privilege) {
    size_t userid = UserNameHash(username);
    if (!users.Size())
      privilege = 10;
    else {
      if (!IsLogin(cur_username))
        throw Exception{"当前用户未登录。"};
      if (login[cur_username] <= privilege)
        throw Exception{"创建用户权限不足。"};
      if (users.Exist(userid))
        throw Exception{"用户已存在。"};
      return 0;
    }
    User new_user{username, password, name, mail_addr, privilege};
    users.Insert(userid, 0, new_user);
    return 1;
  }
  bool Login(const string &username, const string &password) {
    size_t userid = UserNameHash(username);
    if (IsLogin(username))
      throw Exception{"用户已登录。"};
    User target_user{users.Get(userid, 0)};
    if (!~target_user.Privilege())
      throw Exception{"用户名不存在。"};
    if (password != target_user.Password())
      throw Exception{"密码错误。"};
    return login[username] = target_user.Privilege(), 1;
  }
  bool Logout(const string &username) { return login.erase(username); }
  string QueryProfile(const string &cur_username, const string &username) {
    size_t userid = UserNameHash(username);
    if (!IsLogin(cur_username))
      throw Exception{"当前用户未登录。"};
    User target_user{users.Get(userid, 0)};
    if (!~target_user.Privilege() ||
        cur_username != username &&
            login[cur_username] <= target_user.Privilege())
      return "-1";
    return username + " " + target_user.Name() + " " + target_user.MailAddr() +
           " " + std::to_string(target_user.Privilege());
  }
  string ModifyProfile(const string &cur_username, const string &username,
                       const string &password, const string &name,
                       const string &mail_addr, int &privilege) {
    size_t userid = UserNameHash(username);
    if (!IsLogin(cur_username))
      throw Exception{"当前用户未登录。"};
    User target_user{users.Get(userid, 0)};
    if (!~target_user.Privilege() ||
        cur_username != username &&
            login[cur_username] <= target_user.Privilege())
      return "-1";
    // 权限默认 -1.
    if (~privilege) {
      if (privilege >= login[cur_username])
        throw Exception{"权限不足。"};
      target_user.ChangePrivilege(privilege);
      if (IsLogin(username)) login[username] = privilege;
    }
    if (!password.empty()) target_user.ChangePassword(password);
    if (!name.empty()) target_user.ChangeName(name);
    if (!mail_addr.empty()) target_user.ChangeMailAddr(mail_addr);
    users.Modify(userid, 0, target_user);
    return username + " " + target_user.Name() + " " + target_user.MailAddr() +
           " " + std::to_string(target_user.Privilege());
  }

  void Clean() {
    users.Clear();
    login.clear();
  }
};

#endif  // TICKETSYSTEM_USERS_HPP_