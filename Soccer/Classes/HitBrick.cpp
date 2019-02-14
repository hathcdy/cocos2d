#pragma execution_character_set("utf-8")
#include "HitBrick.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#define database UserDefault::getInstance()

USING_NS_CC;
using namespace CocosDenshion;

void HitBrick::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* HitBrick::createScene() {
  srand((unsigned)time(NULL));
  auto scene = Scene::createWithPhysics();

  scene->getPhysicsWorld()->setAutoStep(true);

  // Debug ģʽ
  // scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
  scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
  auto layer = HitBrick::create();
  layer->setPhysicsWorld(scene->getPhysicsWorld());
  layer->setJoint();
  scene->addChild(layer);
  return scene;
}

// on "init" you need to initialize your instance
bool HitBrick::init() {
  //////////////////////////////
  // 1. super init first
  if (!Layer::init()) {
    return false;
  }
  visibleSize = Director::getInstance()->getVisibleSize();

  auto edgeSp = Sprite::create();  //����һ������
  auto boundBody = PhysicsBody::createEdgeBox(visibleSize, PhysicsMaterial(0.0f, 1.0f, 0.0f), 3);  //edgebox�ǲ��ܸ�����ײӰ���һ�ָ��壬����������������������ı߽�
  edgeSp->setPosition(visibleSize.width / 2, visibleSize.height / 2);  //λ����������Ļ����
  edgeSp->setPhysicsBody(boundBody);
  addChild(edgeSp);


  preloadMusic(); // Ԥ������Ч

  addSprite();    // ��ӱ����͸��־���
  addListener();  // ��Ӽ����� 
  addPlayer();    // ��������
  BrickGeneraetd();  // ����ש��

  schedule(schedule_selector(HitBrick::update), 0.01f, kRepeatForever, 0.1f);

  onBall = true;

  spFactor = 0;
  return true;
}

// �ؽ����ӣ��̶��������
// Todo
void HitBrick::setJoint() {
	joint1 = PhysicsJointPin::construct(ball->getPhysicsBody(), player->getPhysicsBody(), 
		ball->getPosition());
	m_world->addJoint(joint1);
}


// Ԥ������Ч
void HitBrick::preloadMusic() {
  auto sae = SimpleAudioEngine::getInstance();
  sae->preloadEffect("gameover.mp3");
  sae->preloadBackgroundMusic("bgm.mp3");
  sae->playBackgroundMusic("bgm.mp3", true);
}

// ��ӱ����͸��־���
void HitBrick::addSprite() {
  // add background
  auto bgSprite = Sprite::create("bg.png");
  bgSprite->setPosition(visibleSize / 2);
  bgSprite->setScale(visibleSize.width / bgSprite->getContentSize().width, visibleSize.height / bgSprite->getContentSize().height);
  this->addChild(bgSprite, 0);


  // add ship
  ship = Sprite::create("ship.png");
  ship->setScale(visibleSize.width / ship->getContentSize().width * 0.97, 1.2f);
  ship->setPosition(visibleSize.width / 2, 0);
  auto shipbody = PhysicsBody::createBox(ship->getContentSize(), PhysicsMaterial(100.0f, 0.0f, 1.0f));
  shipbody->setCategoryBitmask(0xFFFFFFFF);
  shipbody->setCollisionBitmask(0xFFFFFFFF);
  shipbody->setContactTestBitmask(0xFFFFFFFF);
  shipbody->setTag(1);
  shipbody->setDynamic(false);  // ??????�I?????????, ????????????��??
  ship->setPhysicsBody(shipbody);
  this->addChild(ship, 1);

  // add sun and cloud
  auto sunSprite = Sprite::create("sun.png");
  sunSprite->setPosition(rand() % (int)(visibleSize.width - 200) + 100, 550);
  this->addChild(sunSprite);
  auto cloudSprite1 = Sprite::create("cloud.png");
  cloudSprite1->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite1);
  auto cloudSprite2 = Sprite::create("cloud.png");
  cloudSprite2->setPosition(rand() % (int)(visibleSize.width - 200) + 100, rand() % 100 + 450);
  this->addChild(cloudSprite2);
}

// ��Ӽ�����
void HitBrick::addListener() {
  auto keyboardListener = EventListenerKeyboard::create();
  keyboardListener->onKeyPressed = CC_CALLBACK_2(HitBrick::onKeyPressed, this);
  keyboardListener->onKeyReleased = CC_CALLBACK_2(HitBrick::onKeyReleased, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

  auto contactListener = EventListenerPhysicsContact::create();
  contactListener->onContactBegin = CC_CALLBACK_1(HitBrick::onConcactBegin, this);
  //contactListener->onContactPostSolve = CC_CALLBACK_1(HitBrick::onContactPostSolve, this);
  contactListener->onContactSeparate = CC_CALLBACK_1(HitBrick::onContactSeperate, this);
  _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

// ������ɫ
void HitBrick::addPlayer() {

  player = Sprite::create("bar.png");
  int xpos = visibleSize.width / 2;

  player->setScale(0.1f, 0.1f);
  player->setPosition(Vec2(xpos, ship->getContentSize().height - player->getContentSize().height*0.1f));
  // ���ð�ĸ�������
  // Todo
  auto playerbody = PhysicsBody::createBox(player->getContentSize(), PhysicsMaterial(10.0f, 1.0f, 0.0f));
  playerbody->setCategoryBitmask(0x01);
  playerbody->setCollisionBitmask(0x01);
  playerbody->setContactTestBitmask(0x01);
  playerbody->setDynamic(false); 
  playerbody->setTag(4);
  player->setPhysicsBody(playerbody);

  this->addChild(player, 2);
  
  ball = Sprite::create("ball.png");
  ball->setPosition(Vec2(xpos, player->getPosition().y + ball->getContentSize().height*0.1f - 28));
  ball->setScale(0.1f, 0.1f);
  // ������ĸ�������
  // Todo
  auto ballbody = PhysicsBody::createBox(ball->getContentSize(), PhysicsMaterial(1.0f, 1.0f, 0.0f));
  ballbody->setCategoryBitmask(0x03);
  ballbody->setCollisionBitmask(0x03);
  ballbody->setContactTestBitmask(0x03);
  ballbody->setGravityEnable(false);
  ballbody->setTag(2);  
  //ballbody->setRotationEnable(false);
  ball->setPhysicsBody(ballbody);

  addChild(ball, 3);
  
  //����Ч��
  ParticleMeteor* meteor = ParticleMeteor::create();
  meteor->setPosition(ball->getContentSize().width / 2, ball->getContentSize().height / 2);
  meteor->setPositionType(ParticleSystem::PositionType::FREE);
  meteor->setScale(5.0f);
  ball->addChild(meteor);
}

// ʵ�ּ򵥵�����Ч��
// Todo
void HitBrick::update(float dt) {
	if (spHolded && spFactor <= 300) {
		spFactor++;
	}
	if (onBall) {
		ball->getPhysicsBody()->setRotationEnable(true);
	}
	else {
		ball->getPhysicsBody()->setRotationEnable(false);
	}
	if (isMove) {
		if (player->getPositionX() < 60) {
			player->getPhysicsBody()->setVelocity(Vec2(5, 0));
		}
		if (player->getPositionX() > visibleSize.width - 60) {
			player->getPhysicsBody()->setVelocity(Vec2(-5, 0));
		}
	}
	if (joint1 != nullptr) {
		ball->getPhysicsBody()->setRotationEnable(false);
	}
}




// Todo
void HitBrick::BrickGeneraetd() {

	for (int i = 0; i < 3; i++) {
		int cw = 0;
		while (cw <= visibleSize.width) {
			auto box = Sprite::create("box.png");
			// Ϊש�����ø�������
			// Todody
			auto boxbody = PhysicsBody::createBox(box->getContentSize(), PhysicsMaterial(10.0f, 1.0f, 0.0f));
			boxbody->setCategoryBitmask(0x01);
			boxbody->setCollisionBitmask(0x01);
			boxbody->setContactTestBitmask(0x01);
			boxbody->setDynamic(false);
			boxbody->setTag(3);
			box->setPhysicsBody(boxbody);
			box->setPosition(cw, visibleSize.height - 22 * i - 11);
			this->addChild(box, 2);
			cw += 38;
		}

	}

}


// ����
void HitBrick::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {

  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	  isMove = true;
	  player->getPhysicsBody()->setVelocity(Vec2(-300, 0));	  
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    // �����ƶ�
    // Todo
	  isMove = true;
	  player->getPhysicsBody()->setVelocity(Vec2(300, 0));
	break;
  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE: // ��ʼ����
	  spHolded = true;
	  break;
  default:
    break;
  }
}

// �ͷŰ���
void HitBrick::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
  switch (code) {
  case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	  if (player->getPhysicsBody()->getVelocity().x < 0) {
		  player->getPhysicsBody()->setVelocity(Vec2(0, 0));
		  isMove = false;
	  }
	  break;
  case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
    // ֹͣ�˶�
	  if (player->getPhysicsBody()->getVelocity().x > 0) {
		  player->getPhysicsBody()->setVelocity(Vec2(0, 0));
		  isMove = false;
	  }	  
    // Todo
    break;
  case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:   // ����������С����
	  spHolded = false;
	  if (joint1 != nullptr)
	  {
		  m_world->removeJoint(joint1, true);
		  joint1 = nullptr;
	  }
	  if (onBall) {
		  ball->getPhysicsBody()->setVelocity(Vec2(0, 300 + 2 * spFactor));
		  ball->getPhysicsBody()->setGravityEnable(false);
		  onBall = false;		
		  spFactor = 0;
	  }
    break;

  default:
    break;
  }
}

// ��ײ���
// Todo
bool HitBrick::onConcactBegin(PhysicsContact & contact) {
  auto c1 = contact.getShapeA(), c2 = contact.getShapeB();
  int tag1 = c1->getBody()->getTag(), tag2 = c2->getBody()->getTag();
  printf("%d %d\n", tag1, tag2);
  if ((tag1 == 1 && tag2 == 2) || (tag1 == 2 && tag2 == 1)) {
	  GameOver();
  }
  else if (tag1 == 2 && tag2 == 3 ) {
	  auto brick = (Sprite*)c2->getBody()->getNode();
	  brick->removeFromParent();
  }
  else if (tag1 == 3 && tag2 == 2) {
	  auto brick = (Sprite*)c1->getBody()->getNode();
	  brick->removeFromParent();
  } 
  else if ((tag1 == 2 && tag2 == 4) || (tag1 == 4 && tag2 == 2)) {
	  onBall = true;
  }  
  return true;
}

//void HitBrick::onContactPostSolve(PhysicsContact & contact) {
//	ball->getPhysicsBody()->setRotationEnable(true);
//}

void HitBrick::onContactSeperate(PhysicsContact & contact) {
	onBall = false;
}


void HitBrick::GameOver() {

	_eventDispatcher->removeAllEventListeners();
	ball->getPhysicsBody()->setVelocity(Vec2(0, 0));
	player->getPhysicsBody()->setVelocity(Vec2(0, 0));
  SimpleAudioEngine::getInstance()->stopBackgroundMusic("bgm.mp3");
  SimpleAudioEngine::getInstance()->playEffect("gameover.mp3", false);

  auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
  label1->setColor(Color3B(0, 0, 0));
  label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
  this->addChild(label1);

  auto label2 = Label::createWithTTF("����", "fonts/STXINWEI.TTF", 40);
  label2->setColor(Color3B(0, 0, 0));
  auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(HitBrick::replayCallback, this));
  Menu* replay = Menu::create(replayBtn, NULL);
  replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
  this->addChild(replay);

  auto label3 = Label::createWithTTF("�˳�", "fonts/STXINWEI.TTF", 40);
  label3->setColor(Color3B(0, 0, 0));
  auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(HitBrick::exitCallback, this));
  Menu* exit = Menu::create(exitBtn, NULL);
  exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
  this->addChild(exit);
}

// ���������水ť��Ӧ����
void HitBrick::replayCallback(Ref * pSender) {
  Director::getInstance()->replaceScene(HitBrick::createScene());
}

// �˳�
void HitBrick::exitCallback(Ref * pSender) {
  Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
  exit(0);
#endif
}
