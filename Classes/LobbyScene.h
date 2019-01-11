#ifndef __LOBBY_SCENE_H__
#define __LOBBY_SCENE_H__

#include "cocos2d.h"
#include "ui/UITextField.h"
#include "ui/UIText.h"
#include "ui/UIButton.h"
#include "ui/UIScrollView.h"
#include "SimpleAudioEngine.h"

using namespace cocos2d;
using namespace ui;
using namespace CocosDenshion;

class LobbyScene : public cocos2d::Scene
{
public:
  static cocos2d::Scene* createScene();

  virtual bool init();

  // a selector callback
  void menuCloseCallback(cocos2d::Ref* pSender);

  CREATE_FUNC(LobbyScene);

private:
  void CreateButton();


  Button single_play_button_;

  Node* ui_node_;
  Vec2 center_;
};

#endif // __LOBBY_SCENE_H__
