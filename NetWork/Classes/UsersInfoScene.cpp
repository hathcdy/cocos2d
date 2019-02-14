#include "UsersInfoScene.h"
#include "json\document.h"
#include "Utils.h"
#include <sstream>

using namespace rapidjson;

cocos2d::Scene * UsersInfoScene::createScene() {
  return UsersInfoScene::create();
}

bool UsersInfoScene::init() {
  if (!Scene::init()) return false;

  auto visibleSize = Director::getInstance()->getVisibleSize();
  Vec2 origin = Director::getInstance()->getVisibleOrigin();

  auto getUserButton = MenuItemFont::create("Get User", CC_CALLBACK_1(UsersInfoScene::getUserButtonCallback, this));
  if (getUserButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + getUserButton->getContentSize().height / 2;
    getUserButton->setPosition(Vec2(x, y));
  }

  auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
    Director::getInstance()->popScene();
  });
  if (backButton) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - backButton->getContentSize().height / 2;
    backButton->setPosition(Vec2(x, y));
  }

  auto menu = Menu::create(getUserButton, backButton, NULL);
  menu->setPosition(Vec2::ZERO);
  this->addChild(menu, 1);

  limitInput = TextField::create("limit", "arial", 24);
  if (limitInput) {
    float x = origin.x + visibleSize.width / 2;
    float y = origin.y + visibleSize.height - 100.0f;
    limitInput->setPosition(Vec2(x, y));
    this->addChild(limitInput, 1);
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

void UsersInfoScene::getUserButtonCallback(Ref * pSender) {
  // Your code here
	HttpRequest* request = new HttpRequest();
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(UsersInfoScene::onHttpRequestCompleted, this));
	request->setUrl("http://127.0.0.1:8000/users?limit=" + limitInput->getString());

	/*const char * postData = serializeRapidjsonDocument(document).c_str();
	request->setRequestData(postData, strlen(postData));*/
	HttpClient::getInstance()->send(request);
	request->release();
}

void UsersInfoScene::onHttpRequestCompleted(HttpClient* sender, HttpResponse* response) {
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
	//CCLOG("%d", responseStr.size());

	rapidjson::Document d;
	d.Parse<0>(responseStr.c_str());
	if (d.HasParseError()) {
		CCLOG("GetParseError %s\n", d.GetParseError());
	}

	if (d.IsObject() && d.HasMember("status") && d.HasMember("msg")) {
		bool status = d["status"].GetBool();
		std::string msg = d["msg"].GetString();
		rapidjson::Value data(rapidjson::kArrayType);

		std::string result;
		if (status) {
			result = "Succeed!\n" + msg + "\n";
		}
		else {
			result = "Failed!\n" + msg + "\n";
		}

		data = d["data"].GetArray();
		//CCLOG("%d", data.Size());
		std::string username;
		rapidjson::Value deck(rapidjson::kArrayType);
		rapidjson::Value cards(rapidjson::kObjectType);
		rapidjson::Value card(rapidjson::kObjectType);
		for (int i = 0; i < data.Size(); i++) {
			result += "username: ";
			result += data[i]["username"].GetString();
			result += "\n";
			result += "deck:\n";
			deck = data[i]["deck"].GetArray();
			for (int j = 0; j < deck.Size(); j++) {
				cards = deck[j].GetObjectW();				
				rapidjson::Value::ConstMemberIterator it = cards.MemberBegin();
				while (it != cards.MemberEnd()) {
					CCLOG("%s", it->name.GetString());
					CCLOG("%d", it->value.GetInt());
					std::string name = it->name.GetString();
					int num = it->value.GetInt();
					std::string number;
					std::stringstream ss;
					ss << num;
					ss >> number;
					result += name;
					result += ":";
					result += number;
					result += "\n";
					it++;
				}		
			}
		}
		messageBox->setString(result);
	}
}

