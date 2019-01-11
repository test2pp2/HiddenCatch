#include "SinglePlayScene.h"
#include "SimpleAudioEngine.h"
#include "ConstHiddenCatch.h"
#include "User.h"
#include "json11.hpp"

USING_NS_CC;

struct StageInfo {
  int max_stage_id;
  int stage_id;
  std::string left_image_url;
  std::string right_image_url;

};

Scene* SinglePlayScene::createScene()
{
  return SinglePlayScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename) {
  printf("Error while loading: %s\n", filename);
  printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool SinglePlayScene::init()
{
  //////////////////////////////
  // 1. super init first
  if (!Scene::init())
  {
    return false;
  }

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();
  center_ = Vec2(visibleSize.width / 2.0f + origin.x, visibleSize.height / 2.0f + origin.y);

  RequestStageInfo(User::GetLastStageId());
  CreateLoadingAsset();

  /*
  loading_label_ = Label::createWithTTF("Loading...", "fonts/BMDOHYEON.ttf", 50);
  if (loading_label_ == nullptr) {
    problemLoading("'fonts/Marker Felt.ttf'");
  } else {
    loading_label_->setPosition(center_);
    this->addChild(loading_label_);
  }

  auto bottom_background_sprite_ = Sprite::create("sprites/BottomBackground.png");
  if (bottom_background_sprite_ == nullptr) {
    problemLoading("'BottomBackground.png'");
  } else {
    bottom_background_sprite_->setPosition(Vec2(center_.x, kBottomSpriteHeight / 2.0f));
    this->addChild(bottom_background_sprite_, 0);
  }
  */

  // �̹��� ��ġ ����
  left_image_position_.x  = (center_.x - (kTimerSpriteWidth / 2.0f)) - (kImageWidth / 2.0f);
  left_image_position_.y  =  center_.y + kBottomSpriteHeight / 2.0f;
  right_image_position_.x = (center_.x + (kTimerSpriteWidth / 2.0f)) + (kImageWidth / 2.0f);
  right_image_position_.y =  center_.y + kBottomSpriteHeight / 2.0f;

  //StartDownloadLeftImage("https://s3.ap-northeast-2.amazonaws.com/img.repository/0/left/bbbbb.png");
  //StartDownloadRightImage("https://s3.ap-northeast-2.amazonaws.com/img.repository/0/left/bbbbb.png");

  auto touch_listener = EventListenerTouchOneByOne::create();
  touch_listener->setSwallowTouches(true);
  touch_listener->onTouchBegan = [&](Touch* touch, Event* event) {
    //Vec2 point = touch->getLocation();
    //CCLOG("start touch x: %f, touch y: %f", point.x, point.y);
    return true;
  };

  touch_listener->onTouchEnded = [&](Touch* touch, Event* event) {
    Vec2 point = touch->getLocation();
    CCLOG("end touch x: %f, touch y: %f", point.x, point.y);
    return true;
  };

  touch_listener->onTouchCancelled = [&](Touch* touch, Event* event) {
    CCLOG("touch canceld");
    return true;
  };

  _eventDispatcher->addEventListenerWithSceneGraphPriority(touch_listener, this);
  
  //this->scheduleUpdate();
  
  return true;
}

void SinglePlayScene::update(float dt) {

}

void SinglePlayScene::RequestStageInfo(int stage_id) {
  std::string url = "http://192.168.0.166:3000/stage/req/" + std::to_string(stage_id);
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
    auto top_user_name = json_object["top_user"].string_value();
    
    auto loading_label = Label::createWithTTF(ccsf2("%s", top_user_name.c_str()), NormalFont, 80);
    loading_label->setPosition(Vec2(center_.x, center_.y - 200.0f));
    loading_label->setColor(Color3B(255, 255, 255));
    loading_label->enableShadow(Color4B::BLACK, Size(2, -2), 0);
    loading_label->enableOutline(Color4B::BLACK, 5);
    loading_ui_node_->addChild(loading_label);
  });

  HttpClient::getInstance()->send(request);
  request->release();
}

void SinglePlayScene::CreateLoadingAsset() {
  loading_ui_node_ = Node::create();
  addChild(loading_ui_node_, 0);

  auto loading_label = Label::createWithTTF("Loading...", NormalFont, 80);
  loading_label->setPosition(Vec2(center_.x, center_.y + 100.0f));
  loading_label->setColor(Color3B(255, 255, 255));
  loading_label->enableShadow(Color4B::BLACK, Size(2, -2), 0);
  loading_label->enableOutline(Color4B::BLACK, 5);
  loading_ui_node_->addChild(loading_label);
}

void SinglePlayScene::CreateTimer() {
  timer_bar_sprite_ = Sprite::create("sprites/TimerBar.png");

  auto timer_outline_sprite = Sprite::create("sprites/TimerOutliner.png");
  timer_outline_sprite->setPosition(Vec2(center_.x, center_.y + kBottomSpriteHeight / 2.0f));
  timer_outline_sprite->setVisible(true);
  this->addChild(timer_outline_sprite, 1);

  progress_timer_bar_ = ProgressTimer::create(timer_bar_sprite_);
  progress_timer_bar_->setPosition(Vec2(center_.x, center_.y + kBottomSpriteHeight / 2.0f));
  progress_timer_bar_->setMidpoint(Vec2(0.0f, -1.0f));
  progress_timer_bar_->setBarChangeRate(Vec2(0, 1));
  progress_timer_bar_->setType(ProgressTimer::Type::BAR);
  progress_timer_bar_->setPercentage(100);
  this->addChild(progress_timer_bar_, 1);
  // Ÿ�̸� ������ ����
  this->schedule(SEL_SCHEDULE(&SinglePlayScene::OnUpdateTimer), 1.0f / 10.0f);
}

void SinglePlayScene::OnUpdateTimer(float /*dt*/) {
  const float kTimerMaxSec = 45;
  const float percentage = progress_timer_bar_->getPercentage();
  CCLOG("Percentage: %f", percentage);
  progress_timer_bar_->setPercentage(percentage - (100 / (kTimerMaxSec * 10)));
  //progress_timeer_bar_->setPercentage(percentage - 100.0f / (60.0f * kTimerMaxSec));
}

void SinglePlayScene::menuCloseCallback(Ref* pSender)
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

void SinglePlayScene::StartDownloadLeftImage(std::string url) {
  network::HttpRequest* request = new (std::nothrow) network::HttpRequest();
  request->setUrl(url.c_str());
  request->setRequestType(network::HttpRequest::Type::GET);
  request->setResponseCallback(CC_CALLBACK_2(SinglePlayScene::OnCompleteDownloadLeftImage, this));
  network::HttpClient::getInstance()->send(request);
  request->release();
}

void SinglePlayScene::OnCompleteDownloadLeftImage(
  network::HttpClient* sender, 
  network::HttpResponse* response
) {
  if (!response || !response->isSucceed()) {
    // ��Ʈ��ũ ���°� �����ʽ��ϴ�.
    return;
  }
  std::vector<char>* buffer = response->getResponseData();

  Image* image = new Image();
  image->initWithImageData(reinterpret_cast<const unsigned char*>(&(buffer->front())), buffer->size());
  left_image_texture.initWithImage(image);

  left_image_sprite_ = Sprite::createWithTexture(&left_image_texture);


  left_image_sprite_->setPosition(left_image_position_);
  this->addChild(left_image_sprite_, 2);
  delete image;
  ++download_image_count_;
  OnCompleteDownloadallImage();
}

void SinglePlayScene::StartDownloadRightImage(std::string url) {
  network::HttpRequest* request = new (std::nothrow) network::HttpRequest();
  request->setUrl(url.c_str());
  request->setRequestType(network::HttpRequest::Type::GET);
  request->setResponseCallback(CC_CALLBACK_2(SinglePlayScene::OnCompleteDownloadRightImage, this));
  network::HttpClient::getInstance()->send(request);
  request->release();
}

void SinglePlayScene::OnCompleteDownloadRightImage(
  network::HttpClient* sender,
  network::HttpResponse* response
) {
  if (!response || !response->isSucceed()) {
    // ��Ʈ��ũ ���°� �����ʽ��ϴ�.
    return;
  }
  std::vector<char>* buffer = response->getResponseData();

  Image* image = new Image();
  image->initWithImageData(reinterpret_cast<const unsigned char*>(&(buffer->front())), buffer->size());
  right_image_texture.initWithImage(image);
  right_image_sprite_ = Sprite::createWithTexture(&right_image_texture);
  right_image_sprite_->setPosition(right_image_position_);
  this->addChild(right_image_sprite_, 2);
  delete image;
  ++download_image_count_;
  OnCompleteDownloadallImage();
}

void SinglePlayScene::OnCompleteDownloadallImage() {
  if (download_image_count_ >= 2) {
    CreateTimer();
  }
}
