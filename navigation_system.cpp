#define _CRT_SECURE_NO_WARNINGS 1
#include <graphics.h>              // 引用图形库头文件
#include <conio.h>
#include <stdio.h>
#include <windows.h>				// 用到了定时函数sleep()
#include <math.h>
#include <string.h>

#define MAX 10   // 最大景点数量
#define INF 9999 // 无穷大表示不可达

// 景点结构体
typedef struct {
	char name[30];
	char code[10];
	char description[100];
} Spot;

// 图结构体
typedef struct {
	Spot spots[MAX];
	int edges[MAX][MAX];  // 邻接矩阵存储路径长度
	int numSpots;         // 景点数量
} Graph;

typedef struct {
	int x;       // 圆心x坐标
	int y;       // 圆心y坐标
	int radius;  // 半径
	COLORREF color; // 颜色
	char code;   // 代号
} Circle;


// 初始化图
void initGraph(Graph* g) {
	g->numSpots = 0;
	for (int i = 0; i < MAX; i++) {
		for (int j = 0; j < MAX; j++) {
			g->edges[i][j] = (i == j) ? 0 : INF;
		}
	}
}

// 添加景点
void addSpot(Graph* g, const char* name, const char* code, const char* description) {
	if (g->numSpots < MAX) {
		strcpy(g->spots[g->numSpots].name, name);
		strcpy(g->spots[g->numSpots].code, code);
		strcpy(g->spots[g->numSpots].description, description);
		g->numSpots++;
	}
}

// 添加路径
void addEdge(Graph* g, int start, int end, int length) {
	g->edges[start][end] = length;
	g->edges[end][start] = length;
}


void page() {
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 设置窗口的宽度和高度
	int windowWidth = 1280;
	int windowHeight = 800;

	// 计算窗口左上角的位置，确保窗口居中
	int windowPosX = (screenWidth - windowWidth) / 2;
	int windowPosY = (screenHeight - windowHeight) / 2;

	// 初始化图形窗口
	HWND hwnd = GetHWnd();
	SetWindowPos(hwnd, HWND_TOP, windowPosX, windowPosY, 0, 0, SWP_NOSIZE);

	IMAGE bgImage;
	loadimage(&bgImage, _T("1.jpg"), 1080, 800);

	setbkcolor(WHITE);
	cleardevice();
	putimage(240, 0, &bgImage);

	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, 1);
	line(400, 0, 400, 800);

	setfillcolor(RGB(107, 196, 227));
	fillrectangle(0, 0, 1280, 50);
	fillrectangle(0, 750, 1280, 800);

	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 40;
	_tcscpy(f.lfFaceName, _T("宋体"));
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);
	settextcolor(RGB(87, 176, 187));
	outtextxy(90, 150, "校园导游咨询");

	RECT rect1 = { 100, 290, 300, 340 };
	RECT rect2 = { 100, 360, 300, 410 };
	RECT rect3 = { 100, 643, 190, 693 };
	RECT rect4 = { 210, 643, 300, 693 };
	RECT rect5 = { 50, 430, 350, 620 };
	int radius = 20;

	setlinecolor(WHITE);
	LOGFONT f1;
	gettextstyle(&f1);
	f1.lfHeight = 30;
	_tcscpy(f1.lfFaceName, _T("幼圆"));
	f1.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f1);
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	outtextxy(110, 300, "查询景点信息");
	outtextxy(110, 370, "查询最短路径");
	f1.lfHeight = 35;
	settextstyle(&f1);
	outtextxy(110, 650, "重置");
	outtextxy(220, 650, "退出");
}


void printCirclesByCode(char code) {
	Circle circles[10] = {
		{1076, 450, 20, RED, '0'},
		{907, 252, 20, GREEN, '1'},
		{818, 627, 20, BLUE, '2'},
		{997, 629, 20, YELLOW, '3'},
		{1075, 271, 20, CYAN, '4'},
		{1075, 157, 20, MAGENTA, '5'},
		{600, 419, 20, BROWN, '6'},
		{600, 542, 20, DARKGRAY, '7'},
		{678, 155, 20, LIGHTGRAY, '8'},
		{904, 450, 20, BLACK, '9'}
	};

	for (int i = 0; i < 10; i++) {
		if (circles[i].code == code) {
			setfillcolor(RGB(107, 196, 227));
			fillcircle(circles[i].x, circles[i].y, circles[i].radius);
		}
	}
}


// 显示景点信息
void showSpotInfo(Graph* g, int index) {
	char cir;
	RECT rect5 = { 50, 430, 350, 620 };
	int radius = 20;
	setfillcolor(WHITE);
	fillroundrect(rect5.left, rect5.top, rect5.right, rect5.bottom, radius, radius);

	char text[100];
	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	_tcscpy(f.lfFaceName, _T("宋体"));
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);
	settextcolor(RGB(87, 176, 187));

	if (index >= 0 && index < g->numSpots) {
		sprintf(text, "景点：%s (%s)\n简介：%s",
			g->spots[index].name, g->spots[index].code, g->spots[index].description);
		drawtext(text, &rect5, DT_LEFT | DT_TOP | DT_WORDBREAK);
		cir = g->spots[index].code[0];
		printCirclesByCode(cir);
	}
	else {
		drawtext("景点不存在", &rect5, DT_LEFT | DT_TOP | DT_WORDBREAK);
	}
}


// Dijkstra算法查询任意两个景点之间的最短路径
void dijkstra(Graph* g, int start, int end) {
	char cir[10];
	RECT rect5 = { 50, 430, 350, 620 };
	int radius = 20;
	setfillcolor(WHITE);
	fillroundrect(rect5.left, rect5.top, rect5.right, rect5.bottom, radius, radius);

	char text[100];
	char temp[100] = "路径：";
	int tempLen = 0;

	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 30;
	_tcscpy(f.lfFaceName, _T("宋体"));
	f.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f);
	settextcolor(RGB(87, 176, 187));

	int dist[MAX], visited[MAX], prev[MAX];
	for (int i = 0; i < g->numSpots; i++) {
		dist[i] = INF;
		visited[i] = 0;
		prev[i] = -1;
	}
	dist[start] = 0;

	for (int i = 0; i < g->numSpots; i++) {
		int min = INF, u = -1;
		for (int j = 0; j < g->numSpots; j++) {
			if (!visited[j] && dist[j] < min) {
				u = j;
				min = dist[j];
			}
		}
		if (u == -1) break;
		visited[u] = 1;

		for (int v = 0; v < g->numSpots; v++) {
			if (!visited[v] && g->edges[u][v] != INF && dist[u] + g->edges[u][v] < dist[v]) {
				dist[v] = dist[u] + g->edges[u][v];
				prev[v] = u;
			}
		}
	}

	if (dist[end] == INF) {
		sprintf(text, "从 %s 到 %s 无法到达！\n", g->spots[start].name, g->spots[end].name);
		drawtext(text, &rect5, DT_LEFT | DT_TOP | DT_WORDBREAK);
	}
	else {
		sprintf(text, "最短路径长度：%d\n", dist[end]);
		int path[MAX], count = 0, v = end;
		while (v != -1) {
			path[count++] = v;
			v = prev[v];
		}

		tempLen += sprintf(temp + tempLen, "路径：");
		for (int j = count - 1; j >= 0; j--) {
			tempLen += sprintf(temp + tempLen, "%s", g->spots[path[j]].name);
			cir[j] = g->spots[path[j]].code[0];
			if (j > 0) {
				tempLen += sprintf(temp + tempLen, " -> ");
			}
		}
		tempLen += sprintf(temp + tempLen, "\n%s", text);
		drawtext(temp, &rect5, DT_LEFT | DT_TOP | DT_WORDBREAK);

		for (int k = 0; k < count; k++) {
			printCirclesByCode(cir[k]);
		}
	}
}


int mouse(Graph g) {
	MOUSEMSG msg;
	RECT rect1 = { 100, 290, 300, 340 };
	RECT rect2 = { 100, 360, 300, 410 };
	RECT rect3 = { 100, 643, 190, 693 };
	RECT rect4 = { 210, 643, 300, 693 };
	RECT rect5 = { 50, 430, 350, 620 };
	int radius = 20;

	LOGFONT f1;
	gettextstyle(&f1);
	f1.lfHeight = 30;
	_tcscpy(f1.lfFaceName, _T("幼圆"));
	f1.lfQuality = ANTIALIASED_QUALITY;
	settextstyle(&f1);
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	f1.lfHeight = 35;
	settextstyle(&f1);

	int start, end;
	char s[5];

	while (true) {
		start = -1, end = -1;
		msg = GetMouseMsg();

		switch (msg.uMsg) {
		case WM_MOUSEMOVE:
			if (msg.x >= rect1.left && msg.x <= rect1.right && msg.y >= rect1.top && msg.y <= rect1.bottom) {
				setfillcolor(RGB(229, 247, 255));
				fillroundrect(rect1.left, rect1.top, rect1.right, rect1.bottom, radius, radius);
				f1.lfHeight = 30;
				settextstyle(&f1);
				settextcolor(BLACK);
				outtextxy(110, 300, "查询景点信息");
			}
			else if (msg.x >= rect2.left && msg.x <= rect2.right && msg.y >= rect2.top && msg.y <= rect2.bottom) {
				setfillcolor(RGB(229, 247, 255));
				fillroundrect(rect2.left, rect2.top, rect2.right, rect2.bottom, radius, radius);
				f1.lfHeight = 30;
				settextstyle(&f1);
				settextcolor(BLACK);
				outtextxy(110, 370, "查询最短路径");
			}
			else if (msg.x >= rect3.left && msg.x <= rect3.right && msg.y >= rect3.top && msg.y <= rect3.bottom) {
				setfillcolor(RGB(229, 247, 255));
				fillroundrect(rect3.left, rect3.top, rect3.right, rect3.bottom, radius, radius);
				f1.lfHeight = 35;
				settextstyle(&f1);
				settextcolor(BLACK);
				outtextxy(110, 650, "重置");
			}
			else if (msg.x >= rect4.left && msg.x <= rect4.right && msg.y >= rect4.top && msg.y <= rect4.bottom) {
				setfillcolor(RGB(229, 247, 255));
				fillroundrect(rect4.left, rect4.top, rect4.right, rect4.bottom, radius, radius);
				f1.lfHeight = 35;
				settextstyle(&f1);
				settextcolor(BLACK);
				outtextxy(220, 650, "退出");
			}
			else {
				setfillcolor(WHITE);
				fillroundrect(rect1.left, rect1.top, rect1.right, rect1.bottom, radius, radius);
				fillroundrect(rect2.left, rect2.top, rect2.right, rect2.bottom, radius, radius);
				fillroundrect(rect3.left, rect3.top, rect3.right, rect3.bottom, radius, radius);
				fillroundrect(rect4.left, rect4.top, rect4.right, rect4.bottom, radius, radius);
				f1.lfHeight = 30;
				settextstyle(&f1);
				settextcolor(BLACK);
				outtextxy(110, 300, "查询景点信息");
				outtextxy(110, 370, "查询最短路径");
				f1.lfHeight = 35;
				settextstyle(&f1);
				outtextxy(110, 650, "重置");
				outtextxy(220, 650, "退出");
			}
			break;

		case WM_LBUTTONDOWN:
			for (int i = 0; i <= 10; i++) {
				setlinecolor(RGB(25 * i, 25 * i, 25 * i));
				circle(msg.x, msg.y, 2 * i);
				Sleep(20);
				setlinecolor(getbkcolor());
				circle(msg.x, msg.y, 2 * i);
			}

			if (msg.x >= rect1.left && msg.x <= rect1.right && msg.y >= rect1.top && msg.y <= rect1.bottom) {
				InputBox(s, 5, "输入想查询的景点编号");
				sscanf(s, "%d", &start);
				settextstyle(&f1);
				if (start >= 0 && start <= 9) {
					page();
					showSpotInfo(&g, start);
				}
				else {
					page();
					setfillcolor(WHITE);
					fillroundrect(rect5.left, rect5.top, rect5.right, rect5.bottom, radius, radius);
					settextcolor(RGB(87, 176, 187));
					drawtext("请输入有效值", &rect5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
			}
			else if (msg.x >= rect2.left && msg.x <= rect2.right && msg.y >= rect2.top && msg.y <= rect2.bottom) {
				InputBox(s, 5, "输入起点和终点编号（空格分开）");
				sscanf(s, "%d%d", &start, &end);
				if (start >= 0 && start <= 9 && end >= 0 && end <= 9) {
					page();
					dijkstra(&g, start, end);
				}
				else {
					page();
					setfillcolor(WHITE);
					fillroundrect(rect5.left, rect5.top, rect5.right, rect5.bottom, radius, radius);
					settextcolor(RGB(87, 176, 187));
					drawtext("请输入有效值", &rect5, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}
			}
			else if (msg.x >= rect3.left && msg.x <= rect3.right && msg.y >= rect3.top && msg.y <= rect3.bottom) {
				page();
			}
			else if (msg.x >= rect4.left && msg.x <= rect4.right && msg.y >= rect4.top && msg.y <= rect4.bottom) {
				return 0;
			}
			break;
		}
	}

	system("pause");
}

void freeGraph(Graph* g) {
	// 当前 Graph 使用静态数组，不存在需要 free 的动态内存。
	if (g == NULL) return;
	g->numSpots = 0;
	for (int i = 0; i < MAX; i++) {
		for (int j = 0; j < MAX; j++) {
			g->edges[i][j] = (i == j) ? 0 : INF;
		}
	}
	// 清空 spot 名称等（可选，但有助于完全重置）
	for (int i = 0; i < MAX; i++) {
		g->spots[i].name[0] = '\0';
		g->spots[i].code[0] = '\0';
		g->spots[i].description[0] = '\0';
	}
}

int main() {
	Graph g;
	initGraph(&g);

	// 添加景点
	addSpot(&g, "校门", "0", "校园正门，迎接八方来客");
	addSpot(&g, "图书馆", "1", "丰富的藏书和安静的学习环境");
	addSpot(&g, "教学楼A", "2", "主要的教学楼之一");
	addSpot(&g, "教学楼B", "3", "多功能教室集中地");
	addSpot(&g, "行政楼", "4", "学校行政部门所在地");
	addSpot(&g, "体育馆", "5", "大型体育赛事场所");
	addSpot(&g, "学生宿舍", "6", "学生住宿区");
	addSpot(&g, "食堂", "7", "师生就餐的地方");
	addSpot(&g, "实验楼", "8", "实验和科研的主要场所");
	addSpot(&g, "文化广场", "9", "师生休闲和举办活动的广场");

	// 添加路径
	addEdge(&g, 0, 9, 400);
	addEdge(&g, 0, 4, 400);
	addEdge(&g, 4, 5, 200);
	addEdge(&g, 5, 8, 800);
	addEdge(&g, 0, 3, 600);
	addEdge(&g, 3, 2, 400);
	addEdge(&g, 6, 9, 820);
	addEdge(&g, 6, 7, 300);
	addEdge(&g, 6, 8, 980);
	addEdge(&g, 7, 9, 1080);
	addEdge(&g, 7, 2, 720);
	addEdge(&g, 1, 9, 500);
	addEdge(&g, 2, 9, 600);
	addEdge(&g, 2, 8, 1200);
	addEdge(&g, 3, 9, 600);
	addEdge(&g, 8, 9, 1000);

	initgraph(1280, 800);
	page();
	mouse(g);
	closegraph();

	freeGraph(&g);

	return 0;
}
