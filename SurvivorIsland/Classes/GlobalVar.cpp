#include "GlobalVar.h"

GlobalVar* GlobalVar::instance = NULL;

GlobalVar* GlobalVar::getInstance() {
	if (instance == NULL) instance = new GlobalVar();
	return instance;
}
GlobalVar::GlobalVar() {
	mapFile = "map0.tmx";
	double_player = true;
}
void GlobalVar::setMapFile(string m) { mapFile = m; }
void GlobalVar::setDoublePlayer(bool b) { double_player = b; }
string GlobalVar::getMapFile() { return mapFile; }
bool GlobalVar::getDoublePlayer() { return double_player; }
int GlobalVar::getAttack1() { return attack_Type_For_Player1; }
int GlobalVar::getAttack2() { return attack_Type_For_Player2; }

void GlobalVar::clear() {
	//控制player1的移动
	player1_isMove_hori = false;
	player1_isMove_vert = false;

	//控制player2的移动
	player2_isMove_hori = false;
	player2_isMove_vert = false;

	// 当前player是否执行完动画
	player1_done = true;
	player2_done = true;
	// 当前玩家的移动帧动画移动方向
	player1_moving_direction = "West";
	player2_moving_direction = "West";

	
	// 玩家现在用着的武器类型
	attack_Type_For_Player1 = 0;
	attack_Type_For_Player2 = 0;

	// 玩家朝向
	player1_toward = "West";
	player2_toward = "West";

	// 玩家射击动作是否终止
	player1_isShotDone = true;
	player2_isShotDone = true;

	// 玩家血量
	player1HP = 100;
	player2HP = 100;

	// 玩家的击杀分数
	player1Score = 0;
	player2Score = 0;
}