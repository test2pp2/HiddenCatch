#include "User.h"
#include "base/CCUserDefault.h"

int User::GetLastStageId() {
  return cocos2d::UserDefault::getInstance()->getIntegerForKey("StageId", 0);
}

void User::SetLastStageId(int stage_id) {
  return cocos2d::UserDefault::getInstance()->setIntegerForKey("StageId", stage_id);
}

User::User() {

}

std::string User::user_id() const {
  return user_id_;
}

void User::set_user_id(std::string user_id) {
  user_id_ = user_id;
}
