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
	//����player1���ƶ�
	player1_isMove_hori = false;
	player1_isMove_vert = false;

	//����player2���ƶ�
	player2_isMove_hori = false;
	player2_isMove_vert = false;

	// ��ǰplayer�Ƿ�ִ���궯��
	player1_done = true;
	player2_done = true;
	// ��ǰ��ҵ��ƶ�֡�����ƶ�����
	player1_moving_direction = "West";
	player2_moving_direction = "West";

	
	// ����������ŵ���������
	attack_Type_For_Player1 = 0;
	attack_Type_For_Player2 = 0;

	// ��ҳ���
	player1_toward = "West";
	player2_toward = "West";

	// �����������Ƿ���ֹ
	player1_isShotDone = true;
	player2_isShotDone = true;

	// ���Ѫ��
	player1HP = 100;
	player2HP = 100;

	// ��ҵĻ�ɱ����
	player1Score = 0;
	player2Score = 0;
}