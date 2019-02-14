#pragma once
#include "cocos2d.h"
#include "GlobalVar.h"
using namespace cocos2d;

class MonsterFactory :public cocos2d::Ref {
public:
	//��ȡ��������
	static MonsterFactory* getInstance();
	//����һ��С������洢�������й���
	Sprite* createSmallMonster();
	//����һ���������洢�������й���
	Sprite* createBigMonster();
	//���ɴ�ֵ��ӵ�
	Sprite* createMonsterBullet();
	//�ж�ĳ�����ܷ���ĳ�����ƶ�
	bool CanMove(Sprite* mons, Vec2 dir);
	//�������е����й��ﶼ����ɫ�ƶ���ͨ�������������еĹ���ܷ���
	void moveMonster(Vec2 playerPos1, Vec2 playerPos2, float time);
	//����ɫ�Ƿ��ڹ��﹥����Χ��
	void checkAttacker(Vec2 playerPos1, Vec2 playerPos2);
	//���ĳ������Թ���ķ�λ
	std::string getDirection(Vec2 dir);
	// ����ǹ����
	void hitByPistol(Sprite* mons);
	// ��uzi����
	void hitByUzi(Sprite* mons);
	// ����������
	void hitByShotGun(Sprite* mons);
	// �����׻���
	void hitByGrenade(Sprite* mons);
	// ���������
	void hitByMissile(Sprite* mons);
	// ���������
	void hitByLaserGun(Sprite* mons);
	//�Ƴ�����
	void removeMonster(Sprite*);
	//�ж���ײ
	Sprite* collider(Rect rect);
	//��ʼ������֡����
	void initSpriteFrame();
	//��ȡĳλ���ڵ�ͼ�е�����
	Point getPositionInMap(Point);
	//�õ�����
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