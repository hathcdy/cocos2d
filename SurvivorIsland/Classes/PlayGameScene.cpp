#include "PlayGameScene.h"
#include "MenuScene.h"
#include "SimpleAudioEngine.h"
#include "GlobalVar.h"
#include "Player.h"
#include "Bullet.h"

#pragma execution_character_set("utf-8")
#define database	UserDefault::getInstance()
USING_NS_CC;

Scene* PlayGameScene::createScene()
{
	return PlayGameScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
	printf("Error while loading: %s\n", filename);
	printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in PlayGameSceneScene.cpp\n");
}

// on "init" you need to initialize your instance
bool PlayGameScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	visibleSize = Director::getInstance()->getVisibleSize();

	
	//加载地图
	tileMap = TMXTiledMap::create(GlobalVar::getInstance()->getMapFile());
	background = tileMap->layerNamed("BackGround");
	blockage = tileMap->layerNamed("Blockage");
	blockage->setVisible(false);
	this->addChild(tileMap, -1);

	monsterFac = MonsterFactory::getInstance();

	// 预加载帧动画
	preloadAnimation();
	// 添加玩家
	addPlayer();
	//添加怪物
	addMonsters();
	// 预加载音乐
	preloadMusic();
	//添加显示数据的label
	addDataLabel();

	//设置键盘事件监听器
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(PlayGameScene::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(PlayGameScene::onKeyReleased, this);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	// 设置自定义监听事件
	auto listenCustom = EventListenerCustom::create("meet", CC_CALLBACK_1(PlayGameScene::meet, this));
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenCustom, this);

	// 调度器
	schedule(schedule_selector(PlayGameScene::update), 0.05f, kRepeatForever, 0);
	// 怪物定期向玩家移动
	schedule(schedule_selector(PlayGameScene::updateMonsterPosition), 0.4f, kRepeatForever, 0.4f);
	// 让距离足够近的怪物攻击角色
	schedule(schedule_selector(PlayGameScene::updateAttack), 3.0f, kRepeatForever, 0);
	
	return true;
}

void PlayGameScene::addPlayer() {
	//通过getObjectGroup获得对象层
	TMXObjectGroup *objects = tileMap->getObjectGroup("Object");
	auto player1ShowUpPoint = objects->getObject("Player1ShowUpPoint");
	auto player2ShowUpPoint = objects->getObject("Player2ShowUpPoint");
	
	// 获得Player工厂实例
	auto playerFac = Factory::getInstance();
	// 创建玩家，获取工厂中玩家队列Vector
	Vector<Sprite*> playerVector = playerFac->createPlayer();

	// 设置玩家1位置,添加进场景
	player1 = playerVector.at(0);
	int x1 = player1ShowUpPoint["x"].asInt();
	int y1 = player1ShowUpPoint["y"].asInt();
	player1->setPosition(x1 + tileMap->getTileSize().width / 2, y1 + tileMap->getTileSize().height / 2);
	player1->setScale(0.5);
	this->addChild(player1, 1);

	// 若双人模式添加玩家2进入场景
	if (GlobalVar::getInstance()->getDoublePlayer() == true) {
		player2 = playerVector.at(1);
		// 设置Player2的位置
		int x2 = player2ShowUpPoint["x"].asInt();
		int y2 = player2ShowUpPoint["y"].asInt();
		player2->setPosition(x2 + tileMap->getTileSize().width / 2, y2 + tileMap->getTileSize().height / 2);
		player2->setScale(0.5);
		this->addChild(player2, 1);
	}
	else player2 = NULL;
	//设置视角
	this->setViewPointCenter(getCentralPoint());
}

void PlayGameScene::addMonsters() {
	//通过getObjectGroup获得对象层
	TMXObjectGroup *objects = tileMap->getObjectGroup("Object");
	int count = 0;
	for (auto object : objects->getObjects()) {
		ValueMap& dict = object.asValueMap();
		//得到怪物出生的区域
		if (dict["Enemy"].asInt() == 1) {
			//区域左下角x和y
			int x = dict["x"].asInt();
			int y = dict["y"].asInt();
			//区域宽和高
			int width = dict["width"].asInt();
			int height = dict["height"].asInt();
			//区域行和列数
			int row = width / tileMap->getTileSize().width;
			int col = height / tileMap->getTileSize().height;
			for (int i = 0; i < row; ++i) {
				for (int j = 0; j < col; ++j) {
					//怪物出生的坐标
					int enemyX = x + tileMap->getTileSize().width / 2 + i * tileMap->getTileSize().width;
					int enemyY = y + tileMap->getTileSize().height / 2 + j * tileMap->getTileSize().height;
					Point enemyPoint = Point(enemyX, enemyY);
					if (i == 0) {
						auto monster2 = monsterFac->createBigMonster();
						monster2->setScale(0.5);
						monster2->setPosition(enemyPoint);
						this->addChild(monster2, 1);
					}
					else {
						auto monster1 = monsterFac->createSmallMonster();
						monster1->setScale(0.5);
						monster1->setPosition(enemyPoint);
						this->addChild(monster1, 1);
					}
				}
			}
		}
	}
}

void PlayGameScene::addDataLabel()
{	
	//玩家1相关信息
	label_player1 = Label::createWithTTF("Player1", "fonts/Marker Felt.ttf", 24);
	auto position = getCentralPoint();
	auto winSize = Director::getInstance()->getWinSize();
	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);
	int mapWidth = tileMap->getMapSize().width * tileMap->getTileSize().width;
	x = MIN(x, mapWidth - winSize.width / 2);
	int mapHeight = tileMap->getMapSize().height * tileMap->getTileSize().height;
	y = MIN(y, mapHeight - winSize.height / 2);
	auto actualPosition = Point(x, y);
	label_player1->setPosition(actualPosition + Vec2(-visibleSize.width / 2, visibleSize.height / 2 - 10));
	label_player1->setAnchorPoint(Vec2(0, 1.0));
	this->addChild(label_player1, 0);
	hp1 = Label::createWithTTF("Hp: 100", "fonts/Marker Felt.ttf", 24);
	hp1->setPosition(label_player1->getPosition() - Vec2(0, 30));
	hp1->setAnchorPoint(Vec2(0, 1.0));
	this->addChild(hp1);
	attackType1 = Label::createWithTTF("Weapon: Uzi", "fonts/Marker Felt.ttf", 24);
	attackType1->setPosition(hp1->getPosition() - Vec2(0, 30));
	attackType1->setAnchorPoint(Vec2(0, 1.0));
	this->addChild(attackType1);
	count1 = Label::createWithTTF("Bullets left: 50", "fonts/Marker Felt.ttf", 24);
	count1->setPosition(attackType1->getPosition() - Vec2(0, 30));
	count1->setAnchorPoint(Vec2(0, 1.0));
	this->addChild(count1);
	score1 = Label::createWithTTF("Score: 0", "fonts/Marker Felt.ttf", 24);
	score1->setPosition(count1->getPosition() - Vec2(0, 30));
	score1->setAnchorPoint(Vec2(0, 1.0));
	this->addChild(score1);
	//玩家2相关信息
	if (!GlobalVar::getInstance()->getDoublePlayer()) return;
	label_player2 = Label::createWithTTF("Player2", "fonts/Marker Felt.ttf", 24);
	label_player2->setPosition(actualPosition + Vec2(visibleSize.width / 2, visibleSize.height / 2 - 10));
	label_player2->setAnchorPoint(Vec2(1.0, 1.0));
	this->addChild(label_player2, 0);
	hp2 = Label::createWithTTF("Hp: 100", "fonts/Marker Felt.ttf", 24);
	hp2->setPosition(label_player2->getPosition() - Vec2(0, 30));
	hp2->setAnchorPoint(Vec2(1.0, 1.0));
	this->addChild(hp2);
	attackType2 = Label::createWithTTF("Weapon: Uzi", "fonts/Marker Felt.ttf", 24);
	attackType2->setPosition(hp2->getPosition() - Vec2(0, 30));
	attackType2->setAnchorPoint(Vec2(1.0, 1.0));
	this->addChild(attackType2);
	count2 = Label::createWithTTF("Bullets left: 50", "fonts/Marker Felt.ttf", 24);
	count2->setPosition(attackType2->getPosition() - Vec2(0, 30));
	count2->setAnchorPoint(Vec2(1.0, 1.0));
	this->addChild(count2);
	score2 = Label::createWithTTF("Score: 0", "fonts/Marker Felt.ttf", 24);
	score2->setPosition(count2->getPosition() - Vec2(0, 30));
	score2->setAnchorPoint(Vec2(1.0, 1.0));
	this->addChild(score2);

	// 添加退出菜单按钮
	//exitItem = MenuItemImage::create("exitItem1.png","exitItem2.png", CC_CALLBACK_1(PlayGameScene::exitItemCallBack, this));
	//auto exitLabel = Label::createWithTTF("EXIT", "fonts/Marker Felt.ttf", 24);
	//exitItem = MenuItemLabel::create(exitLabel, CC_CALLBACK_0(PlayGameScene::exitItemCallBack, this));
	//exitItem->setAnchorPoint(Vec2(1.0, 0));//右下角为锚点
	//exitItem->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	//this->addChild(exitItem, 1);
	//auto menu = Menu::create(exitItem,NULL);
	//menu->setPosition(Vec2::ZERO);
	//this->addChild(menu, 1);
}

//把窗口的视角设置到人物身上
void PlayGameScene::setViewPointCenter(cocos2d::Point position) {
	//求出屏幕的范围包括宽和高
	auto winSize = Director::getInstance()->getWinSize();

	//显示屏幕中心点的坐标大于屏幕宽和高的一半,防止部分视图在屏幕之外，即屏幕移出地图边界下/左方
	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);

	//显示屏幕中心的坐标小于边界减去屏幕宽和高的一半，防止部分视图在屏幕之外，即屏幕超出地图边界上/右方

	//整个瓦片地图的宽
	//getMapSize().width瓦片地图横向有多少个瓦片
	//getTileSize().width每一个瓦片的宽度
	int mapWidth = tileMap->getMapSize().width * tileMap->getTileSize().width;
	x = MIN(x, mapWidth - winSize.width / 2);
	//整个瓦片地图的高
	int mapHeight = tileMap->getMapSize().height * tileMap->getTileSize().height;
	y = MIN(y, mapHeight - winSize.height / 2);
	auto actualPosition = Point(x, y);

	auto centerOfView = Point(winSize.width / 2, winSize.height / 2);
	auto viewPoint = centerOfView - actualPosition;
	//重置显示屏幕的中心点
	this->setPosition(viewPoint);
}

//更新场景
void PlayGameScene::update(float f) {	
	//水平方向上移动
	if (GlobalVar::getInstance()->player1_isMove_hori) this->movePlayer(player1, GlobalVar::getInstance()->player1_moveKey_hori);
	if (GlobalVar::getInstance()->player2_isMove_hori && GlobalVar::getInstance()->getDoublePlayer())
		this->movePlayer(player2, GlobalVar::getInstance()->player2_moveKey_hori);
	//垂直方向上移动
	if (GlobalVar::getInstance()->player1_isMove_vert) this->movePlayer(player1, GlobalVar::getInstance()->player1_moveKey_vert);
	if (GlobalVar::getInstance()->player2_isMove_vert && GlobalVar::getInstance()->getDoublePlayer())
		this->movePlayer(player2, GlobalVar::getInstance()->player2_moveKey_vert);
	this->setViewPointCenter(getCentralPoint());


	auto playerFac = Factory::getInstance();

	// 更新角色当前帧动画
	playerFac->update_Player_Animation(player1, true);
	if (GlobalVar::getInstance()->getDoublePlayer() == true) {
		playerFac->update_Player_Animation(player2, false);
	}

	// 碰撞检测:(分发自定义事件)
	EventCustom e("meet");
	_eventDispatcher->dispatchEvent(&e);


	// 更新attacker列表
	if (GlobalVar::getInstance()->getDoublePlayer()) {
		monsterFac->checkAttacker(player1->getPosition(), player2->getPosition());
	}
	else {
		monsterFac->checkAttacker(player1->getPosition(), Vec2(0, 0));
	}

	//修改label位置
	//获得视角的中心点，原理采用setViewPoint函数中代码
	auto position = getCentralPoint();
	auto winSize = Director::getInstance()->getWinSize();
	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);
	int mapWidth = tileMap->getMapSize().width * tileMap->getTileSize().width;
	x = MIN(x, mapWidth - winSize.width / 2);
	int mapHeight = tileMap->getMapSize().height * tileMap->getTileSize().height;
	y = MIN(y, mapHeight - winSize.height / 2);
	auto actualPosition = Point(x, y);
	// 设置位置
	label_player1->setPosition(actualPosition + Vec2(-visibleSize.width / 2, visibleSize.height / 2 - 10));
	hp1->setPosition(label_player1->getPosition() - Vec2(0, 30));
	attackType1->setPosition(hp1->getPosition() - Vec2(0, 30));
	count1->setPosition(attackType1->getPosition() - Vec2(0, 30));
	score1->setPosition(count1->getPosition() - Vec2(0, 30));
	if (GlobalVar::getInstance()->getDoublePlayer()) {
		label_player2->setPosition(actualPosition + Vec2(visibleSize.width / 2, visibleSize.height / 2 - 10));
		hp2->setPosition(label_player2->getPosition() - Vec2(0, 30));
		attackType2->setPosition(hp2->getPosition() - Vec2(0, 30));
		count2->setPosition(attackType2->getPosition() - Vec2(0, 30));
		score2->setPosition(count2->getPosition() - Vec2(0, 30));
	}
	// 设置内容
	//exitItem->setPosition(actualPosition + Vec2(visibleSize.width / 2, -visibleSize.height / 2));
	std::string str;
	str = "Hp: " + Value(GlobalVar::getInstance()->player1HP).asString();
	//str = "Bullet in scene: " + Value(bulletFactory::getInstance()->getBulletNumInScene(GlobalVar::getInstance()->attack_Type_For_Player1, true).size()).asString();
	hp1->setString(str);
	str = "Weapon: " + GlobalVar::getInstance()->Weapon[GlobalVar::getInstance()->attack_Type_For_Player1];
	attackType1->setString(str);
	str = "Bullets left: " + Value(bulletFactory::getInstance()->getBulletNum(GlobalVar::getInstance()->attack_Type_For_Player1, true)).asString();
	count1->setString(str);
	str = "Score: " + Value(GlobalVar::getInstance()->player1Score).asString();
	score1->setString(str);
	if (GlobalVar::getInstance()->getDoublePlayer()) {
		str = "Hp: " + Value(GlobalVar::getInstance()->player2HP).asString();
		//str = "Bullet in scene: " + Value(bulletFactory::getInstance()->getBulletNumInScene(GlobalVar::getInstance()->attack_Type_For_Player2, false).size()).asString();
		hp2->setString(str);
		str = "Weapon: " + GlobalVar::getInstance()->Weapon[GlobalVar::getInstance()->attack_Type_For_Player2];
		attackType2->setString(str);
		str = "Bullets left: " + Value(bulletFactory::getInstance()->getBulletNum(GlobalVar::getInstance()->attack_Type_For_Player2, false)).asString();
		count2->setString(str);
		str = "Score: " + Value(GlobalVar::getInstance()->player2Score).asString();
		//str = "Score: bbbbbbbbb";
		score2->setString(str);
	}

	if (MonsterFactory::getInstance()->getMonster().size() <= 0) {
		addMonsters();
	}

	//检测是否应该结束游戏
	if (Factory::getInstance()->getPlayers().size() <= 0) {
		gameOver();
	}
}

// 怪物定期向玩家移动
void PlayGameScene::updateMonsterPosition(float dt) {	
	if (GlobalVar::getInstance()->getDoublePlayer()) {
		if (GlobalVar::getInstance()->player1HP <= 0) {
			monsterFac->moveMonster(Vec2(0, 0), player2->getPosition(), 1.0f);
		}
		else if (GlobalVar::getInstance()->player1HP > 0 && GlobalVar::getInstance()->player2HP <= 0) {
			monsterFac->moveMonster(player1->getPosition(), Vec2(0, 0), 1.0f);			
		}	
		else {
			monsterFac->moveMonster(player1->getPosition(), player2->getPosition(), 1.0f);
		}
	}
	else {
		monsterFac->moveMonster(player1->getPosition(), Vec2(0, 0), 1.0f);
	}
}

void PlayGameScene::updateAttack(float dt) {
	int isHit1 = 0;
	if (GlobalVar::getInstance()->player1HP <= 0) {
		MonsterFactory::getInstance()->clearAttacker1();		
	}
	else {
		isHit1 = SmallMonsterAttack1(player1->getPosition(), 1.0f);
	}
	int isHit2 = 0;
	if (GlobalVar::getInstance()->getDoublePlayer()) {
		if (GlobalVar::getInstance()->player2HP <= 0) {
			MonsterFactory::getInstance()->clearAttacker2();
		}
		else {
			isHit2 = SmallMonsterAttack2(player2->getPosition(), 1.0f);			
		}				
		BigMonsterAttack(player1->getPosition(), player2->getPosition());
	}
	else {
		if (GlobalVar::getInstance()->player1HP <= 0) {
			MonsterFactory::getInstance()->clearAttacker1();
		}
		else {
			BigMonsterAttack(player1->getPosition(), Vec2(0, 0));
		}		
	}
	
	//人物扣血
	if (isHit1 > 0 && GlobalVar::getInstance()->player1HP) {
		Factory::getInstance()->wasHit(player1, isHit1 * 10, true);
	}
	if (GlobalVar::getInstance()->getDoublePlayer() && GlobalVar::getInstance()->player2HP > 0 && isHit2 > 0) {
		Factory::getInstance()->wasHit(player2, isHit2 * 10, false);
	}
}


void PlayGameScene::movePlayer(Sprite* player, char moveKey) {
	//死了不移动
	if (player == player1 && GlobalVar::getInstance()->player1HP <= 0) return;
	if (player == player2 && GlobalVar::getInstance()->player2HP <= 0) return;
	//如果玩家在射击，不允许移动
	if (player == player1 && GlobalVar::getInstance()->player1_moving_direction=="shot") {
		return;
	}
	if (player == player2 && GlobalVar::getInstance()->player2_moving_direction == "shot") {
		return;
	}
	//除以4是因为player进行过缩放，setScale(0.5)
	int mapWidth = tileMap->getMapSize().width * tileMap->getTileSize().width;
	int mapHeight = tileMap->getMapSize().height * tileMap->getTileSize().height;
	//除了防止角色跑出地图外，还要防止角色之间距离超过屏幕，导致有一个角色跑出屏幕
	auto another_player = (player == player1) ? player2 : player1;
	switch (moveKey)
	{
		case 'A':
			if ((player->getPositionX() - 8) > player->getContentSize().width / 4 && distValidBetwPlayer(player, another_player, 'A'))
				this->setPlayerPosition(player, player->getPosition() + Vec2(-8, 0));
			break;
		case 'D':
			if ((player->getPositionX() + 8) < mapWidth - player->getContentSize().width / 4 && distValidBetwPlayer(player, another_player, 'D'))
				this->setPlayerPosition(player, player->getPosition() + Vec2(8, 0));
			break;
		case 'W':
			if ((player->getPositionY() + 8) < mapHeight - player->getContentSize().height / 4 && distValidBetwPlayer(player, another_player, 'W'))
				this->setPlayerPosition(player, player->getPosition() + Vec2(0, 8));
			break;
		case 'S':
			if ((player->getPositionY() - 8) > player->getContentSize().height / 2 && distValidBetwPlayer(player, another_player, 'S'))
				this->setPlayerPosition(player, player->getPosition() + Vec2(0, -8));
		default:
			break;
	}
}

void PlayGameScene::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		GlobalVar::getInstance()->player1_moveKey_hori = 'A';
		GlobalVar::getInstance()->player1_isMove_hori = true;
		Apressed = true;
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		GlobalVar::getInstance()->player1_moveKey_hori = 'D';
		GlobalVar::getInstance()->player1_isMove_hori = true;
		Dpressed = true;
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
	case EventKeyboard::KeyCode::KEY_W:
		GlobalVar::getInstance()->player1_moveKey_vert = 'W';
		GlobalVar::getInstance()->player1_isMove_vert = true;
		Wpressed = true;
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
	case EventKeyboard::KeyCode::KEY_S:
		GlobalVar::getInstance()->player1_moveKey_vert = 'S';
		GlobalVar::getInstance()->player1_isMove_vert = true;
		Spressed = true;
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		GlobalVar::getInstance()->player2_moveKey_hori = 'A';
		GlobalVar::getInstance()->player2_isMove_hori = true;
		left_pressed = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		GlobalVar::getInstance()->player2_moveKey_hori = 'D';
		GlobalVar::getInstance()->player2_isMove_hori = true;
		right_pressed = true;
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		GlobalVar::getInstance()->player2_moveKey_vert = 'W';
		GlobalVar::getInstance()->player2_isMove_vert = true;
		up_pressed = true;
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		GlobalVar::getInstance()->player2_moveKey_vert = 'S';
		GlobalVar::getInstance()->player2_isMove_vert = true;
		down_pressed = true;
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_Q:
	case EventKeyboard::KeyCode::KEY_Q:
		if (GlobalVar::getInstance()->player1HP > 0 && GlobalVar::getInstance()->player1_isShotDone) {
			if (GlobalVar::getInstance()->attack_Type_For_Player1 == 0)
				GlobalVar::getInstance()->attack_Type_For_Player1 = 5;
			else
				GlobalVar::getInstance()->attack_Type_For_Player1--;
		}
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_E:
	case EventKeyboard::KeyCode::KEY_E:
		if (GlobalVar::getInstance()->player1HP > 0 && GlobalVar::getInstance()->player1_isShotDone) {
			int type = (GlobalVar::getInstance()->attack_Type_For_Player1 + 1) % 6;
			GlobalVar::getInstance()->attack_Type_For_Player1 = type;
		}
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		if (GlobalVar::getInstance()->player1HP > 0)
			attack(player1);
		//Factory::getInstance()->removePlayer(player1);
		//gameOver();
		break;
	case EventKeyboard::KeyCode::KEY_1:
		if (GlobalVar::getInstance()->getDoublePlayer() && GlobalVar::getInstance()->player2HP > 0) {
			attack(player2);
			//Factory::getInstance()->wasHit(player2, 15, false);
		}
		break;
	case EventKeyboard::KeyCode::KEY_2:
		if (GlobalVar::getInstance()->getDoublePlayer() && GlobalVar::getInstance()->player2HP > 0 && GlobalVar::getInstance()->player2_isShotDone) {
			if (GlobalVar::getInstance()->attack_Type_For_Player2 == 0)
				GlobalVar::getInstance()->attack_Type_For_Player2 = 5;
			else
				GlobalVar::getInstance()->attack_Type_For_Player2--;
		}
		break;
	case EventKeyboard::KeyCode::KEY_3:
		if (GlobalVar::getInstance()->getDoublePlayer() && GlobalVar::getInstance()->player2HP > 0 && GlobalVar::getInstance()->player2_isShotDone) {
			int type = (GlobalVar::getInstance()->attack_Type_For_Player2 + 1) % 6;
			GlobalVar::getInstance()->attack_Type_For_Player2 = type;
		}
		break;
	case EventKeyboard::KeyCode::KEY_CAPITAL_T:
	case EventKeyboard::KeyCode::KEY_T:
		gameOver();
		break;
	}
}

void PlayGameScene::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
		if (Dpressed == false) GlobalVar::getInstance()->player1_isMove_hori = false;
		else {
			GlobalVar::getInstance()->player1_moveKey_hori = 'D';
		}
		Apressed = false;
		break;
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
		if (Apressed == false) GlobalVar::getInstance()->player1_isMove_hori = false;
		else {
			GlobalVar::getInstance()->player1_moveKey_hori = 'A';
		}
		Dpressed = false;
		break;
	case EventKeyboard::KeyCode::KEY_W:
	case EventKeyboard::KeyCode::KEY_CAPITAL_W:
		if (Spressed == false) GlobalVar::getInstance()->player1_isMove_vert = false;
		else GlobalVar::getInstance()->player1_moveKey_vert = 'S';
		Wpressed = false;
		break;
	case EventKeyboard::KeyCode::KEY_S:
	case EventKeyboard::KeyCode::KEY_CAPITAL_S:
		if (Wpressed == false) GlobalVar::getInstance()->player1_isMove_vert = false;
		else GlobalVar::getInstance()->player1_moveKey_vert = 'W';
		Spressed = false;
		break;
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		if (right_pressed == false) GlobalVar::getInstance()->player2_isMove_hori = false;
		else {
			GlobalVar::getInstance()->player2_moveKey_hori = 'D';
		}
		left_pressed = false;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		if (left_pressed == false) GlobalVar::getInstance()->player2_isMove_hori = false;
		else {
			GlobalVar::getInstance()->player2_moveKey_hori = 'A';
		}
		right_pressed = false;
		break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:
		if (down_pressed == false) GlobalVar::getInstance()->player2_isMove_vert = false;
		else GlobalVar::getInstance()->player2_moveKey_vert = 'S';
		up_pressed = false;
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		if (up_pressed == false) GlobalVar::getInstance()->player2_isMove_vert = false;
		else GlobalVar::getInstance()->player2_moveKey_vert = 'W';
		down_pressed = false;
		break;
	}
}

//计算瓦片的坐标
Point PlayGameScene::tileCoordForPosition(Point position) {
	//原始坐标在画面左上角

	//将人物的目的的坐标的x轴坐标转换成瓦片地图中的x轴的坐标
	int x = position.x / tileMap->getTileSize().width;
	//将人物的目的的坐标的y轴坐标转换成瓦片地图中的y轴的坐标
	int y = ((tileMap->getMapSize().height * tileMap->getTileSize().height) - position.y) / tileMap->getTileSize().height;
	return Point(x, y);
}

//设置角色的位置
void PlayGameScene::setPlayerPosition(Sprite* player, Point position) {
	Point tileCoord = this->tileCoordForPosition(position);
	int tileGid = blockage->getTileGIDAt(tileCoord);
	if (tileGid) {
		auto properties = tileMap->getPropertiesForGID(tileGid).asValueMap();
		if (!properties.empty()) {
			//地图上的障碍物，不能移动到这里
			auto collision = properties["Block"].asString();
			if ("true" == collision) return;
		}
	}
	//防止角色之间重叠
	if (GlobalVar::getInstance()->getDoublePlayer()) {
		auto anotherPlayer = (player == player1) ? player2 : player1;
		if (position.getDistance(anotherPlayer->getPosition()) < 30) return;
	}
	//防止角色与怪物重叠
	for (auto monster : MonsterFactory::getInstance()->getMonster()) {
		if (position.getDistance(monster->getPosition()) < 30) return;
	}
	player->setPosition(position);
}

//得到两个角色坐标的中点
Point PlayGameScene::getCentralPoint() {
	//单人模式时中点即为player1的坐标
	if (GlobalVar::getInstance()->getDoublePlayer() == false) return player1->getPosition();
	if (GlobalVar::getInstance()->player1HP <= 0) return player2->getPosition();
	else if (GlobalVar::getInstance()->player2HP <= 0) return player1->getPosition();
	double x = (player1->getPositionX() + player2->getPositionX()) / 2;
	double y = (player1->getPositionY() + player2->getPositionY()) / 2;
	return Point(x, y);
}

//两个角色之间距离是否大于屏幕大小
bool PlayGameScene::distValidBetwPlayer(Sprite *player, Sprite *another_player, char moveKey) {
	if (GlobalVar::getInstance()->getDoublePlayer() == false) return true;
	int playerWidth = player1->getContentSize().width / 4 + player2->getContentSize().width / 4;
	int playerHeight = player1->getContentSize().height / 4 + player2->getContentSize().height / 2;
	switch (moveKey)
	{
	case 'A':
		if (abs(player->getPositionX() - 8 - another_player->getPositionX()) > visibleSize.width - playerWidth)
			return false;
		break;
	case 'D':
		if (abs(player->getPositionX() + 8 - another_player->getPositionX()) > visibleSize.width - playerWidth)
			return false;
		break;
	case 'W':
		if (abs(player->getPositionY() + 8 - another_player->getPositionY()) > visibleSize.height - playerHeight)
			return false;
		break;
	case 'S':
		if (abs(player->getPositionY() - 8 - another_player->getPositionY()) > visibleSize.height - playerHeight)
			return false;
	default:
		break;
	}
	return true;
}

// 预加载动画
void PlayGameScene::preloadAnimation() {
	// 创建玩家移动帧动画

	// 创建player1移动动画贴图
	auto player1_move_texture = Director::getInstance()->getTextureCache()->addImage("player1_move.png");
	// 创建player1的八方向移动帧动画，预加载到Cache中
	// Cache中存储加入八个Animation: 
	// "player1_move_West", "player1_move_NorthWest",..., "player1_move_SouthWest"
	for (int i = 0; i < 8; i++) {
		player1_move[i].reserve(8);
		for (int j = 0; j < 8; j++) {
			auto frame = SpriteFrame::createWithTexture(player1_move_texture, CC_RECT_PIXELS_TO_POINTS(Rect(128 * j, 128 * i, 128, 128)));
			player1_move[i].pushBack(frame);
		}
		auto player1_move_Animation = Animation::createWithSpriteFrames(player1_move[i], 0.05f);
		player1_move_Animation->setRestoreOriginalFrame(false);
		string name = "player1_move_" + GlobalVar::getInstance()->Direction[i];
		AnimationCache::getInstance()->addAnimation(player1_move_Animation, name);
	}

	// 若双人模式，创建player2移动动画
	if (GlobalVar::getInstance()->getDoublePlayer() == true) {
		auto player2_move_texture = Director::getInstance()->getTextureCache()->addImage("player2_move.png");
		for (int i = 0; i < 8; i++) {
			player2_move[i].reserve(8);
			for (int j = 0; j < 8; j++) {
				auto frame = SpriteFrame::createWithTexture(player2_move_texture, CC_RECT_PIXELS_TO_POINTS(Rect(128 * j, 128 * i, 128, 128)));
				player2_move[i].pushBack(frame);
			}
			auto player2_move_Animation = Animation::createWithSpriteFrames(player2_move[i], 0.1f);
			player2_move_Animation->setRestoreOriginalFrame(false);
			string name = "player2_move_" + GlobalVar::getInstance()->Direction[i];
			AnimationCache::getInstance()->addAnimation(player2_move_Animation, name);
		}
	}

	// 创建玩家射击帧动画
	// 创建player1射击动画贴图
	auto player1_shot_texture = Director::getInstance()->getTextureCache()->addImage("player1_shot.png");
	// 创建player1的八方向射击帧动画，预加载到Cache中
	// Cache中存储加入八个Animation: 
	// "player1_shot_West", "player1_shot_NorthWest",..., "player1_shot_SouthWest"
	for (int i = 0; i < 8; i++) {
		player1_shot[i].reserve(8);
		for (int j = 0; j < 8; j++) {
			auto frame = SpriteFrame::createWithTexture(player1_shot_texture, CC_RECT_PIXELS_TO_POINTS(Rect(128 * j, 128 * i, 128, 128)));
			player1_shot[i].pushBack(frame);
		}
		auto player1_shot_Animation = Animation::createWithSpriteFrames(player1_shot[i], 0.05f);
		player1_shot_Animation->setRestoreOriginalFrame(false);
		string name = "player1_shot_" + GlobalVar::getInstance()->Direction[i];
		AnimationCache::getInstance()->addAnimation(player1_shot_Animation, name);
	}

	// 若双人模式，创建player2射击帧动画
	if (GlobalVar::getInstance()->getDoublePlayer() == true) {
		auto player2_shot_texture = Director::getInstance()->getTextureCache()->addImage("player2_shot.png");
		for (int i = 0; i < 8; i++) {
			player2_shot[i].reserve(8);
			for (int j = 0; j < 8; j++) {
				auto frame = SpriteFrame::createWithTexture(player2_shot_texture, CC_RECT_PIXELS_TO_POINTS(Rect(128 * j, 128 * i, 128, 128)));
				player2_shot[i].pushBack(frame);
			}
			auto player2_shot_Animation = Animation::createWithSpriteFrames(player2_shot[i], 0.05f);
			player2_shot_Animation->setRestoreOriginalFrame(false);
			string name = "player2_shot_" + GlobalVar::getInstance()->Direction[i];
			AnimationCache::getInstance()->addAnimation(player2_shot_Animation, name);
		}
	}

	// 创建monster1移动动画
	auto monster1_move_texture = Director::getInstance()->getTextureCache()->addImage("monster1_move.png");
	for (int i = 0; i < 8; i++) {
		monster1_move[i].reserve(8);
		for (int j = 0; j < 8; j++) {
			auto frame = SpriteFrame::createWithTexture(monster1_move_texture, CC_RECT_PIXELS_TO_POINTS(Rect(128 * j, 128 * i, 128, 128)));
			monster1_move[i].pushBack(frame);
		}
		auto monster1_move_Animation = Animation::createWithSpriteFrames(monster1_move[i], 0.1f);
		monster1_move_Animation->setRestoreOriginalFrame(false);
		string name = "monster1_move_" + GlobalVar::getInstance()->Direction[i];
		AnimationCache::getInstance()->addAnimation(monster1_move_Animation, name);
	}

	// 创建monster1攻击动画
	auto monster1_attack_texture = Director::getInstance()->getTextureCache()->addImage("monster1_attack.png");
	for (int i = 0; i < 8; i++) {
		monster1_attack[i].reserve(8);
		for (int j = 0; j < 8; j++) {
			auto frame = SpriteFrame::createWithTexture(monster1_attack_texture, CC_RECT_PIXELS_TO_POINTS(Rect(128 * j, 128 * i, 128, 128)));
			monster1_attack[i].pushBack(frame);
		}
		auto monster1_attack_Animation = Animation::createWithSpriteFrames(monster1_attack[i], 0.1f);
		monster1_attack_Animation->setRestoreOriginalFrame(false);
		string name = "monster1_attack_" + GlobalVar::getInstance()->Direction[i];
		AnimationCache::getInstance()->addAnimation(monster1_attack_Animation, name);
	}

	// 创建monster2移动动画
	auto monster2_move_texture = Director::getInstance()->getTextureCache()->addImage("monster2_move.png");
	for (int i = 0; i < 8; i++) {
		monster2_move[i].reserve(8);
		for (int j = 0; j < 8; j++) {
			auto frame = SpriteFrame::createWithTexture(monster2_move_texture, CC_RECT_PIXELS_TO_POINTS(Rect(128 * j, 128 * i, 128, 128)));
			monster2_move[i].pushBack(frame);
		}
		auto monster2_move_Animation = Animation::createWithSpriteFrames(monster2_move[i], 0.1f);
		monster2_move_Animation->setRestoreOriginalFrame(false);
		string name = "monster2_move_" + GlobalVar::getInstance()->Direction[i];
		AnimationCache::getInstance()->addAnimation(monster2_move_Animation, name);
	}
}

// 玩家射击
void PlayGameScene::attack(Sprite* player) {
	// 调用playerFac里面的attack函数，产生玩家对应的射击帧动画以及音效
	// 发射对应类型子弹
	auto playerFac = Factory::getInstance();
	auto bulletFac = bulletFactory::getInstance();
	if (player == player1) {
		Sprite* bullet = bulletFac->createBullet(GlobalVar::getInstance()->attack_Type_For_Player1, true);//bulletFac产生子弹
		//无弹药，就播放卡弹音效
		if (bulletFac->getBulletNum(GlobalVar::getInstance()->attack_Type_For_Player1, true) <= 0) {
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/load_bullet.mp3", false);

		}
		//有子弹，就发射子弹
		if (bullet != NULL) {
			playerFac->attack(player, true);//射击帧动画+音效
			Vec2 pos = player1->getPosition();
			bullet->setPosition(pos);//设置子弹位置为玩家1位置
			this->addChild(bullet);//添加子弹进入场景类
			bulletFac->moveBullet(bullet, GlobalVar::getInstance()->attack_Type_For_Player1, true);
		}
		
		
	}
	else {
		Sprite* bullet = bulletFac->createBullet(GlobalVar::getInstance()->attack_Type_For_Player2, false);
		if (bulletFac->getBulletNum(GlobalVar::getInstance()->attack_Type_For_Player2, false) <= 0) {
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/load_bullet.mp3", false);
		}
		if (bullet != NULL) {
			playerFac->attack(player, false);
			Vec2 pos = player2->getPosition();
			bullet->setPosition(pos);
			this->addChild(bullet);
			bulletFac->moveBullet(bullet, GlobalVar::getInstance()->attack_Type_For_Player2, false);
		}
		
	}

}

void PlayGameScene::preloadMusic() {
	auto audioengine = CocosDenshion::SimpleAudioEngine::getInstance();
	audioengine->preloadEffect("music/boss.mp3");
	audioengine->preloadEffect("music/dead1.mp3");
	audioengine->preloadEffect("music/dead2.mp3");
	audioengine->preloadEffect("music/grenade.mp3");
	audioengine->preloadEffect("music/laser_gun.mp3");
	audioengine->preloadEffect("music/mussile.mp3");
	audioengine->preloadEffect("music/pistol.mp3");
	audioengine->preloadEffect("music/shotgun.mp3");
	audioengine->preloadEffect("music/victory.mp3");
	audioengine->preloadEffect("music/warning.mp3");
	audioengine->preloadEffect("music/load_bullet.mp3");
	audioengine->preloadEffect("music/gameover.mp3");
}

void PlayGameScene::BigMonsterAttack(Vec2 playerPos1, Vec2 playerPos2) {
	Vector<Sprite*> attacker1 = MonsterFactory::getInstance()->getAttacker1();
	Vector<Sprite*> attacker2 = MonsterFactory::getInstance()->getAttacker2();
	Vector<Sprite*> monster = MonsterFactory::getInstance()->getMonster();
	for (int i = 0; i < attacker1.size(); i++) {
		//if (i >= attacker1.size()) break;
		// 获取怪物位置和相对角色的方向
		Vec2 Pos = attacker1.at(i)->getPosition();
		Vec2 dir = playerPos1 - Pos;
		dir.normalize();
		// 大怪
		if (attacker1.at(i)->getTag() == 2) {
			auto bullet = MonsterFactory::getInstance()->createMonsterBullet();
			bullet->setPosition(attacker1.at(i)->getPosition());
			auto moveBy = MoveBy::create(5.0f, 350 * dir);
			auto fadeOut = FadeOut::create(1.0f);
			auto removeBullet = CallFunc::create([bullet] {
				MonsterFactory::getInstance()->removeMonsterBullet(bullet);
				//CCLOG("%d", MonsterFactory::getInstance()->getMonsterBullet().size());
			});
			auto seq = Sequence::create(moveBy, removeBullet, fadeOut, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, bullet)), NULL);
			this->addChild(bullet, 2);
			bullet->runAction(seq);
		}
	}
	if (GlobalVar::getInstance()->getDoublePlayer()) {
		if (attacker2.size() <= 0) return;
		for (int i = 0; i < attacker2.size(); i++) {
			//if (i >= attacker2.size()) break;
			Vec2 Pos = attacker2.at(i)->getPosition();
			Vec2 dir = playerPos2 - Pos;
			dir.normalize();
			// 大怪
			if (attacker2.at(i)->getTag() == 2) {
				auto bullet = MonsterFactory::getInstance()->createMonsterBullet();
				bullet->setPosition(attacker2.at(i)->getPosition());
				auto moveBy = MoveBy::create(5.0f, 350 * dir);
				auto fadeOut = FadeOut::create(1.0f);
				auto removeBullet = CallFunc::create([bullet] {
					MonsterFactory::getInstance()->removeMonsterBullet(bullet);
					//CCLOG("%d", MonsterFactory::getInstance()->getMonsterBullet().size());
				});
				auto seq = Sequence::create(moveBy, removeBullet, fadeOut, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, bullet)), NULL);
				this->addChild(bullet, 2);
				bullet->runAction(seq);
			}
		}
	}
}


void PlayGameScene::exitItemCallBack(cocos2d::Ref* pSender)
{
	CCLOG("exit");
}

void PlayGameScene::gameOver()
{	
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("music/gameover.mp3", false);
	//停止所有调度器和监听器
	unschedule(schedule_selector(PlayGameScene::update));
	_eventDispatcher->removeAllEventListeners();
	if (GlobalVar::getInstance()->getDoublePlayer()) {
		int grade_d[3] = { 0 };

		grade_d[0] = database->getIntegerForKey("first_d", grade_d[0]);
		grade_d[1] = database->getIntegerForKey("second_d", grade_d[1]);
		grade_d[2] = database->getIntegerForKey("third_d", grade_d[2]);

		int grade = GlobalVar::getInstance()->player1Score + GlobalVar::getInstance()->player2Score;

		if (grade_d[0] == 0) {
			database->setIntegerForKey("first_d", grade);
		}
		else if (grade_d[1] == 0) {
			if (grade <= grade_d[0])
				database->setIntegerForKey("second_d", grade);
			else {
				database->setIntegerForKey("first_d", grade);
				database->setIntegerForKey("second_d", grade_d[0]);
			}
		}
		else if (grade_d[2] == 0) {
			if (grade >= grade_d[0]) {
				database->setIntegerForKey("first_d", grade);
				database->setIntegerForKey("second_d", grade_d[0]);
				database->setIntegerForKey("third_d", grade_d[1]);
			}
			else if (grade >= grade_d[1]) {
				database->setIntegerForKey("second_d", grade);
				database->setIntegerForKey("third_d", grade_d[1]);
			}
			else {
				database->setIntegerForKey("third_d", grade);
			}
		}
		else {
			if (grade >= grade_d[0]) {
				database->setIntegerForKey("first_d", grade);
				database->setIntegerForKey("second_d", grade_d[0]);
				database->setIntegerForKey("third_d", grade_d[1]);
			}
			else if (grade >= grade_d[1]) {
				database->setIntegerForKey("second_d", grade);
				database->setIntegerForKey("third_d", grade_d[1]);
			}
			else if (grade >= grade_d[2]) {
				database->setIntegerForKey("third_d", grade);
			}
		}
	}
	else {
		int grade_s[3] = { 0 };

		grade_s[0] = database->getIntegerForKey("first_s", grade_s[0]);
		grade_s[1] = database->getIntegerForKey("second_s", grade_s[1]);
		grade_s[2] = database->getIntegerForKey("third_s", grade_s[2]);

		int grade = GlobalVar::getInstance()->player1Score;


		if (grade_s[0] == 0) {
			database->setIntegerForKey("first_s", grade);
		}
		else if (grade_s[1] == 0) {
			if (grade <= grade_s[0])
				database->setIntegerForKey("second_s", grade);
			else {
				database->setIntegerForKey("first_s", grade);
				database->setIntegerForKey("second_s", grade_s[0]);
			}
		}
		else if (grade_s[2] == 0) {
			if (grade >= grade_s[0]) {
				database->setIntegerForKey("first_s", grade);
				database->setIntegerForKey("second_s", grade_s[0]);
				database->setIntegerForKey("third_s", grade_s[1]);
			}
			else if (grade >= grade_s[1]) {
				database->setIntegerForKey("second_s", grade);
				database->setIntegerForKey("third_s", grade_s[1]);
			}
			else {
				database->setIntegerForKey("third_s", grade);
			}
		}
		else {
			if (grade >= grade_s[0]) {
				database->setIntegerForKey("first_s", grade);
				database->setIntegerForKey("second_s", grade_s[0]);
				database->setIntegerForKey("third_s", grade_s[1]);
			}
			else if (grade >= grade_s[1]) {
				database->setIntegerForKey("second_s", grade);
				database->setIntegerForKey("third_s", grade_s[1]);
			}
			else if (grade >= grade_s[2]) {
				database->setIntegerForKey("third_s", grade);
			}
		}

	}
	Factory::getInstance()->clear();
	GlobalVar::getInstance()->clear();
	MonsterFactory::getInstance()->clear();
	bulletFactory::getInstance()->clear();
	auto MenuScene = MenuScene::create();
	Director::getInstance()->replaceScene(TransitionProgressOutIn::create(0.5, MenuScene));
}

void PlayGameScene::goBackCallBack(cocos2d::Ref * pSender)
{
	CCLOG("goback");
}

void PlayGameScene::exitCallBack(cocos2d::Ref * pSender)
{
	CCLOG("exit Game");
}

void PlayGameScene::meet(EventCustom * event)
{
	// 1.判断子弹是否打中怪物
	// 遍历两个Vector进行判断
	//1.1首先是Player1，有6个子弹队列s,判断子弹是否打中怪物

	for (int i = 0; i < 6; i++) {

		Vector<Sprite*> bullets = bulletFactory::getInstance()->getBulletNumInScene(i, true);
		Vector<Sprite*>::iterator it = bullets.begin();

		// 遍历第i类型子弹队列看是否有击中怪物
		for (it = bullets.begin(); it != bullets.end(); ) {
			Rect b = (*it)->getBoundingBox();
			Sprite* mon = MonsterFactory::getInstance()->collider(b);
			// 没有击中怪物
			if (mon == NULL) {
				//std::string str = "Player1---Type: " + Value(i).asString() + " --- not Hit!";
				//CCLOG(str.c_str());
				it++;
			}
			// 击中怪物
			else {
				std::string str = "Player1---Type: " + Value(i).asString() + " --- Hit!";
				CCLOG(str.c_str());
				//1. 移除子弹
				(*it)->stopAllActions();
				//(*it)->removeFromParent();
				bulletFactory::getInstance()->removeBullet((*it), i, true);
				if (it != bullets.begin()) it--;
				// 怪物扣血
				MonsterFactory::getInstance()->wasHit(mon, i);
				// 伤害得分
				if (i == 0) GlobalVar::getInstance()->player1Score += 30;
				else if (i == 1) GlobalVar::getInstance()->player1Score += 20;
				else if (i == 2) GlobalVar::getInstance()->player1Score += 50;
				else if (i == 3) GlobalVar::getInstance()->player1Score += 80;
				else if (i == 4) GlobalVar::getInstance()->player1Score += 150;
				else if (i == 5) GlobalVar::getInstance()->player1Score += 100;

				break;
			}
			if (it != bullets.end())it++;
		}
	}

	//1.2.首先是Player2，有6个子弹队列s,判断子弹是否打中怪物
	for (int i = 0; i < 6; i++) {
		Vector<Sprite*> bullets = bulletFactory::getInstance()->getBulletNumInScene(i, false);
		Vector<Sprite*>::iterator it = bullets.begin();
		//Vector<Sprite*>::iterator it2 = MonsterFactory::getInstance()->getMonster().begin;
		for (it = bullets.begin(); it != bullets.end(); ) {
			Rect b = (*it)->getBoundingBox();
			Sprite* mon = MonsterFactory::getInstance()->collider(b);
			// 没有击中怪物
			if (mon == NULL) {
				//std::string str = "Player2---Type: " + Value(i).asString() + " --- not hit!";
				//CCLOG(str.c_str());
				it++;
			}
			// 击中怪物
			else {
				std::string str = "Player2---Type: " + Value(i).asString() + " ---- Hit!";
				CCLOG(str.c_str());
				//1. 移除子弹
				(*it)->stopAllActions();
				//(*it)->removeFromParent();
				bulletFactory::getInstance()->removeBullet((*it), i, false);
				if (it != bullets.begin()) it--;
				//怪物扣血
				MonsterFactory::getInstance()->wasHit(mon, i);
				// 伤害得分
				if (i == 0) GlobalVar::getInstance()->player2Score += 30;
				else if (i == 1) GlobalVar::getInstance()->player2Score += 20;
				else if (i == 2) GlobalVar::getInstance()->player2Score += 50;
				else if (i == 3) GlobalVar::getInstance()->player2Score += 80;
				else if (i == 4) GlobalVar::getInstance()->player2Score += 150;
				else if (i == 5) GlobalVar::getInstance()->player2Score += 100;
				break;
			}
			if (it != bullets.end())it++;
		}
	}


	// 2.判断大怪的子弹是否打中帅气的玩家
	Vector<Sprite*> monsterBullets = MonsterFactory::getInstance()->getMonsterBullet();
	Vector<Sprite*>::iterator it = monsterBullets.begin();
	// 遍历大怪子弹队列
	for (it = monsterBullets.begin(); it != monsterBullets.end();) {
		Rect b = (*it)->getBoundingBox();
		Vec2 pos = (*it)->getPosition();
		Sprite* player = Factory::getInstance()->collider(pos);
		// 没有打中玩家
		if (player == NULL) {
			it++;
		}
		// 打中玩家
		else if (player == player1 && GlobalVar::getInstance()->player1HP > 0) {
			//1. 移除子弹
			(*it)->stopAllActions();
			MonsterFactory::getInstance()->removeMonsterBullet((*it));
			(*it)->removeFromParent();
			if (it != monsterBullets.begin()) it--;
			//2.人物扣血
			Factory::getInstance()->wasHit(player, 100, true);
		}
		else if (player == player2 && GlobalVar::getInstance()->getDoublePlayer() && GlobalVar::getInstance()->player2HP > 0) {
			//1. 移除子弹
			(*it)->stopAllActions();
			MonsterFactory::getInstance()->removeMonsterBullet((*it));
			(*it)->removeFromParent();
			if (it != monsterBullets.begin()) it--;
			//2.人物扣血
			Factory::getInstance()->wasHit(player, 100, false);
		}
		if (it != monsterBullets.end()) it++;
	}

	for (int i = 0; i < 6; i++) {
		Vector<Sprite*> bullets = bulletFactory::getInstance()->getBulletNumInScene(i, false);
		Vector<Sprite*>::iterator it = bullets.begin();
		//Vector<Sprite*>::iterator it2 = MonsterFactory::getInstance()->getMonster().begin;
		for (it = bullets.begin(); it != bullets.end(); ) {
			Rect b = (*it)->getBoundingBox();
			Sprite* mon = MonsterFactory::getInstance()->collider(b);
			// 没有击中怪物
			if (mon == NULL) {
				//std::string str = "Player2---Type: " + Value(i).asString() + " --- not hit!";
				//CCLOG(str.c_str());
				it++;
			}
			// 击中怪物
			else {
				std::string str = "Player2---Type: " + Value(i).asString() + " ---- Hit!";
				CCLOG(str.c_str());
				//1. 移除子弹
				(*it)->stopAllActions();
				//(*it)->removeFromParent();
				bulletFactory::getInstance()->removeBullet((*it), i, false);
				if (it != bullets.begin()) it--;
				//怪物扣血
				MonsterFactory::getInstance()->wasHit(mon, i);
				// 伤害得分
				if (i == 0) GlobalVar::getInstance()->player2Score += 30;
				else if (i == 1) GlobalVar::getInstance()->player2Score += 20;
				else if (i == 2) GlobalVar::getInstance()->player2Score += 50;
				else if (i == 3) GlobalVar::getInstance()->player2Score += 80;
				else if (i == 4) GlobalVar::getInstance()->player2Score += 150;
				else if (i == 5) GlobalVar::getInstance()->player2Score += 100;
				break;

			}
			if (it != bullets.end())it++;
		}
	}
}

// 让距离P1足够近的小怪发起攻击
int PlayGameScene::SmallMonsterAttack1(Vec2 playerPos1, float time) {
	if (GlobalVar::getInstance()->player1HP <= 0) {
		return 0;
	}
	Vector<Sprite*> attacker1 = MonsterFactory::getInstance()->getAttacker1();
	if (attacker1.size() <= 0) return 0;
	Vector<Sprite*> monster = MonsterFactory::getInstance()->getMonster();
	int isHit = 0;
	for (int i = 0; i < attacker1.size(); i++) {
		// 获取怪物位置和相对角色的方向
		Vec2 Pos = attacker1.at(i)->getPosition();
		Vec2 dir = playerPos1 - Pos;
		dir.normalize();
		// 小怪
		if (attacker1.at(i)->getTag() == 1) {
			std::string prename = "monster1_attack_";
			std::string direction = MonsterFactory::getInstance()->getDirection(dir);
			std::string name = prename + direction;
			// 播放动画后将对应isDone的值设为true							
			auto ifHit = CallFunc::create([&] {
				Rect monsterRect = attacker1.at(i)->getBoundingBox();
				// 怪物攻击范围为自身周围10
				Rect attackRect = Rect(monsterRect.getMinX() - 5, monsterRect.getMinY() + 5,
					monsterRect.getMaxX() - monsterRect.getMinX() + 10, monsterRect.getMaxY() - monsterRect.getMinY() + 10);
				if (attackRect.containsPoint(playerPos1)) {
					isHit++;
				}
			});
			Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
			int index = i;
			attacker1.at(index)->stopAllActions();
			MonsterFactory::getInstance()->setIsDone(index, false);
			/*auto setDoneToTrue = CallFunc::create([index]() {
			MonsterFactory::getInstance()->setIsDone(index, true);
			});*/
			//auto seq = Sequence::create(animate, ifHit, NULL);
			attacker1.at(index)->runAction(animate);
			Rect monsterRect = attacker1.at(i)->getBoundingBox();
			// 怪物攻击范围为自身周围10
			Rect attackRect = Rect(monsterRect.getMinX() - 5, monsterRect.getMinY() + 5,
				monsterRect.getMaxX() - monsterRect.getMinX() + 10, monsterRect.getMaxY() - monsterRect.getMinY() + 10);
			if (attackRect.containsPoint(playerPos1)) {
				isHit++;
			}
			MonsterFactory::getInstance()->setIsDone(index, true);
		}
	}
	CCLOG("P1 %d", isHit);
	return isHit;
}

// 让距离P2足够近的小怪发起攻击
int PlayGameScene::SmallMonsterAttack2(Vec2 playerPos2, float time) {
	if (GlobalVar::getInstance()->player2HP <= 0) {
		return 0;
	}
	Vector<Sprite*> attacker2 = MonsterFactory::getInstance()->getAttacker2();
	if (attacker2.size() <= 0) return 0;
	int isHit = 0;
	Vector<Sprite*> monster = MonsterFactory::getInstance()->getMonster();
	vector<bool> isDone = MonsterFactory::getInstance()->getIsDone();
	for (int i = 0; i < attacker2.size(); i++) {
		//if (i >= attacker2.size()) break;
		Vec2 Pos = attacker2.at(i)->getPosition();
		Vec2 dir = playerPos2 - Pos;
		dir.normalize();
		// 小怪
		if (attacker2.at(i)->getTag() == 1) {
			std::string prename = "monster1_attack_";
			std::string direction = MonsterFactory::getInstance()->getDirection(dir);
			std::string name = prename + direction;
			// 播放动画后将对应isDone的值设为true
			auto ifHit = CallFunc::create([&] {
				Rect monsterRect = attacker2.at(i)->getBoundingBox();
				// 怪物攻击范围为自身周围10
				Rect attackRect = Rect(monsterRect.getMinX() - 5, monsterRect.getMinY() + 5,
					monsterRect.getMaxX() - monsterRect.getMinX() + 10, monsterRect.getMaxY() - monsterRect.getMinY() + 10);
				if (attackRect.containsPoint(playerPos2)) {
					isHit++;
				}
			});
			Animate* animate = Animate::create(AnimationCache::getInstance()->getAnimation(name));
			int index = i;
			attacker2.at(index)->stopAllActions();
			MonsterFactory::getInstance()->setIsDone(index, false);
			/*auto setDoneToTrue = CallFunc::create([index]() {
			MonsterFactory::getInstance()->setIsDone(index, true);
			});*/
			//auto seq = Sequence::create(animate, ifHit, NULL);
			attacker2.at(index)->runAction(animate);
			Rect monsterRect = attacker2.at(i)->getBoundingBox();
			// 怪物攻击范围为自身周围10
			Rect attackRect = Rect(monsterRect.getMinX() - 5, monsterRect.getMinY() + 5,
				monsterRect.getMaxX() - monsterRect.getMinX() + 10, monsterRect.getMaxY() - monsterRect.getMinY() + 10);
			if (attackRect.containsPoint(playerPos2)) {
				isHit++;
			}
			MonsterFactory::getInstance()->setIsDone(index, true);
		}
	}
	CCLOG("P2 %d", isHit);
	return isHit;
}

