#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	void updateTime(float dt);

	void attackItemCallBack(Ref* pSender);
        
	//void deadItemCallBack(Ref* pSender);

	void wItemCallBack(Ref* pSender);
	void aItemCallBack(Ref* pSender);
	void sItemCallBack(Ref* pSender);
	void dItemCallBack(Ref* pSender);

	//随机生成怪物
	void getMonster(float dt);

	//怪物向角色移动
	void moveMonsters(float dt);

	//角色攻击怪物，角色回血
	void hitMonster();

	//Hp为0，角色死亡
	void playerDead();

	//怪物碰到角色，角色掉血
	void hitByMonster(float dt);

	//获取击杀数
	int getKill();

	//更新击杀数
	void updateKill();
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
private:
	bool isDone;
	bool isDead;
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* time;
	cocos2d::Label* kill;
	int dtime;
	int killCount;
	cocos2d::ProgressTimer* pT;
};
