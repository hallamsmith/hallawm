#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WG 10
#define TG 40
#define BG 60
#define PW 1600
#define PH 1000
// A window with an ID, height, width, top left x and y postition
typedef struct Window {
	char winID[11];
	int h, w, x, y;
} Window;

typedef struct ListOfWindows {
	char** list;
	int listlen;
} ListOfWindows;

typedef struct Section {
	int h, w, x, y;
} Section;

typedef struct Layout {
	Section primary, secondary, tertiary;	
} Layout;

// Get the root windows ID, width and height
Window getRoot()
{
	FILE *ROOTP, *RWP, *RHP;
	char ROOT[11], RW[5], RH[5];
	int rw, rh;

	ROOTP = popen("lsw -r", "r");
	RWP = popen("lsw -r | xargs wattr w", "r");
	RHP = popen("lsw -r | xargs wattr h", "r");

	fgets(ROOT, sizeof(ROOT), ROOTP);
	fgets(RW, sizeof(RW), RWP);
	fgets(RH, sizeof(RH), RHP);
	rw = atoi(RW);
	rh = atoi(RH);

	pclose(ROOTP);
	pclose(RWP);
	pclose(RHP);

	Window root;
	strcpy(root.winID, ROOT);
	root.h = rh;
	root.w = rw;

	return root;
}

ListOfWindows getListOfWindows()
{
	FILE *LISTOFWINDOWSP;
	char LISTOFWINDOWS[11];
	char **windows;
	int numwin = 0;
	const int windowcount = 30;
	ListOfWindows low;
	windows = malloc(sizeof(char*) * windowcount);
	if (!windows)
	{	
		perror("malloc failed on windows");
		abort();
	}
	memset(windows, 0, sizeof(char*)*windowcount);
	LISTOFWINDOWSP = popen("lsw", "r");
	while (fgets(LISTOFWINDOWS, sizeof(LISTOFWINDOWS), LISTOFWINDOWSP) != NULL)
	{
		if (strlen(LISTOFWINDOWS) > 1)
		{
			numwin++;
			windows[numwin] = malloc(12);
			strcpy(windows[numwin], LISTOFWINDOWS);			
		}
	}
	low.listlen = numwin;
	low.list = windows;
	pclose(LISTOFWINDOWSP);
	return low;
}

int putWindow(char winID[11], int x, int y, int w, int h)
{
	FILE *WPLACEP;
	char cmd[40];
	sprintf(cmd, "wtp %d %d %d %d %s", x, y, w, h, winID);	
	WPLACEP = popen(cmd, "r");
	pclose(WPLACEP);
	return 0;
}

int placePrimary(ListOfWindows fullList, Layout layout)
{
	Layout main = layout;
	ListOfWindows list = fullList;
	int x, y, w, h;
	x = main.primary.x;
	y = main.primary.y;
	w = main.primary.w;
	h = main.primary.h;
	if (list.listlen < 2)
	{
		w = main.primary.w + main.secondary.w;
	}
	if (list.listlen <= 4)
	{
		h = main.primary.h + main.tertiary.h;
	}
	putWindow(list.list[1], x, y, w, h);
	return 0;
}

int placeSecondary(ListOfWindows fullList, Layout layout)
{
	ListOfWindows list = fullList;
	Layout main = layout;
	int x, y, w, h;
	switch (list.listlen)
	{
		case 0:
			break;
		case 1:
			break;
		case 2:
			h = main.secondary.h;
			break;
		case 3:
			h = main.secondary.h / 2;
			break;
		default:
			h = main.secondary.h / 3;
	}
	w = main.secondary.w;
	x = main.secondary.x;
	y = main.secondary.y;
	int i;
	for (i = 0; i<5; i++)
	{
		int ya;
		if (( i == 0 ) || (i == 1) )
		{
			continue;
		}
		ya = y + (h * (i - 2));
		putWindow(list.list[i], x, ya, w, h);
		if ((list.listlen < 3) && (i == 2))
		{
			break;
		}
		
	}
	return 0;
}

int placeTertiary(ListOfWindows fulllist, Layout layout)
{
	ListOfWindows list = fulllist;
	Layout main = layout;
	int terCount, x, y, w, h;

	if (list.listlen <= 4) {
		return 0;
	}

	terCount = list.listlen - 4;
	switch (terCount)
	{
		case 1:
			x = main.tertiary.x;
			y = main.tertiary.y;
			w = main.tertiary.w;
			h = main.tertiary.h;
			putWindow(list.list[5], x, y, w, h);
			break;
		case 2:
			x = main.tertiary.x;
			y = main.tertiary.y;
			w = main.tertiary.w / 2 + 10;
			h = main.tertiary.h;
			putWindow(list.list[5], x, y, w, h);
			x = main.tertiary.w / 2;
			putWindow(list.list[6], x, y, w, h);
			break;
		case 3:
			x = main.tertiary.x;
			y = main.tertiary.y;
			w = main.tertiary.w / 2 + 10;
			h = main.tertiary.h / 2;
			putWindow(list.list[5], x, y, w, h);
			x = main.tertiary.w / 2;
			putWindow(list.list[6], x, y, w, h);
			x = main.tertiary.x;
			y = main.tertiary.y + (main.tertiary.h / 2);
			w = main.tertiary.w;
			putWindow(list.list[7], x, y, w, h);
			break;
		case 4:
			x = main.tertiary.x;
			y = main.tertiary.y;
			w = main.tertiary.w / 2;
			h = main.tertiary.h / 2;
			putWindow(list.list[5], x, y, w, h);
			x = (main.tertiary.w / 2) + WG;
			putWindow(list.list[6], x, y, w, h);
			x = main.tertiary.x;
			y = main.tertiary.y + (main.tertiary.h / 2);
			w = main.tertiary.w / 2;
			putWindow(list.list[7], x, y, w, h);
			x = (main.tertiary.w / 2) + WG;
			putWindow(list.list[8], x, y, w, h);
			break;
		case 5:
			x = main.tertiary.x;
			y = main.tertiary.y;
			w = main.tertiary.w / 3;
			h = main.tertiary.h / 2;
			putWindow(list.list[5], x, y, w, h);
			x = (main.tertiary.w / 3) + WG;
			putWindow(list.list[6], x, y, w, h);
			x = 2 * (main.tertiary.w / 3) + WG;
			w = main.tertiary.w / 3;
			putWindow(list.list[7], x, y, w, h);
			x = main.tertiary.x;
			y = main.tertiary.y + ( main.tertiary.h / 2 );
			w = main.tertiary.w / 2;
			putWindow(list.list[8], x, y, w, h);
			x = main.tertiary.w / 2 + WG;
			putWindow(list.list[9], x, y, w, h);
			break;
		default:
			x = main.tertiary.x;
			y = main.tertiary.y;
			w = main.tertiary.w / 3;
			h = main.tertiary.h / 2;
			putWindow(list.list[5], x, y, w, h);
			x = (main.tertiary.w / 3) + WG;
			putWindow(list.list[6], x, y, w, h);
			x = 2 * (main.tertiary.w / 3) + WG;
			w = main.tertiary.w / 3;
			putWindow(list.list[7], x, y, w, h);
			x = main.tertiary.x;
			y = main.tertiary.y + ( main.tertiary.h / 2 );
			w = main.tertiary.w / 3;
			putWindow(list.list[8], x, y, w, h);
			x = main.tertiary.w / 3 + WG;
			putWindow(list.list[9], x, y, w, h);
			x = 2 * (main.tertiary.w / 3) + WG;
			putWindow(list.list[10], x, y, w, h);
	}
	printf("%d", list.listlen);
	if (list.listlen > 10)
	{
		for (int i = 11; i <= list.listlen; i++)
		{
			FILE *CMD;
			char cmd[40];
			sprintf(cmd, "mapw -u %s", list.list[i]);
			printf("%s", list.list[i]);
			CMD = popen(cmd, "r");	
			pclose(CMD);
			//printf("TEST");
		}
	}
}

int main()
{
	Window root;
	ListOfWindows low;
	root = getRoot();

	Layout main;
	main.primary.w = PW;
	main.primary.h = PH;
	main.primary.x = WG;
	main.primary.y = TG;
	main.secondary.w = root.w - PW - WG - WG;
	main.secondary.h = root.h - TG - BG;
	main.secondary.x = WG + PW;
	main.secondary.y = TG;
	main.tertiary.h = root.h - main.primary.h - TG - BG; 
	main.tertiary.w = main.primary.w;
	main.tertiary.x = WG;
	main.tertiary.y = PH + TG;

	low = getListOfWindows();	

	placePrimary(low, main);
	placeSecondary(low, main);
	placeTertiary(low, main);

	return 0;
}