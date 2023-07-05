#include "game.h"

#define WIN_WID 1000
#define WIN_HIG 600
#define curX00 180							//第一行第一列草坪块的左上角位置的x值（减去120）（原256）
#define curY00 95							//第一行第一列草坪块的左上角位置的y值
#define cur_Height 100						//每一个草坪块的x的长度
#define cur_Width 81					    //每一个草坪块的y的长度
#define zmNum 10                            //僵尸数量

enum { PeaShooter, SunFlower, WallNut, PotatoMine, CherryBomb, CardCount };
IMAGE* Plants[CardCount][20];	//植物图片
int CardNums[CardCount] = { 0 };	//植物图片数量
bool judgePlant = false;		//判断是否捡起植物
int curX, curY;					//当前植物移动过程中的位置
bool fileExist(char* name);		//判断文件是否存在
int index = -1;					//当前植物索引值
IMAGE back_ground_img;			//游戏背景图片
IMAGE bar_img;					//植物卡槽图片
IMAGE card_img[CardCount];		//植物卡片图片
//植物
struct plant {
	int type;					//植物种类，0：无
	int frame;					//当前是第几帧
};
//全地图植物数组
struct plant AllMap[5][9] = { 0 };
int timer = 0;					//更新时间间隔

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

struct zm
{
	int x, y;//该僵尸所在的坐标
	int frame;//该僵尸当前显示的图片帧位置数
	bool used;//是否出场
	int speed;//移动速度
};
struct zm zms[zmNum];//僵尸总数（10个
IMAGE imgZM[22];//储存僵尸每一帧动画的IMG数组


void InitGame() {
	//加载游戏背景
	loadimage(&back_ground_img, "res/map0.jpg");
	//加载植物卡槽
	loadimage(&bar_img, "res/bar5.png");

	memset(Plants, 0, sizeof(Plants));
	memset(AllMap, 0, sizeof(AllMap));
	//加载植物卡片
	for (int i = 0; i < CardCount; i++) {
		char name[64];
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&card_img[i], name);

		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/Plants/%d/%d.png", i, j + 1);
			if (fileExist(name)) {
				Plants[i][j] = new IMAGE;
				loadimage(Plants[i][j], name);
				CardNums[i]++;
			}
			else {
				break;
			}
		}
	}

	//初始化僵尸
	memset(zms, 0, sizeof(zms));
	for (int i = 0; i < 22; i++) {
		char name[64];
		sprintf_s(name, sizeof(name), "res/zm/0/%d.png", i + 1);
		loadimage(&imgZM[i], name);
	}

	//创建游戏窗口
	initgraph(WIN_WID, WIN_HIG, EX_SHOWCONSOLE);
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

	//种植后的植物
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 9; j++) {
			if (AllMap[i][j].type > 0) {
				int x = curX00 + j * cur_Width;
				int y = curY00 + i * cur_Height;
				int plant_type = AllMap[i][j].type - 1;
				int page = AllMap[i][j].frame;
				putimagePNG(x, y, Plants[plant_type][page]);
			}
		}
	}

	//拖动过程中的植物
	if (index >= 1) {
		IMAGE* zhiwu = Plants[index - 1][0];
		putimagePNG(curX - zhiwu->getwidth() / 2, curY - zhiwu->getheight() / 2, zhiwu);
	}

	//渲染僵尸图片
	for (int i = 0; i < zmNum; i++) {
		if (zms[i].used) {
			IMAGE* img = &imgZM[zms[i].frame];
			putimagePNG(zms[i].x,
				zms[i].y - (img->getheight()),
				img);
		}
	}

	//更新僵尸图片帧位置
	static int count2 = 0;
	if (count2++ == 100) {//动作降速器，机制相当于createZM当中的count
		count2 = 0;
		for (int i = 0; i < zmNum; i++) {
			if (zms[i].used) {
				if (zms[i].frame++ == 21)
					zms[i].frame = 0;
			}
		}
	}


	//结束缓冲
	EndBatchDraw();
}

//点击判断
void Click() {
	ExMessage msg;
	if (peekmessage(&msg)) {
		//首次点击植物卡片
		if (msg.message == WM_LBUTTONDOWN && judgePlant == false) {
			if (msg.x > 338 && msg.x < 338 + 64 * CardCount && msg.y>6 && msg.y < 96) {
				index = (msg.x - 338) / 64 + 1;
				judgePlant = true;
				curX = msg.x;
				curY = msg.y;
			}
		}
		else if (msg.message == WM_MOUSEMOVE && judgePlant == true) {
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONDOWN && judgePlant == true) {
			if (msg.x >= curX00 && msg.x <= curX00 + cur_Width * 9 && msg.y >= curY00 && msg.y <= curY00 + cur_Height * 5) {
				int row = (msg.y - curY00) / cur_Height;
				int col = (msg.x - curX00) / cur_Width;

				//种植
				if (AllMap[row][col].type == 0) {
					AllMap[row][col] = {
						index,
						0
					};
				}
			}
			index = 0;
			judgePlant = false;
		}
	}
}

void createZM() {
	static int zmFre = 0;//创建僵尸的帧间隔，初始200
	static int count = 0;//游戏帧计数器
	if (count++ > zmFre) {//帧计数器大于帧间隔时才创建僵尸，否则无操作
		count = 0;//帧计数器置零
		zmFre = rand() % 300 + 200;//帧间隔随机重置

		//创建僵尸
		int i;
		for (i = 0; i < zmNum && zms[i].used; i++);
		if (i < zmNum) {
			zms[i].used = true;
			zms[i].x = WIN_WID;
			zms[i].y = curY00 + cur_Height * (1 + rand() % 5);
			zms[i].speed = 1;
		}

	}

}

void updateZM() {
	static int count = 0;
	count++;
	if (count > 2) {
		count = 0;
		for (int i = 0; i < zmNum; i++) {
			//僵尸位置更新
			if (zms[i].used) {
				zms[i].x -= zms[i].speed;
				if (zms[i].x <= curX00)
				{
					//游戏失败
					MessageBox(NULL, "over", "over", 0);
					exit(0);
				}
			}
		}
	}
}

void UpdateGame() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 9; j++) {
			int n = AllMap[i][j].type - 1;
			if (AllMap[i][j].type > 0) {
				if (AllMap[i][j].frame < CardNums[n] - 1) {
					AllMap[i][j].frame++;
				}
				else {
					AllMap[i][j].frame = 0;
				}
			}
		}
	}

	createZM();//每一帧调用一次创建僵尸方法
	updateZM();//
}

void GameStart() {
	bool flag = false;
	while (true) {
		Click();
		timer += getDelay();
		if (timer > 40) {
			flag = true;
			timer = 0;
		}
		UpdateWindow();
		if (flag) {
			flag = false;
			UpdateGame();
		}

	}
}