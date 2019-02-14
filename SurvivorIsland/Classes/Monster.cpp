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

//����С���tagΪ1
Sprite* MonsterFactory::createSmallMonster() {
	Sprite* mons = Sprite::create("monster1_move.png", CC_RECT_PIXELS_TO_POINTS(Rect(512, 0, 128, 128)));
	mons->setTag(1);
	monster.pushBack(mons);
	isDone.push_back(true);
	monsterHP.push_back(100);
	return mons;
}

//��������tagΪ2
Sprite* MonsterFactory::createBigMonster() {
	Sprite* mons = Sprite::create("monster2_move.png", CC_RECT_PIXELS_TO_POINTS(Rect(512, 0, 128, 128)));
	mons->setTag(2);
	monster.pushBack(mons);
	isDone.push_back(true);
	monsterHP.push_back(300);
	return mons;
}

//������ֵ��ӵ�
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

// ����ǹ����
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
// ��uzi����
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
// ����������
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
// �����׻���
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
// ���������
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
// ���������
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
		// (-1�� 0) west
		else if (dir.y == 0) {
			return Direction[0];
		}
		// (-1�� 1) northwest
		else {
			return Direction[1];
		}
	}
	else if (dir.x == 0) {
		// (0�� -1) south
		if (dir.y < 0) {
			return Direction[6];
		}
		// (0�� 1) north
		else if (dir.y > 0) {
			return Direction[2];
		}
	}
	else {
		// (1�� -1) southeast
		if (dir.y < 0) {
			return Direction[5];
		}
		// (1�� 0) east
		else if (dir.y == 0) {
			return Direction[4];
		}
		// (1�� 1) northeast
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
			//��ͼ�ϵ��ϰ�������ƶ�������
			auto collision = properties["Block"].asString();
			if ("true" == collision) canMove = false;
		}
	}
	// �ж�Ŀ�ĵ��Ƿ�����������
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
			// ���Ŷ����󽫶�ӦisDone��ֵ��Ϊtrue
			auto setDoneToTrue = CallFunc::create([this, i]() {
				isDone[i] = true;
			});
			//auto seq = Sequence::createWithTwoActions(animate, setDoneToTrue);
			monster.at(i)->runAction(animate);
			if (i >= 0 && i < monster.size()) {
				isDone[i] = true;
			}
		}
		// �ж�Ŀ�ĵ��Ƿ����ϰ���
		Point position = monster.at(i)->getPosition() + dir * 15;
		Point tileCoord = this->getPositionInMap(position);
		int tileGid = blockage->getTileGIDAt(tileCoord);
		if (tileGid) {
			auto properties = tileMap->getPropertiesForGID(tileGid).asValueMap();
			if (!properties.empty()) {
				//��ͼ�ϵ��ϰ�������ƶ�������
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
//			// ���Ŷ����󽫶�ӦisDone��ֵ��Ϊtrue
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





// ����ɫ�Ƿ��ڹ��﹥����Χ��
void MonsterFactory::checkAttacker(Vec2 playerPos1, Vec2 playerPos2) {
	for (auto i : monster) {
		// С��
		if (i->getTag() == 1) {
			Rect monsterRect = i->getBoundingBox();
			// ���﹥����ΧΪ������Χ10
			Rect attackRect = Rect(monsterRect.getMinX() - 5, monsterRect.getMinY() + 5,
				monsterRect.getMaxX() - monsterRect.getMinX() + 10, monsterRect.getMaxY() - monsterRect.getMinY() + 10);
			// ��ɫ�ڹ��﹥����Χ���ҹ��ﲻ��attacker1�У��������attacker1
			if (attackRect.containsPoint(playerPos1) && GlobalVar::getInstance()->player1HP > 0) {
				if (!attacker1.contains(i)) {
					attacker1.pushBack(i);
				}
			}
			// ���򣬽������attacker1���Ƴ��������ڣ�
			else {
				if (attacker1.contains(i)) {
					attacker1.eraseObject(i);
				}
			}
			// ˫��ģʽ
			if (GlobalVar::getInstance()->getDoublePlayer()) {
				// ������P1����������P2�����
				if (i->getPosition().getDistance(playerPos1) > i->getPosition().getDistance(playerPos2) || GlobalVar::getInstance()->player1HP > 0) {
					// P2�ڹ��﹥����Χ��
					if (attackRect.containsPoint(playerPos2) && GlobalVar::getInstance()->player2HP > 0) {
						// ���ԭ��Ŀ��ΪP1,ת��Ŀ��ΪP2
						if (attacker1.contains(i)) {
							attacker1.eraseObject(i);
						}
						if (!attacker2.contains(i)) {
							attacker2.pushBack(i);
						}
					}
					// ���򣬽������attacker2���Ƴ��������ڣ�
					else {
						if (attacker2.contains(i)) {
							attacker2.eraseObject(i);
						}
					}
				}
			}
		}
		// ���
		else {
			// ��ֹ�����ΧΪ����ΪԲ�İ뾶350��Բ
			if (i->getPosition().getDistance(playerPos1) <= 350 && GlobalVar::getInstance()->player1HP > 0) {
				if (!attacker1.contains(i)) {
					attacker1.pushBack(i);
				}
			}
			// �����attacker1�Ƴ��������ڣ�
			else {
				if (attacker1.contains(i)) {
					attacker1.eraseObject(i);
				}
			}
			// ˫��ģʽ
			if (GlobalVar::getInstance()->getDoublePlayer()) {
				// ������P1����������P2�����
				if (i->getPosition().getDistance(playerPos1) > i->getPosition().getDistance(playerPos2) || GlobalVar::getInstance()->player1HP <= 0) {
					// P2�ڹ��﹥����Χ��
					if (i->getPosition().getDistance(playerPos2) <= 350 && GlobalVar::getInstance()->player2HP > 0) {
						// ���ԭ��Ŀ��ΪP1,ת��Ŀ��ΪP2
						if (attacker1.contains(i)) {
							attacker1.eraseObject(i);
						}
						if (!attacker2.contains(i)) {
							attacker2.pushBack(i);
						}
					}
					// ���򣬽������attacker2���Ƴ��������ڣ�
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

//����λ���ڵ�ͼ�е�����
Point MonsterFactory::getPositionInMap(Point position) {
	//�������Ŀ�ĵ������x������ת������Ƭ��ͼ�е�x�������	
	int x = position.x / tileMap->getTileSize().width;
	//�������Ŀ�ĵ������y������ת������Ƭ��ͼ�е�y�������
	int y = ((tileMap->getMapSize().height * tileMap->getTileSize().height) - position.y) / tileMap->getTileSize().height;
	return Point(x, y);
}

//�õ�����
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
		// pistol �˺�30
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

// ��ʼ��,��Ϸ��ʼ�����ʱ����
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




