#include "User.h"
#include "base/CCUserDefault.h"

int User::GetLastStageId() {
  return cocos2d::UserDefault::getInstance()->getIntegerForKey("StageId", 0);
}

void User::SetLastStageId(int stage_id) {
  return cocos2d::UserDefault::getInstance()->setIntegerForKey("StageId", stage_id);
}
