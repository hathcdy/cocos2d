#include "Monster.h"
#include "PlayGameScene.h"
USING_NS_CC;

MonsterFactory* MonsterFactory::factory = NULL;
MonsterFactory::MonsterFactory() {
	initSpriteFrame();
	tileMap = TMXTiledMap::create(GlobalVar::getInstance()->getMapFile());
	blockage = tileMap->layerNamed("Blockage");
}

MonsterFactory* MonsterFactory::getInstance() {
	if (factory == NULL) {
		factory = new MonsterFactory();
	}
	return factory;
}

void MonsterFactory::initSpriteFrame() {
	auto texture = Director::getInstance()->getTextureCache()->addImage("monster_move.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(512, 0, 128, 128)));
		monsterDead.pushBack(frame);
	}
}

//创建小怪物，tag为1
Sprite* MonsterFactory::createSmallMonster() {
	Sprite* mons = Sprite::create("monster1_move.png", CC_RECT_PIXELS_TO_POINTS(Rect(512, 0, 128, 128)));
	mons->setTag(1);
	monster.pushBack(mons);
	isDone.push_back(true);
	monsterHP.push_back(100);
	return mons;
}

//创建大怪物，tag为2
Sprite* MonsterFactory::createBigMonster() {
	Sprite* mons = Sprite::create("monster2_move.png", CC_RECT_PIXELS_TO_POINTS(Rect(512, 0, 128, 128)));
	mons->setTag(2);
	monster.pushBack(mons);
	isDone.push_back(true);
	monsterHP.push_back(300);
	return mons;
}

//创建大怪的子弹
Sprite* MonsterFactory::createMonsterBullet() {
	Sprite* bullet = Sprite::create("monster2_fire.jpg");
	monsterBullet.pushBack(bullet);
	return bullet;
}

void MonsterFactory::removeMonster(Sprite* sp) {
	int index = monster.getIndex(sp);
	auto fadeOut = FadeOut::create(1.0f);
	auto seq = Sequence::create(fadeOut, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sp)), NULL);
	sp->runAction(seq);
	isDone.erase(isDone.begin() + index);
	monsterHP.erase(monsterHP.begin() + index);
	monster.eraseObject(sp);
	if (attacker1.contains(sp)) {
		attacker1.eraseObject(sp);
	}
	if (attacker2.contains(sp)) {
		attacker2.eraseObject(sp);
	}
}

// 被手枪击中
void MonsterFactory::hitByPistol(Sprite* mons) {
	mons->stopAllActions();
	int index = monster.getIndex(mons);
	if (monsterHP[index] <= 20) {
		removeMonster(mons);
	}
	else {
		monsterHP[index] -= 20;
	}
}
// 被uzi击中
void MonsterFactory::hitByUzi(Sprite* mons) {
	mons->stopAllActions();
	int index = monster.getIndex(mons);
	if (monsterHP[index] <= 5) {
		removeMonster(mons);
	}
	else {
		monsterHP[index] -= 5;
	}
}
// 被霰弹击中
void MonsterFactory::hitByShotGun(Sprite* mons) {
	mons->stopAllActions();
	int index = monster.getIndex(mons);
	if (monsterHP[index] <= 50) {
		removeMonster(mons);
	}
	else {
		monsterHP[index] -= 50;
	}
}
// 被手雷击中
void MonsterFactory::hitByGrenade(Sprite* mons) {
	mons->stopAllActions();
	int index = monster.getIndex(mons);
	if (monsterHP[index] <= 100) {
		removeMonster(mons);
	}
	else {
		monsterHP[index] -= 100;
	}
}
// 被火箭击中
void MonsterFactory::hitByMissile(Sprite* mons) {
	mons->stopAllActions();
	int index = monster.getIndex(mons);
	if (monsterHP[index] <= 100) {
		removeMonster(mons);
	}
	else {
		monsterHP[index] -= 100;
	}
}
// 被激光击中
void MonsterFactory::hitByLaserGun(Sprite* mons) {
	mons->stopAllActions();
	int index = monster.getIndex(mons);
	if (monsterHP[index] <= 100) {
		removeMonster(mons);
	}
	else {
		monsterHP[index] -= 100;
	}
}

std::string MonsterFactory::getDirection(Vec2 dir) {
	if (dir.x < 0) {
		// (-1, -1) southwest
		if (dir.y < 0) {
			return Direction[7];
		}
		// (-1， 0) west
		else if (dir.y == 0) {
			return Direction[0];
		}
		// (-1， 1) northwest
		else {
			return Direction[1];
		}
	}
	else if (dir.x == 0) {
		// (0， -1) south
		if (dir.y < 0) {
			return Direction[6];
		}
		// (0， 1) north
		else if (dir.y > 0) {
			return Direction[2];
		}
	}
	else {
		// (1， -1) southeast
		if (dir.y < 0) {
			return Direction[5];
		}
		// (1， 0) east
		else if (dir.y == 0) {
			return Direction[4];
		}
		// (1， 1) northeast
		else {
			return Direction[3];
		}
	}
}

bool MonsterFactory::CanMove(Sprite* mons, Vec2 dir) {
	bool canMove = true;
	Point position = mons->getPosition() + dir * 20;
	Point tileCoord = this->getPositionInMap(position);
	int tileGid = blockage->getTileGIDAt(tileCoord);
	if (tileGid) {
		auto properties = tileMap->getPropertiesForGID(tileGid).asValueMap();
		if (!properties.empty()) {
			//地图上的障碍物，不能移动到这里
			auto collision = properties["Block"].asString();
			if ("true" == collision) canMove = false;
		}
	}
	// 判断目的地是否有其他怪物
	for (auto j : monster) {
		if (mons != j) {
			Rect otherMonster = j->getBoundingBox();
			if (otherMonster.containsPoint(position)) {
				canMove = false;
			}
		}
	}
	return canMove;
}

void MonsterFactory::moveMonster(Vec2 playerPos1, Vec2 playerPos2, float time) {
	tileMap = TMXTiledMap::create(GlobalVar::getInstance()->getMapFile());
	blockage = tileMap->layerNamed("Blockage");
	for (int i = 0; i < monster.size(); i++) {
		bool canMove = true;
		if (attacker1.contains(monster.at(i)) || attacker2.contains(monster.at(i))) continue;
		Vec2 Pos = monster.at(i)->getPosition();
		Vec2 dir = playerPos1 - Pos;
		if (GlobalVar::getInstance()->getDoublePlayer()) {
			if ((Pos.getDistance(playerPos1) > Pos.getDistance(playerPos2) || playerPos1 == Vec2(0, 0)) && playerPos2 != Vec2(0, 0)) {
				dir = playerPos2 - Pos;
			}
		}
		dir.normalize();
		std::string prename = (monster.at(i)->getTag() == 1) ? "monster1_move_" : "monster2_move_";
		std::string direction = getDirection(dir);
		std::string name = prename + direction;
		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
		if (isDone[i]) {
			// 播放动画后将对应isDone的值设为true
			auto setDoneToTrue = CallFunc::create([this, i]() {
				isDone[i] = true;
			});
			//auto seq = Sequence::createWithTwoActions(animate, setDoneToTrue);
			monster.at(i)->runAction(animate);
			if (i >= 0 && i < monster.size()) {
				isDone[i] = true;
			}
		}
		// 判断目的地是否有障碍物
		Point position = monster.at(i)->getPosition() + dir * 15;
		Point tileCoord = this->getPositionInMap(position);
		int tileGid = blockage->getTileGIDAt(tileCoord);
		if (tileGid) {
			auto properties = tileMap->getPropertiesForGID(tileGid).asValueMap();
			if (!properties.empty()) {
				//地图上的障碍物，不能移动到这里
				auto collision = properties["Block"].asString();
				if ("true" == collision) canMove = false;
			}
		}
		for (auto j : monster) {
			if (monster.at(i) != j) {
				Rect otherMonster = j->getBoundingBox();
				if (otherMonster.containsPoint(position)) {
					canMove = false;
				}
			}
		}
		if (canMove) {
			monster.at(i)->runAction(MoveBy::create(time, dir * 15));
		}
		//i->setPosition(currentPosition.x + 4 * dir.x, currentPosition.y + 4 * dir.y);
	}
}

//void MonsterFactory::moveMonster(Vec2 playerPos1, Vec2 playerPos2, float time) {
//	tileMap = TMXTiledMap::create(GlobalVar::getInstance()->getMapFile());
//	blockage = tileMap->layerNamed("Blockage");
//	for (int i = 0; i < monster.size(); i++) {
//		bool canMove = true;
//		if (attacker1.contains(monster.at(i)) || attacker2.contains(monster.at(i))) continue;
//		Vec2 Pos = monster.at(i)->getPosition();
//		Vec2 dir = playerPos1 - Pos;
//		if (GlobalVar::getInstance()->getDoublePlayer()) {
//			if (Pos.getDistance(playerPos1) > Pos.getDistance(playerPos2)) {
//				dir = playerPos2 - Pos;
//			}
//		}
//		dir.normalize();
//		Vec2 nextDir = dir;
//		canMove = CanMove(monster.at(i), nextDir);
//		while (!canMove) {
//			float nextX = nextDir.x * cos(30.0*M_PI / 180.0) + nextDir.y * sin(30.0*M_PI / 180.0);
//			float nextY = -nextDir.x * sin(30.0*M_PI / 180.0) + nextDir.y * cos(30.0*M_PI / 180.0);
//			nextDir = Vec2(nextX, nextY);
//			if (nextDir - dir < Vec2(0.05, 0.05)) {
//				break;
//			}
//			canMove = CanMove(monster.at(i), nextDir);
//		}
//		dir = nextDir;
//		std::string prename = (monster.at(i)->getTag() == 1) ? "monster1_move_" : "monster2_move_";
//		std::string direction = getDirection(dir);
//		std::string name = prename + direction;
//		Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
//		if (isDone[i]) {
//			// 播放动画后将对应isDone的值设为true
//			auto setDoneToTrue = CallFunc::create([this, i]() {
//				isDone[i] = true;
//			});
//			auto seq = Sequence::createWithTwoActions(animate, setDoneToTrue);
//			monster.at(i)->runAction(seq);
//		}
//		if (canMove) {
//			monster.at(i)->runAction(MoveBy::create(time, dir * 10));
//		}
//		//i->setPosition(currentPosition.x + 4 * dir.x, currentPosition.y + 4 * dir.y);
//	}
//}





// 检查角色是否在怪物攻击范围内
void MonsterFactory::checkAttacker(Vec2 playerPos1, Vec2 playerPos2) {
	for (auto i : monster) {
		// 小怪
		if (i->getTag() == 1) {
			Rect monsterRect = i->getBoundingBox();
			// 怪物攻击范围为自身周围10
			Rect attackRect = Rect(monsterRect.getMinX() - 5, monsterRect.getMinY() + 5,
				monsterRect.getMaxX() - monsterRect.getMinX() + 10, monsterRect.getMaxY() - monsterRect.getMinY() + 10);
			// 角色在怪物攻击范围内且怪物不在attacker1中，怪物加入attacker1
			if (attackRect.containsPoint(playerPos1) && GlobalVar::getInstance()->player1HP > 0) {
				if (!attacker1.contains(i)) {
					attacker1.pushBack(i);
				}
			}
			// 否则，将怪物从attacker1中移除（若存在）
			else {
				if (attacker1.contains(i)) {
					attacker1.eraseObject(i);
				}
			}
			// 双人模式
			if (GlobalVar::getInstance()->getDoublePlayer()) {
				// 怪物与P1间距离大于与P2间距离
				if (i->getPosition().getDistance(playerPos1) > i->getPosition().getDistance(playerPos2) || GlobalVar::getInstance()->player1HP > 0) {
					// P2在怪物攻击范围内
					if (attackRect.containsPoint(playerPos2) && GlobalVar::getInstance()->player2HP > 0) {
						// 如果原来目标为P1,转换目标为P2
						if (attacker1.contains(i)) {
							attacker1.eraseObject(i);
						}
						if (!attacker2.contains(i)) {
							attacker2.pushBack(i);
						}
					}
					// 否则，将怪物从attacker2中移除（若存在）
					else {
						if (attacker2.contains(i)) {
							attacker2.eraseObject(i);
						}
					}
				}
			}
		}
		// 大怪
		else {
			// 大怪攻击范围为自身为圆心半径350的圆
			if (i->getPosition().getDistance(playerPos1) <= 350 && GlobalVar::getInstance()->player1HP > 0) {
				if (!attacker1.contains(i)) {
					attacker1.pushBack(i);
				}
			}
			// 否则从attacker1移除（若存在）
			else {
				if (attacker1.contains(i)) {
					attacker1.eraseObject(i);
				}
			}
			// 双人模式
			if (GlobalVar::getInstance()->getDoublePlayer()) {
				// 怪物与P1间距离大于与P2间距离
				if (i->getPosition().getDistance(playerPos1) > i->getPosition().getDistance(playerPos2) || GlobalVar::getInstance()->player1HP <= 0) {
					// P2在怪物攻击范围内
					if (i->getPosition().getDistance(playerPos2) <= 350 && GlobalVar::getInstance()->player2HP > 0) {
						// 如果原来目标为P1,转换目标为P2
						if (attacker1.contains(i)) {
							attacker1.eraseObject(i);
						}
						if (!attacker2.contains(i)) {
							attacker2.pushBack(i);
						}
					}
					// 否则，将怪物从attacker2中移除（若存在）
					else {
						if (attacker2.contains(i)) {
							attacker2.eraseObject(i);
						}
					}
				}
			}
		}
	}

}

Sprite* MonsterFactory::collider(Rect rect) {
	for (auto i : monster) {
		//if (rect.containsPoint(i->getPosition())) {
			//return i;
		//}
		Rect m = i->getBoundingBox();
		if (m.intersectsRect(rect) || rect.intersectsRect(m)) {
			return i;
		}
	}
	return NULL;
}

//计算位置在地图中的坐标
Point MonsterFactory::getPositionInMap(Point position) {
	//将怪物的目的地坐标的x轴坐标转换成瓦片地图中的x轴的坐标	
	int x = position.x / tileMap->getTileSize().width;
	//将怪物的目的地坐标的y轴坐标转换成瓦片地图中的y轴的坐标
	int y = ((tileMap->getMapSize().height * tileMap->getTileSize().height) - position.y) / tileMap->getTileSize().height;
	return Point(x, y);
}

//得到怪物
Vector<Sprite*> MonsterFactory::getMonster() {
	return monster;
}

Vector<Sprite*> MonsterFactory::getAttacker1() {
	return attacker1;
}

Vector<Sprite*> MonsterFactory::getAttacker2() {
	return attacker2;
}

Vector<Sprite*> MonsterFactory::getMonsterBullet() {
	return monsterBullet;
}

vector<bool> MonsterFactory::getIsDone() {
	return isDone;
}

void MonsterFactory::wasHit(Sprite* mons, int type) {
	int index = monster.getIndex(mons);
	mons->stopAllActions();
	switch (type) {
		// pistol 伤害30
	case 0: {
		if (monsterHP[index] <= 30) {
			removeMonster(mons);
		}
		else {
			monsterHP[index] -= 30;
		}
		break;
	}
			// uzi
	case 1: {
		if (monsterHP[index] <= 20) {
			removeMonster(mons);
		}
		else {
			monsterHP[index] -= 20;
		}
		break;
	}
			// shotgun
	case 2: {
		if (monsterHP[index] <= 50) {
			removeMonster(mons);
		}
		else {
			monsterHP[index] -= 50;
		}
		break;
	}
			// grande
	case 3: {
		if (monsterHP[index] <= 80) {
			removeMonster(mons);
		}
		else {
			monsterHP[index] -= 80;
		}
		break;
	}
			// missile
	case 4: {
		if (monsterHP[index] <= 150) {
			removeMonster(mons);
		}
		else {
			monsterHP[index] -= 150;
		}
		break;
	}
			// lasergun
	case 5: {
		if (monsterHP[index] <= 100) {
			removeMonster(mons);
		}
		else {
			monsterHP[index] -= 100;
		}
		break;
	}
	default:
		break;
	}
}

void MonsterFactory::setIsDone(int index, bool isdone) {
	isDone[index] = isdone;
}

void MonsterFactory::removeMonsterBullet(Sprite* bullet) {
	monsterBullet.eraseObject(bullet);
}

// 初始化,游戏开始或结束时调用
void MonsterFactory::clear() {
	monster.clear();
	attacker1.clear();
	attacker2.clear();
	monsterBullet.clear();
	monsterHP.clear();
	isDone.clear();
}

void MonsterFactory::clearAttacker1() {
	attacker1.clear();
}

void MonsterFactory::clearAttacker2() {
	attacker2.clear();
}




