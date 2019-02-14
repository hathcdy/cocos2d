#include"Player.h"
#include"GlobalVar.h"
#include "PlayGameScene.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;

Factory* Factory::factory = NULL;

Factory::Factory() {
	initSpriteFrame();
}

//��ȡ��������
Factory* Factory::getInstance() {
	if (factory == NULL) {
		factory = new Factory();
	}
	return factory;
}

//��ʼ�����֡����
void Factory::initSpriteFrame() {
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258 - 48 * i, 0, 42, 42)));
		monsterDead.pushBack(frame);
	}
}

// ������ң���ӵ����Vector����player
Vector<Sprite*> Factory::createPlayer() {

	//������ң����洢�������й���
	auto player1_texture = Director::getInstance()->getTextureCache()->addImage("player1_move.png");
	Sprite* player1 = Sprite::createWithTexture(player1_texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 128, 128)));
	player.pushBack(player1);
	if (GlobalVar::getInstance()->getDoublePlayer() == true) {
		auto player2_texture = Director::getInstance()->getTextureCache()->addImage("player2_move.png");
		Sprite* player2 = Sprite::createWithTexture(player2_texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 128, 128)));
		player.pushBack(player2);
	}
	return player;
}

//����Ҹ����ƶ�������֡����

void Factory::update_Player_Animation(Sprite* player, bool isPlayer1) {
	// �������Ѿ�������������
	if ((isPlayer1&&GlobalVar::getInstance()->player1HP <= 0) || (!isPlayer1) && GlobalVar::getInstance()->player2HP <= 0) return;
	bool isMove_hori = isPlayer1 ? GlobalVar::getInstance()->player1_isMove_hori : GlobalVar::getInstance()->player2_isMove_hori;
	bool isMove_vert = isPlayer1 ? GlobalVar::getInstance()->player1_isMove_vert : GlobalVar::getInstance()->player2_isMove_vert;
	char moveKey_hori = isPlayer1 ? GlobalVar::getInstance()->player1_moveKey_hori : GlobalVar::getInstance()->player2_moveKey_hori;
	char moveKey_vert = isPlayer1 ? GlobalVar::getInstance()->player1_moveKey_vert : GlobalVar::getInstance()->player2_moveKey_vert;
	bool done = isPlayer1 ? GlobalVar::getInstance()->player1_done : GlobalVar::getInstance()->player2_done;
	string current_direction = isPlayer1 ? GlobalVar::getInstance()->player1_moving_direction : GlobalVar::getInstance()->player2_moving_direction;
	string prename = (isPlayer1) ? "player1_move_" : "player2_move_";
	//bool isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player1_isShotDone : GlobalVar::getInstance()->player2_isShotDone;
	// ���player1��һ�����ƶ�����
	string nextDirection = getDirection(isMove_hori, isMove_vert, moveKey_hori, moveKey_vert);

	//�����ҵ�ǰ��������ƶ�֡���������и��£�ֱ�ӷ���
	if (current_direction == "shot") return;

	// ����һ���ƶ�����Ϊ��ֹ����ֹͣ����֡����
	if (nextDirection == "stop") {
		player->stopAllActions();

		if (isPlayer1) {
			GlobalVar::getInstance()->player1_done = true;
			GlobalVar::getInstance()->player1_moving_direction = nextDirection;
		}
		else {
			GlobalVar::getInstance()->player2_done = true;
			GlobalVar::getInstance()->player2_moving_direction = nextDirection;
		}
		return;
	}

	// ����һ���ƶ���������ڵķ���һ�£� ����ѭ�����Ÿ÷��򶯻�
	if (nextDirection == current_direction && done) {
		GlobalVar::getInstance()->player1_moving_direction = isPlayer1 ? nextDirection : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = isPlayer1 ? GlobalVar::getInstance()->player2_moving_direction : nextDirection;
		string name = prename + nextDirection;
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
		auto setDoneToTrue = CallFunc::create([&,isPlayer1]() {
			//player1_done = isPlayer1 ? true : player1_done;
			//player2_done = isPlayer1 ? player2_done : true;
			if (isPlayer1) GlobalVar::getInstance()->player1_done = true;
			else GlobalVar::getInstance()->player2_done = true;
		});
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		// ��done��Ϊfalse
		GlobalVar::getInstance()->player1_done = isPlayer1 ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = isPlayer1 ? GlobalVar::getInstance()->player2_done : false;
		player->runAction(sequence);

	}

	// ����һ���ƶ���������ڵķ���һ��
	// ��ֹ��ǰ�����֡����������Ϊ�·����֡����
	if (nextDirection != current_direction) {
		GlobalVar::getInstance()->player1_moving_direction = isPlayer1 ? nextDirection : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = isPlayer1 ? GlobalVar::getInstance()->player2_moving_direction : nextDirection;
		// ����·����֡����
		string name = prename + nextDirection;
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
		auto setDoneToTrue = CallFunc::create([&, isPlayer1]() {

			if (isPlayer1) {
				GlobalVar::getInstance()->player1_done = true;
			}
			else {
				GlobalVar::getInstance()->player2_done = true;
			}
			//this->player1_done = isPlayer1 ? true : player1_done;
			//this->player2_done = isPlayer1 ? player2_done : true;
		});
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		// ֹͣ��ǰ֡����
		player->stopAllActions();
		// ��done��Ϊfalse
		GlobalVar::getInstance()->player1_done = isPlayer1 ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = isPlayer1 ? GlobalVar::getInstance()->player2_done : false;
		// ����Ϊ�·�����ƶ�֡����
		player->runAction(sequence);
		//�޸�player����
		if (nextDirection != "stop") {
			GlobalVar::getInstance()->player1_toward = isPlayer1 ? nextDirection : GlobalVar::getInstance()->player1_toward;
			GlobalVar::getInstance()->player2_toward = isPlayer1 ? GlobalVar::getInstance()->player2_toward : nextDirection;
		}
	}

	
}

// ����moveKey��isMove����ƶ�����
string Factory::getDirection(bool isMove_hori, bool isMove_vert, char moveKey_hori, char moveKey_vert) {
	// û��ˮƽ�ƶ�Ҳû�д�ֱ�ƶ�
	if (!isMove_hori && !isMove_vert) return "stop";
	// û�д�ֱ�ƶ���ֻ��ˮƽ�ƶ�������ΪWest, East
	if (isMove_hori && !isMove_vert) return (moveKey_hori == 'A') ? "West" : "East";
	// û��ˮƽ�ƶ���ֻ�д�ֱ�ƶ�������ΪNorth, South
	if (!isMove_hori && isMove_vert) return (moveKey_vert == 'W') ? "North" : "South";
	// ���д�ֱ�ƶ���Ҳ��ˮƽ�ƶ�
	if (moveKey_hori == 'A' && moveKey_vert == 'W') return "NorthWest";
	if (moveKey_hori == 'D' && moveKey_vert == 'W') return "NorthEast";
	if (moveKey_hori == 'A' && moveKey_vert == 'S') return "SouthWest";
	if (moveKey_hori == 'D' && moveKey_vert == 'S') return "SouthEast";
	// �쳣
	return "stop";
}




void Factory::removePlayer(Sprite* sp) {
	sp->stopAllActions();
	//GlobalVar::getInstance()->player1HP = 0;
	auto player_dead_texture = Director::getInstance()->getTextureCache()->addImage("grave.png");
	auto frame = SpriteFrame::createWithTexture(player_dead_texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 32, 43)));
	//auto grave = Sprite::createWithSpriteFrame(frame);
	sp->setSpriteFrame(frame);
	//GlobalVar::getInstance()->player1_dead = true;
	//sp->setScale(1.0);
	//sp->removeFromParent();
	player.eraseObject(sp);
	
}



// ��ײ���
Sprite* Factory::collider(Vec2 pos) {
	for (auto i : player) {
		//if (rect.containsPoint(i->getPosition())) {
		//return i;
		//}
		Rect m = i->getBoundingBox();
		if (m.containsPoint(pos)) {
			return i;
		}
	}
	return NULL;
}

// ��ҹ���
void Factory::attack(Sprite* Player, bool isPlayer1) {
	if (isPlayer1) {
		if (GlobalVar::getInstance()->player1HP <= 0) return;
		string output = "PLAYER1_shot_" + Value(GlobalVar::getInstance()->attack_Type_For_Player1).asString() +
			"	toward:  " + GlobalVar::getInstance()->player1_toward;
		CCLOG(output.c_str());
		shot(Player, GlobalVar::getInstance()->attack_Type_For_Player1, true);
	}
	else {
		if (GlobalVar::getInstance()->player2HP <= 0) return;
		string output = "PLAYER2_shot_" + Value(GlobalVar::getInstance()->attack_Type_For_Player2).asString() +
			"	toward: " + GlobalVar::getInstance()->player2_toward;

		CCLOG(output.c_str());
		shot(Player, GlobalVar::getInstance()->attack_Type_For_Player2, false);
	}
}

// ��Ұ���ͬ���������
void Factory::shot(Sprite* Player, int type, bool isPlayer1) {
	

	bool isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player1_isShotDone : GlobalVar::getInstance()->player2_isShotDone;
	//string moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player1_moving_direction : GlobalVar::getInstance()->player2_moving_direction;
	//string output = (isShotDone) ? "isShotDone:  true" : " isShotDone:  false";
	//CCLOG(output.c_str());

	/****************************************
	 * 1.pistol���
	 *	- ����ȴ�֡�����������ܿ�ʼ��һ��
	 *  - type mod 5 = 0
	 *	- ����: 20hp / shot
	 ******************************************/
	if (type == 0 && isShotDone) {
		// ��done��Ϊfalse
		GlobalVar::getInstance()->player1_isShotDone = (isPlayer1) ? false : GlobalVar::getInstance()->player1_isShotDone;
		GlobalVar::getInstance()->player2_isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player2_isShotDone : false;	//��������в���������
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;	
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//��������в����ƶ�
		
		
		//ֹͣ��ǰ֡����
		Player->stopAllActions();
		// ����֡��������
		string name = (isPlayer1)? ("player1_shot_" + GlobalVar::getInstance()->player1_toward):("player2_shot_" + GlobalVar::getInstance()->player2_toward);
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
		auto setDoneToTrue = CallFunc::create([&,isPlayer1]() {
			if (isPlayer1) {
				GlobalVar::getInstance()->player1_done = true;
				GlobalVar::getInstance()->player1_moving_direction = "stop";
				GlobalVar::getInstance()->player1_isShotDone = true;
			}
			else {
				GlobalVar::getInstance()->player2_done = true;
				GlobalVar::getInstance()->player2_moving_direction = "stop";
				GlobalVar::getInstance()->player2_isShotDone = true;
			}
		});
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		Player->runAction(sequence);
		// ������Ч
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/pistol.mp3", false);
	}
	
	//isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player1_isShotDone : GlobalVar::getInstance()->player2_isShotDone;
	//output = (isShotDone) ? "isShotDone:  true" : " isShotDone:  false";
	//CCLOG(output.c_str());
	//auto global = GlobalVar::getInstance();
	
	/************************** 
	 * 2.uzi ���
	 *	- ����������������õȴ�֡����ֹͣ
	 *	- ����:	5hp / shot
	 */
	if (type == 1 ) {
		// ��done��Ϊfalse
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//��������в����ƶ�

		//ֹͣ��ǰ֡����
		Player->stopAllActions();
		// ����֡��������
		string name = (isPlayer1) ? ("player1_shot_" + GlobalVar::getInstance()->player1_toward) : ("player2_shot_" + GlobalVar::getInstance()->player2_toward);
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
		auto setDoneToTrue = CallFunc::create([&, isPlayer1]() {
			if (isPlayer1) {
				GlobalVar::getInstance()->player1_done = true;
				GlobalVar::getInstance()->player1_moving_direction = "stop";
			}
			else {
				GlobalVar::getInstance()->player2_done = true;
				GlobalVar::getInstance()->player2_moving_direction = "stop";
			}
		});
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		Player->runAction(sequence);
		// ������Ч
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/uzi.mp3", false);
	}

	/**************************
	* 3.shotgun ���
	*	- ������������������õȴ�֡����ֹͣ
	*	- ����:	50hp / shot
	*/
	if (type == 2 && isShotDone) {
		// ��done��Ϊfalse
		GlobalVar::getInstance()->player1_isShotDone = (isPlayer1) ? false : GlobalVar::getInstance()->player1_isShotDone;
		GlobalVar::getInstance()->player2_isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player2_isShotDone : false;	//��������в���������
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//��������в����ƶ�


		//ֹͣ��ǰ֡����
		Player->stopAllActions();
		// ����֡��������
		string name = (isPlayer1) ? ("player1_shot_" + GlobalVar::getInstance()->player1_toward) : ("player2_shot_" + GlobalVar::getInstance()->player2_toward);
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
		auto setDoneToTrue = CallFunc::create([&, isPlayer1]() {

			if (isPlayer1) {
				GlobalVar::getInstance()->player1_done = true;
				GlobalVar::getInstance()->player1_moving_direction = "stop";
				GlobalVar::getInstance()->player1_isShotDone = true;
			}
			else {
				GlobalVar::getInstance()->player2_done = true;
				GlobalVar::getInstance()->player2_moving_direction = "stop";
				GlobalVar::getInstance()->player2_isShotDone = true;
			}
		});
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		Player->runAction(sequence);
		// ������Ч
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/shotgun.mp3", false);
	}
	
	/*********************
	 *	4.grenade ����
	 *	- ��������������
	 *	- ���� 100hp / shot
	 ************************/
	if (type == 3 && isShotDone) {
		// ��done��Ϊfalse
		GlobalVar::getInstance()->player1_isShotDone = (isPlayer1) ? false : GlobalVar::getInstance()->player1_isShotDone;
		GlobalVar::getInstance()->player2_isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player2_isShotDone : false;	//��������в���������
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//��������в����ƶ�


		//ֹͣ��ǰ֡����
		Player->stopAllActions();
		// ����֡��������
		string name = (isPlayer1) ? ("player1_shot_" + GlobalVar::getInstance()->player1_toward) : ("player2_shot_" + GlobalVar::getInstance()->player2_toward);
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
		auto setDoneToTrue = CallFunc::create([&, isPlayer1]() {

			if (isPlayer1) {
				GlobalVar::getInstance()->player1_done = true;
				GlobalVar::getInstance()->player1_moving_direction = "stop";
				GlobalVar::getInstance()->player1_isShotDone = true;
			}
			else {
				GlobalVar::getInstance()->player2_done = true;
				GlobalVar::getInstance()->player2_moving_direction = "stop";
				GlobalVar::getInstance()->player2_isShotDone = true;
			}
		});
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		Player->runAction(sequence);
		// ������Ч
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/grenade.mp3", false);
	}

	/*********************
	*	5.missile ���Ͳ
	*	- �������������
	*   - ���������ȴ��0.5s	
	*	��- ��ը���˺��� ����˵��ը��һ����Χ�����˺���Orʲô��
	*	- ���� 100hp / shot
	************************/
	if (type == 4 && isShotDone) {
		// ��done��Ϊfalse
		GlobalVar::getInstance()->player1_isShotDone = (isPlayer1) ? false : GlobalVar::getInstance()->player1_isShotDone;
		GlobalVar::getInstance()->player2_isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player2_isShotDone : false;	//��������в���������
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//��������в����ƶ�


		//ֹͣ��ǰ֡����
		Player->stopAllActions();
		// ����֡��������
		string name = (isPlayer1) ? ("player1_shot_" + GlobalVar::getInstance()->player1_toward) : ("player2_shot_" + GlobalVar::getInstance()->player2_toward);
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
		auto setDoneToTrue = CallFunc::create([&, isPlayer1]() {

			if (isPlayer1) {
				GlobalVar::getInstance()->player1_done = true;
				GlobalVar::getInstance()->player1_moving_direction = "stop";
				GlobalVar::getInstance()->player1_isShotDone = true;
			}
			else {
				GlobalVar::getInstance()->player2_done = true;
				GlobalVar::getInstance()->player2_moving_direction = "stop";
				GlobalVar::getInstance()->player2_isShotDone = true;
			}
		});
		auto sequence = Sequence::create(animate, DelayTime::create(0.5) , setDoneToTrue, NULL);
		Player->runAction(sequence);
		// ������Ч
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/missile.mp3", false);
	}


	/*********************
	*	6.laser_gun ����ǹ
	*	- �������������������д�͸Ч�������ڷ�Χ�ڵ����й��ﶼ�յ��˺�
	*	- ���� 100hp / shot
	************************/
	if (type == 5 && isShotDone) {
		// ��done��Ϊfalse
		GlobalVar::getInstance()->player1_isShotDone = (isPlayer1) ? false : GlobalVar::getInstance()->player1_isShotDone;
		GlobalVar::getInstance()->player2_isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player2_isShotDone : false;	//��������в���������
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//��������в����ƶ�


		//ֹͣ��ǰ֡����
		Player->stopAllActions();
		// ����֡��������
		string name = (isPlayer1) ? ("player1_shot_" + GlobalVar::getInstance()->player1_toward) : ("player2_shot_" + GlobalVar::getInstance()->player2_toward);
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
		auto setDoneToTrue = CallFunc::create([&, isPlayer1]() {

			if (isPlayer1) {
				GlobalVar::getInstance()->player1_done = true;
				GlobalVar::getInstance()->player1_moving_direction = "stop";
				GlobalVar::getInstance()->player1_isShotDone = true;
			}
			else {
				GlobalVar::getInstance()->player2_done = true;
				GlobalVar::getInstance()->player2_moving_direction = "stop";
				GlobalVar::getInstance()->player2_isShotDone = true;
			}
		});
		auto sequence = Sequence::create(animate, setDoneToTrue, NULL);
		Player->runAction(sequence);
		// ������Ч
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/laser_gun.mp3", false);
	}



}


// ������
void Factory::wasHit(Sprite* Player, int HP, bool isPlayer1) {
	if (isPlayer1) {
		GlobalVar::getInstance()->player1HP -= HP;
		if (GlobalVar::getInstance()->player1HP <= 0) removePlayer(Player);
		// ������Ч
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/dead1.mp3", false);
	}
	else {
		GlobalVar::getInstance()->player2HP -= HP;
		if (GlobalVar::getInstance()->player2HP <= 0) removePlayer(Player);
		// ������Ч
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/dead2.mp3", false);
	}
	
}

Vector<Sprite*> Factory::getPlayers()
{
	return player;
}

void Factory::clear() {
	player.clear();
}





