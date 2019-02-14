#include"Player.h"
#include"GlobalVar.h"
#include "PlayGameScene.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;

Factory* Factory::factory = NULL;

Factory::Factory() {
	initSpriteFrame();
}

//获取单例工厂
Factory* Factory::getInstance() {
	if (factory == NULL) {
		factory = new Factory();
	}
	return factory;
}

//初始化玩家帧动画
void Factory::initSpriteFrame() {
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258 - 48 * i, 0, 42, 42)));
		monsterDead.pushBack(frame);
	}
}

// 创建玩家，添加到玩家Vector队列player
Vector<Sprite*> Factory::createPlayer() {

	//生成玩家，并存储到容器中管理
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

//让玩家更新移动动作的帧动画

void Factory::update_Player_Animation(Sprite* player, bool isPlayer1) {
	// 如果玩家已经死亡，不更新
	if ((isPlayer1&&GlobalVar::getInstance()->player1HP <= 0) || (!isPlayer1) && GlobalVar::getInstance()->player2HP <= 0) return;
	bool isMove_hori = isPlayer1 ? GlobalVar::getInstance()->player1_isMove_hori : GlobalVar::getInstance()->player2_isMove_hori;
	bool isMove_vert = isPlayer1 ? GlobalVar::getInstance()->player1_isMove_vert : GlobalVar::getInstance()->player2_isMove_vert;
	char moveKey_hori = isPlayer1 ? GlobalVar::getInstance()->player1_moveKey_hori : GlobalVar::getInstance()->player2_moveKey_hori;
	char moveKey_vert = isPlayer1 ? GlobalVar::getInstance()->player1_moveKey_vert : GlobalVar::getInstance()->player2_moveKey_vert;
	bool done = isPlayer1 ? GlobalVar::getInstance()->player1_done : GlobalVar::getInstance()->player2_done;
	string current_direction = isPlayer1 ? GlobalVar::getInstance()->player1_moving_direction : GlobalVar::getInstance()->player2_moving_direction;
	string prename = (isPlayer1) ? "player1_move_" : "player2_move_";
	//bool isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player1_isShotDone : GlobalVar::getInstance()->player2_isShotDone;
	// 获得player1下一步的移动方向
	string nextDirection = getDirection(isMove_hori, isMove_vert, moveKey_hori, moveKey_vert);

	//如果玩家当前在射击，移动帧动画不进行更新，直接返回
	if (current_direction == "shot") return;

	// 若下一步移动方向为静止，则停止所有帧动画
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

	// 若下一步移动方向跟现在的方向一致， 接连循环播放该方向动画
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
		// 将done设为false
		GlobalVar::getInstance()->player1_done = isPlayer1 ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = isPlayer1 ? GlobalVar::getInstance()->player2_done : false;
		player->runAction(sequence);

	}

	// 若下一步移动方向跟现在的方向不一致
	// 终止当前方向的帧动画，更新为新方向的帧动画
	if (nextDirection != current_direction) {
		GlobalVar::getInstance()->player1_moving_direction = isPlayer1 ? nextDirection : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = isPlayer1 ? GlobalVar::getInstance()->player2_moving_direction : nextDirection;
		// 获得新方向的帧动画
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
		// 停止当前帧动画
		player->stopAllActions();
		// 将done设为false
		GlobalVar::getInstance()->player1_done = isPlayer1 ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = isPlayer1 ? GlobalVar::getInstance()->player2_done : false;
		// 更新为新方向的移动帧动画
		player->runAction(sequence);
		//修改player朝向
		if (nextDirection != "stop") {
			GlobalVar::getInstance()->player1_toward = isPlayer1 ? nextDirection : GlobalVar::getInstance()->player1_toward;
			GlobalVar::getInstance()->player2_toward = isPlayer1 ? GlobalVar::getInstance()->player2_toward : nextDirection;
		}
	}

	
}

// 根据moveKey和isMove获得移动方向
string Factory::getDirection(bool isMove_hori, bool isMove_vert, char moveKey_hori, char moveKey_vert) {
	// 没有水平移动也没有垂直移动
	if (!isMove_hori && !isMove_vert) return "stop";
	// 没有垂直移动，只有水平移动，方向为West, East
	if (isMove_hori && !isMove_vert) return (moveKey_hori == 'A') ? "West" : "East";
	// 没有水平移动，只有垂直移动，方向为North, South
	if (!isMove_hori && isMove_vert) return (moveKey_vert == 'W') ? "North" : "South";
	// 既有垂直移动，也有水平移动
	if (moveKey_hori == 'A' && moveKey_vert == 'W') return "NorthWest";
	if (moveKey_hori == 'D' && moveKey_vert == 'W') return "NorthEast";
	if (moveKey_hori == 'A' && moveKey_vert == 'S') return "SouthWest";
	if (moveKey_hori == 'D' && moveKey_vert == 'S') return "SouthEast";
	// 异常
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



// 碰撞检测
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

// 玩家攻击
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

// 玩家按不同武器的射击
void Factory::shot(Sprite* Player, int type, bool isPlayer1) {
	

	bool isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player1_isShotDone : GlobalVar::getInstance()->player2_isShotDone;
	//string moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player1_moving_direction : GlobalVar::getInstance()->player2_moving_direction;
	//string output = (isShotDone) ? "isShotDone:  true" : " isShotDone:  false";
	//CCLOG(output.c_str());

	/****************************************
	 * 1.pistol射击
	 *	- 必须等待帧动画结束才能开始下一发
	 *  - type mod 5 = 0
	 *	- 威力: 20hp / shot
	 ******************************************/
	if (type == 0 && isShotDone) {
		// 将done设为false
		GlobalVar::getInstance()->player1_isShotDone = (isPlayer1) ? false : GlobalVar::getInstance()->player1_isShotDone;
		GlobalVar::getInstance()->player2_isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player2_isShotDone : false;	//射击过程中不允许连发
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;	
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//射击过程中不能移动
		
		
		//停止当前帧动画
		Player->stopAllActions();
		// 设置帧动画序列
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
		// 设置音效
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/pistol.mp3", false);
	}
	
	//isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player1_isShotDone : GlobalVar::getInstance()->player2_isShotDone;
	//output = (isShotDone) ? "isShotDone:  true" : " isShotDone:  false";
	//CCLOG(output.c_str());
	//auto global = GlobalVar::getInstance();
	
	/************************** 
	 * 2.uzi 射击
	 *	- 可以连续射击，不用等待帧动画停止
	 *	- 威力:	5hp / shot
	 */
	if (type == 1 ) {
		// 将done设为false
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//射击过程中不能移动

		//停止当前帧动画
		Player->stopAllActions();
		// 设置帧动画序列
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
		// 设置音效
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/uzi.mp3", false);
	}

	/**************************
	* 3.shotgun 射击
	*	- 不可以连续射击，不用等待帧动画停止
	*	- 威力:	50hp / shot
	*/
	if (type == 2 && isShotDone) {
		// 将done设为false
		GlobalVar::getInstance()->player1_isShotDone = (isPlayer1) ? false : GlobalVar::getInstance()->player1_isShotDone;
		GlobalVar::getInstance()->player2_isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player2_isShotDone : false;	//射击过程中不允许连发
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//射击过程中不能移动


		//停止当前帧动画
		Player->stopAllActions();
		// 设置帧动画序列
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
		// 设置音效
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/shotgun.mp3", false);
	}
	
	/*********************
	 *	4.grenade 手榴弹
	 *	- 不可以连续抛掷
	 *	- 威力 100hp / shot
	 ************************/
	if (type == 3 && isShotDone) {
		// 将done设为false
		GlobalVar::getInstance()->player1_isShotDone = (isPlayer1) ? false : GlobalVar::getInstance()->player1_isShotDone;
		GlobalVar::getInstance()->player2_isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player2_isShotDone : false;	//射击过程中不允许连发
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//射击过程中不能移动


		//停止当前帧动画
		Player->stopAllActions();
		// 设置帧动画序列
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
		// 设置音效
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/grenade.mp3", false);
	}

	/*********************
	*	5.missile 火箭筒
	*	- 不可以连续射击
	*   - 射击额外冷却：0.5s	
	*	？- 爆炸性伤害？ 例如说爆炸的一定范围内有伤害？Or什么？
	*	- 威力 100hp / shot
	************************/
	if (type == 4 && isShotDone) {
		// 将done设为false
		GlobalVar::getInstance()->player1_isShotDone = (isPlayer1) ? false : GlobalVar::getInstance()->player1_isShotDone;
		GlobalVar::getInstance()->player2_isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player2_isShotDone : false;	//射击过程中不允许连发
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//射击过程中不能移动


		//停止当前帧动画
		Player->stopAllActions();
		// 设置帧动画序列
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
		// 设置音效
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/missile.mp3", false);
	}


	/*********************
	*	6.laser_gun 激光枪
	*	- 不可以连续抛掷，具有穿透效果，即在范围内的所有怪物都收到伤害
	*	- 威力 100hp / shot
	************************/
	if (type == 5 && isShotDone) {
		// 将done设为false
		GlobalVar::getInstance()->player1_isShotDone = (isPlayer1) ? false : GlobalVar::getInstance()->player1_isShotDone;
		GlobalVar::getInstance()->player2_isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player2_isShotDone : false;	//射击过程中不允许连发
		GlobalVar::getInstance()->player1_done = (isPlayer1) ? false : GlobalVar::getInstance()->player1_done;
		GlobalVar::getInstance()->player2_done = (isPlayer1) ? GlobalVar::getInstance()->player2_done : false;
		GlobalVar::getInstance()->player1_moving_direction = (isPlayer1) ? "shot" : GlobalVar::getInstance()->player1_moving_direction;
		GlobalVar::getInstance()->player2_moving_direction = (isPlayer1) ? GlobalVar::getInstance()->player2_moving_direction : "shot";//射击过程中不能移动


		//停止当前帧动画
		Player->stopAllActions();
		// 设置帧动画序列
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
		// 设置音效
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/laser_gun.mp3", false);
	}



}


// 被击中
void Factory::wasHit(Sprite* Player, int HP, bool isPlayer1) {
	if (isPlayer1) {
		GlobalVar::getInstance()->player1HP -= HP;
		if (GlobalVar::getInstance()->player1HP <= 0) removePlayer(Player);
		// 设置音效
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/dead1.mp3", false);
	}
	else {
		GlobalVar::getInstance()->player2HP -= HP;
		if (GlobalVar::getInstance()->player2HP <= 0) removePlayer(Player);
		// 设置音效
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





