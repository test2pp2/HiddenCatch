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

  int hint_count() const;
  void set_hint_count(int hint_count);

  int stop_timer_count() const;
  void set_stop_timer_count(int stop_timer_count);

private:
  User();
  User(const User& user) = delete;
  User& operator=(const User& user) = delete;

  std::string user_id_;
  int hint_count_;
  int stop_timer_count_;
};

#endif
