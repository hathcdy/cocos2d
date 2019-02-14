#pragma once
#include "cocos2d.h"
#include "PlayGameScene.h"
using namespace cocos2d;

class Factory :public cocos2d::Ref {
public:
	//��ȡ��������
	static Factory* getInstance();

	// ����player�����洢�������й���
	Vector<Sprite*> createPlayer();

	// ����player�ƶ�������֡����
	void update_Player_Animation(Sprite* player, bool isPlayer1);

	// ����moveKey��isMove����ƶ�����
	string getDirection(bool isMove_hori, bool isMove_vert, char moveKey_hori, char moveKey_vert);

	// ��ҹ���
	void attack(Sprite* Player, bool isPlayer1);

	void movePlayer(Vec2 playerPos, float time);
	
	//�Ƴ�����
	void removePlayer(Sprite*);
	
	//�ж���ײ
	Sprite* collider(Vec2 pos);

	//��ʼ������֡����
	void initSpriteFrame();

	// ��Ұ���ͬ���������
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