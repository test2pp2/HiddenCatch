#ifndef __SINGLE_PLAY_SCENE_H__
#define __SINGLE_PLAY_SCENE_H__

#include "cocos2d.h"
#include "ui/UILoadingBar.h"
#include "ui/UIButton.h"
#include "network/HttpClient.h"
#include "SimpleAudioEngine.h"
#include "CommonPlayUtil.h"

using namespace cocos2d;
using namespace ui;
using namespace CocosDenshion;

class SinglePlay : public cocos2d::Scene
{
public:
  static cocos2d::Scene* createScene();

  virtual bool init();

  void menuCloseCallback(cocos2d::Ref* pSender);
  
  
private:
  void update(float dt);

  void RequestStageInfo(int stage_id, std::string uid);
  void CreateLoadingAsset();

  void CreateTimer();
  void CreateButton();
  void CreateLabel(int current_stage_count, int total_stage_count);
  void OnUpdateTimer(float dt);

  void StartDownloadLeftImage(std::string url);
  void StartDownloadRightImage(std::string url);
  void OnCompleteDownloadLeftImage(network::HttpClient* sender, network::HttpResponse* response);
  void OnCompleteDownloadRightImage(network::HttpClient* sender, network::HttpResponse* response);
  void OnCompleteDownloadallImage();

  void OpenDoor(float duration);
  void CloseDoor(float duration);

  Vec2 center_;

  Label* loading_label_ = nullptr;
  Label* stage_label_ = nullptr;
  Label* credit_label_ = nullptr;
  Label* total_spot_count_label_ = nullptr;
  Label* found_spot_count_label_ = nullptr;

  int stage_count_ = 10;
  int total_hidden_point_count_ = 5;
  int found_hidden_point_count_ = 0;

  Texture2D left_image_texture;
  Texture2D right_image_texture;

  Sprite* left_image_sprite_ = nullptr;
  Sprite* right_image_sprite_ = nullptr;

  Vec2 left_image_position_;
  Vec2 right_image_position_;
  int download_image_count_ = 0;

  Sprite* timer_bar_sprite_ = nullptr;
  ProgressTimer* progress_timer_bar_;

  Node* loading_ui_node_ = nullptr;
  Node* ui_node_ = nullptr;

  Sprite* left_door_sprite_ = nullptr;
  Sprite* right_door_sprite_ = nullptr;

  Sprite* bottom_background_sprite_ = nullptr;

  Button* pause_button_ = nullptr;
  bool paused_ = true;

  std::vector<HiddenPoint> hidden_points_;
  std::vector<Rect> left_hidden_rects_;
  std::vector<Rect> right_hidden_rects_;

  CREATE_FUNC(SinglePlay);
};

#endif // __SINGLE_PLAY_SCENE_H__
