#pragma once
#include "cocos2d.h"
#include"GlobalVar.h"
using namespace cocos2d;

class bulletFactory :public cocos2d::Ref {
public:
	//获取单例工厂
	static bulletFactory* getInstance();
	//生成一个子弹，并存储到容器中管理
	Sprite* createBullet(int type, bool isPlayer1);
	//让容器中的所有怪物都往角色移动，通过容器管理所有的怪物很方便
	void moveMonster(Vec2 playerPos, float time);
	//移除子弹
	void removeBullet(Sprite* sp, int type, bool isPlayer1);
	//判断碰撞
	Sprite* collider(Rect rect);
	void initSpriteFrame();
	// 获得现有子弹数量
	int getBulletNum(int type, bool isPlayer1);
	// 获得场景中子弹的队列
	Vector<Sprite*> getBulletNumInScene(int type, bool isPlayer1);
	// 根据子弹类型、玩家朝向调节子弹大小和方向
	void setScale_and_setRotation(Sprite* b, int type, bool isPlayer1);
	// 子弹发射之后的轨迹运行
	void moveBullet(Sprite* b, int type, bool isPlayer1);
	void clear();
	//获得对应子弹队列
	Vector<Sprite*> getBullet(int attack);
private:
	bulletFactory();
	// 6种类型的子弹队列，存着场景中的子弹
	Vector<Sprite*> bulletForPlayer1[6];
	Vector<Sprite*> bulletForPlayer2[6];
	// 玩家现在拥有的弹药数目
	int bulletNumForPlayer1[6] = { 9999,9999,9999,9999,9999,9999 };
	int bulletNumForPlayer2[6] = { 9999,9999,9999,9999,9999,9999 };

	static bulletFactory* bulletfactory;
};