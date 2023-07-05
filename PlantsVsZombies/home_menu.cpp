#include "home_menu.h"

void startUI(int* just) {
	IMAGE imgBg2, imgMENU1, imgMENU2, imgMENU_exit1, imgMENU_exit2;
	loadimage(&imgBg2, "res/menu.png");
	loadimage(&imgMENU1, "res/menu1.png");
	loadimage(&imgMENU2, "res/menu2.png");
	loadimage(&imgMENU_exit1, "res/menu_quit1.png");
	loadimage(&imgMENU_exit2, "res/menu_quit2.png");
	int flag = 1, flag_exit = 1;
	mciSendString("open res/audio/bg.mp3 alias bg", 0, 0, 0); 	//播放背景音乐并修改音量
	mciSendString("play bg repeat", 0, 0, 0);
	mciSendString("setaudio bg volume to 300", 0, 0, 0);
	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgBg2);
		putimagePNG(475, 75, flag ? &imgMENU1 : &imgMENU2);
		putimagePNG(812, 508, flag_exit ? &imgMENU_exit1 : &imgMENU_exit2);
		ExMessage msg;
		if (peekmessage(&msg)) {
			if (msg.message == WM_MOUSEMOVE &&
				msg.x > 474 && msg.x < 474 + 331 &&
				msg.y>75 && msg.y < 75 + 140) {
				flag = 0;
			}//
			else if (!(msg.x > 474 && msg.x < 474 + 331 &&
				msg.y>75 && msg.y < 75 + 140 ||
				msg.x > 812 && msg.x < 812 + 47 &&
				msg.y>508 && msg.y < 508 + 27)) {
				flag = 1;
				flag_exit = 1;
			}//判断鼠标是否在开始图片的外面
			else if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 474 && msg.x < 474 + 331 &&
				msg.y>75 && msg.y < 75 + 140) {
				flag = 2;
			}//判断鼠标是否在图片范围内进行一次左键按下
			else if (msg.message == WM_LBUTTONUP && flag == 2) {
				*just = 1;
				break;
			}//判断鼠标左键是否松开一次
			else if (msg.message == WM_MOUSEMOVE &&
				msg.x > 812 && msg.x < 812 + 47 &&
				msg.y>508 && msg.y < 508 + 27) {
				flag_exit = 0;
			}//
			else if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 812 && msg.x < 812 + 47 &&
				msg.y>508 && msg.y < 508 + 27) {
				flag_exit = 2;
			}//判断鼠标是否在图片范围内进行一次左键按下
			else if (msg.message == WM_LBUTTONUP && flag_exit == 2) {
				*just = 2;
				break;
			}//判断鼠标左键是否松开一次
		}
		EndBatchDraw();
	}
}
