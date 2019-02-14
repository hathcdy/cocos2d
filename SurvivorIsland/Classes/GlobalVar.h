#pragma once
#ifndef GLOBAL_VAR_H_
#define GLOBAL_VAL_H

#include "cocos2d.h"
using namespace std;
//���ȫ�ֱ������࣬����ģʽ
class GlobalVar : public cocos2d::Ref{
private:
	GlobalVar();
	static GlobalVar* instance;
	//�Ƿ�˫��ģʽ
	bool double_player;
	//�ļ���Դ
	string mapFile;
public:
	static GlobalVar* getInstance();
	string getMapFile();
	bool getDoublePlayer();
	void setMapFile(string m);
	void setDoublePlayer(bool b);
	//�����������
	int getAttack1();
	int getAttack2();

	void clear();

	//����player1���ƶ�
	bool player1_isMove_hori = false;
	bool player1_isMove_vert = false;
	char player1_moveKey_hori;
	char player1_moveKey_vert;

	//����player2���ƶ�
	bool player2_isMove_hori = false;
	bool player2_isMove_vert = false;
	char player2_moveKey_hori;
	char player2_moveKey_vert;

	// ��ǰplayer�Ƿ�ִ���궯��
	bool player1_done = true;
	bool player2_done = true;
	// ��ǰ��ҵ��ƶ�֡�����ƶ�����
	string player1_moving_direction = "West";
	string player2_moving_direction = "West";

	// �˷���λ
	string Direction[8] = { "West", "NorthWest", "North", "NorthEast", "East", "SouthEast", "South", "SouthWest" };

	// ����������ŵ���������
	int attack_Type_For_Player1 = 0;
	int attack_Type_For_Player2 = 0;

	// ��ҳ���
	string player1_toward = "West";
	string player2_toward = "West";

	// �����������Ƿ���ֹ
	bool player1_isShotDone = true;
	bool player2_isShotDone = true;

	// ���Ѫ��
	int player1HP = 100;
	int player2HP = 100;

	// ��������
	string Weapon[6] = { "Pistol", "Uzi", "Shotgun", "Grenade", "Missile", "Laser_gun" };

	// ��ҵĻ�ɱ����
	int player1Score = 0;
	int player2Score = 0;

};
#endif GLOBAL_VAL_H
