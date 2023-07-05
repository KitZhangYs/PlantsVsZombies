#include "game.h"

#define WIN_WID 1000
#define WIN_HIG 600
#define curX00 180							//第一行第一列草坪块的左上角位置的x值（减去120）（原256）
#define curY00 95							//第一行第一列草坪块的左上角位置的y值
#define cur_Height 100						//每一个草坪块的x的长度
#define cur_Width 81					    //每一个草坪块的y的长度

enum { PeaShooter, SunFlower, WallNut, PotatoMine, CherryBomb,CardCount };
IMAGE* Plants[CardCount][20];	//植物图片
int CardNums[CardCount] = {0};	//植物图片数量
bool judgePlant = false;		//判断是否捡起植物
int curX, curY;					//当前植物移动过程中的位置
bool fileExist(char* name);		//判断文件是否存在
int index = -1;					//当前植物索引值
IMAGE back_ground_img;			//游戏背景图片
IMAGE bar_img;					//植物卡槽图片
IMAGE card_img[CardCount];		//植物卡片图片
IMAGE sun_img[29];				//阳光图片
//植物
struct plant {
	int type;					//植物种类，0：无
	int frame;					//当前是第几帧
};
//全地图植物数组
struct plant AllMap[5][9] = {0};
int timer = 0;					//更新时间间隔
//阳光
struct SunShine {
	int x, y;					//当前坐标
	int frame;					//阳光帧序号
	int endY;					//飘落的目标y坐标
	bool used;					//是否被使用
	int timer;					//阳光计时器
};

//阳光池
struct SunShine balls[10];

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
	char name[64];
	memset(Plants, 0, sizeof(Plants));
	memset(AllMap, 0, sizeof(AllMap));
	memset(balls, 0, sizeof(balls));
	//加载植物卡片
	for (int i = 0; i < CardCount; i++) {
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&card_img[i], name);

		for (int j = 0; j < 20; j++) {
			sprintf_s(name,sizeof(name),"res/Plants/%d/%d.png",i,j+1);
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

	//加载阳光图片
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&sun_img[i], name);
	}

	//随机种子
	srand(time(NULL));

	//创建游戏窗口
	initgraph(WIN_WID, WIN_HIG, EX_SHOWCONSOLE);
}

//游戏背景
void PutBackGround() {
	//背景
	putimage(0, 0, &back_ground_img);
	//植物卡槽
	putimagePNG(250, 0, &bar_img);
	//植物卡牌
	for (int i = 0; i < CardCount; i++) {
		putimagePNG(338 + i * 64, 6, &card_img[i]);
	}
}

//种植后的植物
void PutPlants() {
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
}

//拖动过程中的植物
void PutDrag() {
	if (index >= 1) {
		IMAGE* zhiwu = Plants[index - 1][0];
		putimagePNG(curX - zhiwu->getwidth() / 2, curY - zhiwu->getheight() / 2, zhiwu);
	}
}

//从天空飘落的阳光
void PutSunShine1() {
	int ballnums = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballnums; i++) {
		if (balls[i].used) {
			int n = balls[i].frame;
			putimagePNG(balls[i].x, balls[i].y, &sun_img[n]);
		}
	}
}


//游戏窗口
void UpdateWindow() {
	//开始缓冲
	BeginBatchDraw();

	//游戏背景
	PutBackGround();

	//种植后的植物
	PutPlants();

	//拖动过程中的植物
	PutDrag();

	//随机生成的阳光
	PutSunShine1();

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

//更新植物动画帧
void PlantSwing() {
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
}

//随机创建阳光数据
void CreateSunshine() {
	static int count = 0;		//静态变量
	static int fre = 100;
	count++;
	if (count >= fre) {
		fre = 100 + rand() % 100;
		count = 0;

		//从阳光池中任取一个可以使用的阳光
		int ballnums = sizeof(balls) / sizeof(balls[0]);

		int i;
		for (i = 0; i < ballnums && balls[i].used; i++);
		if (i >= ballnums)
			return;

		balls[i].used = true;
		balls[i].frame = 0;
		balls[i].x = curX00 + (rand() % (9 * cur_Width));	//随机生成阳光x轴
		balls[i].endY = curY00 + ((rand() % 5) * cur_Height);//随机生成阳光的终点Y坐标
		balls[i].y = 60;
		balls[i].timer = 0;
		printf("%d\n", i);
	}
}

//更新阳光状态
void UpdateSunshine() {
	int ballnums = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballnums; i++) {
		if (balls[i].used) {
			balls[i].frame = (balls[i].frame + 1) % 29;

			if (balls[i].y / balls[i].endY) {
				if (balls[i].timer <= 120) {
					balls[i].timer++;
				}
				else {
					balls[i].used = false;
					balls[i].timer = 0;
				}
			}
			else {
				balls[i].y += 2;
			}
		}
	}
}

//更新游戏内信息
void UpdateGame() {
	PlantSwing();
	CreateSunshine();
	UpdateSunshine();
}

//开始游戏
void GameStart() {
	bool flag = false;
	mciSendString("play res/audio/readysetplant.mp3", 0, 0, 0);
	mciSendString("open res/audio/grasswalk.mp3 alias bg2", 0, 0, 0);
	mciSendString("play bg2 repeat", 0, 0, 0);
	mciSendString("setaudio bg2 volume to 300", 0, 0, 0);
	while (true){
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