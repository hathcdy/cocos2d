#pragma once
#include "cocos2d.h"
#include "PlayGameScene.h"
using namespace cocos2d;

class Factory :public cocos2d::Ref {
public:
	//获取单例工厂
	static Factory* getInstance();

	// 生成player，并存储到容器中管理
	Vector<Sprite*> createPlayer();

	// 更新player移动动作的帧动画
	void update_Player_Animation(Sprite* player, bool isPlayer1);

	// 根据moveKey和isMove获得移动方向
	string getDirection(bool isMove_hori, bool isMove_vert, char moveKey_hori, char moveKey_vert);

	// 玩家攻击
	void attack(Sprite* Player, bool isPlayer1);

	void movePlayer(Vec2 playerPos, float time);
	
	//移除怪物
	void removePlayer(Sprite*);
	
	//判断碰撞
	Sprite* collider(Vec2 pos);

	//初始化怪物帧动画
	void initSpriteFrame();

	// 玩家按不同武器的射击
	void shot(Sprite* Player, int type, bool isPlayer1);

	void wasHit(Sprite* Player, int HP, bool isPlayer1);
	
	Vector<Sprite*> getPlayers();

	void clear();
	
private:
	static Factory* factory;
	Factory();
	Vector<Sprite*> player;
	cocos2d::Vector<SpriteFrame*> monsterDead;

	
};