#pragma once
#include "all.h"

/*
* Thistlewaite's Algorithm
* Original Author: Jaap
* Modified by YangShuoLiu
* Description:
* This is a algorithm for cube solving based on ida* and prune
*/

class CubeInput
{
private:
	void init() { colorToFace.resize(54); }
public:
	CubeInput() { init(); }
	CubeInput(const std::vector<int> & faceColor) { init(); SetColor(faceColor); }
	void SetColor(const std::vector<int> & faceColor);
	std::vector<char> colorToFace;
};

class CubeSolver
{
public:
	CubeSolver() 
	{ 
		init();
	}
	CubeSolver(const CubeInput & ci) { init(); SetInput(ci); }
	void init();
	void SetInput(const CubeInput & ci);
	void Solve(std::vector<char> & Movements,std::vector<int> & MovementsAmount);
private:

	std::vector<std::vector<char> > argv;

	char *faces = "RLFBUD",
		*order = "AECGBFDHIJKLMSNTROQP",
		*bithash = "TdXhQaRbEFIJUZfijeYV",
		*perm = "AIBJTMROCLDKSNQPEKFIMSPRGJHLNTOQAGCEMTNSBFDHORPQ",
		pos[20], ori[20], val[20],
		TEMP,
		*tables[8];
	int move[20], moveamount[20], phase = 0;
	int tablesize[8] = { 1,4096,  6561,4096,  256,1536,  13824,576 };
	char CHAROFFSET;
	void SWAP(char &a, char &b) { TEMP = a; a = b; b = TEMP; }

	void Cycle(char*p, char*a);

	void Twist(int i, int a);

	void Position_Reset();

	int Perm_To_Num(char* p);

	void Num_To_Perm(char* p, int n, int o);

	int Get_Position(int t);

	void Set_Position(int t, int n);

	void Do_Move(int m);

	void Fill_Table(int ti);

	bool Search_Phase(int movesleft, int movesdone, int lastmove);


};
