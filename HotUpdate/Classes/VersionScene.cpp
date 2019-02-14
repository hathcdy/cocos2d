#include "VersionScene.h"
#include "network\HttpClient.h"
#include "json\document.h"
#include "Utils.h"

USING_NS_CC;
using namespace cocos2d::network;

void VersionScene::upgradeWithJson(const rapidjson::Document &doc) {
  // 传入json更新数据的方法
}

cocos2d::Scene * VersionScene::createScene() {
  return VersionScene::create();
}

bool VersionScene::init() {
  if (!Scene::init()) return false;

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto upgradeButton = MenuItemFont::create("Upgrade", CC_CALLBACK_1(VersionScene::upgradeVersion, this));
  if (upgradeButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + upgradeButton->getContentSize().height / 2;
    upgradeButton->setPosition(Vec2(x, y));
  }

  auto leftButton = MenuItemFont::create("<-", [this](Ref *sender) { this->switchPicture(true); });
  if (leftButton) {
    float x = origin.x + visibleSize.width / 2 - 100;
    float y = origin.y + leftButton->getContentSize().height / 2 + 50;
    leftButton->setPosition(Vec2(x, y));
  }

  auto rightButton = MenuItemFont::create("->", [this](Ref *sender) { this->switchPicture(false); });
  if (rightButton) {
    float x = origin.x + visibleSize.width / 2 + 100;
    float y = origin.y + rightButton->getContentSize().height / 2 + 50;
    rightButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(upgradeButton, backButton, leftButton, rightButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  versionInput = TextField::create("Version Here", "arial", 24);
  if (versionInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    versionInput->setPosition(Vec2(x, y));
    this->addChild(versionInput, 1);
  }

  picture = Sprite::create("winged_kuriboh.jpg");
  if (picture) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height / 2;
    picture->setPosition(Vec2(x, y));
    this->addChild(picture);
  }

  pictureName = Label::create();
  if (pictureName) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 150.0f;
    pictureName->setPosition(Vec2(x, y));
    this->addChild(pictureName);
  }

  return true;
}

void VersionScene::upgradeVersion(Ref *sender) {
  // 按下升级时调用该方法
}

void VersionScene::switchPicture(bool isLeft) {
  // 按下左/右按钮时调用该方法
}
