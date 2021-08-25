#include <stdio.h>
#include <windows.h>
#include <graphics.h>
#include "atlimage.h"
#include <time.h>

IMAGE image, img; // 截图图像
int coor[800][700]; // 保存截图（处理后）中所有的像素点
int blockX, blockY; // 目标方块顶部坐标
int peopleX, peopleY; // 角色的坐标
int touchX, touchY; // 模拟按下的坐标
int x, y;
char str[100];

BOOL isNewBlock(int color);
BOOL colorFun(COLORREF color1, COLORREF color2, int diff);

int main()
{
	srand((unsigned int)time(NULL));
	while (1)
	{
		// 截图并保存
		printf("capturing data and screen：");
		system("adb shell screencap -p /storage/emulated/0/rock/screen.png");
		system("adb pull /storage/emulated/0/rock/screen.png"); //不可缺少

		// 截图格式转换 png -> jpg
		CImage cimage;
		cimage.Load("screen.png");
		cimage.Save("screen.jpg");
		loadimage(&image, "screen.jpg"); // 把截图保存到image
	   
		// 切割图片，取到游戏区，舍弃非游戏区域，提高判断效率
		SetWorkingImage(&image);
		//从坐标(100,600)位置开始截取，800*700，即截取图片的左上角坐标是(100,600)
		getimage(&img, 100, 600, 800, 700); 
		SetWorkingImage(&img);
		printf("getting the role and block location...\n");
		// 扫描角色坐标，从下往上（从上往下扫描到头部会停止）
		for (y = 699; y >= 0; y--)  //逐行扫描， 700行，每行800列
		{
			for (x = 0; x < 800; x++)
			{
				coor[x][y] = getpixel(x, y);// 保存像素点
				if (colorFun(coor[x][y], RGB(55, 60, 100), 10))
				{
					peopleX = x;
					peopleY = y;
					goto getRole;
				}
			}
		}
	getRole:
		//  判断角色在左边还是右边，加快图形判断效率，处理极小形方块的bug
		for (y = 0; y < 700; y++)
		{
			/*  //右边
			 x = 0;
			 for (; x < peopleX - 60; x++)  {

			 }
			*/

			/*  //左边
			 x = peopleX + 60;
			 for (; x < 800; x++) {

			 }
			*/

			x = ((peopleX < 400) ? peopleX + 60 : 0);
			for (; x < ((peopleX < 400) ? 800 : peopleX - 60); x++)
			{
				coor[x][y] = getpixel(x, y); // 获取像素点
				if (isNewBlock(coor[x][y])) // 判断是否是新的盒子
				{
					blockX = x;
					blockY = y;
					goto getBlock;
				}
			}
		}
	getBlock:
		printf("calculate jump and touch coordinates...\n");
		// 计算落点
		// 故意计算的不是很准确
		// 准确的落点是目标方块定点和底点之间的中点
		int targetX = blockX + 5;  //10和95可以进一步用随机值代替，加强防外挂监测能力
		int targetY = blockY + 60;
		// 根据随机数种子模拟手指按下坐标，防外挂检测
		touchX = rand() % 80 + 200; // 200-279
		touchY = rand() % 85 + 300; // 300-384
		int distance = sqrt(double((targetY - peopleY)*(targetY - peopleY) + (targetX - peopleX)*(targetX - peopleX)));
		int time = 1.35 * distance;
		//少许偏移的滑动，来模拟人的“长按”操作
		//每次都是准确位置的“长按”，可能被检测为“外挂”
		sprintf_s(str, sizeof(str), "adb shell input swipe %d %d %d %d %d", touchX, touchY, touchX + 1, touchY + 1, time);
		printf("executing：(%d,%d)->(%d,%d) touching (%d,%d) for %dms\n", peopleX, peopleY, targetX, targetY, touchX, touchY, time);
		system(str);
		//Sleep(1500);
		int waitTime = rand() % 1000 + 1000;
		Sleep(waitTime);
	}
	return 0;
}

// 判断颜色是否相似，diff 越小越相似
BOOL colorFun(COLORREF color1, COLORREF color2, int diff)
{
	// 两个像素点，在RGB三种颜色分量上的差方和（差的平方之和），再开根号
	return sqrt(double((GetRValue(color2) - GetRValue(color1))*(GetRValue(color2) - GetRValue(color1)) + (GetGValue(color2) - GetGValue(color1))*(GetGValue(color2) - GetGValue(color1)) + (GetBValue(color2) - GetBValue(color1))*(GetBValue(color2) - GetBValue(color1)))) < diff;
}

// 判断是否是新的盒子
BOOL isNewBlock(int color)
{
	int r = GetRValue(color);
	int g = GetGValue(color);
	int b = GetBValue(color);

	if (colorFun(color, RGB(246, 246, 246), 10))//浅白色
		return TRUE;
	else if (colorFun(color, RGB(250, 250, 250), 10))
		return TRUE;
	else if (colorFun(color, RGB(255, 255, 255), 0))//纯白色
		return TRUE;
	else if (colorFun(color, RGB(100, 148, 106), 20))//墨绿色
		return TRUE;
	else if (colorFun(color, RGB(113, 113, 113), 10))//深灰色 
		return TRUE;
	else if (colorFun(color, RGB(245, 128, 58), 10))//橙色
		return TRUE;
	else if (colorFun(color, RGB(186, 239, 69), 10))//浅绿色
		return TRUE;
	else if (colorFun(color, RGB(234, 203, 174), 10))//木质桌子
		return TRUE;
	else if (colorFun(color, RGB(254, 240, 89), 10))//黄色
		return TRUE;
	else if (r > 124 && r < 134 && g>111 && g < 121 && b > 219 && b < 229)//紫色相间
		return TRUE;
	else if (r > 75 && r < 85 && g>158 && g < 165 && b > 85 && b < 95)//大圆绿柱子
		return TRUE;
	else if (colorFun(color, RGB(254, 74, 83), 10))//红色
		return TRUE;
	else if (colorFun(color, RGB(152, 114, 111), 10))//华岩石
		return TRUE;
	else if (colorFun(color, RGB(117, 117, 117), 10))//马桶
		return TRUE;
	else if (colorFun(color, RGB(225, 199, 142), 10))
		return TRUE;
	else if (colorFun(color, RGB(241, 241, 241), 10))//书本
		return TRUE;
	else if (colorFun(color, RGB(255, 172, 178), 10))//粉色盒子
		return TRUE;
	else if (colorFun(color, RGB(73, 73, 73), 3))//奶茶杯子
		return TRUE;
	else if (colorFun(color, RGB(147, 147, 147), 10))//类似唱片机
		return TRUE;
	return FALSE;
}