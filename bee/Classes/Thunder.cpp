#include "Thunder.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Thunder::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool Thunder::init() {
	if (!Layer::init()) {
		return false;
	}
	stoneType = 0;
	isMove = false;  // 是否点击飞船
	visibleSize = Director::getInstance()->getVisibleSize();

	// 创建背景
	auto bgsprite = Sprite::create("bg.jpg");
	bgsprite->setPosition(visibleSize / 2);
	bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width,
		visibleSize.height / bgsprite->getContentSize().height);
	this->addChild(bgsprite, 0);

	// 创建飞船
	player = Sprite::create("player.png");
	player->setAnchorPoint(Vec2(0.5, 0.5));
	player->setPosition(visibleSize.width / 2, player->getContentSize().height);
	player->setName("player");
	this->addChild(player, 1);

	// 显示陨石和子弹数量
	enemysNum = Label::createWithTTF("enemys: 0", "fonts/arial.TTF", 20);
	enemysNum->setColor(Color3B(255, 255, 255));
	enemysNum->setPosition(50, 60);
	this->addChild(enemysNum, 3);
	bulletsNum = Label::createWithTTF("bullets: 0", "fonts/arial.TTF", 20);
	bulletsNum->setColor(Color3B(255, 255, 255));
	bulletsNum->setPosition(50, 30);
	this->addChild(bulletsNum, 3);

	addEnemy(5);        // 初始化陨石
	preloadMusic();     // 预加载音乐
	playBgm();          // 播放背景音乐
	explosion();        // 创建爆炸帧动画

	// 添加监听器
	addTouchListener();
	addKeyboardListener();
	addCustomListener();

	// 调度器
	schedule(schedule_selector(Thunder::update), 0.04f, kRepeatForever, 0);

	return true;
}

//预加载音乐文件
void Thunder::preloadMusic() {
	// Todo
	auto audio = SimpleAudioEngine::getInstance();

	//加载背景音乐
	audio->preloadBackgroundMusic("music/bgm/mp3");

	//加载爆炸和开火音效
	audio->preloadEffect("music/explore.wav");
	audio->preloadEffect("music/fire.wav");
}

//播放背景音乐
void Thunder::playBgm() {
	// Todo
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm.mp3", true);
}

//初始化陨石
void Thunder::addEnemy(int n) {
	enemys.clear();
	for (int i = 2; i >= 0; --i) {
		char enemyPath[20];
		sprintf(enemyPath, "stone%d.png", 3 - i);
		double width = visibleSize.width / (n + 1.0),
			height = visibleSize.height - (50 * (i + 1));
		for (int j = 0; j < n; ++j) {
			auto enemy = Sprite::create(enemyPath);
			enemy->setAnchorPoint(Vec2(0.5, 0.5));
			enemy->setScale(0.5, 0.5);
			enemy->setPosition(width * (j + 1), height);
			enemys.push_back(enemy);
			addChild(enemy, 1);
		}
	}
}

// 陨石向下移动并生成新的一行(加分项)
void Thunder::newEnemy() {
	// Todo
	char enemyPath[20];
	if (stoneType == 3) {
		stoneType = 0;
	}
	stoneType++;
	list<Sprite*>::iterator it;
	it = enemys.begin(); 
	sprintf(enemyPath, "stone%d.png", stoneType);
	double width = visibleSize.width / (5 + 1.0),
		height = visibleSize.height - 50;
	auto move = MoveBy::create(0.4f, Vec2(0, -50));	
	it = enemys.begin();
	while (it != enemys.end()) {
		(*it)->runAction(move->clone());
		it++;
	}
	for (int i = 0; i < 5; i++) {
		auto enemy = Sprite::create(enemyPath);
		enemy->setAnchorPoint(Vec2(0.5, 0.5));
		enemy->setScale(0.5, 0.5);
		enemy->setPosition(width * (i + 0.5), height);
		enemys.push_back(enemy);
		addChild(enemy, 1);
	}
}

// 移动飞船
void Thunder::movePlane(char c) {
	// Todo
	auto position = player->getPosition();
	switch (c) {
	case 'A':
		if (position.x >= 64) {
			auto moveA = MoveBy::create(0.4f, Vec2(-32, 0));
			player->runAction(moveA);
		}
		else {
			player->setPosition(64, player->getPositionY());
		}
		break;
	case 'D':
		if (position.x <= visibleSize.width - 64) {
			auto moveD = MoveBy::create(0.4f, Vec2(32, 0));
			player->runAction(moveD);
		}
		else {
			player->setPosition(visibleSize.width - 64, player->getPositionY());
		}
		break;
	default:
		break;
	}
}

//发射子弹
void Thunder::fire() {
	auto bullet = Sprite::create("bullet.png");
	bullet->setAnchorPoint(Vec2(0.5, 0.5));
	bullets.push_back(bullet);
	bullet->setPosition(player->getPosition());
	addChild(bullet, 1);
	SimpleAudioEngine::getInstance()->playEffect("music/fire.wav", false);

	//让子弹飞
	auto fly = MoveBy::create(3.0f, Vec2(0, 1000));
	bullet->runAction(fly);

	isClick = false;
	// 移除飞出屏幕外的子弹
	// Todo
}

void Thunder::removeBullets() {
	auto position = Vec2();
	list<Sprite*>::iterator it;
	it = bullets.begin();
	while (it != bullets.end()) {
		position = (*it)->getPosition();
		if (position.y >= visibleSize.height)
		{
			(*it)->removeFromParent();
			it = bullets.erase(it);
		}
		else
		{
			it++;
		}
	}
}

// 切割爆炸动画帧
void Thunder::explosion() {
	// Todo
	auto exploseTexture = Director::getInstance()->getTextureCache()->addImage("explosion.png");
	explose.reserve(8);
	for (int i = 0; i < 5; i++) {
		auto frame = SpriteFrame::createWithTexture(exploseTexture, CC_RECT_PIXELS_TO_POINTS(Rect(185 * i, 0, 185, 212)));
		explose.pushBack(frame);
	}
	for (int i = 0; i < 3; i++) {
		auto frame = SpriteFrame::createWithTexture(exploseTexture, CC_RECT_PIXELS_TO_POINTS(Rect(185 * i, 212, 185, 212)));
		explose.pushBack(frame);
	}
	auto exploseAnimation = Animation::createWithSpriteFrames(explose, 0.1f);
	AnimationCache::getInstance()->addAnimation(exploseAnimation, "explose");
}

void Thunder::update(float f) {
	// 实时更新页面内陨石和子弹数量(不得删除)

	// 移除飞出屏幕外的子弹
	removeBullets();

	// 要求数量显示正确(加分项)
	char str[15];
	sprintf(str, "enemys: %d", enemys.size());
	enemysNum->setString(str);
	sprintf(str, "bullets: %d", bullets.size());
	bulletsNum->setString(str);

	// 飞船移动
	if (isMove)
		this->movePlane(movekey);

	if (isClick)
		fire();

	static int ct = 0;
	static int dir = 4;
	++ct;
	if (ct == 120)
		ct = 40, dir = -dir;
	else if (ct == 80) {
		dir = -dir;
		newEnemy();  // 陨石向下移动并生成新的一行(加分项)
	}
	else if (ct == 20)
		ct = 40, dir = -dir;

	//陨石左右移动
	for (Sprite* s : enemys) {
		if (s != NULL) {
			s->setPosition(s->getPosition() + Vec2(dir, 0));
		}
	}

	// 分发自定义事件
	EventCustom e("meet");
	_eventDispatcher->dispatchEvent(&e);

	EventCustom d("dead");
	_eventDispatcher->dispatchEvent(&d);

}

// 自定义碰撞事件
void Thunder::meet(EventCustom * event) {
	// 判断子弹是否打中陨石并执行对应操作
	// Todo
	list<Sprite*>::iterator it1;
	list<Sprite*>::iterator it2;
	it1 = bullets.begin();	
	while (it1 != bullets.end()) {
		it2 = enemys.begin();
		while (it2 != enemys.end()) {
			if ((*it1)->getPosition().getDistance((*it2)->getPosition()) < 25) {
				(*it1)->removeFromParent();
				it1 = bullets.erase(it1);
				Boom((*it2));
				it2 = enemys.erase(it2);
				break;
			}
			it2++;
		}
		if (it1 != bullets.end())		
			it1++;
	}	
}

void Thunder::meet1(EventCustom * event) {
	list<Sprite*>::iterator it;
	it = enemys.begin();
	if ((*it)->getPosition().y < 0) {
		Boom(player);
		stopAc();
		return;
	}
	while (it != enemys.end()) {
		if (player->getPosition().getDistance((*it)->getPosition()) < 32) {
			Boom(player);
			Boom((*it));
			if (it == enemys.begin()) {
				enemys.erase(it);
				it = enemys.begin();
			}
			else {
				it = enemys.erase(it);
			}
			stopAc();
			return;
		}
		else
		{
			it++;
		}
	}

}


void Thunder::Boom(Sprite* enemy) {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playEffect("music/explore.wav");
	auto explose = AnimationCache::getInstance()->getAnimation("explose");
	auto exp = Animate::create(explose);
	Sequence* seq = Sequence::createWithTwoActions(exp, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, enemy)));
	enemy->runAction(seq);	
}



void Thunder::stopAc() {
    // Todo
	auto gameOver = Sprite::create("gameOver.png");
	gameOver->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(gameOver, 1);
	unschedule(schedule_selector(Thunder::update));
	bulletsNum->setString("bullets: 0");
	_eventDispatcher->removeAllEventListeners();
}



// 添加自定义监听器
void Thunder::addCustomListener() {
	// Todo
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	auto deadListener = EventListenerCustom::create("dead", CC_CALLBACK_1(Thunder::meet1, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
	_eventDispatcher->addEventListenerWithFixedPriority(deadListener, 1);
}

// 添加键盘事件监听器
void Thunder::addKeyboardListener() {
	// Todo
	auto keyBoardListener = EventListenerKeyboard::create();
	keyBoardListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	keyBoardListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithFixedPriority(keyBoardListener, 1);
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_A:
		movekey = 'A';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
	case EventKeyboard::KeyCode::KEY_D:
		movekey = 'D';
		isMove = true;
		break;
	case EventKeyboard::KeyCode::KEY_SPACE:
		fire();
		break;
	}
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case EventKeyboard::KeyCode::KEY_A:
	case EventKeyboard::KeyCode::KEY_CAPITAL_A:
	case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case EventKeyboard::KeyCode::KEY_D:
	case EventKeyboard::KeyCode::KEY_CAPITAL_D:
		isMove = false;
		break;
	}
}

// 添加触摸事件监听器
void Thunder::addTouchListener() {
	// Todo
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(Thunder::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(Thunder::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
}

// 鼠标点击发射炮弹
bool Thunder::onTouchBegan(Touch *touch, Event *event) {
	if (touch->getLocation().getDistance(player->getPosition()) <= 30)
        isClick = true;
    // Todo
	return true;
}

void Thunder::onTouchEnded(Touch *touch, Event *event) {
	isClick = false;
}

// 当鼠标按住飞船后可控制飞船移动 (加分项)
void Thunder::onTouchMoved(Touch *touch, Event *event) {
	// Todo

	Vec2 delta = touch->getDelta();
	/*auto move = MoveBy::create(0.4f, Vec2(delta.x, 0));
	player->runAction(move);*/
	if (player->getPositionX() >= 50 && player->getPositionX() <= visibleSize.width - 50) {
		player->setPosition(Vec2(player->getPositionX() + delta.x, player->getPositionY()));
	}
	else if (player->getPositionX() < 50) {
		player->setPosition(Vec2(50, player->getPositionY()));
	}
	else {
		player->setPosition(Vec2(visibleSize.width - 50, player->getPositionY()));
	}
}
