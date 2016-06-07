#include "CubeSolver.h"
#include "common.h"
#include <iostream>
void CubeSolver::init()
{
	CHAROFFSET = 65;
	argv.resize(20);
	for (int i = 0; i < 12; i++)argv[i].resize(2);
	for (int i = 12; i < 20; i++)argv[i].resize(3);
}
void CubeSolver::SetInput(const CubeInput & ci)
{
	//set argv;
	//input:  UF UR UB UL  DF DR DB DL  FR FL BR BL  UFR URB UBL ULF   DRF DFL DLB DBR
	int order1[] = {43,1,41,28,37,10,39,19,46,7,50,34,52,16,48,25,5,30,3,23,12,32,14,21,
	};
	int order2[] = {44,2,27,38,29,9,36,11,18,42,20,0,47,33,8,45,6,26,51,24,17,53,15,35};
	for (int i = 0; i < 12; i++)
	{
		argv[i][0] = (ci.colorToFace)[order1[i * 2]];
		argv[i][1] = (ci.colorToFace)[order1[i * 2+1]];
	}
	for (int i = 0; i < 8; i++)
	{
		argv[12+i][0] = (ci.colorToFace)[order2[i * 3]];
		argv[12+i][1] = (ci.colorToFace)[order2[i * 3 + 1]];
		argv[12+i][2] = (ci.colorToFace)[order2[i * 3 + 2]];
	}	
}
void CubeSolver::Solve(std::vector<char> & Movements,std::vector<int> & MovementsAmount)
{
	//argv检查合法性
	char * validSeq = "FBLRUD";
	for (int i = 0; i < argv.size(); i++)
	{
		for (int j = 0; j < argv[i].size(); j++)
		{
			for (int k = 0; k < 6; k++)
				if (std::strchr("FBLRUD", argv[i][j]) == NULL)
				{
					return;
				}
		}
	}
	//测试
	//for (int i = 0; i < argv.size(); i++)
	//{
	//	for (int j = 0; j < argv[i].size(); j++)
	//	{
	//		std::cout << argv[i][j];
	//	}
	//	std::cout << " ";
	//}
	
	int f, i = 0, j = 0, k = 0, pc, mor;

	// initialise tables
	for (; k < 20; k++) val[k] = k < 12 ? 2 : 3;
	for (; j < 8; j++) Fill_Table(j);

	// read input, 20 pieces worth
	for (; i < 20; i++) {
		f = pc = k = mor = 0;
		for (; f < val[i]; f++) {
			// read input from stdin, or...
			//     do{cin>>c;}while(c==' ');
			//     j=strchr(faces,c)-faces;
			// ...from command line and get face number of facelet
			j = std::strchr(faces, argv[i][f]) - faces;
			// keep track of principal facelet for orientation
			if (j > k) { k = j; mor = f; }
			//construct bit hash code
			pc += 1 << j;
		}
		// find which cubelet it belongs, i.e. the label for this piece
		for (f = 0; f < 20; f++)
			if (pc == bithash[f] - 64) break;
		// store piece
		pos[order[i] - CHAROFFSET] = f;
		ori[order[i] - CHAROFFSET] = mor%val[i];
	}

	//solve the cube
	// four phases
	for (; phase < 8; phase += 2) {
	//	if (phase >= 4)break;
		// try each depth till solved
		for (j = 0; !Search_Phase(j, 0, 9); j++);
		//output result of this phase
		for (i = 0; i < j; i++)
		{
			//std::cout << "FBRLUD"[move[i]] << moveamount[i];
			Movements.push_back((char)"FBRLUD"[move[i]]);
			MovementsAmount.push_back(moveamount[i]);
		}
	}

}

void CubeSolver::Cycle(char*p, char*a) {
	SWAP(p[*a - CHAROFFSET], p[a[1] - CHAROFFSET]);
	SWAP(p[*a - CHAROFFSET], p[a[2] - CHAROFFSET]);
	SWAP(p[*a - CHAROFFSET], p[a[3] - CHAROFFSET]);
}

void CubeSolver::Twist(int i, int a) {
	i -= CHAROFFSET;
	ori[i] = (ori[i] + a + 1) % val[i];
}
void CubeSolver::Position_Reset() {
	for (int i = 0; i < 20; pos[i] = i, ori[i++] = 0);
}
int CubeSolver::Perm_To_Num(char* p) {
	int n = 0;
	for (int a = 0; a < 4; a++) {
		n *= 4 - a;
		for (int b = a; ++b < 4; )
			if (p[b] < p[a]) n++;
	}
	return n;
}

void CubeSolver::Num_To_Perm(char* p, int n, int o) {
	p += o;
	p[3] = o;
	for (int a = 3; a--;) {
		p[a] = n % (4 - a) + o;
		n /= 4 - a;
		for (int b = a; ++b < 4; )
			if (p[b] >= p[a]) p[b]++;
	}
}


int CubeSolver::Get_Position(int t)
{
	int i = -1, n = 0;
	switch (t) {

		//得到边块的朝向状态
	case 1:

		for (; ++i < 12;) n += ori[i] << i;
		break;
		//把角块的朝向调整好，中间层的棱块放到中间层
		//
	case 2://cornertwist，得到角块的朝向状态
		   // get base 3 number of 8 digits - each digit is corner twist
		for (i = 20; --i > 11;) n = n * 3 + ori[i];
		break;
	case 3://middle edge choice			//得到中间层块的状态
		   // 12 bits, set bit if edge belongs in Um middle slice
		for (; ++i < 12;) n += (pos[i] & 8) ? (1 << i) : 0;
		break;

		//到了这里，所有中间层的棱块已经在中间层，所以现在只考虑前8个棱块，即相当于复位Fm
		//调整好角块的相对位置，即相对的面上只有相对面的颜色
		//并且四个角块为一组属于上下两个面的相交的对角线上
	case 4://ud slice choice
		   // 8 bits, set bit if UD edge belongs in Fm middle slice
		for (; ++i < 8;) n += (pos[i] & 4) ? (1 << i) : 0;
		break;
	case 5://tetrad choice, twist and parity
		int corn[8], j, k, l, corn2[4];
		// 8 bits, set bit if corner belongs in second tetrad.
		// also separate pieces for twist/parity determination
		k = j = 0;
		for (; ++i < 8;)	//8个角块
			if ((l = pos[i + 12] - 12) & 4) {			//如果是16 17 18 19 块
				corn[l] = k++;		//16 17 18 19 出现的顺序
				n += 1 << i;			//16 17 18 19 块在8个块0-7中的相对位置，用bit标记
			}
			else corn[j++] = l;		//12 13 14 15 出现的顺序
									//Find permutation of second tetrad after solving first
			for (i = 0; i < 4; i++) corn2[i] = corn[4 + corn[i]];
			//Solve one piece of second tetrad
			for (; --i;) corn2[i] ^= corn2[0];
			// encode parity/tetrad twist
			n = n * 6 + corn2[1] * 2 - 2;
			if (corn2[3] < corn2[2])n++;
			break;
	case 6://two edge and one corner orbit, permutation
		n = Perm_To_Num(pos) * 576 + Perm_To_Num(pos + 4) * 24 + Perm_To_Num(pos + 12);
		break;
	case 7://one edge and one corner orbit, permutation
		n = Perm_To_Num(pos + 8) * 24 + Perm_To_Num(pos + 16);
		break;
	}
	return n;
}
// sets cube to any position which has index n in table t
void CubeSolver::Set_Position(int t, int n) 
{
	int i = 0, j = 12, k = 0;
	char *corn = "QRSTQRTSQSRTQTRSQSTRQTSR";
	Position_Reset();
	switch (t) {
		// case 0 does nothing so leaves cube solved
	case 1://edgeflip
		for (; i < 12; i++, n >>= 1) ori[i] = n & 1;
		break;
	case 2://cornertwist
		for (i = 12; i < 20; i++, n /= 3) ori[i] = n % 3;
		break;
	case 3://middle edge choice
		for (; i < 12; i++, n >>= 1) pos[i] = 8 * n & 8;
		break;
	case 4://ud slice choice
		for (; i < 8; i++, n >>= 1) pos[i] = 4 * n & 4;
		break;
	case 5://tetrad choice,parity,twist
		corn += n % 6 * 4;
		n /= 6;
		for (; i < 8; i++, n >>= 1)
			pos[i + 12] = n & 1 ? corn[k++] - CHAROFFSET : j++;
		break;
	case 6://slice permutations
		Num_To_Perm(pos, n % 24, 12); n /= 24;
		Num_To_Perm(pos, n % 24, 4); n /= 24;
		Num_To_Perm(pos, n, 0);
		break;
	case 7://corner permutations
		Num_To_Perm(pos, n / 24, 8);
		Num_To_Perm(pos, n % 24, 16);
		break;
	}
}


//do a clockwise quarter turn cube move
void CubeSolver::Do_Move(int m) 
{
	char *p = perm + 8 * m, i = 8;
	//cycle the edges
	Cycle(pos, p);
	Cycle(ori, p);
	//cycle the corners
	Cycle(pos, p + 4);
	Cycle(ori, p + 4);
	//twist corners if RLFB
	//the orientation of conner will be changed with single colockwise quarter turn
	if (m < 4)
		for (; --i > 3;) Twist(p[i], i & 1);
	//flip edges if FB
	if (m < 2)
		for (i = 4; i--;) Twist(p[i], 0);
	// G1 is defined by <U D L R F2 B2> 
}

// calculate a pruning table
void CubeSolver::Fill_Table(int ti) 
{
	int n = 1, l = 1, tl = tablesize[ti];
	// alocate table memory
	char* tb = tables[ti] = new char[tl];
	//clear table
	memset(tb, 0, tl);
	//mark solved position as depth 1
	Position_Reset();
	tb[Get_Position(ti)] = 1;

	// while there are positions of depth l
	while (n) {
		n = 0;
		// find each position of depth l
		for (int i = 0; i < tl; i++) {
			if (tb[i] == l) {		//找到初始状态，从初始状态开始
									//construct that cube position
				Set_Position(ti, i);
				// try each face any amount			//
				for (int f = 0; f < 6; f++) {			//对每个状态进行6个方向的操作
					for (int q = 1; q < 4; q++) {			//每个方向有三种操作，-1 0 +1
						Do_Move(f);
						// get resulting position
						int r = Get_Position(ti);
						// if move as allowed in that phase, and position is a new one
						if ((q == 2 || f >= (ti & 6)) && !tb[r]) {			//tb[r] == false 当前状态没有被探索过 q==2 代表旋转180度，任何群都是合法的
																			//检查 f>=(ti&6)这个条件时，说明q!= 2 即是1/4旋转，这样的话，操作的合法性取决于阶段
																			// mark that position as depth l+1
							tb[r] = l + 1;
							n++;
						}
					}
					Do_Move(f);		//复原
				}
			}
		}
		l++;
	}
}

// Pruned tree search. recursive.
bool CubeSolver::Search_Phase(int movesleft, int movesdone, int lastmove)
{

	if (tables[phase][Get_Position(phase)] - 1 > movesleft ||
		tables[phase + 1][Get_Position(phase + 1)] - 1 > movesleft) return false;

	if (!movesleft) return true;


	for (int i = 6; i--;) {

		if (i - lastmove && (i - lastmove + 1 || i | 1)) {
			move[movesdone] = i;

			for (int j = 0; ++j < 4;) {

				Do_Move(i);
				moveamount[movesdone] = j;

				if ((j == 2 || i >= phase) &&

					Search_Phase(movesleft - 1, movesdone + 1, i)) return true;
			}

			Do_Move(i);
		}
	}

	return false;
}

void CubeInput::SetColor(const std::vector<int> & faceColor)
{
	char *colorMap = "FBUDLR";
	for (int i = 0; i < faceColor.size(); i++)
	{
		if (faceColor[i] != -1)
			colorToFace[i] = colorMap[(int)faceColor[i]];
		else
			colorToFace[i] = '-';
	}
}