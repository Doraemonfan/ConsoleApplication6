#include "stdafx.h"
#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <time.h>

#define WIDTH 1024
#define HIGH 768
#define MOVER_NUM 8000
#define FRICTION 0.96f

typedef struct MOVER {
	COLORREF color;
	float x;
	float y;
	float vX;
	float vY;
	float radius;
};

MOVER movers[MOVER_NUM];
int mouseX, mouseY;
int preMouseX, preMouseY;
int mouseVX, mouseVY;
bool isMouseDownL, isMouseDownR;
float toDist, blowDist, stirDist;

void startup() {
	srand((unsigned)time(NULL));
	for (int i = 0; i < MOVER_NUM; i++) {
		movers[i].color = RGB(rand() % 255, rand() % 255, rand() % 255);
		movers[i].x = float(rand() % WIDTH);
		movers[i].y = float(rand() % HIGH);
		movers[i].vX = float(cos(float(i))) * (rand() % 34);
		movers[i].vY = float(sin(float(i))) * (rand() % 34);
		movers[i].radius = (rand() % 34) / 15.0f;
	}

	mouseX = preMouseX = WIDTH / 2;
	mouseY = preMouseY = HIGH / 2;

	isMouseDownL = false;
	isMouseDownR = false;

	toDist = WIDTH *  0.86f;
	blowDist = WIDTH * 0.5f;
	stirDist = WIDTH * 0.125f;

	initgraph(WIDTH, HIGH);
	BeginBatchDraw();
}

void delay(DWORD ms) {
	static DWORD oldtime = GetTickCount();
	while (GetTickCount() - oldtime < ms)
		Sleep(1);
	oldtime = GetTickCount();
}

void show() {
	clearrectangle(0, 0, WIDTH, HIGH);

	for (int i = 0; i < MOVER_NUM; i++) {
		setcolor(movers[i].color);
		setfillcolor(movers[i].color);
		fillcircle(int(movers[i].x + 0.5), int(movers[i].y + 0.5), int(movers[i].radius + 0.5));
	 }

	FlushBatchDraw();
	delay(2);
}

void updateWithoutInput() {
	mouseVX = mouseX - preMouseX;
	mouseVY = mouseY - preMouseY;

	preMouseX = mouseX;
	preMouseY = mouseY;

	for (int i = 0; i < MOVER_NUM; i++) {
		float x = movers[i].x;
		float y = movers[i].y;
		float vX = movers[i].vX;
		float vY = movers[i].vY;
		
		float dX = x - mouseX;
		float dY = y - mouseY;
		float d = sqrt(dX * dX + dY * dY);

		if (!d) { dX /= d; dY /= d; }
		else { dX = 0; dY = 0; }

		if (d < toDist) {
			float toAcc = (1 - d / toDist) * WIDTH * 0.0014f;
			vX -= dX * toAcc;
			vY -= dY * toAcc;
		}

		if (isMouseDownL && d < blowDist) {
			float blowAcc = (1 - d / blowDist) * WIDTH * 10;
			vX += dX * blowAcc + 0.5f - float(rand()) / RAND_MAX;
			vY += dY * blowAcc + 0.5f - float(rand()) / RAND_MAX;
		}

		if (d < stirDist) {
			float mAcc = (1 - d / stirDist) * WIDTH * 0.00026f;
			vX += mouseVX * mAcc;
			vY += mouseVY * mAcc;
		}

		if (isMouseDownR) {
			x = mouseX;
			y = mouseY;
		}

		vX *= FRICTION;
		vY *= FRICTION;

		float avgVX = fabs(vX);
		float avgVY = fabs(vY);
		float avgV = (avgVX + avgVY) * 0.5f;
		if (avgVX < 0.1) vX *= float(rand()) / RAND_MAX * 3;
		if (avgVY < 0.1) vY *= float(rand()) / RAND_MAX * 3;
		float sc = avgV * 0.65f;
		movers[i].radius = max(min(sc, 9.5f), 0.4f);

		x += vX;
		y += vY;

		if (x > WIDTH) { x = WIDTH; vX = -vX; }
		else if (x < 0) { x = 0; vX = -vX; }
		if (y > HIGH) { y = HIGH; vY = -vY; }
		else if (y < 0) { y = 0; vY = -vY; }

		movers[i].x = x;
		movers[i].y = y;
		movers[i].vX = vX;
		movers[i].vY = vY;
	}
}

void updateWithInput() {
	MOUSEMSG m;
	if (MouseHit()) {
		m = GetMouseMsg();
		if (m.uMsg == WM_MOUSEMOVE) { mouseX = m.x; mouseY = m.y; }
		else if (m.uMsg == WM_LBUTTONDOWN) { isMouseDownL = true; }
		else if (m.uMsg == WM_LBUTTONUP) { isMouseDownL = false; }
		else if (m.uMsg == WM_RBUTTONDOWN) { isMouseDownR = true; }
		else if (m.uMsg == WM_RBUTTONUP) {isMouseDownR = false; }
	}
}



void gameover() {
	EndBatchDraw();
	closegraph(); 
}

int main() {
	startup();
	for (;;) {
		show();
		updateWithoutInput();
		updateWithInput();
	}
	gameover();

	return 0;
}