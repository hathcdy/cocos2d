#pragma once
#ifndef GLOBAL_VAR_H_
#define GLOBAL_VAL_H

#include "cocos2d.h"
using namespace std;
//存放全局变量的类，单例模式
class GlobalVar : public cocos2d::Ref{
private:
	GlobalVar();
	static GlobalVar* instance;
	//是否双人模式
	bool double_player;
	//文件来源
	string mapFile;
public:
	static GlobalVar* getInstance();
	string getMapFile();
	bool getDoublePlayer();
	void setMapFile(string m);
	void setDoublePlayer(bool b);
	//获得武器类型
	int getAttack1();
	int getAttack2();

	void clear();

	//控制player1的移动
	bool player1_isMove_hori = false;
	bool player1_isMove_vert = false;
	char player1_moveKey_hori;
	char player1_moveKey_vert;

	//控制player2的移动
	bool player2_isMove_hori = false;
	bool player2_isMove_vert = false;
	char player2_moveKey_hori;
	char player2_moveKey_vert;

	// 当前player是否执行完动画
	bool player1_done = true;
	bool player2_done = true;
	// 当前玩家的移动帧动画移动方向
	string player1_moving_direction = "West";
	string player2_moving_direction = "West";

	// 八方向方位
	string Direction[8] = { "West", "NorthWest", "North", "NorthEast", "East", "SouthEast", "South", "SouthWest" };

	// 玩家现在用着的武器类型
	int attack_Type_For_Player1 = 0;
	int attack_Type_For_Player2 = 0;

	// 玩家朝向
	string player1_toward = "West";
	string player2_toward = "West";

	// 玩家射击动作是否终止
	bool player1_isShotDone = true;
	bool player2_isShotDone = true;

	// 玩家血量
	int player1HP = 100;
	int player2HP = 100;

	// 武器名称
	string Weapon[6] = { "Pistol", "Uzi", "Shotgun", "Grenade", "Missile", "Laser_gun" };

	// 玩家的击杀分数
	int player1Score = 0;
	int player2Score = 0;

};
#endif GLOBAL_VAL_H
