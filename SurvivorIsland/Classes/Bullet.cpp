#include"Bullet.h"
#include"GlobalVar.h"
USING_NS_CC;

bulletFactory* bulletFactory::bulletfactory = NULL;
bulletFactory::bulletFactory() {
	initSpriteFrame();
}

bulletFactory* bulletFactory::getInstance() {
	if (bulletfactory == NULL) {
		bulletfactory = new bulletFactory();
	}
	return bulletfactory;
}

// 产生子弹，进入Vector中，返回子弹Sprite*
Sprite* bulletFactory::createBullet(int type, bool isPlayer1) {
	
	// 根据不同类型的子弹，创建到场景子弹队列中，同时剩余弹药数要减一
	// 如果子弹不足，返回NULL
	if (isPlayer1 && bulletNumForPlayer1[type] <= 0) return NULL;
	if (!isPlayer1 && bulletNumForPlayer2[type] <= 0) return NULL;
	
	// 判断是否执行完射击动作(除了UZI都不能连发)
	bool isShotDone = (isPlayer1) ? GlobalVar::getInstance()->player1_isShotDone : GlobalVar::getInstance()->player2_isShotDone;
	if (type != 1 && !isShotDone) return NULL;
	std::string pic = "bullet" + Value(type+1).asString() + ".png";
	Sprite* b = Sprite::create(pic.c_str());
	setScale_and_setRotation(b, type, isPlayer1);
	if (isPlayer1) {
		bulletForPlayer1[type].pushBack(b);
		bulletNumForPlayer1[type]--;
	}
	else {
		bulletForPlayer2[type].pushBack(b);
		bulletNumForPlayer2[type]--;
	}
	return b;
}

//移除子弹
void bulletFactory::removeBullet(Sprite* sp, int type, bool isPlayer1) {
	/*
	Animation* anim = Animation::createWithSpriteFrames(monsterDead, 0.2);
	Animate* ani = Animate::create(anim);
	Sequence* seq = Sequence::create(ani, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sp)), NULL);
	sp->runAction(seq);
	monster.eraseObject(sp);
	*/
	if (isPlayer1) {
		bulletForPlayer1[type].eraseObject(sp);
	}
	else {
		bulletForPlayer2[type].eraseObject(sp);
	}
	sp->removeFromParent();
}

void bulletFactory::moveMonster(Vec2 playerPos, float time) {
	/*
	for (auto i : monster) {
		Vec2 Pos = i->getPosition();
		Vec2 dir = playerPos - Pos;
		dir.normalize();
		i->runAction(MoveBy::create(time, dir * 30));
	}
	*/
}

//判断碰撞
Sprite* bulletFactory::collider(Rect rect) {
	/*
	for (auto i : monster) {
		if (rect.containsPoint(i->getPosition())) {
			return i;
		}
	}
	return NULL;
	*/
	return NULL;
}

void bulletFactory::initSpriteFrame() {
	/*
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258 - 48 * i, 0, 42, 42)));
		monsterDead.pushBack(frame);
	}
	*/
	//auto bullet1_texture = Director::getInstance()->getTextureCache()->addImage("bullet1");
}

// 获得现有子弹数量
int bulletFactory::getBulletNum(int type, bool isPlayer1)
{
	if (isPlayer1) return bulletNumForPlayer1[type];
	return bulletNumForPlayer2[type];
}

// 获得场景中子弹的队列
Vector<Sprite*> bulletFactory::getBulletNumInScene(int type, bool isPlayer1)
{
	if (isPlayer1) return bulletForPlayer1[type];
	return bulletForPlayer2[type];

}

// 根据子弹类型、玩家朝向调节子弹大小和方向和锚点(激光枪)
void bulletFactory::setScale_and_setRotation(Sprite* b, int type, bool isPlayer1)
{

	// 获得玩家的朝向toward
	std::string toward = (isPlayer1) ? GlobalVar::getInstance()->player1_toward : GlobalVar::getInstance()->player2_toward;
	//根据type调整精灵大小，根据toward设置锚点
	switch (type)
	{
	case 0 :
		b->setScale(0.5);
		break;
	case 1 :
		b->setScaleY(0.5);
		break;
	case 2 :
		b->setScale(0.5);
		break;
	case 3 :
		b->setScale(0.2);
		break;
	case 4 :
		b->setScaleY(1.5);
		break;
	case 5 :
		b->setScaleY(0.5);
		b->ignoreAnchorPointForPosition(false);
		b->setAnchorPoint(ccp(1, 0));
		break;
	default:
		break;
	}

	//根据朝向toward设置子弹旋转
	float angle;
	for (int i = 0; i < 8; i++) {
		if (toward == GlobalVar::getInstance()->Direction[i]) {
			angle = i * 45;
			break;
		}
	}
	b->setRotation(angle);
}


// 子弹发射之后的轨迹运行
void bulletFactory::moveBullet(Sprite * b, int type, bool isPlayer1) {
	// 获得玩家朝向
	std::string toward = (isPlayer1) ? GlobalVar::getInstance()->player1_toward : GlobalVar::getInstance()->player2_toward;
	Vec2 nextNearPos = Vec2(0, 0);
	Vec2 nextFarPos = Vec2(0, 0);
	if (toward == GlobalVar::getInstance()->Direction[0]) {
		nextNearPos = Vec2(-300, 0);
		nextFarPos = Vec2(-500,0);
	}
	else if (toward == GlobalVar::getInstance()->Direction[1]) {
		nextNearPos = Vec2(-200, 200);
		nextFarPos = Vec2(-350, 350);
	}
	else if (toward == GlobalVar::getInstance()->Direction[2]) {
		nextNearPos = Vec2(0, 300);
		nextFarPos = Vec2(0, 500);
	}
	else if (toward == GlobalVar::getInstance()->Direction[3]) {
		nextNearPos = Vec2(200, 200);
		nextFarPos = Vec2(350, 350);
	}
	else if (toward == GlobalVar::getInstance()->Direction[4]) {
		nextNearPos = Vec2(300, 0);
		nextFarPos = Vec2(500, 0);
	}
	else if (toward == GlobalVar::getInstance()->Direction[5]) {
		nextNearPos = Vec2(200, -200);
		nextFarPos = Vec2(350, -350);
	}
	else if (toward == GlobalVar::getInstance()->Direction[6]) {
		nextNearPos = Vec2(0, -300);
		nextFarPos = Vec2(0, -500);
	}
	else if (toward == GlobalVar::getInstance()->Direction[7]) {
		nextNearPos = Vec2(-200, -200);
		nextFarPos = Vec2(-350, -350);
	}
	auto MoveToNear = MoveBy::create(0.5f, nextNearPos);
	auto MoveToFar = MoveBy::create(0.5f, nextFarPos);
	auto disapper = CallFunc::create([&, b, type, isPlayer1]() {
		removeBullet(b, type, isPlayer1);
	});
	auto sequence1 = Sequence::create(MoveToNear, disapper, NULL);
	auto sequence2 = Sequence::create(MoveToFar, disapper, NULL);

	switch (type)
	{
	case 0:
	case 1:
	case 2:
		b->runAction(sequence1);
		break;
	case 3:
	case 4:
	case 5:
		b->runAction(sequence2);
	default:
		break;
	}
}

void bulletFactory::clear() {
	bulletForPlayer1->clear();
	bulletForPlayer2->clear();
	// 玩家现在拥有的弹药数目
	for (int i = 0; i < 6; i++) {

		bulletNumForPlayer1[i] = 9999;
		bulletNumForPlayer2[i] = 9999;
	}

}






