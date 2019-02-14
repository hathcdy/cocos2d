#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

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

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }	

	auto hideItem = MenuItemLabel::create(Label::createWithTTF("Hide", "fonts/Marker Felt.ttf", 20), CC_CALLBACK_1(HelloWorld::hideCallBack, this));
	float x1 = origin.x + visibleSize.width - hideItem->getContentSize().width/2;
	float y1 = origin.y + hideItem->getContentSize().height*1.5;
	hideItem->setPosition(x1, y1);

	auto showItem = MenuItemLabel::create(Label::createWithTTF("Show", "fonts/Marker Felt.ttf", 20), CC_CALLBACK_1(HelloWorld::showCallBack, this));
	float x2 = origin.x + visibleSize.width - showItem->getContentSize().width/2;
	float y2 = origin.y + showItem->getContentSize().height*3;
	showItem->setPosition(x2, y2);

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, showItem, hideItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
	auto *NameString = CCDictionary::createWithContentsOfFile("Name.xml");
	const char *name = ((String*)NameString->objectForKey("name"))->getCString();

	auto NameLabel = Label::createWithTTF(name, "fonts/msyh.ttf", 24);

	if (NameLabel == nullptr)
	{
		problemLoading("'fonts/msyh.ttf'");
	}
	else
	{
		NameLabel->setPosition(Vec2(origin.x + visibleSize.width/2,
									origin.y + visibleSize.height - NameLabel->getContentSize().height));

		this->addChild(NameLabel, 1);
	}

    auto label = Label::createWithTTF("16340016", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        // position the label on the center of the screen
        label->setPosition(Vec2(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - 3*label->getContentSize().height));

        // add the label as a child to this layer
        this->addChild(label, 1);
    }

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("sysu.jpg");
	sprite->setName("image");
    if (sprite == nullptr)
    {
        problemLoading("'sysu.jpg'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

void HelloWorld::hideCallBack(Ref* pSender) 
{
	auto image = this->getChildByName("image");
	if (image->isVisible())
	{		
		image->setVisible(false);
	}
	else
	{
		return;
	}
}

void HelloWorld::showCallBack(Ref* pSender) 
{
	auto image = this->getChildByName("image");
	if (image->isVisible())
	{		
		return;
	}
	else
	{
		image->setVisible(true);
	}
}
