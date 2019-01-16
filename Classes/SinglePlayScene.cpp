#include "SinglePlayScene.h"
#include "SimpleAudioEngine.h"
#include "ConstHiddenCatch.h"
#include "User.h"
#include "json11.hpp"

struct StageInfo {
  int max_stage_id;
  int stage_id;
  std::string left_image_url;
  std::string right_image_url;

};

Scene* SinglePlay::createScene()
{
  return SinglePlay::create();
}

static void SetEnableButton(Button* button) {
  button->setBright(true);
  button->setEnabled(true);
}

static void SetDisableButton(Button* button) {
  button->setBright(false);
  button->setEnabled(false);
}

static int FindHiddenPointIndex(const std::vector<HiddenPoint>& hidden_point) {
  for (auto i = 0; i < hidden_point.size(); ++i) {
    if (!hidden_point[i].found) return i;
  }
  return -1;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename) {
  printf("Error while loading: %s\n", filename);
  printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool SinglePlay::init()
{
  if (!Scene::init())
  {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  center_ = Vec2(visibleSize.width / 2.0f + origin.x, visibleSize.height / 2.0f + origin.y);

  RequestStageInfo(User::GetLastStageId(), User::Instance().user_id());

  /*
  loading_label_ = Label::createWithTTF("Loading...", "fonts/BMDOHYEON.ttf", 50);
  if (loading_label_ == nullptr) {
    problemLoading("'fonts/Marker Felt.ttf'");
  } else {
    loading_label_->setPosition(center_);
    this->addChild(loading_label_);
  }
  */
  bottom_background_sprite_ = Sprite::create("sprites/BottomBackground.png");
  if (bottom_background_sprite_ == nullptr) {
    problemLoading("'BottomBackground.png'");
  } else {
    bottom_background_sprite_->setPosition(Vec2(center_.x, kBottomSpriteHeight / 2.0f));
    this->addChild(bottom_background_sprite_);
  }

  // 이미지 위치 지정
  left_image_position_.x  = center_.x - (kImageWidth / 2.0f) - (kMiddleSpace/2.0f);
  left_image_position_.y  = center_.y + (kBottomSpriteHeight  + kTimerOutlinerHeight) / 2.0f;
  right_image_position_.x = center_.x + (kImageWidth / 2.0f) + (kMiddleSpace / 2.0f);
  right_image_position_.y = center_.y + (kBottomSpriteHeight + kTimerOutlinerHeight) / 2.0f;

  left_door_sprite_ = Sprite::create("sprites/LeftDoor.png");
  left_door_sprite_->setPosition(left_image_position_.x, left_image_position_.y);
  this->addChild(left_door_sprite_, 1);

  right_door_sprite_ = Sprite::create("sprites/RightDoor.png");
  right_door_sprite_->setPosition(right_image_position_.x, right_image_position_.y);
  this->addChild(right_door_sprite_, 1);

  CreateLoadingAsset();

  auto touch_listener = EventListenerTouchOneByOne::create();
  touch_listener->setSwallowTouches(true);
  touch_listener->onTouchBegan = [&](Touch* touch, Event* event) {
    Vec2 point = touch->getLocation();
    CCLOG("end touch x: %f, touch y: %f", point.x, point.y);
    if (left_hidden_rects_.size() <= 0) {
      return false;
    }

    for (auto i = 0; i < left_hidden_rects_.size(); ++i) {
      if (hidden_points_[i].found) {
        continue;
      }
      if (left_hidden_rects_[i].containsPoint(point)) {
        CCLOG("Left Found");
        HandleCorrectPoint(i);
      }
    }
    //hidden_points_
    for (auto i = 0; i < right_hidden_rects_.size(); ++i) {
      if (hidden_points_[i].found) {
        continue;
      }
      if (right_hidden_rects_[i].containsPoint(point)) {
        CCLOG("Right Found");
        HandleCorrectPoint(i);
      }
    }
    return true;
  };

  touch_listener->onTouchEnded = [&](Touch* touch, Event* event) {
    return true;
  };

  touch_listener->onTouchCancelled = [&](Touch* touch, Event* event) {
    CCLOG("touch canceld");
    return true;
  };

  _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);
  
  //this->scheduleUpdate();
  ui_node_ = Node::create();
  this->addChild(ui_node_, 2);
  CreateTimer();
  CreateButton();
  //CreateLabel();
  return true;
}

void SinglePlay::update(float dt) {

}

//===========================================================================
void SinglePlay::RequestStageInfo(int stage_id, std::string uid) {
  std::string url = WebServerUrl + "/stage/req/" + std::to_string(stage_id) + "/" + uid;
  using namespace network;
  auto request = new HttpRequest();
  request->setUrl(url.c_str());
  request->setRequestType(HttpRequest::Type::GET);
  request->setResponseCallback([&, this] (
      network::HttpClient* sender,
      network::HttpResponse* response
    ) {
    if (!response) {
      return;
    }

    const auto response_code = response->getResponseCode();
    if (!response->isSucceed()) {
      return;
    }

    if (response_code != 200) {
      return;
    }

    std::string err;
    std::vector<char>* buffer = response->getResponseData();
    char* concatenated = (char*)malloc(buffer->size() + 1);
    std::string string2(buffer->begin(), buffer->end());
    strcpy(concatenated, string2.c_str());
    auto json_object = json11::Json::parse(concatenated, err);

    hidden_points_.clear();
    left_hidden_rects_.clear();
    right_hidden_rects_.clear();

    for (auto& hidden_point : json_object["hidden_points"].array_items()) {
      HiddenPoint tmp;
      tmp.x = static_cast<float>(hidden_point["x"].int_value());
      tmp.y = static_cast<float>(hidden_point["y"].int_value());
      tmp.width = hidden_point["width"].int_value();
      tmp.height = hidden_point["height"].int_value();
      hidden_points_.emplace_back(tmp);
    }

    auto current_stage_count = json_object["current_stage_count"].int_value();
    auto total_stage_count = json_object["total_stage_count"].int_value();

    total_hidden_point_count_ = hidden_points_.size();
    CreateLabel(current_stage_count + 1, total_stage_count);

    auto left_image_url = json_object["left_image_url"].string_value();
    auto right_image_url = json_object["right_image_url"].string_value();
    StartDownloadLeftImage(left_image_url.c_str());
    StartDownloadRightImage(right_image_url.c_str());

    for (auto hidden_point : hidden_points_) {
      auto x = hidden_point.x - (hidden_point.width * 0.5f);
      auto y = (hidden_point.y + kTimerOutlinerHeight + kBottomSpriteHeight) - (hidden_point.height * 0.5f);
      Rect left_rect(x, y, hidden_point.width, hidden_point.height);
      left_hidden_rects_.push_back(left_rect);

      Rect right_rect(x + kMiddleSpace + kImageWidth, y, hidden_point.width, hidden_point.height);
      right_hidden_rects_.push_back(right_rect);
    }
    
    /*
    auto top_user_name = json_object["top_user"].string_value();
    auto loading_label = Label::createWithTTF(ccsf2("%s", top_user_name.c_str()), NormalFont, 80);
    loading_label->setPosition(Vec2(center_.x, center_.y - 200.0f));
    loading_label->setColor(Color3B(255, 255, 255));
    loading_label->enableShadow(Color4B::BLACK, Size(2, -2), 0);
    loading_label->enableOutline(Color4B::BLACK, 5);
    loading_ui_node_->addChild(loading_label);
    */
  });

  HttpClient::getInstance()->send(request);
  request->release();
}

/***************************************************************************
/*
/*  로딩 화면
/*
***/
void SinglePlay::CreateLoadingAsset() {
  loading_label_ = Label::createWithTTF("Loading...", NormalFont, 80);
  loading_label_->setPosition(Vec2(center_.x, center_.y + 100.0f));
  loading_label_->setColor(Color3B(255, 255, 255));
  loading_label_->enableShadow(Color4B::BLACK, Size(2, -2), 0);
  loading_label_->enableOutline(Color4B::BLACK, 5);
  this->addChild(loading_label_, 2);
}

/***************************************************************************
/*
/*  타이머 생성
/*
***/
void SinglePlay::CreateTimer() {
  timer_bar_sprite_ = Sprite::create("sprites/TimerBar.png");
  timer_bar_sprite_->setColor(Color3B(0, 255, 0));
  auto timer_outline_sprite = Sprite::create("sprites/TimerOutliner.png");
  const Size size = timer_outline_sprite->getContentSize();
  timer_outline_sprite->setPosition(center_.x, size.height / 2.0f + kBottomSpriteHeight);
  timer_outline_sprite->setVisible(true);
  ui_node_->addChild(timer_outline_sprite, 1);

  progress_timer_bar_ = ProgressTimer::create(timer_bar_sprite_);
  progress_timer_bar_->setPosition(timer_outline_sprite->getPosition());
  progress_timer_bar_->setMidpoint(Vec2(-1.0f, 0.0f));
  progress_timer_bar_->setBarChangeRate(Vec2(1, 0));
  progress_timer_bar_->setType(ProgressTimer::Type::BAR);
  progress_timer_bar_->setPercentage(100);
  ui_node_->addChild(progress_timer_bar_, 1);
  // 타이머 스케줄 시작
  this->schedule(SEL_SCHEDULE(&SinglePlay::OnUpdateTimer), 1.0f / 10.0f);
}

/***************************************************************************
/*
/* 타이머 업데이트
/*
***/
void SinglePlay::OnUpdateTimer(float /*dt*/) {
  if (paused_ || stopped_timer) {
    return;
  }
  const float kTimerMaxSec = 45;
  const float percentage = progress_timer_bar_->getPercentage();
  //CCLOG("Percentage: %f", percentage);
  progress_timer_bar_->setPercentage(percentage - (100 / (kTimerMaxSec * 10)));
  const float current_percentage = progress_timer_bar_->getPercentage();
  if (current_percentage <= 0.0f) {
    CCLOG("Game over: %f", percentage);
    paused_ = true;
  } else if (current_percentage < 20.0f) {
    timer_bar_sprite_->setColor(Color3B(255, 0, 0));
  } else if (current_percentage < 50.0f) {
    timer_bar_sprite_->setColor(Color3B(255, 211, 0));
  }
  //progress_timeer_bar_->setPercentage(percentage - 100.0f / (60.0f * kTimerMaxSec));
}

/****************************************************************************
*
* 힌트 버튼 및 스탑 버튼 생성
*
***/
void SinglePlay::CreateButton() {
  auto hint_button = Button::create("ui/HintButton.png", "ui/HintButton.png", "ui/HintButton.png");
  hint_button->setPosition(Vec2(center_.x + 300.0f, kBottomSpriteHeight / 2.0f));
  hint_button->addTouchEventListener([&, hint_button](Ref* sender, Widget::TouchEventType type) {
    if (type == Widget::TouchEventType::BEGAN) {
      if (paused_)  return;
      if (User::Instance().hint_count() <= 0)  return;
      auto audio = SimpleAudioEngine::getInstance();
      audio->playEffect("sounds/ButtonClick.mp3");
      const auto scale_action = ScaleTo::create(0.1f, 1.1f);
      hint_button->runAction(scale_action);
    } else if (type == Widget::TouchEventType::ENDED) {
      const auto scale_action = ScaleTo::create(0.1f, 1.0f);
      hint_button->runAction(scale_action);

      auto index = FindHiddenPointIndex(hidden_points_);
      if (index < 0) {
        SetDisableButton(hint_button);
        return;
      }
      auto hint_count = User::Instance().hint_count();
      hint_count = std::max(0, --hint_count);
      HandleCorrectPoint(index);

      User::Instance().set_hint_count(hint_count);
      if (hint_count <= 0) {
        SetDisableButton(hint_button);
      }
      return;
    } else if (type == Widget::TouchEventType::CANCELED) {
      const auto scale_action = ScaleTo::create(0.1f, 1.0f);
      hint_button->runAction(scale_action);
    }
  });
  ui_node_->addChild(hint_button);
  if (User::Instance().hint_count() <= 0) {
    SetDisableButton(hint_button);
  }

  auto stop_timer_button = Button::create("ui/StopTimerButton.png", "ui/StopTimerButton.png", "ui/StopTimerButton.png");
  stop_timer_button->setPosition(Vec2(center_.x + 600.0f, kBottomSpriteHeight / 2.0f));
  stop_timer_button->addTouchEventListener([&, stop_timer_button](Ref* sender, Widget::TouchEventType type) {
    if (type == Widget::TouchEventType::BEGAN) {
      if (paused_ || stopped_timer || User::Instance().stop_timer_count() <= 0) {
        return;
      }
      auto audio = SimpleAudioEngine::getInstance();
      audio->playEffect("sounds/ButtonClick.mp3");
      const auto scale_action = ScaleTo::create(0.1f, 1.1f);
      stop_timer_button->runAction(scale_action);
    } else if (type == Widget::TouchEventType::ENDED) {
      const auto scale_action = ScaleTo::create(0.1f, 1.0f);
      stop_timer_button->runAction(scale_action);
      auto stop_timer_count = User::Instance().stop_timer_count();
      stop_timer_count = std::max(0, --stop_timer_count);
      User::Instance().set_stop_timer_count(stop_timer_count);
      // 시간 10초간 멈추기
      stopped_timer = true;
      auto callback = CallFunc::create([&] {
        SetEnableButton(stop_timer_button);
        stopped_timer = false;
      });
      auto seq = Sequence::create(DelayTime::create(10.0f), callback, nullptr);
      stop_timer_button->runAction(seq);
      SetDisableButton(stop_timer_button);
      return;
    } else if (type == Widget::TouchEventType::CANCELED) {
      const auto scale_action = ScaleTo::create(0.1f, 1.0f);
      stop_timer_button->runAction(scale_action);
    }
  });
  ui_node_->addChild(stop_timer_button);
  if (User::Instance().stop_timer_count() <= 0) {
    SetDisableButton(stop_timer_button);
  }

  pause_button_ = Button::create("ui/PauseButton.png", "ui/PauseButton.png", "ui/PauseButton.png");
  pause_button_->setPosition(Vec2(1920.0f - 100.0f, kBottomSpriteHeight / 2.0f));
  pause_button_->addTouchEventListener([&](Ref* sender, Widget::TouchEventType type) {
    if (type == Widget::TouchEventType::BEGAN) {
      if (paused_)  return;
      auto audio = SimpleAudioEngine::getInstance();
      audio->playEffect("sounds/ButtonClick.mp3");
      const auto scale_action = ScaleTo::create(0.1f, 1.1f);
      pause_button_->runAction(scale_action);
    } else if (type == Widget::TouchEventType::ENDED) {
      const auto scale_action = ScaleTo::create(0.1f, 1.0f);
      pause_button_->runAction(scale_action);
      CloseDoor(1.2f);
      return;
    } else if (type == Widget::TouchEventType::CANCELED) {
      const auto scale_action = ScaleTo::create(0.1f, 1.0f);
      pause_button_->runAction(scale_action);
    }
  });
  ui_node_->addChild(pause_button_);
}

/****************************************************************************
* 
* 스테이지 및 크레딧 라벨 생성
*
***/
void SinglePlay::CreateLabel(int current_stage_count, int total_stage_count) {
  stage_label_ = Label::createWithTTF(ccsf2("STAGE  %d / %d", current_stage_count, total_stage_count), NormalFont, 30);
  stage_label_->setPosition(
    stage_label_->getContentSize().width / 2.0f + 20.0f,
    stage_label_->getContentSize().height / 2.0f + 20.0f
  );
  stage_label_->setColor(Color3B(255, 255, 255));
  stage_label_->enableShadow(Color4B::BLACK, Size(1, -1), 0);
  stage_label_->enableOutline(Color4B::BLACK, 2);
  ui_node_->addChild(stage_label_);

  total_spot_count_label_ = Label::createWithTTF(ccsf2("%d", total_hidden_point_count_), NormalFont, 60);
  total_spot_count_label_->setPosition(
    bottom_background_sprite_->getPosition()
  );
  total_spot_count_label_->setColor(Color3B(255, 211, 0));
  total_spot_count_label_->enableShadow(Color4B::BLACK, Size(1, -1), 0);
  total_spot_count_label_->enableOutline(Color4B::BLACK, 2);
  ui_node_->addChild(total_spot_count_label_);
}

void SinglePlay::menuCloseCallback(Ref* pSender)
{
  //Close the cocos2d-x game scene and quit the application
  Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif

  /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

  //EventCustom customEndEvent("game_scene_close_event");
  //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void SinglePlay::StartDownloadLeftImage(std::string url) {
  network::HttpRequest* request = new (std::nothrow) network::HttpRequest();
  request->setUrl(url.c_str());
  request->setRequestType(network::HttpRequest::Type::GET);
  request->setResponseCallback(CC_CALLBACK_2(SinglePlay::OnCompleteDownloadLeftImage, this));
  network::HttpClient::getInstance()->send(request);
  request->release();
}

void SinglePlay::OnCompleteDownloadLeftImage(
  network::HttpClient* sender, 
  network::HttpResponse* response
) {
  if (!response || !response->isSucceed()) {
    // TODO(test2): exception 처리
    // 네트워크 상태가 좋지않습니다.
    return;
  }
  std::vector<char>* buffer = response->getResponseData();

  Image* image = new Image();
  image->initWithImageData(reinterpret_cast<const unsigned char*>(&(buffer->front())), buffer->size());
  left_image_texture.initWithImage(image);

  left_image_sprite_ = Sprite::createWithTexture(&left_image_texture);
  left_image_sprite_->setPosition(left_image_position_);
  this->addChild(left_image_sprite_);
  delete image;
  ++download_image_count_;
  OnCompleteDownloadallImage();
}

void SinglePlay::StartDownloadRightImage(std::string url) {
  network::HttpRequest* request = new (std::nothrow) network::HttpRequest();
  request->setUrl(url.c_str());
  request->setRequestType(network::HttpRequest::Type::GET);
  request->setResponseCallback(CC_CALLBACK_2(SinglePlay::OnCompleteDownloadRightImage, this));
  network::HttpClient::getInstance()->send(request);
  request->release();
}

void SinglePlay::OnCompleteDownloadRightImage(
  network::HttpClient* sender,
  network::HttpResponse* response
) {
  if (!response || !response->isSucceed()) {
    // TODO(test2): exception 처리
    // 네트워크 상태가 좋지않습니다.
    return;
  }
  std::vector<char>* buffer = response->getResponseData();

  Image* image = new Image();
  image->initWithImageData(reinterpret_cast<const unsigned char*>(&(buffer->front())), buffer->size());
  right_image_texture.initWithImage(image);
  right_image_sprite_ = Sprite::createWithTexture(&right_image_texture);
  right_image_sprite_->setPosition(right_image_position_);
  this->addChild(right_image_sprite_);
  delete image;
  ++download_image_count_;
  OnCompleteDownloadallImage();
}

void SinglePlay::OnCompleteDownloadallImage() {
  if (download_image_count_ >= 2) {
    loading_label_->setVisible(false);
    OpenDoor(1.2f);
  }
}

void SinglePlay::OpenDoor(float duration) {
  auto callback = CallFunc::create([&] {
    paused_ = false;
    pause_button_->setBright(true);
    pause_button_->setVisible(true);
  });

  auto seq = Sequence::create(
    MoveTo::create(
      duration,
      Vec2(
        left_door_sprite_->getPosition().x - left_door_sprite_->getContentSize().width,
        left_door_sprite_->getPosition().y
      )
    ),
    callback,
    nullptr);
  left_door_sprite_->runAction(seq);

  const auto move_to = MoveTo::create(
    duration,
    Vec2(right_door_sprite_->getPosition().x + right_door_sprite_->getContentSize().width,
         right_door_sprite_->getPosition().y
    )
  );
  right_door_sprite_->runAction(move_to);
}

void SinglePlay::CloseDoor(float duration) {
  paused_ = true;
  pause_button_->setBright(false);
  pause_button_->setVisible(false);

  auto callback = CallFunc::create([&] {
  });

  auto seq = Sequence::create(
    MoveTo::create(
      duration,
      Vec2(left_image_position_)
    ),
    callback,
    nullptr);
  left_door_sprite_->runAction(seq);

  const auto move_to = MoveTo::create(
    duration,
    Vec2(right_image_position_)
  );
  right_door_sprite_->runAction(move_to);
}

void SinglePlay::HandleCorrectPoint(size_t index) {
  hidden_points_[index].found = true;
  StartCircleAnimation(Vec2(hidden_points_[index].x, hidden_points_[index].y));
  total_spot_count_label_->setString(ccsf2("%d", --total_hidden_point_count_));
}

void SinglePlay::StartCircleAnimation(const Vec2& pos) {
  const Vec2 left_pos  = { pos.x, pos.y + kTimerOutlinerHeight + kBottomSpriteHeight };
  const Vec2 right_pos = { pos.x + kMiddleSpace + kImageWidth, left_pos.y };
  auto circle_animation = Animation::create();
  circle_animation->setDelayPerUnit(0.1f);
  circle_animation->addSpriteFrameWithFile("animation/CorrectCircle/Circle_0.png");
  circle_animation->addSpriteFrameWithFile("animation/CorrectCircle/Circle_1.png");
  circle_animation->addSpriteFrameWithFile("animation/CorrectCircle/Circle_2.png");
  circle_animation->addSpriteFrameWithFile("animation/CorrectCircle/Circle_3.png");
  circle_animation->addSpriteFrameWithFile("animation/CorrectCircle/Circle_4.png");

  auto left_correct_circle = Sprite::create("animation/CorrectCircle/Circle_0.png");
  left_correct_circle->setPosition(left_pos);
  left_correct_circle->setScale(0.5f);
  left_correct_circle->runAction(Animate::create(circle_animation));
  this->addChild(left_correct_circle);

  auto right_correct_circle = Sprite::create("animation/CorrectCircle/Circle_0.png");
  right_correct_circle->setPosition(right_pos);
  right_correct_circle->setScale(0.5f);
  right_correct_circle->runAction(Animate::create(circle_animation));
  this->addChild(right_correct_circle);
}
