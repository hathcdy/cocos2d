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

	//������ɹ���
	void getMonster(float dt);

	//�������ɫ�ƶ�
	void moveMonsters(float dt);

	//��ɫ���������ɫ��Ѫ
	void hitMonster();

	//HpΪ0����ɫ����
	void playerDead();

	//����������ɫ����ɫ��Ѫ
	void hitByMonster(float dt);

	//��ȡ��ɱ��
	int getKill();

	//���»�ɱ��
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
