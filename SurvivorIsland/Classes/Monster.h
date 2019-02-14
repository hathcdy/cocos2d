#pragma once
#include "cocos2d.h"
#include "GlobalVar.h"
using namespace cocos2d;

class MonsterFactory :public cocos2d::Ref {
public:
	//获取单例工厂
	static MonsterFactory* getInstance();
	//生成一个小怪物，并存储到容器中管理
	Sprite* createSmallMonster();
	//生成一个大怪物，并存储到容器中管理
	Sprite* createBigMonster();
	//生成大怪的子弹
	Sprite* createMonsterBullet();
	//判断某怪物能否向某方向移动
	bool CanMove(Sprite* mons, Vec2 dir);
	//让容器中的所有怪物都往角色移动，通过容器管理所有的怪物很方便
	void moveMonster(Vec2 playerPos1, Vec2 playerPos2, float time);
	//检查角色是否在怪物攻击范围内
	void checkAttacker(Vec2 playerPos1, Vec2 playerPos2);
	//获得某方向相对怪物的方位
	std::string getDirection(Vec2 dir);
	// 被手枪击中
	void hitByPistol(Sprite* mons);
	// 被uzi击中
	void hitByUzi(Sprite* mons);
	// 被霰弹击中
	void hitByShotGun(Sprite* mons);
	// 被手雷击中
	void hitByGrenade(Sprite* mons);
	// 被火箭击中
	void hitByMissile(Sprite* mons);
	// 被激光击中
	void hitByLaserGun(Sprite* mons);
	//移除怪物
	void removeMonster(Sprite*);
	//判断碰撞
	Sprite* collider(Rect rect);
	//初始化怪物帧动画
	void initSpriteFrame();
	//获取某位置在地图中的坐标
	Point getPositionInMap(Point);
	//得到怪物
	Vector<Sprite*> getMonster();
	Vector<Sprite*> getAttacker1();
	Vector<Sprite*> getAttacker2();
	Vector<Sprite*> getMonsterBullet();
	vector<bool> getIsDone();
	void wasHit(Sprite * mons, int type);
	void setIsDone(int index, bool isdone);
	void removeMonsterBullet(Sprite * bullet);
	void clear();
	void clearAttacker1();
	void clearAttacker2();
private:
	MonsterFactory();
	Vector<Sprite*> monster;
	Vector<Sprite*> attacker1;
	Vector<Sprite*> attacker2;
	Vector<Sprite*> monsterBullet;
	vector<int> monsterHP;
	vector<bool> isDone;
	cocos2d::Vector<SpriteFrame*> monsterDead;
	static MonsterFactory* factory;
	std::string Direction[8] = { "West", "NorthWest", "North", "NorthEast", "East", "SouthEast", "South", "SouthWest" };
	cocos2d::TMXTiledMap *tileMap;
	cocos2d::TMXLayer *blockage;
};