#include <graphics.h>
#include <stdio.h>
#include <graphics.h>
#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

int main(void)
{
   int gdriver = DETECT, gmode, errorcode;
   int x,y,dx,dy;
   registerbgidriver(EGAVGA_driver);
   initgraph(&gdriver, &gmode, "");
   errorcode = graphresult();
   if (errorcode != grOk)
   {
	  printf("Graphics error: %s\n", grapherrormsg(errorcode));
	  printf("Press any key to halt:");
	  getch();
	  exit(1);
   }
   setfillstyle(1,RED);
   bar(100,100,600,400);
   x=200;y=200;dx=100;dy=100;
//   setfillstyle(1,YELLOW);
//   bar(x-1,y-1,x+dx+1,y+dy+1);
	setfillstyle(SOLID_FILL,LIGHTGRAY);
	bar(x,y,x+dx,y+dy);
	setcolor(BLACK);
	moveto(x,y);
	linerel(dx,0);
	linerel(0,dy);
	linerel(-dx,0);
	linerel(0,-dy);
	setcolor(DARKGRAY);
	moveto(x+dx-1,x+dx-1);
	linerel(0,2-dy);
	moveto(x+dx-1,x+dx-1);
	linerel(2-dx,0);
	setcolor(WHITE);
	moveto(x+1,y+1);
	linerel(dx-2,0);
	moveto(x+1,y+1);
	linerel(0,dy-2);



   getch();
   closegraph();
   return 0;
}
