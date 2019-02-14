#ifndef __PLAYGAME_SCENE_H__
#define __PLAYGAME_SCENE_H__

#include "cocos2d.h"
#include "GlobalVar.h"
#include "Player.h"
#include "Monster.h"
#include "Bullet.h"

using namespace std;
using namespace cocos2d;
class PlayGameScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();


	

	// implement the "static create()" method manually
	CREATE_FUNC(PlayGameScene);
private:
	void addPlayer();
	void addMonsters();
	void addDataLabel();//�����ʾ���ݵ�label
	void setViewPointCenter(cocos2d::Point);
	void update(float f);
	void updateMonsterPosition(float dt);
	void updateAttack(float dt);
	int SmallMonsterAttack1(Vec2 playerPos1, float time);
	int SmallMonsterAttack2(Vec2 playerPos2, float time);
	void BigMonsterAttack(Vec2 playerPos1, Vec2 playerPos2);
	void onKeyPressed(EventKeyboard::KeyCode, Event*);
	void onKeyReleased(EventKeyboard::KeyCode, Event*);
	void movePlayer(Sprite*, char);
	void setPlayerPosition(Sprite*, Point);
	// Ԥ����֡����
	void preloadAnimation();
	// ������
	void attack(Sprite* player);
	void preloadMusic();
	// �˳���ť��Ӧ�¼�
	void exitItemCallBack(cocos2d::Ref* pSender);
	void goBackCallBack(cocos2d::Ref* pSender);
	void exitCallBack(cocos2d::Ref* pSender);
	// ��ײ���
	void meet(EventCustom * event);
	// gameover
	void gameOver();
	

	Point tileCoordForPosition(Point);
	Point getCentralPoint();
	bool distValidBetwPlayer(Sprite*, Sprite*, char);

	cocos2d::TMXTiledMap *tileMap;
	cocos2d::TMXLayer *background;
	cocos2d::TMXLayer *blockage;
	cocos2d::TMXLayer *foreground;
	cocos2d::Sprite *player1;
	cocos2d::Sprite *player2;
	Size visibleSize;

	bool Apressed;
	bool Dpressed;
	bool Wpressed;
	bool Spressed;
	
	bool left_pressed;
	bool right_pressed;
	bool up_pressed;
	bool down_pressed;
	
	//����������
	Label *label_player1;
	Label *label_player2;
	Label *hp1;
	Label *hp2;//Ѫ��
	Label *attackType1;
	Label *attackType2;//��������
	Label *count1;
	Label *count2;//ʣ���ӵ���Ŀ
	Label *score1;
	Label *score2;//ʣ���ӵ���Ŀ

	//�˳���ť
	MenuItemImage* exitItem;
	//MenuItemLabel* exitItem;
	//�˵�
	//Menu* menu;



	//Factory* playerFac = Factory::getInstance();
	// player�ƶ�֡����
	cocos2d::Vector<SpriteFrame*> player1_move[8];
	cocos2d::Vector<SpriteFrame*> player2_move[8];
	// player���֡����
	cocos2d::Vector<SpriteFrame*> player1_shot[8];
	cocos2d::Vector<SpriteFrame*> player2_shot[8];

	// monster�ƶ�֡����
	cocos2d::Vector<SpriteFrame*> monster1_move[8];
	cocos2d::Vector<SpriteFrame*> monster2_move[8];

	// monster����֡����
	cocos2d::Vector<SpriteFrame*> monster1_attack[8];

	// ���﹤��
	MonsterFactory* monsterFac;

	
};

#endif // __PLAYGAME_SCENE_H__