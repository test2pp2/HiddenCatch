#ifndef __CONST_HIDDEN_CATCH_H__
#define __CONST_HIDDEN_CATCH_H__

#define ccsf2(...) CCString::createWithFormat(__VA_ARGS__)->getCString()

const float kTimerSpriteWidth = 40.0f; 
const float kBottomSpriteHeight = 138.0f;

const float kTimerOutlinerHeight = 16.0f;

const float kMiddleSpace = 6.0f;

const float kImageWidth = (1920.0f / 2.0f) - kMiddleSpace / 2.0f; // 957.0f
const float kImageHeight = 940.0f - 16.0f;                        // 924.0f;

// Timer = 34 x 940
// ImageSize = 957 x 924

static const std::string NormalFont = "fonts/BMDOHYEON.ttf";

//static const std::string WebServerUrl = "http://192.168.0.166:3000";
static const std::string WebServerUrl = "http://127.0.0.1:3000";


#endif

// 957 * 2 + 6