#ifndef __CONST_HIDDEN_CATCH_H__
#define __CONST_HIDDEN_CATCH_H__

const float kTimerSpriteWidth = 40.0f; 
const float kBottomSpriteHeight = 128.0f; // =>130.0f

const float kImageWidth = 940.0f; 
const float kImageHeight = 950.0f;

// Timer = 34 x 940
// ImageSize = 940 x 950

static const std::string NormalFont = "fonts/BMDOHYEON.ttf";
//static const std::string WebServerUrl = "http://192.168.0.166:3000";
static const std::string WebServerUrl = "http://127.0.0.1:3000";

#define ccsf2(...) CCString::createWithFormat(__VA_ARGS__)->getCString()

#endif

