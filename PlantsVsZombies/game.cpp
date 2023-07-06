#include "game.h"

#define WIN_WID 1000
#define WIN_HIG 600
#define curX00 181							//第一行第一列草坪块的左上角位置的x值（减去120）（原256）
#define curY00 95							//第一行第一列草坪块的左上角位置的y值
#define cur_Height 100						//每一个草坪块的x的长度
#define cur_Width 81					    //每一个草坪块的y的长度
#define zmNum 10                            //僵尸数量
#define BULLET_MAX 10000						//子弹池数目

enum { PeaShooter, SunFlower, WallNut, PotatoMine, CherryBomb, CardCount };
IMAGE* Plants[CardCount][20];	//植物图片
int CardNums[CardCount] = { 0 };	//植物图片数量
bool judgePlant = false;		//判断是否捡起植物
bool judgeShovel = false;		//判断是否捡起铲子
int curX, curY;					//当前植物移动过程中的位置
bool fileExist(char* name);		//判断文件是否存在
int index = -1;					//当前植物索引值
IMAGE back_ground_img;			//游戏背景图片
IMAGE bar_img;					//植物卡槽图片
IMAGE card_img[CardCount];		//植物卡片图片
IMAGE sun_img[29];				//阳光图片
IMAGE shovel_img;				//铲子图片
IMAGE shovel_slot_img;			//铲子槽位图片
IMAGE bul_img[2];					//子弹图片
int zm_nums[5];					//每行僵尸数量

//植物
struct plant {
	int type;					//植物种类，0：无
	int frame;					//当前是第几帧
	int timer;					//植物功能冷却时间（向日葵生产阳光、豌豆发射子弹等）
	int hp;						//植物生命值
	int beingEaten;				//正在被几个人撅
};

//全地图植物数组
struct plant AllMap[5][9] = { 0 };
int timer = 0;					//更新时间间隔

//阳光
struct SunShine {
	int x, y;					//当前坐标
	int frame;					//阳光帧序号
	int endY;					//飘落的目标y坐标
	bool used;					//是否被使用
	int timer;					//阳光计时器
	float xoff;					//x偏移量
	float yoff;					//y偏移量
};

//子弹
struct bullet{
	int x, y;					//子弹当前坐标
	int row;					//子弹所在行数
	int dmg;					//子弹伤害
	bool used;					//子弹使用状态
};

//阳光池
struct SunShine balls[100];

//子弹池
struct bullet bullets[BULLET_MAX];

int SunShineValue = 150;

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
	int type;//僵尸种类 普通0 路障1 铁桶2
	int x, y;//该僵尸所在的坐标
	int frame;//该僵尸当前显示的图片帧位置数
	bool used;//是否出场
	int speed;//移动速度
	bool eating;//是否在吃
	int row;
};
struct zm zms[zmNum];//僵尸总数（10个
IMAGE imgZM[3][22];//储存三种僵尸每一帧动画的IMG数组
IMAGE imgZMEating[3][21];//储存三种僵尸吃饭动画的IMG数组，最多21张

//游戏内容初始化
void InitGame() {
	//加载游戏背景
	loadimage(&back_ground_img, "res/map0.jpg");
	//加载植物卡槽
	loadimage(&bar_img, "res/bar5.png");
	char name[64];
	memset(Plants, 0, sizeof(Plants));		//初始化植物图片
	memset(AllMap, 0, sizeof(AllMap));		//初始化全地图植物
	memset(balls, 0, sizeof(balls));		//初始化阳光池
	memset(zm_nums,0,sizeof(zm_nums)) ;		//初始化僵尸数量
	memset(bullets, 0, sizeof(bullets));	//初始化子弹池
	SunShineValue = 150;
	//加载植物卡片
	for (int i = 0; i < CardCount; i++) {
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


	//加载阳光图片
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&sun_img[i], name);
	}

	//随机种子
	srand(time(NULL));

	//初始化僵尸行走图片
	memset(zms, 0, sizeof(zms));
	for (int j = 0; j < 3; j++) {//每种僵尸行走图片都有22帧，所以可以直接二重循环
		for (int i = 0; i < 22; i++) {
			char name[64];
			sprintf_s(name, sizeof(name), "res/zm/%d/%d.png",j, i + 1);
			loadimage(&imgZM[j][i], name);
		}
	}
	//初始化僵尸吃饭图片
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < 21; i++) {
			char name[64];
			sprintf_s(name, sizeof(name), "res/zm_eat/%d/%d.png", j, i + 1);
			loadimage(&imgZMEating[j][i], name);
		}
	}

	//加载铲子槽位图片
	loadimage(&shovel_slot_img, "res/shovelSlot.png");
	//加载铲子图片
	loadimage(&shovel_img,"res/shovel.png");

	//加载子弹图片
	loadimage(&bul_img[0], "res/bullet_normal.png");
	loadimage(&bul_img[1], "res/bullet_blast.png");

	//创建游戏窗口
	initgraph(WIN_WID, WIN_HIG, EX_SHOWCONSOLE);

	//设置字体
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 25;
	strcpy(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;  //抗锯齿
	settextstyle(&f);
	setbkmode(TRANSPARENT); //背景模式
	setcolor(BLACK);
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
	//铲子槽位
	putimagePNG(338 + 8 * 64 + 10,10,&shovel_slot_img);
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
	if (index >= 1 && judgePlant == true) {
		IMAGE* zhiwu = Plants[index - 1][0];
		putimagePNG(curX - zhiwu->getwidth() / 2, curY - zhiwu->getheight() / 2, zhiwu);
	}
}

//从天空飘落的阳光
void PutSunShine1() {
	int ballnums = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballnums; i++) {
		if (balls[i].used || balls[i].xoff != 0) {
			int n = balls[i].frame;
			putimagePNG(balls[i].x, balls[i].y, &sun_img[n]);
		}
	}
}

//阳光值
void PutSunShineValue() {
	char score[8];
	sprintf_s(score, sizeof(score), "%d", SunShineValue);
	outtextxy(curX00 + 100, curY00 - 28, score);
}

//铲子
void PutShovel() {				//渲染未拖动的以及拖动过程中的铲子
	if (judgeShovel == true)
		putimagePNG(curX - 32, curY - 32, &shovel_img);
	else
		putimagePNG(338 + 8*64 + 10, 10,&shovel_img );		//铲子70像素宽
}


//绘制僵尸并更新图片帧
void drawZM() {

	//渲染僵尸图片
	for (int i = 0; i < zmNum; i++) {
		if (zms[i].used) {
			IMAGE* img;
			if(zms[i].eating){		//根据僵尸是否在吃饭判断加载哪个数组的图片
				img = &imgZMEating[zms[i].type][zms[i].frame];
			}
			else {
				img = &imgZM[zms[i].type][zms[i].frame];
			}
			putimagePNG(zms[i].x,
				zms[i].y - (img->getheight()),
				img);				//统一渲染
			
		}
	}

	//更新僵尸图片帧位置
	static int count2 = 0;//动作降速器，机制同理于createZM当中的count
	if (count2++ == 100) {
		count2 = 0;
		for (int i = 0; i < zmNum; i++) {
			if (zms[i].used) {
				int frameMax;
				if (!zms[i].eating) {//如果没在吃饭，判断僵尸种类
					switch (zms[i].type)
					{
					case 0:frameMax = 21; break;//普通僵尸22帧
					case 1:frameMax = 20; break;//路障    21帧
					default:frameMax = 14;break;//铁桶    15帧
					}
				}
				else {
					if (zms[i].type == 0) {//在吃饭，如果是普通僵尸，就是21帧
						frameMax = 20;
					}
					else {						//不是普通僵尸，就11帧
						frameMax = 10;
					}
				}

				if (zms[i].frame++ == frameMax)
					zms[i].frame = 0;//此时frame已经等于图片帧数，下一次渲染就会数组越界，所以置零
			}
		}
	}
}


//子弹
void PutBullet() {
	for (int i = 0; i < BULLET_MAX; i++) {
		if (bullets[i].x > 0) {
			if (bullets[i].used) {
				putimagePNG(bullets[i].x, bullets[i].y, &bul_img[0]);
			}
			else {
				putimagePNG(bullets[i].x, bullets[i].y, &bul_img[1]);

			}
		}
	}
}

//游戏窗口
void UpdateWindow() {
	//开始缓冲
	BeginBatchDraw();

	//游戏背景
	PutBackGround();

	//子弹图片
	PutBullet();

	//种植后的植物
	PutPlants();

	//铲子
	PutShovel();

	//拖动过程中的植物
	PutDrag();

	//随机生成的阳光
	PutSunShine1();

	//阳光值
	PutSunShineValue();

	//渲染所有已生成的僵尸
	drawZM();

	//结束缓冲
	EndBatchDraw();
}

//选取植物
void CatchPlant(ExMessage* msg) {
	index = (msg->x - 338) / 64 + 1;
	judgePlant = true;
	curX = msg->x;
	curY = msg->y;
	PlaySound("res/audio/select.wav", NULL, SND_FILENAME | SND_ASYNC);
}

//初始化植物
void InitPlant(int row, int col, int type) {
	AllMap[row][col].type = type;
	AllMap[row][col].frame = 0;
	int p_hp = 0;
	switch (type - 1)
	{
	case PeaShooter:
		p_hp = 300;
		break;
	case SunFlower:
		p_hp = 300;
		break;
	case WallNut:
		p_hp = 4000;
		break;
	case PotatoMine:
		p_hp = 300;
		break;
	default:
		break;
	}
	AllMap[row][col].hp = p_hp;
	AllMap[row][col].timer = 0;
}

//种植植物
void Planting(ExMessage* msg) {
	if (msg->x >= curX00 && msg->x <= curX00 + cur_Width * 9 && msg->y >= curY00 && msg->y <= curY00 + cur_Height * 5) {
		int row = (msg->y - curY00) / cur_Height;
		int col = (msg->x - curX00) / cur_Width;

		//种植
		if (AllMap[row][col].type == 0) {
			InitPlant(row, col, index);
			PlaySound("res/audio/plantdown.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
	}
	index = 0;
	judgePlant = false;
}

//收集阳光
void CollectSunShine(ExMessage* msg) {
	int ballnums = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballnums; i++) {
		if (balls[i].used) {
			int nowX = balls[i].x;
			int nowY = balls[i].y;
			if (msg->x > nowX && msg->x < nowX + 79 && msg->y > nowY && msg->y < nowY + 79) {
				PlaySound("res/audio/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);
				//设置偏移量
				float destY = 0;
				float destX = 262;
				float angle = atan((nowY - destY) / (nowX - destX));
				balls[i].xoff = 8 * cos(angle);
				balls[i].yoff = 8 * sin(angle);
				balls[i].used = false;
			}
		}
	}
}

//拾取铲子
void UseShovel(ExMessage* msg) {
	judgeShovel = true;
	curX = msg->x;
	curY = msg->y;
}

//铲除植物
void DeletePlant(ExMessage* msg) {
	if (msg->x >= curX00 && msg->x <= curX00 + cur_Width * 9 && msg->y >= curY00 && msg->y <= curY00 + cur_Height * 5) {
		int row = (msg->y - curY00) / cur_Height;
		int col = (msg->x - curX00) / cur_Width;

		//移除植物
		if (AllMap[row][col].type != 0) {
			AllMap[row][col].type = 0;
			PlaySound("res/audio/plantdown.wav", NULL, SND_FILENAME | SND_ASYNC);
		}
	}
	index = 0;
	judgeShovel = false;
}

//点击判断
void Click() {
	ExMessage msg;
	if (peekmessage(&msg)) {
		//首次点击植物卡片
		if (msg.message == WM_LBUTTONDOWN && judgePlant == false && judgeShovel == false) {
			if (msg.x > 338 && msg.x < 338 + 64 * CardCount && msg.y>6 && msg.y < 96) {
				CatchPlant(&msg);
			}
			else if (msg.x > 338 + 8 * 64 + 10 && msg.x < 338 + 8 * 64 + 80 && msg.y > 10 && msg.y < 80) {
				UseShovel(&msg);
			}
			else {
				CollectSunShine(&msg);
			}
		}
		//植物卡牌或铲子拖动
		else if (msg.message == WM_MOUSEMOVE && (judgePlant == true || judgeShovel == true)) {
			curX = msg.x;
			curY = msg.y;
		}
		//植物卡牌种植
		else if (msg.message == WM_LBUTTONDOWN && judgePlant == true && judgeShovel == false) {
			Planting(&msg);
		}
		//铲除植物
		else if (msg.message == WM_LBUTTONDOWN && judgePlant == false && judgeShovel == true) {
			DeletePlant(&msg);
		}
		//移除当前状态（不再拖动植物，不再拿取铲子）
		else if (msg.message == WM_RBUTTONDOWN) {
			judgePlant = false;
			judgeShovel = false;
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

//向日葵生产阳光
void SunFlowerSunshine() {
	static int fre = 100;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 9; j++) {
			if ((AllMap[i][j].type - 1) == SunFlower) {
				if (AllMap[i][j].timer > fre) {
					//从阳光池中任取一个可以使用的阳光
					int ballnums = sizeof(balls) / sizeof(balls[0]);

					int k;
					for (k = 0; k < ballnums && balls[k].used; k++);
					if (k >= ballnums)
						return;

					balls[k].used = true;
					balls[k].frame = 0;
					balls[k].x = curX00 + (j * cur_Width);	//x轴
					balls[k].endY = curY00 + (i * cur_Height);//Y坐标
					balls[k].y = balls[k].endY;
					balls[k].timer = -240;
					balls[k].xoff = 0;
					balls[k].yoff = 0;
					AllMap[i][j].timer = 0;
					fre = 100 + rand() % 50;
				}
				else {
					AllMap[i][j].timer++;
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
		balls[i].x = curX00 + 84 + (rand() % (9 * cur_Width));	//随机生成阳光x轴
		balls[i].endY = curY00 + ((rand() % 5) * cur_Height);//随机生成阳光的终点Y坐标
		balls[i].y = 60;
		balls[i].timer = 0;
		balls[i].xoff = 0;
		balls[i].yoff = 0;
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
		else if (balls[i].xoff != 0 || balls[i].yoff != 0) {
			float destY = 0;
			float destX = 262;
			float angle = atan((balls[i].y - destY) / (balls[i].x - destX));
			balls[i].xoff = 16 * cos(angle);
			balls[i].yoff = 16 * sin(angle);
			balls[i].x -= balls[i].xoff;
			if (balls[i].yoff < 0) {
				balls[i].yoff = -balls[i].yoff;
			}
			balls[i].y -= balls[i].yoff;
			if (balls[i].y < 0 || balls[i].x < 262) {
				balls[i].xoff = 0;
				balls[i].yoff = 0;
				SunShineValue += 25;
			}
		}
	}
}

//豌豆射手发射子弹
void FiringBullets() {
	static int fre = 35;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 9 ; j++) {
			if (AllMap[i][j].type - 1 == PeaShooter) {
				if (AllMap[i][j].timer >= fre) {
					int k;
					for (k = 0; k < BULLET_MAX && bullets[k].used; k++);
					int x = curX00 + j * cur_Width;
					int y = curY00 + i * cur_Height;
					bullets[k].used = true;
					bullets[k].row = i + 1;
					bullets[k].x = x + 60;
					bullets[k].y = y + 10;
					bullets[k].dmg = 20;
					AllMap[i][j].timer = 0;
					int flag = rand() % 2;				//随机播放这两个音效中的一个
					if (flag == 2)
						PlaySound("res/audio/shootpea.wav", NULL, SND_FILENAME | SND_ASYNC);
					else
						PlaySound("res/audio/shootpea2.wav", NULL, SND_FILENAME | SND_ASYNC);
				}
				else {
					AllMap[i][j].timer++;
				}
			}
		}
	}
}

//创造僵尸
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
			zms[i].row = (rand() % 5) + 1;
			zm_nums[zms[i].row]++;//标记此行僵尸数量，不等于0即令豌豆射手吐痰
			zms[i].type = rand()%3;//随机僵尸种类
			zms[i].used = true;
			zms[i].eating = false;
			zms[i].x = WIN_WID;
			zms[i].y = curY00 + cur_Height *  zms[i].row - 20;
			zms[i].speed = 1;
			zms[i].frame = 0;
		}

	}

}

//更新僵尸位置和状态
void updateZM() {
	static int count = 0;
	count++;
	if (count > 2) {
		count = 0;
		//遍历所有僵尸，更新一次状态
		for (int i = 0; i < zmNum; i++) {
			if (zms[i].used) {
				//判断僵尸有没有在吃饭
				if (!zms[i].eating) {
					//没有则移动，僵尸位置更新
					zms[i].x -= zms[i].speed;
					
					//移动后立即判断僵尸能不能开饭
					for (int t = 0; t < 9; t++) {
						if (AllMap[zms[i].row-1][t].type != 0) {//遍历判断该行每一个格子有无植物
							int leftX = curX00 + (t-1) * cur_Width - 20;
							int rigthX = leftX + cur_Width;
							if (zms[i].x <= rigthX && zms[i].x >= leftX)//如果有，则判断该僵尸位置是否可以开吃
							{
								zms[i].eating = true;//使僵尸正在吃饭
								zms[i].frame = 0;//并且从吃饭图片的第一帧开始播放
								AllMap[zms[i].row - 1][t].beingEaten ++;//使该格植物被吃
							}
						}
					}

					//再判断游戏是否已经失败
					if (zms[i].x <= (curX00 - 20))
					{
						//游戏失败
						MessageBox(NULL, "over", "over", 0);
						exit(0);
					}
				}
				else
				{
					//在吃饭就判断有没有吃完
					for (int t = 0; t < 9; t++) {
						if (AllMap[zms[i].row - 1][t].type == 0) {//遍历判断该行每一个格子有无植物
							int leftX = curX00 + (t - 1) * cur_Width - 20;
							int rigthX = leftX + cur_Width;
							if (zms[i].x <= rigthX && zms[i].x >= leftX)//如果没植物，则判断该僵尸是否在这个位置
							{
								zms[i].eating = false;//使僵尸不再继续吃饭
								zms[i].frame = 0;//并且从行走图片的第一帧开始播放
							}
						}
					}
				}
			}
		}

		//遍历所有植物，使被吃的植物都扣一次血
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 9; j++) {
				if (AllMap[i][j].beingEaten)
				{
					AllMap[i][j].hp-= AllMap[i][j].beingEaten;
					if (AllMap[i][j].hp<=0)//被撅死力（悲）
					{
						AllMap[i][j].type = 0;
						AllMap[i][j].beingEaten = 0;
						AllMap[i][j].frame = 0;
						AllMap[i][j].hp = 0;
						AllMap[i][j].timer = 0;
					}
				}
			}
		}
	}
}

//更新子弹状态
void UpdateBullet() {
	for (int i = 0; i < BULLET_MAX; i++) {
		if (bullets[i].used) {
			bullets[i].x += 4;
			if (bullets[i].x >= 1000) {
				bullets[i].x = 0;
				bullets[i].y = 0;
				bullets[i].used = false;
			}
		}
	}
}

//更新游戏内信息
void UpdateGame() {
	PlantSwing();
	CreateSunshine();
	SunFlowerSunshine();
	UpdateSunshine();
	UpdateBullet();
	FiringBullets();

	createZM();//每一帧调用一次的方法创建僵尸
	updateZM();//每一帧刷新一次僵尸
}

//开始游戏
void GameStart() {
	int just = 0;//判断开始游戏还是退出，just=1开始，juat=2退出
	startUI(&just);
	if (just == 1) {
		mciSendString("close bg", 0, 0, 0);
		bool flag = false;
		mciSendString("play res/audio/readysetplant.mp3", 0, 0, 0);
		mciSendString("open res/audio/grasswalk.mp3 alias bg2", 0, 0, 0);
		//mciSendString("play bg2 repeat", 0, 0, 0);
		//mciSendString("setaudio bg2 volume to 300", 0, 0, 0);
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
}