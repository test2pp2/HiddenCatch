#ifndef __USER_H__
#define __USER_H__
#include <string>

class User {
public:
  static int GetLastStageId();
  static void SetLastStageId(int stage_id);

  static User& Instance() {
    static User instance;
    return instance;
  }

  std::string user_id() const;
  void set_user_id(std::string user_id);

private:
  User();
  User(const User& user) = delete;
  User& operator=(const User& user) = delete;

  std::string user_id_;
};

#endif
