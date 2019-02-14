#include "ModifyUserScene.h"
#include "Utils.h"
#include "json\document.h"
#include "json\rapidjson.h"
#include "json\stringbuffer.h"
#include "json\writer.h"

using namespace rapidjson;
using namespace cocos2d::network;
using namespace cocos2d::ui;

cocos2d::Scene * ModifyUserScene::createScene() {
  return ModifyUserScene::create();
}

bool ModifyUserScene::init() {
  if (!Scene::init()) return false;
  
  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto postDeckButton = MenuItemFont::create("Post Deck", CC_CALLBACK_1(ModifyUserScene::putDeckButtonCallback, this));
  if (postDeckButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + postDeckButton->getContentSize().height / 2;
    postDeckButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(postDeckButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  deckInput = TextField::create("Deck json here", "arial", 24);
  if (deckInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    deckInput->setPosition(Vec2(x, y));
    this->addChild(deckInput, 1);
  }

  messageBox = Label::create("", "arial", 30);
  if (messageBox) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height / 2;
    messageBox->setPosition(Vec2(x, y));
    this->addChild(messageBox, 1);
  }

  return true;
}

void ModifyUserScene::putDeckButtonCallback(Ref * pSender) {
  // Your code here
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::PUT);
	request->setResponseCallback(CC_CALLBACK_2(ModifyUserScene::onHttpRequestCompleted, this));
	request->setUrl("http://127.0.0.1:8000/users");
	
	CCLOG("%s", deckInput->getString());

	std::string requestStr = "{\"deck\":";
	requestStr += deckInput->getString();
	requestStr += "}";
	CCLOG(requestStr.c_str());
	request->setRequestData(requestStr.c_str(), requestStr.size());
	/*const char * postData = serializeRapidjsonDocument(document).c_str();
	request->setRequestData(postData, strlen(postData));*/
	HttpClient::getInstance()->send(request);
	request->release();
}


void ModifyUserScene::onHttpRequestCompleted(HttpClient * sender, HttpResponse * response) {
	if (!response) {
		return;
	}
	if (!response->isSucceed()) {
		CCLOG("response fail!");
		CCLOG("error buffer:%s", response->getErrorBuffer());
		return;
	}

	std::vector<char> *buffer = response->getResponseData();
	std::string responseStr;
	for (int i = 0; i < buffer->size(); i++) {
		printf("%c", (*buffer)[i]);
		responseStr += (*buffer)[i];
	}
	CCLOG("%s", responseStr.c_str());
	CCLOG("%d", responseStr.size());

	rapidjson::Document d;
	d.Parse<0>(responseStr.c_str());
	if (d.HasParseError()) {
		CCLOG("GetParseError %s\n", d.GetParseError());
	}

	if (d.IsObject() && d.HasMember("status") && d.HasMember("msg")) {
		bool status = d["status"].GetBool();
		std::string msg = d["msg"].GetString();
		std::string result;
		if (status) {
			result = "Succeed!\n" + msg;
		}
		else {
			result = "Failed!\n" + msg;
		}
		messageBox->setString(result);
	}
}