#ifndef __COMMON_PLAY_UTIL_H__
#define __COMMON_PLAY_UTIL_H__

#include "cocos2d.h"

cocos2d::Vec2 ConvertImageToPlay(cocos2d::Vec2 image_position);

void ProblemLoading(const char* filename);

struct HiddenPoint {
  float x;
  float y;
  int width;
  int height;
  bool found = false;
};

#endif
