#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "ui/CocosGUI.h"
#include "Monster.h"
#include "sqlite3.h"
#include <sstream>
#pragma execution_character_set("utf-8")

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

	isDead = false;

	TMXTiledMap* tmx = TMXTiledMap::create("map.tmx");
	tmx->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	tmx->setAnchorPoint(Vec2(0.5, 0.5));
	tmx->setScale(Director::getInstance()->getContentScaleFactor()*2);
	this->addChild(tmx, 0);

	sqlite3* pdb = NULL;

	std::string path = "save.db";

	int result = sqlite3_open(path.c_str(), &pdb);

	std::string sql = "create table count(ID int primary key not null, kill int)";

	result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);

	sql = "insert into count values(1, 0)";

	result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	
	isDone = true;
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

	time = Label::createWithTTF("180", "fonts/arial.ttf", 36);
	time->setPosition(visibleSize.width / 2, visibleSize.height - 50);
	this->addChild(time);

	killCount = getKill();
	
	std::stringstream ss;
	ss << killCount;

	std::string killC = ss.str();

	kill = Label::createWithTTF(killC.c_str(), "fonts/arial.ttf", 36);
	kill->setPosition(visibleSize.width / 2, visibleSize.height - 100);
	this->addChild(kill);

	dtime = 180;
	schedule(schedule_selector(HelloWorld::updateTime), 1.0f, kRepeatForever, 0);

	schedule(schedule_selector(HelloWorld::getMonster), 3.0f, kRepeatForever, 1);

	schedule(schedule_selector(HelloWorld::moveMonsters), 3.0f, kRepeatForever, 2);

	schedule(schedule_selector(HelloWorld::hitByMonster), 1.0f, kRepeatForever, 2);

	//创建一张贴图
	auto texture = Director::getInstance()->getTextureCache()->addImage("$lucia_2.png");
	//从贴图中以像素单位切割，创建关键帧
	auto frame0 = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(0, 0, 113, 113)));
	//使用第一帧创建精灵
	player = Sprite::createWithSpriteFrame(frame0);
	player->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2));
	addChild(player, 3);

	//hp条
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	//使用hp条设置progressBar
	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(100);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 1);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 0);
	
	auto attackLabel = MenuItemLabel::create(Label::createWithTTF("Y", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::attackItemCallBack, this));
	attackLabel->setPosition(Vec2::ZERO);

	//auto deadLabel = MenuItemLabel::create(Label::createWithTTF("X", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::deadItemCallBack, this));
	//deadLabel->setPosition(Vec2(50, 50));

	auto menu = Menu::create(attackLabel, NULL);
	menu->setPosition(visibleSize.width - 75, 25);
	this->addChild(menu, 4);

	auto wLabel = MenuItemLabel::create(Label::createWithTTF("W", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::wItemCallBack, this));
	wLabel->setPosition(Vec2(50, 50));

	auto aLabel = MenuItemLabel::create(Label::createWithTTF("A", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::aItemCallBack, this));
	aLabel->setPosition(Vec2::ZERO);

	auto sLabel = MenuItemLabel::create(Label::createWithTTF("S", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::sItemCallBack, this));
	sLabel->setPosition(Vec2(50, 0));

	auto dLabel = MenuItemLabel::create(Label::createWithTTF("D", "fonts/arial.ttf", 36), CC_CALLBACK_1(HelloWorld::dItemCallBack, this));
	dLabel->setPosition(Vec2(100, 0));

	auto menu1 = Menu::create(wLabel, aLabel, sLabel, dLabel, NULL);
	menu1->setPosition(25, 25);
	this->addChild(menu1, 4);

	// 静态动画
	idle.reserve(1);
	idle.pushBack(frame0);

	// 攻击动画
	attack.reserve(17);
	for (int i = 0; i < 17; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(113 * i, 0, 113, 113)));
		attack.pushBack(frame);
	}
	auto attackAnimation = Animation::createWithSpriteFrames(attack, 0.1f);
	AnimationCache::getInstance()->addAnimation(attackAnimation, "attack");

	// 可以仿照攻击动画
	// 死亡动画(帧数：22帧，高：90，宽：79）
	auto texture2 = Director::getInstance()->getTextureCache()->addImage("$lucia_dead.png");
	// Todo
	dead.reserve(22);
	for (int i = 0; i < 22; i++) {
		auto frame = SpriteFrame::createWithTexture(texture2, CC_RECT_PIXELS_TO_POINTS(Rect(79 * i, 0, 79, 90)));
		dead.pushBack(frame);
	}
	auto deadAnimation = Animation::createWithSpriteFrames(dead, 0.1f);
	AnimationCache::getInstance()->addAnimation(deadAnimation, "dead");

	// 运动动画(帧数：8帧，高：101，宽：68）
	auto texture3 = Director::getInstance()->getTextureCache()->addImage("$lucia_forward.png");
	// Todo
	run.reserve(8);
	for (int i = 0; i < 8; i++) {
		auto frame = SpriteFrame::createWithTexture(texture3, CC_RECT_PIXELS_TO_POINTS(Rect(68 * i, 0, 68, 101)));
		run.pushBack(frame);
	}
	auto runAnimation = Animation::createWithSpriteFrames(run, 0.05f);
	AnimationCache::getInstance()->addAnimation(runAnimation, "run");

    return true;
}

void HelloWorld::attackItemCallBack(Ref* pSender) {
	if (isDead) {
		return;
	}
	if (isDone == false) {
		return;
	} else {
		isDone = false;
		auto attackAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("attack"));
		auto callFunc = CallFunc::create([&] { isDone = true; });
		auto attackSeq = Sequence::createWithTwoActions(attackAnimate, callFunc);
		player->runAction(attackSeq);
		hitMonster();
	}
}

//void HelloWorld::deadItemCallBack(Ref* pSender) {
//	if (isDone == false) {
//		return;
//	} else {
//		isDone = false;
//		auto deadAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("dead"));
//		auto callFunc = CallFunc::create([&] {isDone = true; });
//		auto deadSeq = Sequence::createWithTwoActions(deadAnimate, callFunc);
//		player->runAction(deadSeq);
//	}
//}

void HelloWorld::wItemCallBack(Ref* pSender) {
	if (isDead) {
		return;
	}
	auto position = player->getPosition();
	isDone = false;
	auto runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));
	if (position.y + 40 >= visibleSize.height - 30) {
		player->runAction(runAnimate);
		isDone = true;
		return;
	}
	player->runAction(runAnimate);
	auto moveW = MoveBy::create(0.4f, Vec2(0, 40));
	player->runAction(moveW);
	isDone = true;
}

void HelloWorld::aItemCallBack(Ref* pSender) {
	if (isDead) {
		return;
	}
	auto position = player->getPosition();	
	player->setFlipX(true);
	isDone = false;
	auto runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));	
	if (position.x <= 40) {
		player->runAction(runAnimate);
		isDone = true;
		return;
	}
	player->runAction(runAnimate);
	auto moveA = MoveBy::create(0.4f, Vec2(-40, 0));
	player->runAction(moveA);
	isDone = true;
}

void HelloWorld::sItemCallBack(Ref* pSender) {
	if (isDead) {
		return;
	}
	auto position = player->getPosition();
	isDone = false;
	auto runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));
	if (position.y - 40 <= 40) {
		player->runAction(runAnimate);
		isDone = true;
		return;
	}
	player->runAction(runAnimate);
	auto moveS = MoveBy::create(0.4f, Vec2(0, -40));
	player->runAction(moveS);
	isDone = true;
}

void HelloWorld::dItemCallBack(Ref* pSender) {
	if (isDead) {
		return;
	}
	auto position = player->getPosition();
	player->setFlipX(false);
	isDone = false;
	auto runAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("run"));
	if (position.x + 40 >= visibleSize.width) {
		player->runAction(runAnimate);
		isDone = true;
		return;
	}
	player->runAction(runAnimate);
	auto moveD = MoveBy::create(0.4f, Vec2(40, 0));
	player->runAction(moveD);
	isDone = true;
}

void HelloWorld::updateTime(float dt) {
	char buffer[10];
	dtime--;
	if (dtime <= 0) {
		dtime = 0;		
		playerDead();
	}
	sprintf(buffer, "%d", dtime);
	time->setString(buffer);
}

void HelloWorld::getMonster(float dt) {
	auto fac = Factory::getInstance();
	auto m = fac->createMonster();
	float x = random(origin.x, visibleSize.width);
	float y = random(origin.y, visibleSize.height);
	m->setPosition(x, y);
	this->addChild(m, 4);
}

void HelloWorld::moveMonsters(float dt) {
	auto fac = Factory::getInstance();
	Vec2 playerPos = player->getPosition();
	fac->moveMonster(playerPos, 2.0f);
}

void HelloWorld::hitMonster() {
	if (isDead) {
		return;
	}
	Rect playerRect = player->getBoundingBox();
	Rect attackRect = Rect(playerRect.getMinX() - 40, playerRect.getMinY(),
		playerRect.getMaxX() - playerRect.getMinX() + 80,
		playerRect.getMaxY() - playerRect.getMinY());
	auto fac = Factory::getInstance();
	Sprite* collision = fac->collider(attackRect);
	//角色攻击怪物
	if (collision != NULL) {
		updateKill();
		int hpCount = pT->getPercentage();
		if (hpCount <= 80) {
			pT->setPercentage(hpCount + 20);
		}
		else {
			pT->setPercentage(100);
		}
		fac->removeMonster(collision);
	}
}

void HelloWorld::hitByMonster(float dt) {
	auto fac = Factory::getInstance();
	Sprite* collision = fac->collider(player->getBoundingBox());
	//怪物碰到角色
	if (collision != NULL) {
		fac->removeMonster(collision);
		int hpCount = pT->getPercentage();
		if (hpCount - 20 > 0) {
			pT->setPercentage(hpCount - 20);
		}
		else {
			pT->setPercentage(0);
			playerDead();
		}
	}
}

//角色死亡，取消所有事件
void HelloWorld::playerDead() {
	if (isDone == false) {
		return;
	} else {
		isDone = false;
		auto deadanimate = Animate::create(AnimationCache::getInstance()->getAnimation("dead"));
		auto callfunc = CallFunc::create([&] {isDone = true; });
		auto deadseq = Sequence::createWithTwoActions(deadanimate, callfunc);
		player->runAction(deadseq);
		unschedule(schedule_selector(HelloWorld::updateTime));
		unschedule(schedule_selector(HelloWorld::hitByMonster));
		unschedule(schedule_selector(HelloWorld::getMonster));
		unschedule(schedule_selector(HelloWorld::moveMonsters));
		isDead = true;
	}
}

int HelloWorld::getKill() {
	sqlite3* pdb = NULL;
	std::string path = "save.db";
	int result = sqlite3_open(path.c_str(), &pdb);
	char ** re;
	int row, col;
	sqlite3_get_table(pdb, "select * from count", &re, &row, &col, NULL);
	int count = atoi(re[3]);
	log("%d", count);
	sqlite3_free_table(re);
	return count;
}

void HelloWorld::updateKill() {
	killCount = getKill();
	killCount++;
	std::string killC;
	std::stringstream ss;
	ss << killCount;
	killC = ss.str();
	sqlite3* pdb = NULL;
	std::string path = "save.db";
	int result = sqlite3_open(path.c_str(), &pdb);
	std::string sql = "update count set kill='" + killC + "'where id=1";
	result = sqlite3_exec(pdb, sql.c_str(), NULL, NULL, NULL);
	char buffer[10];
	sprintf(buffer, "%d", killCount);
	kill->setString(buffer);
}