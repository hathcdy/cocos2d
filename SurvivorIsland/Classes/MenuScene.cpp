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

	preloadMusic();     // Ԥ��������
	playBgm();          // ���ű�������

	CCDictionary* message = CCDictionary::createWithContentsOfFile("chinese.xml");    //��ȡxml�ļ����ļ���ResourcesĿ¼��

	auto titleKey = message->valueForKey("title");    //����key����ȡvalue
	const char* titleString = titleKey->getCString();    //��valueת��Ϊ�ַ���

	auto singleKey = message->valueForKey("singleModel");
	const char* singleString = singleKey->getCString();

	auto doubleKey = message->valueForKey("doubleModel");
	const char* doubleString = doubleKey->getCString();

	auto scoreKey = message->valueForKey("score");
	const char* scoreString = scoreKey->getCString();

	//�����˵������ͼƬ���������
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
	helpItem->setScaleY(40.0 / helpItem->getContentSize().height);//����ͼƬ���С

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
	//�����Ϸ��ͼԤ��ͼ

	mapsprite = Sprite::create("maps/map0.png");
	mapsprite->setScaleX(600.0 / mapsprite->getContentSize().width);
	mapsprite->setScaleY(300.0 / mapsprite->getContentSize().height);//��Ļ��С����
	mapsprite->setPosition(Vec2(visibleSize.width + origin.x - 400, visibleSize.height / 2 + origin.y));
	this->addChild(mapsprite, 1);

	preItem->setPosition(mapsprite->getPosition() + Vec2(-340, 0));
	nextItem->setPosition(mapsprite->getPosition() + Vec2(340, 0));
	//�����Ϸ����

	auto title = Label::createWithTTF(titleString, "fonts/Calligraphy.ttf", 65);
	title->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - title->getContentSize().height));
	title->setColor(ccc3(0, 0, 0));
	this->addChild(title, 1);

	helpItem->setPosition(title->getPosition() + Vec2(title->getContentSize().width / 2 + 40, 0));
	singleItem->setPosition(Vec2(100 + origin.x, visibleSize.height / 2 + origin.y + 150));
	doubelItem->setPosition(Vec2(100 + origin.x, visibleSize.height / 2 + origin.y));
	scoreItem->setPosition(Vec2(100 + origin.x, visibleSize.height / 2 + origin.y - 150));
	// ��ӱ���
	auto backsprite = Sprite::create("background.jpg");
	backsprite->setScaleX((float)visibleSize.width / backsprite->getContentSize().width);
	backsprite->setScaleY((float)visibleSize.height / backsprite->getContentSize().height);//��Ļ��С����
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
		const char* helpString = helpKey->getCString();//��ȡ�����淨��Ϣ
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
}//��ȡ��һ��ͼƬ��Ԥ��ͼ

void MenuScene::preItemCallback(cocos2d::Ref* pSender) {
	mapID = (mapID + 2) % 3;
	stringstream stream;
	stream << mapID;
	string filename = "maps/map" + stream.str() + ".png";
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(filename);
	mapsprite->setTexture(texture);
} //��ȡ��һ��ͼƬ��Ԥ��ͼ

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
}//��ʷ�÷ֻ�ȡ

void MenuScene::singleItemCallback(cocos2d::Ref* pSender) {
	GlobalVar::getInstance()->setDoublePlayer(false);
	stringstream stream;
	stream << mapID;
	string filename = "map" + stream.str() + ".tmx"; //��ȡtmx�ļ�·��
	GlobalVar::getInstance()->setMapFile(filename);
	auto gameScene = PlayGameScene::create();
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(0.5, gameScene));
}
void MenuScene::doubleItemCallback(cocos2d::Ref* pSender) {
	GlobalVar::getInstance()->setDoublePlayer(true);
	stringstream stream;
	stream << mapID;
	string filename = "map" + stream.str() + ".tmx"; //��ȡtmx�ļ�·��
	GlobalVar::getInstance()->setMapFile(filename);
	auto gameScene = PlayGameScene::create();
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(0.5, gameScene));
}

void MenuScene::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm_menu.mp3");
}//Ԥ���������ļ�

void MenuScene::playBgm() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm_menu.mp3", true);
}//���ű�������

