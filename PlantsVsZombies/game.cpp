#include "game.h"

#define WIN_WID 900
#define WIN_HIG 600

enum { PeaShooter, SunFlower, WallNut, PotatoMine, CherryBomb, CardCount };
IMAGE* plant[CardCount][20];
bool judgePlant = false;		//判断是否捡起植物
int curX, curY;					//当前植物移动过程中的位置
bool fileExist(char* name);		//判断文件是否存在
int index = -1;					//当前植物索引值
IMAGE back_ground_img;
IMAGE bar_img;
IMAGE card_img[CardCount];

bool fileExist(char* name) {
	FILE* fp = fopen(name, "r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}

void InitGame() {
	//加载游戏背景
	loadimage(&back_ground_img, "res/map0.jpg");
	//加载植物卡槽
	loadimage(&bar_img, "res/bar5.png");

	memset(plant, 0, sizeof(plant));
	//加载植物卡片
	for (int i = 0; i < CardCount; i++) {
		char name[64];
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&card_img[i], name);

		for (int j = 0; j < 20; j++) {
			sprintf_s(name,sizeof(name),"res/Plants/%d/%d.png",i,j+1);
			if (fileExist(name)) {
				plant[i][j] = new IMAGE;
				loadimage(plant[i][j], name);
			}
			else {
				break;
			}
		}
	}

	//创建游戏窗口
	initgraph(WIN_WID, WIN_HIG);
}

void UpdateWindow() {
	//开始缓冲
	BeginBatchDraw();

	//背景
	putimage(0, 0, &back_ground_img);
	//植物卡槽
	putimagePNG(250, 0, &bar_img);
	//植物卡牌
	for (int i = 0; i < CardCount; i++) {
		putimagePNG(338 + i * 64, 6, &card_img[i]);
	}

	//拖动过程中的植物
	if (index >= 0) {
		putimagePNG(curX,curY,plant[index][0]);
	}

	//结束缓冲
	EndBatchDraw();
}

void Click() {
	ExMessage msg;
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN && judgePlant == false) {
			if (msg.x > 338 && msg.x < 338 + 64 * CardCount && msg.y>6 && msg.y < 96) {
				index = (msg.x - 338) / 64;
				judgePlant = true;
				printf("%d\n", index);
			}
		}
		else if (msg.message == WM_MOUSEMOVE && judgePlant == true) {
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONDOWN && judgePlant == true) {
			index = -1;
			judgePlant = false;
		}
	}
}

void GameStart() {
	while (true){
		Click();
		UpdateWindow();
		
	}
}