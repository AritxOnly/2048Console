/*
* The console version aim to simplify the whole game process
* v1.0(20231123)
* Main Game Structure
* v1.1-beta(20231127)
* ��������ʵ����...
* debug���:���Ҳ���������bug
* v1.1(20231127)
* ��ɻ������ܣ��޸�1.0bug
* v1.2(20231201)
* ���ƹ��ܣ������Ѷ�ѡ�����Զ����Ѷȣ���дUI������
* v1.3(20231203)
* 2048���ֿ����ɫ
* v2.0(20231204)
* ʵ�����а��������а�Ĵ洢
* v2.0(20231205)
* �޸���ʵ��ԭ��2048�����з�0��һ���������Ļ��Ͳ�������2/4�Ĺ���
* v2.1(20231207)
* ä��ģʽ��ʵ��
*/

#pragma warning(disable:6031)	//����rand�����ķ���ֵ�����Ա���

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

int gameStatus_g = 1;	//��Ϸ״̬��0��ʧ�ܡ�1�����ڽ��С�2��Ӯ��
int exit_g = 0;	//��Ϸ�Ƿ���Ҫ�˳�
int boxmode_g;	//ä��ģʽ״̬

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

//�ṹ���ʼ��������ʵ�����а�
struct Rank {
	int Title;	//ͷ�Σ�������
	int Score;	//������ķ���ֵ
	//��ʵд���������ֽṹ����û��Ҫ�ģ������ֲ���ɾ�ˣ���������Ի�������Ӵ���ɶ��԰�
};

//UI����
void UIWelcomePage();
void UIDiscription();
void UIGameStarter(int map[][N], struct Rank rank[]);
void UIGameExitPage(int map[][N], struct Rank rank[]);
void UIRankPage(struct Rank rank[], int);
void UIExit();

//��Ϸ���Ĳ���
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

//�ⲿIO���֣����а��ܵ�ʵ��
void StructRankInit(struct Rank rank[]);
void ExtScoreRead(struct Rank rank[]);
int ExtScoreInsert(struct Rank rank[], int);
void ExtScoreWrite(struct Rank rank[]);

int main() {
	int map[N][N] = {0};
	struct Rank rank[10];	//���ֻ��ȡǰʮ��

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
	cout << "��ӭ����2048��" << endl;
	cout << "����һ����Aritx Zhou�����Ŀ���̨С��Ϸ" << endl;
	cout << "��Ϸ�����˲�ͬ���Ѷ��Լ�һ��Ȥζģʽ" << endl;
	cout << "�뾡�����ܰɣ�~" << endl;
	cout << "--------------v2.1---------------" << endl;
	cout << "�����ҽ�չʾ��Ϸ�淨����s������" << endl;
	cout << "�밴������..." << endl;
	char input = _getche();
	if (input != 's') {
		system("cls");
		UIDiscription();
	}
	else {
		system("cls");
		cout << endl << "���ڽ��㴫�͵�ģʽѡ����棡" << endl;
	}
	system("pause");
	system("cls");
}

void UIDiscription() {
	cout << "-------------�淨����--------------" << endl;
	cout << "����Ϸ�У��㽫����������ʵ��2048��������" << endl;
	cout << "������Ҫ����ֻ��ѡ��ģʽ����ʼ��Ϸ" << endl;
	cout << "��Ϸ���������Ҫ�˳������������밴e" << endl;
	cout << "�������Ͼ�������Ҫ֪��������~" << endl;
	cout << "���ϻὫ�㴫�͵�ģʽѡ����棡" << endl;
}

void UIGameStarter(int map[][N], struct Rank rank[]) {
	ClearMap(map);
	boxmode_g = 0;	//ä��ģʽ��

	cout << "-------------��ʼ��Ϸ--------------" << endl;
	cout << "��ѡ����Ϸ�Ѷȣ�" << endl;
	cout << "1.��ģʽ" << endl;
	cout << "2.�е�ģʽ��Ĭ��ѡ��" << endl;
	cout << "3.����ģʽ" << endl;
	cout << "4.�Զ����Ѷ�" << endl;
	cout << "5.Ȥζģʽ-ä��ģʽ��Beta��" << endl;
	cout << "r.�鿴���а�" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
	cout << "e.�˳���Ϸ" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	cout << "----------------------------------" << endl;
	cout << "����������ѡ��" << endl;
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
		cout << "--------------�Զ���---------------" << endl;
		cout << "�������Զ�����Ѷȣ�1~50��Ĭ��20����";
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
	cout << "��⵽�㰴����e�����������˻�ģʽѡ��ҳ..." << endl;
	system("pause");
	getchar();
	system("cls");
	UIGameStarter(map, rank);
}

void UIRankPage(struct Rank rank[], int title) {
	//���а�UI����
	system("cls");
	cout << "--------------���а�---------------" << endl;
	for (int i = 0; i < 10; i++) {
		if (rank[i].Title == title)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 + 7 * 0x10);	//ǰ���ڣ�������
		cout << setw(3) << rank[i].Title << setw(7) << rank[i].Score << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);	
	}
	cout << "----------------------------------" << endl;
	system("pause");
	system("cls");
}

void UIExit() {
	system("cls");
	cout << "��Ϸ�˳���..." << endl;
	cout << "�ټ�~��ӭ�ٴ�����..." << endl;
	system("pause");
}

//���Ĳ���
void ClearMap(int map[][N]) {
	//�����ظ���Ϸʱ����������
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			map[i][j] = 0;
	gameStatus_g = 1;
}

void InitMap(int map[][N], int& t) {
	//��ʼ��2048����
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
	bool flag = false;	//�Ƿ��gameStatus������Ӱ�죬Ҳ�����ж���Ϸ�Ƿ��ܼ���

	/*
	* ö�ٿ����������
	* 1�������Ϸ��������Ȼ����0����ϷҪôӮ�ˣ�2��Ҫô���ڽ��У�1��
	* 2�������Ϸ��������2048����Ϸ������Ӯ�ˣ�2��
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
		return;	//�Ѿ��ҵ���Ŀǰ����Ϸ״̬���˳�����

	/*
	* ���δ��������������������Ҫ�ж����������Ƿ��п��Ժϲ�����
	* ����пɺϲ�����������Ϸ״̬���ڽ��У�1��
	* û�У���Ϸ��������Ҳ���ʧ�ܣ�0��
	*/
	for (int i = 0; i < N && !flag; i++) {
		for (int j = 0; j < N - 1 && !flag; j++) {
			//�������Ƿ��кϲ�����������ж�
			if (map[i][j] == map[i][j + 1]) {
				gameStatus_g = 1;
				flag = true;
			}
		}
	}

	for (int i = 0; i < N && !flag; i++) {
		for (int j = 0; j < N - 1 && !flag; j++) {
			//�������Ƿ��кϲ�����������ж�
			if (map[j][i] == map[j + 1][i]) {
				gameStatus_g = 1;
				flag = true;
			}
		}
	}

	if (flag)
		return;
	else {
		gameStatus_g = 0;	//����
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

//�������������г��ֵ�dif����������Ϸ�Ѷȣ�ֵ��[0,100]����������Ŀ����ԣ��ٷ��ƣ�

void RandGen(int map[][N], int dif) {
	int i_r = rand() % 4;	//�����
	int j_r = rand() % 4;	//�����
	if (map[i_r][j_r] != 0)
		RandGen(map, dif);
	else {
		int pos = rand() % 100;	//����һ���������ͨ����dif�Ƚ��ж�����4/2
		if (pos <= dif)
			map[i_r][j_r] = 4;
		else
			map[i_r][j_r] = 2;
	}
}

bool RandGenFlag(int map[][N], int direction) {	//direction 1/3�����£�2/4������
	bool randGenFlag = false;
	//�ж��Ƿ���Ҫ����һ�������2/4
	//����λ��
	if (direction == 1) {
		for (int i = 1; i < N && !randGenFlag; i++) {
			for (int j = 0; j < N && !randGenFlag; j++) {
				if (map[i - 1][j] == 0 && map[i][j] != 0)
					randGenFlag = true;
			}
		}
	}
	//����λ��
	if (direction == 3) {
		for (int i = 0; i < N - 1 && !randGenFlag; i++) {
			for (int j = 0; j < N && !randGenFlag; j++) {
				if (map[i + 1][j] == 0 && map[i][j] != 0)
					randGenFlag = true;
			}
		}
	}
	//����λ��
	if (direction == 2) {
		for (int i = 1; i < N && !randGenFlag; i++) {
			for (int j = 0; j < N && !randGenFlag; j++) {
				if (map[j][i - 1] == 0 && map[j][i] != 0)
					randGenFlag = true;
			}
		}
	}
	//����λ��
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
				//�������Ƿ��кϲ�����������ж�
				if (map[i][j] == map[i][j + 1] && map[i][j] != 0) {
					randGenFlag = true;
				}
			}
		}
	}

	if (direction % 2 == 1) {
		for (int i = 0; i < N && !randGenFlag; i++) {
			for (int j = 0; j < N - 1 && !randGenFlag; j++) {
				//�������Ƿ��кϲ�����������ж�
				if (map[j][i] == map[j + 1][i] && map[j][i] != 0) {
					randGenFlag = true;
				}
			}
		}
	}
	if (randGenFlag)
		return true;

	return false;
	//������GameStatus�жϵĴ��룬˼·һ��
}

void UpCopy(int map[][N], int dif) {
	bool randFlag = RandGenFlag(map, 1);

	//�������������������м�Ӱ��ϲ�������0
	for (int t = 1; t < N; t++) {
		//t�����������ʵ�ּ�����ѭ��
		for (int i = t ; i > 0; i--) {
			//i���У���������ѡ��
			for (int j = 0; j < N; j++) {
				if (map[i - 1][j] == 0)
					Swap(map[i - 1][j], map[i][j]);
			}
		}
	}

	//�ϲ�����
	for (int i = 1; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (map[i - 1][j] == map[i][j]) {
				map[i - 1][j] <<= 1;	//λ����ʵ��*2
				map[i][j] = 0;		
				if (boxmode_g == 1 && map[i - 1][j] != 0)
					map[i - 1][j] = RandBox(map[i - 1][j]);	//ä�������
			}
		}
	}

	//�ٴ�������������������ϲ�����������0
	for (int t = 1; t < N; t++) {
		//t�����������ʵ�ּ�����ѭ��
		for (int i = t; i > 0; i--) {
			//i���У���������ѡ��
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

	//�������������������м�Ӱ��ϲ�������0
	for (int t = N - 1; t > 0; t--) {
		//t�����������ʵ�ּ�����ѭ��
		for (int i = t - 1; i < N; i++) {
			//i���У���������ѡ��
			for (int j = 0; j < N; j++) {
				if (map[i + 1][j] == 0)
					Swap(map[i + 1][j], map[i][j]);
			}
		}
	}

	//�ϲ�����
	for (int i = N - 2; i >= 0; i--) {
		for (int j = 0; j < N; j++) {
			if (map[i + 1][j] == map[i][j]) {
				map[i + 1][j] <<= 1;	//λ����ʵ��*2
				map[i][j] = 0;
				if (boxmode_g == 1 && map[i + 1][j] != 0)
					map[i + 1][j] = RandBox(map[i + 1][j]);	//ä�������
			}
		}
	}

	//�ٴ�������������������ϲ�����������0
	for (int t = N - 1; t > 0; t--) {
		//t�����������ʵ�ּ�����ѭ��
		for (int i = t - 1; i < N; i++) {
			//i���У���������ѡ��
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

	//�������������������м�Ӱ��ϲ�������0
	for (int t = 1; t < N; t++) {
		//t�����������ʵ�ּ�����ѭ��
		for (int i = t; i > 0; i--) {
			//i���У���������ѡ��
			for (int j = 0; j < N; j++) {
				if (map[j][i - 1] == 0)
					Swap(map[j][i - 1], map[j][i]);
			}
		}
	}

	//�ϲ�����
	for (int i = 1; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (map[j][i - 1] == map[j][i]) {
				map[j][i - 1] <<= 1;	//λ����ʵ��*2
				map[j][i] = 0; 
				if (boxmode_g == 1 && map[j][i - 1] != 0)
					map[j][i - 1] = RandBox(map[j][i - 1]);	//ä�������
			}
		}
	}

	//�ٴ�������������������ϲ�����������0
	for (int t = 1; t < N; t++) {
		//t�Ǽ�������ʵ�ּ�����ѭ��
		for (int i = t; i > 0; i--) {
			//i���У���������ѡ��
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

	//�ϲ�����
	for (int i = N - 2; i >= 0; i--) {
		for (int j = 0; j < N; j++) {
			if (map[j][i + 1] == map[j][i]) {
				map[j][i + 1] <<= 1;	//λ����ʵ��*2
				map[j][i] = 0;
				if (boxmode_g == 1 && map[j][i + 1] != 0)
					map[j][i + 1] = RandBox(map[j][i + 1]);	//ä�������
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
	//��ȡ��ǰλ�õ����֣�����������ɫ
	switch (n) {
	case 2:
		return 15;	//����ɫ
		break;
	case 4:
		return 6;	//��ɫ
		break;
	case 8:
		return 14;	//����ɫ
		break;
	case 16:
		return 11;	//��ǳ��ɫ
		break;
	case 32:
		return 3;	//ǳ��ɫ
		break;
	case 64:
		return 10;	//����ɫ
		break;
	case 128:
		return 2;	//��ɫ
		break;
	case 256:
		return 1;	//��ɫ
		break;
	case 512:
		return 9;	//����ɫ
		break;
	case 1024:
		return 13;	//����ɫ
		break;
	case 2048:
		return 5;	//��ɫ
		break;
	default:
		return 8;	//��ɫ
		break;
	}
}

void PrintMap(int map[][N]) {
	cout << "-------------------------------" << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), TextColor(map[i][j]));	//�������������ɫ
			cout << setw(5) << map[i][j];
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);	//�������������ɫ
		}
		cout << endl;
	}
	cout << "-------------------------------" << endl;
	//�ɼ������
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0 + 7 * 0x10);	//ǰ���ڣ�������
	cout << "Score:" << Score(map);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

void GameMain(int map[][N], int dif, struct Rank rank[]) {
	//Initiallize the game
	srand(time(0));
	rand();	//����һ���������̭��Ч���
	int* temp = new int;	//�������ɶ�̬�����洢���ɴ���
	*temp = 2;
	InitMap(map, *temp);
	delete temp;	//��ʱ�������̬�����Ǻ�ϰ��~

	system("cls");

	PrintMap(map);

	while (gameStatus_g == 1) {
		//�������
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
		CheckGameOver(map);	//�����Ϸ״̬
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
	cout << "����һ�֣�(y/n)" << endl;
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

//���а��ܵ�ʵ��

void StructRankInit(struct Rank rank[]) {
	//��ʼ���ṹ��
	for (int i = 0; i < 10; i++) {
		rank[i].Title = i + 1;
		rank[i].Score = 0;
	}
}

void ExtScoreRead(struct Rank rank[]) {
	//�ⲿ�ļ���ȡ�ɼ�����
	ifstream fin("DoNotDelete.rank");	//�²�ΪʲôҪ��������֣��ļ�����
	//ofstream fout("DoNotDelete.rank");	//�ļ�д������
	//������ʼ�����

	for (int i = 0; i < 10; i++)
		fin >> rank[i].Title >> rank[i].Score;
}

int ExtScoreInsert(struct Rank rank[], int score) {
	if (score <= rank[9].Score)
		return -1;

	bool flag = true;	//����Ƿ���ֱ���С������
	int pos;	//�����Ӧ�ò�������
	for (pos = 0; pos < 10 && flag; pos++) {
		if (rank[pos].Score <= score)
			flag = false;
	}
	pos--;
	for (int i = 8; i >= pos; i--) {
		rank[i + 1].Score = rank[i].Score;	//�������һ��
	}
	rank[pos].Score = score;

	return rank[pos].Title;
}

void ExtScoreWrite(struct Rank rank[]) {
	ofstream fout("DoNotDelete.rank");
	for (int i = 0; i < 10; i++)
		fout << rank[i].Title << " " << rank[i].Score << endl;
}