#pragma once
#include "cocos2d.h"
#include"GlobalVar.h"
using namespace cocos2d;

class bulletFactory :public cocos2d::Ref {
public:
	//��ȡ��������
	static bulletFactory* getInstance();
	//����һ���ӵ������洢�������й���
	Sprite* createBullet(int type, bool isPlayer1);
	//�������е����й��ﶼ����ɫ�ƶ���ͨ�������������еĹ���ܷ���
	void moveMonster(Vec2 playerPos, float time);
	//�Ƴ��ӵ�
	void removeBullet(Sprite* sp, int type, bool isPlayer1);
	//�ж���ײ
	Sprite* collider(Rect rect);
	void initSpriteFrame();
	// ��������ӵ�����
	int getBulletNum(int type, bool isPlayer1);
	// ��ó������ӵ��Ķ���
	Vector<Sprite*> getBulletNumInScene(int type, bool isPlayer1);
	// �����ӵ����͡���ҳ�������ӵ���С�ͷ���
	void setScale_and_setRotation(Sprite* b, int type, bool isPlayer1);
	// �ӵ�����֮��Ĺ켣����
	void moveBullet(Sprite* b, int type, bool isPlayer1);
	void clear();
	//��ö�Ӧ�ӵ�����
	Vector<Sprite*> getBullet(int attack);
private:
	bulletFactory();
	// 6�����͵��ӵ����У����ų����е��ӵ�
	Vector<Sprite*> bulletForPlayer1[6];
	Vector<Sprite*> bulletForPlayer2[6];
	// �������ӵ�еĵ�ҩ��Ŀ
	int bulletNumForPlayer1[6] = { 9999,9999,9999,9999,9999,9999 };
	int bulletNumForPlayer2[6] = { 9999,9999,9999,9999,9999,9999 };

	static bulletFactory* bulletfactory;
};