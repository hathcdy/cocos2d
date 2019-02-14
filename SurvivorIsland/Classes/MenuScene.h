#ifndef __MenuScene_SCENE_H__
#define __MenuScene_SCENE_H__

#include "ui\CocosGUI.h"
#include "cocos2d.h"

using namespace cocos2d::ui;

class MenuScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	void preloadMusic();
	void playBgm();

	// a selector callback
	void singleItemCallback(cocos2d::Ref* pSender);
	void doubleItemCallback(cocos2d::Ref* pSender);
	void helpItemCallback(cocos2d::Ref* pSender);
	void nextItemCallback(cocos2d::Ref* pSender);
	void preItemCallback(cocos2d::Ref* pSender);
	void scoreItemCallback(cocos2d::Ref* pSender);
	int mapID;
	// implement the "static create()" method manually
	CREATE_FUNC(MenuScene);
private:
	cocos2d::Sprite* mapsprite;
	TextField *helpMessage;
	TextField *historygrade;
};

#endif // __MenuScene_SCENE_H__
