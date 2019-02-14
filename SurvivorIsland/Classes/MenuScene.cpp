#include "MenuScene.h"
#include "SimpleAudioEngine.h"
#include "GlobalVar.h"
#include "PlayGameScene.h"
#include "SimpleAudioEngine.h"

#pragma execution_character_set("utf-8")
#define database	UserDefault::getInstance()

USING_NS_CC;

using namespace std;
using namespace CocosDenshion;

Scene* MenuScene::createScene()
{
	return MenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MenuSceneScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	mapID = 0;
	historygrade = TextField::create("", "Dengb", 14);
	helpMessage = TextField::create("", "Dengb", 12);

	preloadMusic();     // 预加载音乐
	playBgm();          // 播放背景音乐

	CCDictionary* message = CCDictionary::createWithContentsOfFile("chinese.xml");    //读取xml文件，文件在Resources目录下

	auto titleKey = message->valueForKey("title");    //根据key，获取value
	const char* titleString = titleKey->getCString();    //将value转化为字符串

	auto singleKey = message->valueForKey("singleModel");
	const char* singleString = singleKey->getCString();

	auto doubleKey = message->valueForKey("doubleModel");
	const char* doubleString = doubleKey->getCString();

	auto scoreKey = message->valueForKey("score");
	const char* scoreString = scoreKey->getCString();

	//创建菜单项包括图片项和文字项
	auto helpItem = MenuItemImage::create(
		"help.jpg",
		"help.jpg",
		CC_CALLBACK_1(MenuScene::helpItemCallback, this));
	auto nextItem = MenuItemImage::create(
		"next.png",
		"next.png",
		CC_CALLBACK_1(MenuScene::nextItemCallback, this));
	auto preItem = MenuItemImage::create(
		"pre.png",
		"pre.png",
		CC_CALLBACK_1(MenuScene::preItemCallback, this));

	preItem->setScaleX(50.0 / preItem->getContentSize().width);
	preItem->setScaleY(50.0 / preItem->getContentSize().height);
	nextItem->setScaleX(50.0 / nextItem->getContentSize().width);
	nextItem->setScaleY(50.0 / nextItem->getContentSize().height);
	helpItem->setScaleX(40.0 / helpItem->getContentSize().width);
	helpItem->setScaleY(40.0 / helpItem->getContentSize().height);//设置图片项大小

	auto singleLabel = Label::createWithTTF(singleString, "fonts/Calligraphy.ttf", 45);
	auto doubleLabel = Label::createWithTTF(doubleString, "fonts/Calligraphy.ttf", 45);
	auto scoreLabel = Label::createWithTTF(scoreString, "fonts/Calligraphy.ttf", 45);

	singleLabel->setColor(ccc3(0, 0, 0));
	doubleLabel->setColor(ccc3(0, 0, 0));
	scoreLabel->setColor(ccc3(0, 0, 0));

	auto singleItem = MenuItemLabel::create(singleLabel, CC_CALLBACK_1(MenuScene::singleItemCallback, this));
	auto doubelItem = MenuItemLabel::create(doubleLabel, CC_CALLBACK_1(MenuScene::doubleItemCallback, this));
	auto scoreItem = MenuItemLabel::create(scoreLabel, CC_CALLBACK_1(MenuScene::scoreItemCallback, this));
	// create menu, it's an autorelease object


	/////////////////////////////
	//添加游戏地图预览图

	mapsprite = Sprite::create("maps/map0.png");
	mapsprite->setScaleX(600.0 / mapsprite->getContentSize().width);
	mapsprite->setScaleY(300.0 / mapsprite->getContentSize().height);//屏幕大小填满
	mapsprite->setPosition(Vec2(visibleSize.width + origin.x - 400, visibleSize.height / 2 + origin.y));
	this->addChild(mapsprite, 1);

	preItem->setPosition(mapsprite->getPosition() + Vec2(-340, 0));
	nextItem->setPosition(mapsprite->getPosition() + Vec2(340, 0));
	//添加游戏标题

	auto title = Label::createWithTTF(titleString, "fonts/Calligraphy.ttf", 65);
	title->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - title->getContentSize().height));
	title->setColor(ccc3(0, 0, 0));
	this->addChild(title, 1);

	helpItem->setPosition(title->getPosition() + Vec2(title->getContentSize().width / 2 + 40, 0));
	singleItem->setPosition(Vec2(100 + origin.x, visibleSize.height / 2 + origin.y + 150));
	doubelItem->setPosition(Vec2(100 + origin.x, visibleSize.height / 2 + origin.y));
	scoreItem->setPosition(Vec2(100 + origin.x, visibleSize.height / 2 + origin.y - 150));
	// 添加背景
	auto backsprite = Sprite::create("background.jpg");
	backsprite->setScaleX((float)visibleSize.width / backsprite->getContentSize().width);
	backsprite->setScaleY((float)visibleSize.height / backsprite->getContentSize().height);//屏幕大小填满
	backsprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(backsprite, 0);

	auto menu = Menu::create(helpItem, scoreItem, singleItem, doubelItem, nextItem, preItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	helpMessage->setPosition(helpItem->getPosition() + Vec2(0, -60));
	helpMessage->setColor(ccc3(0, 0, 0));
	this->addChild(helpMessage, 1);

	historygrade->setPosition(scoreItem->getPosition() + Vec2(-20, -60));
	historygrade->setColor(ccc3(0, 0, 0));
	this->addChild(historygrade, 1);

	return true;
}


void MenuScene::helpItemCallback(cocos2d::Ref* pSender) {
	if (helpMessage->getString().length() != 0)
		helpMessage->setString("");
	else {
		CCDictionary* message = CCDictionary::createWithContentsOfFile("chinese.xml");
		auto helpKey = message->valueForKey("helpMessage");
		const char* helpString = helpKey->getCString();//获取中文玩法信息
		helpMessage->setString(helpString);
	}
}

void MenuScene::nextItemCallback(cocos2d::Ref* pSender) {
	mapID = (mapID + 1) % 3;
	stringstream stream;
	stream << mapID;
	string filename = "maps/map" + stream.str() + ".png";
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(filename);
	mapsprite->setTexture(texture);
}//获取下一张图片的预览图

void MenuScene::preItemCallback(cocos2d::Ref* pSender) {
	mapID = (mapID + 2) % 3;
	stringstream stream;
	stream << mapID;
	string filename = "maps/map" + stream.str() + ".png";
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(filename);
	mapsprite->setTexture(texture);
} //获取上一张图片的预览图

void MenuScene::scoreItemCallback(cocos2d::Ref* pSender) {
	if (historygrade->getString().length() != 0)
		historygrade->setString("");
	else {
		int grade_s[3] = { 0 };
		int grade_d[3] = { 0 };

		grade_s[0] = database->getIntegerForKey("first_s", grade_s[0]);
		grade_s[1] = database->getIntegerForKey("second_s", grade_s[1]);
		grade_s[2] = database->getIntegerForKey("third_s", grade_s[2]);
		grade_d[0] = database->getIntegerForKey("first_d", grade_d[0]);
		grade_d[1] = database->getIntegerForKey("second_d", grade_d[1]);
		grade_d[2] = database->getIntegerForKey("third_d", grade_d[2]);

		CCDictionary* message = CCDictionary::createWithContentsOfFile("chinese.xml");
		auto singleKey = message->valueForKey("singleModel");
		const char* singleString = singleKey->getCString();

		auto doubleKey = message->valueForKey("doubleModel");
		const char* doubleString = doubleKey->getCString();

		string ss = doubleString;
		string st = singleString;
		string gradeString = st + "   " + ss + "\n";

		stringstream stream;
		stream << grade_s;

		for (int i = 0; i<3; i++) {
			stringstream stream;
			stream << grade_s[i];
			string t = stream.str() + "          ";
			stringstream stream2;
			stream2 << grade_d[i];
			gradeString += t + stream2.str() + "\n";
		}

		historygrade->setString(gradeString);
	}
}//历史得分获取

void MenuScene::singleItemCallback(cocos2d::Ref* pSender) {
	GlobalVar::getInstance()->setDoublePlayer(false);
	stringstream stream;
	stream << mapID;
	string filename = "map" + stream.str() + ".tmx"; //获取tmx文件路径
	GlobalVar::getInstance()->setMapFile(filename);
	auto gameScene = PlayGameScene::create();
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(0.5, gameScene));
}
void MenuScene::doubleItemCallback(cocos2d::Ref* pSender) {
	GlobalVar::getInstance()->setDoublePlayer(true);
	stringstream stream;
	stream << mapID;
	string filename = "map" + stream.str() + ".tmx"; //获取tmx文件路径
	GlobalVar::getInstance()->setMapFile(filename);
	auto gameScene = PlayGameScene::create();
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(0.5, gameScene));
}

void MenuScene::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm_menu.mp3");
}//预加载音乐文件

void MenuScene::playBgm() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm_menu.mp3", true);
}//播放背景音乐

