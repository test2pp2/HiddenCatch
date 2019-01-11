#ifndef __SINGLE_PLAY_SCENE_H__
#define __SINGLE_PLAY_SCENE_H__

#include "cocos2d.h"
#include "ui/UILoadingBar.h"
#include "network/HttpClient.h"

using namespace cocos2d;

class SinglePlayScene : public cocos2d::Scene
{
public:
  static cocos2d::Scene* createScene();

  virtual bool init();

  void menuCloseCallback(cocos2d::Ref* pSender);
  
  
private:
  void update(float dt);

  void RequestStageInfo(int stage_id);
  void CreateLoadingAsset();

  void CreateTimer();
  void OnUpdateTimer(float dt);

  void StartDownloadLeftImage(std::string url);
  void StartDownloadRightImage(std::string url);
  void OnCompleteDownloadLeftImage(network::HttpClient* sender, network::HttpResponse* response);
  void OnCompleteDownloadRightImage(network::HttpClient* sender, network::HttpResponse* response);
  void OnCompleteDownloadallImage();

  Vec2 center_;

  Label* loading_label_ = nullptr;

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
  
  CREATE_FUNC(SinglePlayScene);
};

#endif // __SINGLE_PLAY_SCENE_H__
