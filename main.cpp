#include <stdio.h>
#include <windows.h>
#include <graphics.h>
#include "atlimage.h"
#include <time.h>

IMAGE image, img; // ��ͼͼ��
int coor[800][700]; // �����ͼ������������е����ص�
int blockX, blockY; // Ŀ�귽�鶥������
int peopleX, peopleY; // ��ɫ������
int touchX, touchY; // ģ�ⰴ�µ�����
int x, y;
char str[100];

BOOL isNewBlock(int color);
BOOL colorFun(COLORREF color1, COLORREF color2, int diff);

int main()
{
	srand((unsigned int)time(NULL));
	while (1)
	{
		// ��ͼ������
		printf("capturing data and screen��");
		system("adb shell screencap -p /storage/emulated/0/rock/screen.png");
		system("adb pull /storage/emulated/0/rock/screen.png"); //����ȱ��

		// ��ͼ��ʽת�� png -> jpg
		CImage cimage;
		cimage.Load("screen.png");
		cimage.Save("screen.jpg");
		loadimage(&image, "screen.jpg"); // �ѽ�ͼ���浽image
	   
		// �и�ͼƬ��ȡ����Ϸ������������Ϸ��������ж�Ч��
		SetWorkingImage(&image);
		//������(100,600)λ�ÿ�ʼ��ȡ��800*700������ȡͼƬ�����Ͻ�������(100,600)
		getimage(&img, 100, 600, 800, 700); 
		SetWorkingImage(&img);
		printf("getting the role and block location...\n");
		// ɨ���ɫ���꣬�������ϣ���������ɨ�赽ͷ����ֹͣ��
		for (y = 699; y >= 0; y--)  //����ɨ�裬 700�У�ÿ��800��
		{
			for (x = 0; x < 800; x++)
			{
				coor[x][y] = getpixel(x, y);// �������ص�
				if (colorFun(coor[x][y], RGB(55, 60, 100), 10))
				{
					peopleX = x;
					peopleY = y;
					goto getRole;
				}
			}
		}
	getRole:
		//  �жϽ�ɫ����߻����ұߣ��ӿ�ͼ���ж�Ч�ʣ�����С�η����bug
		for (y = 0; y < 700; y++)
		{
			/*  //�ұ�
			 x = 0;
			 for (; x < peopleX - 60; x++)  {

			 }
			*/

			/*  //���
			 x = peopleX + 60;
			 for (; x < 800; x++) {

			 }
			*/

			x = ((peopleX < 400) ? peopleX + 60 : 0);
			for (; x < ((peopleX < 400) ? 800 : peopleX - 60); x++)
			{
				coor[x][y] = getpixel(x, y); // ��ȡ���ص�
				if (isNewBlock(coor[x][y])) // �ж��Ƿ����µĺ���
				{
					blockX = x;
					blockY = y;
					goto getBlock;
				}
			}
		}
	getBlock:
		printf("calculate jump and touch coordinates...\n");
		// �������
		// �������Ĳ��Ǻ�׼ȷ
		// ׼ȷ�������Ŀ�귽�鶨��͵׵�֮����е�
		int targetX = blockX + 5;  //10��95���Խ�һ�������ֵ���棬��ǿ����Ҽ������
		int targetY = blockY + 60;
		// �������������ģ����ָ�������꣬����Ҽ��
		touchX = rand() % 80 + 200; // 200-279
		touchY = rand() % 85 + 300; // 300-384
		int distance = sqrt(double((targetY - peopleY)*(targetY - peopleY) + (targetX - peopleX)*(targetX - peopleX)));
		int time = 1.35 * distance;
		//����ƫ�ƵĻ�������ģ���˵ġ�����������
		//ÿ�ζ���׼ȷλ�õġ������������ܱ����Ϊ����ҡ�
		sprintf_s(str, sizeof(str), "adb shell input swipe %d %d %d %d %d", touchX, touchY, touchX + 1, touchY + 1, time);
		printf("executing��(%d,%d)->(%d,%d) touching (%d,%d) for %dms\n", peopleX, peopleY, targetX, targetY, touchX, touchY, time);
		system(str);
		//Sleep(1500);
		int waitTime = rand() % 1000 + 1000;
		Sleep(waitTime);
	}
	return 0;
}

// �ж���ɫ�Ƿ����ƣ�diff ԽСԽ����
BOOL colorFun(COLORREF color1, COLORREF color2, int diff)
{
	// �������ص㣬��RGB������ɫ�����ϵĲ�ͣ����ƽ��֮�ͣ����ٿ�����
	return sqrt(double((GetRValue(color2) - GetRValue(color1))*(GetRValue(color2) - GetRValue(color1)) + (GetGValue(color2) - GetGValue(color1))*(GetGValue(color2) - GetGValue(color1)) + (GetBValue(color2) - GetBValue(color1))*(GetBValue(color2) - GetBValue(color1)))) < diff;
}

// �ж��Ƿ����µĺ���
BOOL isNewBlock(int color)
{
	int r = GetRValue(color);
	int g = GetGValue(color);
	int b = GetBValue(color);

	if (colorFun(color, RGB(246, 246, 246), 10))//ǳ��ɫ
		return TRUE;
	else if (colorFun(color, RGB(250, 250, 250), 10))
		return TRUE;
	else if (colorFun(color, RGB(255, 255, 255), 0))//����ɫ
		return TRUE;
	else if (colorFun(color, RGB(100, 148, 106), 20))//ī��ɫ
		return TRUE;
	else if (colorFun(color, RGB(113, 113, 113), 10))//���ɫ 
		return TRUE;
	else if (colorFun(color, RGB(245, 128, 58), 10))//��ɫ
		return TRUE;
	else if (colorFun(color, RGB(186, 239, 69), 10))//ǳ��ɫ
		return TRUE;
	else if (colorFun(color, RGB(234, 203, 174), 10))//ľ������
		return TRUE;
	else if (colorFun(color, RGB(254, 240, 89), 10))//��ɫ
		return TRUE;
	else if (r > 124 && r < 134 && g>111 && g < 121 && b > 219 && b < 229)//��ɫ���
		return TRUE;
	else if (r > 75 && r < 85 && g>158 && g < 165 && b > 85 && b < 95)//��Բ������
		return TRUE;
	else if (colorFun(color, RGB(254, 74, 83), 10))//��ɫ
		return TRUE;
	else if (colorFun(color, RGB(152, 114, 111), 10))//����ʯ
		return TRUE;
	else if (colorFun(color, RGB(117, 117, 117), 10))//��Ͱ
		return TRUE;
	else if (colorFun(color, RGB(225, 199, 142), 10))
		return TRUE;
	else if (colorFun(color, RGB(241, 241, 241), 10))//�鱾
		return TRUE;
	else if (colorFun(color, RGB(255, 172, 178), 10))//��ɫ����
		return TRUE;
	else if (colorFun(color, RGB(73, 73, 73), 3))//�̲豭��
		return TRUE;
	else if (colorFun(color, RGB(147, 147, 147), 10))//���Ƴ�Ƭ��
		return TRUE;
	return FALSE;
}