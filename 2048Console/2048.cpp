/*
* The console version aim to simplify the whole game process
* v1.0(20231123)
* Main Game Structure
* v1.1-beta(20231127)
* 基础功能实现中...
* debug结果:向右操作有严重bug
* v1.1(20231127)
* 完成基础功能，修复1.0bug
* v1.2(20231201)
* 完善功能，增加难度选择与自定义难度，编写UI与文字
* v1.3(20231203)
* 2048数字块的颜色
* v2.0(20231204)
* 实现排行榜功能与排行榜的存储
* v2.0(20231205)
* 修复并实现原版2048若现有非0数一个都不动的话就不会生成2/4的功能
* v2.1(20231207)
* 盲盒模式的实现
*/

#pragma warning(disable:6031)	//忽略rand函数的返回值被忽略报错

//#define LOG

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <iomanip>
#include <time.h>
#include <conio.h>
#include <fstream>
#include <windows.h>

#define N 4
#define EASY_DIF 10
#define MID_DIF 20
#define HARD_DIF 40

using namespace std;

int gameStatus_g = 1;	//游戏状态：0是失败、1是正在进行、2是赢了
int exit_g = 0;	//游戏是否需要退出
int boxmode_g;	//盲盒模式状态

#ifdef LOG
int log_cnt = 0;
#endif

inline void Swap(int& a, int& b) {
	int temp = a;
	a = b;
	b = temp;
}

inline int Max(int a, int b) {
	return (a > b) ? a : b;
}

#ifdef LOG
void LogMap(int map[][N], int input) {
	ofstream fout("log.txt", ios::app);
	fout << "count:" << log_cnt << endl;
	fout << "input:" << input << endl;
	fout << "-------------------------------" << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			fout << setw(5) << map[i][j];
		fout << endl;
	}
	fout << "-------------------------------" << endl;
}
#endif

//结构体初始化：用于实现排行榜
struct Rank {
	int Title;	//头衔，即排名
	int Score;	//它代表的分数值
	//其实写到后来发现结构体是没必要的，但是又不想删了，就美其名曰这是增加代码可读性吧
};

//UI部分
void UIWelcomePage();
void UIDiscription();
void UIGameStarter(int map[][N], struct Rank rank[]);
void UIGameExitPage(int map[][N], struct Rank rank[]);
void UIRankPage(struct Rank rank[], int);
void UIExit();

//游戏核心部分
void ClearMap(int map[][N]);
void InitMap(int map[][N], int& t);
void CheckGameOver(int map[][N]);
int RandBox(int);
void RandGen(int map[][N], int);
bool RandGenFlag(int map[][N], int);
void UpCopy(int map[][N], int);
void DnCopy(int map[][N], int);
void LfCopy(int map[][N], int);
void RtCopy(int map[][N], int);
int Score(int map[][N]);
int TextColor(int);
void PrintMap(int map[][N]);
void GameMain(int map[][N], int, struct Rank rank[]);

//外部IO部分：排行榜功能的实现
void StructRankInit(struct Rank rank[]);
void ExtScoreRead(struct Rank rank[]);
int ExtScoreInsert(struct Rank rank[], int);
void ExtScoreWrite(struct Rank rank[]);

int main() {
	int map[N][N] = {0};
	struct Rank rank[10];	//最多只读取前十名

	StructRankInit(rank);
	ExtScoreRead(rank);

	UIWelcomePage();
	while (!exit_g) {
		UIGameStarter(map, rank);
	}
	ExtScoreWrite(rank);
	UIExit();

	return 0;
}

void UIWelcomePage() {
	cout << "--------------2048---------------" << endl;
	cout << "欢迎来到2048！" << endl;
	cout << "这是一款由Aritx Zhou开发的控制台小游戏" << endl;
	cout << "游戏包含了不同的难度以及一个趣味模式" << endl;
	cout << "请尽情享受吧！~" << endl;
	cout << "--------------v2.1---------------" << endl;
	cout << "下面我将展示游戏玩法（按s跳过）" << endl;
	cout << "请按键继续..." << endl;
	char input = _getche();
	if (input != 's') {
		system("cls");
		UIDiscription();
	}
	else {
		system("cls");
		cout << endl << "现在将你传送到模式选择界面！" << endl;
	}
	system("pause");
	system("cls");
}

void UIDiscription() {
	cout << "-------------玩法介绍--------------" << endl;
	cout << "在游戏中，你将用上下左右实现2048滑动操作" << endl;
	cout << "你所需要做的只有选择模式，开始游戏" << endl;
	cout << "游戏中如果你想要退出或者重来，请按e" << endl;
	cout << "好了以上就是你需要知道的内容~" << endl;
	cout << "马上会将你传送到模式选择界面！" << endl;
}

void UIGameStarter(int map[][N], struct Rank rank[]) {
	ClearMap(map);
	boxmode_g = 0;	//盲盒模式关

	cout << "-------------开始游戏--------------" << endl;
	cout << "请选择游戏难度：" << endl;
	cout << "1.简单模式" << endl;
	cout << "2.中等模式（默认选择）" << endl;
	cout << "3.困难模式" << endl;
	cout << "4.自定义难度" << endl;
	cout << "5.趣味模式-盲盒模式（Beta）" << endl;
	cout << "r.查看排行榜" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
	cout << "e.退出游戏" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	cout << "----------------------------------" << endl;
	cout << "请输入您的选择：" << endl;
	char c;
	cin >> c;
	switch (c) {
	case '1':
		GameMain(map, EASY_DIF, rank);
		break;
	case '3':
		GameMain(map, HARD_DIF, rank);
		break;
	case '4':
		int dif;
		system("cls");
		cout << "--------------自定义---------------" << endl;
		cout << "请输入自定义的难度（1~50，默认20）：";
		cin >> dif;
		if (dif >= 1 && dif <= 50)
			GameMain(map, dif, rank);
		else
			GameMain(map, 20, rank);
		break;
	case '5':
		//Beta
		boxmode_g = 1;
		GameMain(map, MID_DIF, rank);
		break;
	case 'r':
		UIRankPage(rank, -1);
		break;
	case 'e':
		exit_g = 1;
		break;
	default:
		GameMain(map, MID_DIF, rank);
	}
}

void UIGameExitPage(int map[][N], struct Rank rank[]) {
	system("cls");
	cout << "检测到你按下了e，现在正在退回模式选择页..." << endl;
	system("pause");
	getchar();
	system("cls");
	UIGameStarter(map, rank);
}

void UIRankPage(struct Rank rank[], int title) {
	//排行榜UI界面
	system("cls");
	cout << "--------------排行榜---------------" << endl;
	for (int i = 0; i < 10; i++) {
		if (rank[i].Title == title)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 + 7 * 0x10);	//前景黑，背景白
		cout << setw(3) << rank[i].Title << setw(7) << rank[i].Score << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);	
	}
	cout << "----------------------------------" << endl;
	system("pause");
	system("cls");
}

void UIExit() {
	system("cls");
	cout << "游戏退出中..." << endl;
	cout << "再见~欢迎再次游玩..." << endl;
	system("pause");
}

//核心部分
void ClearMap(int map[][N]) {
	//用于重复游戏时的棋盘重置
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			map[i][j] = 0;
	gameStatus_g = 1;
}

void InitMap(int map[][N], int& t) {
	//初始化2048棋盘
	if (t == 0)
		return;
	int i_r = rand() % 4;
	int j_r = rand() % 4;
	if (map[i_r][j_r] != 0)
		InitMap(map, t);
	else {
		map[i_r][j_r] = 2;
		InitMap(map, --t);
	}
}

void CheckGameOver(int map[][N]) {
	bool flag = false;	//是否对gameStatus产生了影响，也用于判断游戏是否还能继续

	/*
	* 枚举看两种情况：
	* 1、如果游戏棋盘中仍然存在0，游戏要么赢了（2）要么还在进行（1）
	* 2、如果游戏棋盘中有2048，游戏结束，赢了（2）
	*/
	for (int i = 0; i < N && !flag; i++)
		for (int j = 0; j < N && !flag; j++) {
			if (map[i][j] == 0) {
				gameStatus_g = 1;
				flag = true;
			}
			if (map[i][j] == 2048) {
				gameStatus_g = 2;
				flag = true;
			}
		}

	if (flag)
		return;	//已经找到了目前的游戏状态，退出函数

	/*
	* 如果未出现上述两种情况，则就要判断上下左右是否有可以合并的数
	* 如果有可合并的数，则游戏状态还在进行（1）
	* 没有，游戏结束，玩家操作失败（0）
	*/
	for (int i = 0; i < N && !flag; i++) {
		for (int j = 0; j < N - 1 && !flag; j++) {
			//对左右是否有合并的情况进行判断
			if (map[i][j] == map[i][j + 1]) {
				gameStatus_g = 1;
				flag = true;
			}
		}
	}

	for (int i = 0; i < N && !flag; i++) {
		for (int j = 0; j < N - 1 && !flag; j++) {
			//对上下是否有合并的情况进行判断
			if (map[j][i] == map[j + 1][i]) {
				gameStatus_g = 1;
				flag = true;
			}
		}
	}

	if (flag)
		return;
	else {
		gameStatus_g = 0;	//输了
		return;
	}
}

int RandBox(int a) {
	int r = rand() % 100;
	if (r >= 95)
		return Max(a, 256);
	if (r >= 85 && r < 95)
		return Max(a, 128);
	if (r >= 70 && r < 85)
		return Max(a, 64);
	if (r >= 50 && r < 70)
		return Max(a, 32);
	if (r >= 20 && r < 50)
		return Max(a, 16);
	return a;
}

//从这里往后所有出现的dif变量代表游戏难度，值域[0,100]，代表随机的可能性（百分制）

void RandGen(int map[][N], int dif) {
	int i_r = rand() % 4;	//行随机
	int j_r = rand() % 4;	//列随机
	if (map[i_r][j_r] != 0)
		RandGen(map, dif);
	else {
		int pos = rand() % 100;	//生成一个随机数，通过与dif比较判断生成4/2
		if (pos <= dif)
			map[i_r][j_r] = 4;
		else
			map[i_r][j_r] = 2;
	}
}

bool RandGenFlag(int map[][N], int direction) {	//direction 1/3是上下，2/4是左右
	bool randGenFlag = false;
	//判断是否需要生成一个随机的2/4
	//上移位？
	if (direction == 1) {
		for (int i = 1; i < N && !randGenFlag; i++) {
			for (int j = 0; j < N && !randGenFlag; j++) {
				if (map[i - 1][j] == 0 && map[i][j] != 0)
					randGenFlag = true;
			}
		}
	}
	//下移位？
	if (direction == 3) {
		for (int i = 0; i < N - 1 && !randGenFlag; i++) {
			for (int j = 0; j < N && !randGenFlag; j++) {
				if (map[i + 1][j] == 0 && map[i][j] != 0)
					randGenFlag = true;
			}
		}
	}
	//左移位？
	if (direction == 2) {
		for (int i = 1; i < N && !randGenFlag; i++) {
			for (int j = 0; j < N && !randGenFlag; j++) {
				if (map[j][i - 1] == 0 && map[j][i] != 0)
					randGenFlag = true;
			}
		}
	}
	//右移位？
	if (direction == 4) {
		for (int i = 0; i < N - 1 && !randGenFlag; i++) {
			for (int j = 0; j < N && !randGenFlag; j++) {
				if (map[j][i + 1] == 0 && map[j][i] != 0)
					randGenFlag = true;
			}
		}
	}
	
	if (randGenFlag)
		return true;

	if (direction % 2 == 0) {
		for (int i = 0; i < N && !randGenFlag; i++) {
			for (int j = 0; j < N - 1 && !randGenFlag; j++) {
				//对左右是否有合并的情况进行判断
				if (map[i][j] == map[i][j + 1] && map[i][j] != 0) {
					randGenFlag = true;
				}
			}
		}
	}

	if (direction % 2 == 1) {
		for (int i = 0; i < N && !randGenFlag; i++) {
			for (int j = 0; j < N - 1 && !randGenFlag; j++) {
				//对上下是否有合并的情况进行判断
				if (map[j][i] == map[j + 1][i] && map[j][i] != 0) {
					randGenFlag = true;
				}
			}
		}
	}
	if (randGenFlag)
		return true;

	return false;
	//移用了GameStatus判断的代码，思路一致
}

void UpCopy(int map[][N], int dif) {
	bool randFlag = RandGenFlag(map, 1);

	//先整体上移消除棋盘中间影响合并操作的0
	for (int t = 1; t < N; t++) {
		//t是类计数器，实现记数型循环
		for (int i = t ; i > 0; i--) {
			//i是行，从下往上选择
			for (int j = 0; j < N; j++) {
				if (map[i - 1][j] == 0)
					Swap(map[i - 1][j], map[i][j]);
			}
		}
	}

	//合并操作
	for (int i = 1; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (map[i - 1][j] == map[i][j]) {
				map[i - 1][j] <<= 1;	//位操作实现*2
				map[i][j] = 0;		
				if (boxmode_g == 1 && map[i - 1][j] != 0)
					map[i - 1][j] = RandBox(map[i - 1][j]);	//盲盒随机数
			}
		}
	}

	//再次整体上移消除棋盘里合并操作产生的0
	for (int t = 1; t < N; t++) {
		//t是类计数器，实现记数型循环
		for (int i = t; i > 0; i--) {
			//i是行，从下往上选择
			for (int j = 0; j < N; j++) {
				if (map[i - 1][j] == 0)
					Swap(map[i - 1][j], map[i][j]);
			}
		}
	}

	if (randFlag)
		RandGen(map, dif);

#ifdef LOG
	LogMap(map, 72);
#endif
}

void DnCopy(int map[][N], int dif) {
	bool randFlag = RandGenFlag(map, 3);

	//先整体下移消除棋盘中间影响合并操作的0
	for (int t = N - 1; t > 0; t--) {
		//t是类计数器，实现记数型循环
		for (int i = t - 1; i < N; i++) {
			//i是行，从上往下选择
			for (int j = 0; j < N; j++) {
				if (map[i + 1][j] == 0)
					Swap(map[i + 1][j], map[i][j]);
			}
		}
	}

	//合并操作
	for (int i = N - 2; i >= 0; i--) {
		for (int j = 0; j < N; j++) {
			if (map[i + 1][j] == map[i][j]) {
				map[i + 1][j] <<= 1;	//位操作实现*2
				map[i][j] = 0;
				if (boxmode_g == 1 && map[i + 1][j] != 0)
					map[i + 1][j] = RandBox(map[i + 1][j]);	//盲盒随机数
			}
		}
	}

	//再次整体下移消除棋盘里合并操作产生的0
	for (int t = N - 1; t > 0; t--) {
		//t是类计数器，实现记数型循环
		for (int i = t - 1; i < N; i++) {
			//i是行，从上往下选择
			for (int j = 0; j < N; j++) {
				if (map[i + 1][j] == 0)
					Swap(map[i + 1][j], map[i][j]);
			}
		}
	}

	if (randFlag)
		RandGen(map, dif);

#ifdef LOG
	LogMap(map, 80);
#endif
}

void LfCopy(int map[][N], int dif) {
	bool randFlag = RandGenFlag(map, 2);

	//先整体左移消除棋盘中间影响合并操作的0
	for (int t = 1; t < N; t++) {
		//t是类计数器，实现记数型循环
		for (int i = t; i > 0; i--) {
			//i是列，从右往左选择
			for (int j = 0; j < N; j++) {
				if (map[j][i - 1] == 0)
					Swap(map[j][i - 1], map[j][i]);
			}
		}
	}

	//合并操作
	for (int i = 1; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (map[j][i - 1] == map[j][i]) {
				map[j][i - 1] <<= 1;	//位操作实现*2
				map[j][i] = 0; 
				if (boxmode_g == 1 && map[j][i - 1] != 0)
					map[j][i - 1] = RandBox(map[j][i - 1]);	//盲盒随机数
			}
		}
	}

	//再次整体左移消除棋盘里合并操作产生的0
	for (int t = 1; t < N; t++) {
		//t是计数器，实现记数型循环
		for (int i = t; i > 0; i--) {
			//i是列，从右往左选择
			for (int j = 0; j < N; j++) {
				if (map[j][i - 1] == 0)
					Swap(map[j][i - 1], map[j][i]);
			}
		}
	}

	if (randFlag)
		RandGen(map, dif);

#ifdef LOG
	LogMap(map, 75);
#endif
}

void RtCopy(int map[][N], int dif) {
	bool randFlag = RandGenFlag(map, 4);

	for (int t = N - 1; t > 0; t--) {
		for (int i = t - 1; i < N - 1; i++) {
			for (int j = 0; j < N; j++) {
				if (map[j][i + 1] == 0)
					Swap(map[j][i + 1], map[j][i]);
			}
		}
	}

	//合并操作
	for (int i = N - 2; i >= 0; i--) {
		for (int j = 0; j < N; j++) {
			if (map[j][i + 1] == map[j][i]) {
				map[j][i + 1] <<= 1;	//位操作实现*2
				map[j][i] = 0;
				if (boxmode_g == 1 && map[j][i + 1] != 0)
					map[j][i + 1] = RandBox(map[j][i + 1]);	//盲盒随机数
			}
		}
	}

	for (int t = N - 1; t > 0; t--) {
		for (int i = t - 1; i < N - 1; i++) {
			for (int j = 0; j < N; j++) {
				if (map[j][i + 1] == 0)
					Swap(map[j][i + 1], map[j][i]);
			}
		}
	}

	if (randFlag)
		RandGen(map, dif);

#ifdef LOG
	LogMap(map, 77);
#endif
}

int Score(int map[][N]) {
	int c = 0;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			c += map[i][j];
	return c;
}

int TextColor(int n) {
	//读取当前位置的数字，设置字体颜色
	switch (n) {
	case 2:
		return 15;	//亮白色
		break;
	case 4:
		return 6;	//黄色
		break;
	case 8:
		return 14;	//淡黄色
		break;
	case 16:
		return 11;	//淡浅绿色
		break;
	case 32:
		return 3;	//浅绿色
		break;
	case 64:
		return 10;	//淡绿色
		break;
	case 128:
		return 2;	//绿色
		break;
	case 256:
		return 1;	//蓝色
		break;
	case 512:
		return 9;	//淡蓝色
		break;
	case 1024:
		return 13;	//淡紫色
		break;
	case 2048:
		return 5;	//紫色
		break;
	default:
		return 8;	//灰色
		break;
	}
}

void PrintMap(int map[][N]) {
	cout << "-------------------------------" << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), TextColor(map[i][j]));	//更改输出字体颜色
			cout << setw(5) << map[i][j];
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);	//更改输出字体颜色
		}
		cout << endl;
	}
	cout << "-------------------------------" << endl;
	//成绩的输出
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 + 7 * 0x10);	//前景黑，背景白
	cout << "Score:" << Score(map);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void GameMain(int map[][N], int dif, struct Rank rank[]) {
	//Initiallize the game
	srand(time(0));
	rand();	//生成一个随机数淘汰无效随机
	int* temp = new int;	//这里生成动态变量存储生成次数
	*temp = 2;
	InitMap(map, *temp);
	delete temp;	//及时处理掉动态变量是好习惯~

	system("cls");

	PrintMap(map);

	while (gameStatus_g == 1) {
		//输入操作
		if (_getche() == 'e') {
			int title = ExtScoreInsert(rank, Score(map));
			UIRankPage(rank, title);
			UIGameExitPage(map, rank);
			return;
		}
		unsigned char c = _getche();
		switch (c) {
		case 72:
			UpCopy(map, dif);
			break;
		case 80:
			DnCopy(map, dif);
			break;
		case 75:
			LfCopy(map, dif);
			break;
		case 77:
			RtCopy(map, dif);
			break;
		default:
			break;
		}

#ifdef LOG
		log_cnt++;
#endif

		system("cls");
		PrintMap(map);
		CheckGameOver(map);	//检查游戏状态
	}

	if (gameStatus_g == 2) {
		system("cls");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 + 2 * 0x10);
		cout << "You win!" << endl; 
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		cout << "Final score is:" << Score(map);
	}

	if (gameStatus_g == 0) {
		system("cls");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7 + 4 * 0x10);
		cout << "You lose!" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		cout << "Final score is:" << Score(map);
	}

	system("pause");
	system("cls");

	int title = ExtScoreInsert(rank, Score(map));
	UIRankPage(rank, title);

	cout << endl;
	cout << "再来一局？(y/n)" << endl;
	char input;
	cin >> input;
	switch (input) {
	case 'n':
		system("cls");
		exit_g = 1;
		break;
	default:
		system("cls");
		UIGameStarter(map, rank);
		break;
	}
}

//排行榜功能的实现

void StructRankInit(struct Rank rank[]) {
	//初始化结构体
	for (int i = 0; i < 10; i++) {
		rank[i].Title = i + 1;
		rank[i].Score = 0;
	}
}

void ExtScoreRead(struct Rank rank[]) {
	//外部文件读取成绩排名
	ifstream fin("DoNotDelete.rank");	//猜猜为什么要叫这个名字，文件读入
	//ofstream fout("DoNotDelete.rank");	//文件写入设置
	//函数初始化完成

	for (int i = 0; i < 10; i++)
		fin >> rank[i].Title >> rank[i].Score;
}

int ExtScoreInsert(struct Rank rank[], int score) {
	if (score <= rank[9].Score)
		return -1;

	bool flag = true;	//标记是否出现比它小的数？
	int pos;	//标记它应该插入哪里
	for (pos = 0; pos < 10 && flag; pos++) {
		if (rank[pos].Score <= score)
			flag = false;
	}
	pos--;
	for (int i = 8; i >= pos; i--) {
		rank[i + 1].Score = rank[i].Score;	//整体后移一项
	}
	rank[pos].Score = score;

	return rank[pos].Title;
}

void ExtScoreWrite(struct Rank rank[]) {
	ofstream fout("DoNotDelete.rank");
	for (int i = 0; i < 10; i++)
		fout << rank[i].Title << " " << rank[i].Score << endl;
}