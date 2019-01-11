#include "LobbyScene.h"
#include "CommonPlayUtil.h"
#include "SinglePlayScene.h"

Scene* LobbyScene::createScene() {
  return LobbyScene::create();
}

bool LobbyScene::init() {
  //////////////////////////////
  // 1. super init first
  if (!Scene::init()) {
    return false;
  }

  const auto visibleSize = Director::getInstance()->getVisibleSize();
  const Vec2 origin = Director::getInstance()->getVisibleOrigin();
  center_ = Vec2(visibleSize.width / 2.0f + origin.x, visibleSize.height / 2.0f + origin.y);

  ui_node_ = Node::create();
  addChild(ui_node_, 1);

  auto pink_pencil_sprite = Sprite::create("sprites/PinkPencil.png");
  if (pink_pencil_sprite == nullptr) {
    ProblemLoading("'sprites/PinkPencil.png'");
  } else {
    pink_pencil_sprite->setScale(0.8f);
    pink_pencil_sprite->setPosition(center_.x - 400, center_.y + 200);
    ui_node_->addChild(pink_pencil_sprite, 0);
  }

  auto blue_pencil_sprite = Sprite::create("sprites/BluePencil.png");
  if (blue_pencil_sprite == nullptr) {
    ProblemLoading("'sprites/BluePencil.png'");
  } else {
    blue_pencil_sprite->setScale(0.8f);
    blue_pencil_sprite->setPosition(center_.x + 400, center_.y + 200);
    ui_node_->addChild(blue_pencil_sprite, 0);
  }

  auto sun_sprite = Sprite::create("sprites/Sun.png");
  if (sun_sprite == nullptr) {
    ProblemLoading("'sprites/Sun.png'");
  } else {
    sun_sprite->setScale(0.07f);
    sun_sprite->setPosition(center_.x, center_.y + 100);
    ui_node_->addChild(sun_sprite, 0);
  }

  const auto sun_rotation_action = RotateBy::create(10, 360);
  const auto sun_rotation_sequence = Sequence::create(sun_rotation_action, 0);
  const auto sun_rotation_repeat = RepeatForever::create(sun_rotation_sequence);
  sun_sprite->runAction(sun_rotation_repeat);

  CreateButton();

  return true;
}

void LobbyScene::CreateButton() {
  auto single_play_button = Button::create("ui/SinglePlayButton.png", "ui/SinglePlayButton.png", "ui/SinglePlayButton.png");
  single_play_button->setPosition(Vec2(center_.x - 400.0f, center_.y - 280.0f));
  single_play_button->addTouchEventListener([&, single_play_button](Ref* sender, Widget::TouchEventType type) {
    if (type == Widget::TouchEventType::BEGAN) {
      auto audio = SimpleAudioEngine::getInstance();
      audio->playEffect("Sounds/ButtonClick.mp3");
      const auto scale_action = ScaleTo::create(0.2f, 1.2f);
      single_play_button->runAction(scale_action);
    } else if (type == Widget::TouchEventType::ENDED) {
      const auto scale_action = ScaleTo::create(0.2f, 1.0f);
      single_play_button->runAction(scale_action);
      Director::getInstance()->replaceScene(SinglePlayScene::createScene());
      return;
    } else if (type == Widget::TouchEventType::CANCELED) {
      const auto scale_action = ScaleTo::create(0.2f, 1.0f);
      single_play_button->runAction(scale_action);
    }
  });
  ui_node_->addChild(single_play_button);
}

void LobbyScene::menuCloseCallback(Ref* pSender) {
  Director::getInstance()->end();

  /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

  //EventCustom customEndEvent("game_scene_close_event");
  //_eventDispatcher->dispatchEvent(&customEndEvent);


}
