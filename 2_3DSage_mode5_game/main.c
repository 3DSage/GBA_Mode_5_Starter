//--------------------------By:3DSage-------------------------------------------
//               https://www.youtube.com/c/3DSage
//------------------------------------------------------------------------------

//---#defines---
#include "gba.h"
//---Math functions---
#include <math.h> 
//---Load textures---
#include "textures/player_1.c"
#include "textures/player_2.c"
#include "textures/ball.c"
#include "textures/end.c"
#include "textures/title.c"

//---Global variables---
#define  GBA_SW 160                                        //actual gba screen width
#define  SW     120                                        //game screen width
#define  SH      80                                        //game screen height
#define RGB(r,g,b) ((r)+((g)<<5)+((b)<<10))                //15 bit, 0-31, 5bit=r, 5bit=g, 5bit=b 
int lastFr=0,FPS=0;                                        //for frames per second
int gameState=0;                                           //game state, title, game, ending
int endScreenTimer=0;                                      //time in second to hold on the ending

typedef struct                                             //player
{
 int x,y;                                                  //position
 int eat;                                                  //eat when moving
 const u16* map;                                           //point to current map
}Player; Player P;

typedef struct                                             //ball
{
 int x,y;                                                  //position
 int rot;                                                  //rotate 0-360
 int frame;                                                //4 frames to animate
}Ball; Ball B;

void clearBackground()
{int x,y;
 for(x=0;x<SW;x++)
 {                                                         //rgb values 0-31
  for(y=0;y<SH;y++){ VRAM[y*GBA_SW+x]=RGB(8,12,16);}       //clear all 120x80 pixels
 }
}

void buttons()                                             //buttons to press
{
 P.eat=0;                                                  //not moving, turn off eating
 if(KEY_R ){ P.eat=1; P.x+=2; if(P.x>SW-16){ P.x=SW-16;}}  //move right
 if(KEY_L ){ P.eat=1; P.x-=2; if(P.x<    0){ P.x=    0;}}  //move left
 if(KEY_U ){ P.eat=1; P.y-=2; if(P.y<    0){ P.y=    0;}}  //move up
 if(KEY_D ){ P.eat=1; P.y+=2; if(P.y>SH-16){ P.y=SH-16;}}  //move down
 if(KEY_A ){ } 
 if(KEY_B ){ } 
 if(KEY_LS){ } 
 if(KEY_RS){ } 
 if(KEY_ST){ } 
 if(KEY_SL){ } 
}

IN_IWRAM void drawImage(int w,int h, int xo,int yo, const u16* map, int to)
{int x,y,c;          //image w/h, position offset,  texture name, texture offset
 for(x=0;x<w;x++) 
 {                
  for(y=0;y<h;y++){ c=map[(y+to*h)*w+x]; if(c>0){ VRAM[(y+yo)*GBA_SW+x+xo]=c;}} 
 }
}

void gameUpdates()
{
 //player eat animation when moving
 if(P.eat==1 && FPS%4==0){ P.map=player_2_Map;} else{ P.map=player_1_Map;}
 //ball animate image
 B.frame+=1; if(B.frame>3){ B.frame=0;}
 //ball animate position
 B.x=((LUT_Sin[B.rot])>>4)+90;
 B.y=((LUT_Cos[B.rot])>>4)+50;
 B.rot+=10; if(B.rot>359){ B.rot-=360;} 
 //ball collision detection, end game
 if(B.x<P.x+8+5 && B.x>P.x+8-5 && B.y<P.y+8+5 && B.y>P.y+8-5){ gameState=2;}	 
}

//---MUSIC----------------------------------------------------------------------
typedef struct
{
 u16* song;
 int tic;
 int spd;
 int size;
 int onOff;
}Music; Music M[3];

u16 notes[] = 
{
   44, 157,  263, 363,  458,  547, 631,  711, 786,  856, 923,  986,//C2,C2#, D2,D2#, E2, F2,F2#, G2,G2#, A2,A2#, B2 
 1046,1102, 1155,1205, 1253, 1297,1340, 1379,1417, 1452,1486, 1517,//C3,C3#, D3,D3#, E3, F3,F3#, G3,G3#, A3,A3#, B3 	
 1547,1575, 1602,1627, 1650, 1673,1694, 1714,1732, 1750,1767, 1783,//C4,C4#, D4,D4#, E4, F4,F4#, G4,G4#, A4,A4#, B4
 1798,1812, 1825,1837, 1849, 1860,1871, 1881,1890, 1899,1907, 1915,//C5,C5#, D5,D5#, E5, F5,F5#, G5,G5#, A5,A5#, B5
 1923,1930, 1936,1943, 1949, 1954,1959, 1964,1969, 1974,1978, 1982,//C6,C6#, D6,D6#, E6, F6,F6#, G6,G6#, A6,A6#, B6
 1985,1989, 1992,1995, 1998, 2001,2004, 2006,2009, 2011,2013 ,2015,//C7,C7#, D7,D7#, E7, F7,F7#, G7,G7#, A7,A7#, B7
};

u16 song_1[]={ 10,0,10, 0, 3,2,3,2, 7,22,0,8, 9,24,0,12};          //title song
u16 song_2[]={ 2,2,0,0, 3,7,5,0, 4,0,4,0, 4,4,6,10};               //game  song
u16 song_3[]={ 60,58,56,54, 52,50,48,46, 44,42,40,38, 36,34,32,30};//end   song

void playSong(int s, int loop)
{
 if(FPS%M[s].spd==0 && M[s].onOff==1) 
 {
  int note=M[s].song[M[s].tic];
  if(note>0){ PlayNote(notes[note],64);}
  M[s].tic+=1; if(M[s].tic>M[s].size){ M[s].tic=0; if(loop==0){ M[s].onOff=0;}}
 }
}//-----------------------------------------------------------------------------

void init()
{
 P.x=20; P.y=15; P.eat=0; P.map=player_1_Map;                        //init player
 B.x= 0; B.y= 0; B.rot=0; B.frame=0;                                 //init ball
 endScreenTimer=0;                                                   //clear timer
 //init music
 M[0].song=song_1; M[0].spd=4; M[0].tic=0; M[0].size=15; M[0].onOff=1;
 M[1].song=song_2; M[1].spd=2; M[1].tic=0; M[1].size=15; M[1].onOff=1;
 M[2].song=song_3; M[2].spd=1; M[2].tic=0; M[2].size=15; M[2].onOff=1;
}

int main()
{int x,y; 
 //Init mode 5------------------------------------------------------------------
 *(u16*)0x4000000 = 0x405;                                           //mode 5 background 2
 *(u16*)0x400010A = 0x82;                                            //enable timer for fps
 *(u16*)0x400010E = 0x84;                                            //cnt timer overflow

 //scale small mode 5 screen to full screen-------------------------------------
 REG_BG2PA=256/2;                                                    //256=normal 128=scale 
 REG_BG2PD=256/2;                                                    //256=normal 128=scale 
  
 init();                                                             //init game variables

 while(1) 
 { 
  if(REG_TM2D>>12!=lastFr)                                           //draw 15 frames a second
  {  
   if(gameState==0)  //title screen--------------------------------------------- 
   {  
    playSong(0,1);                                                   //play title song
    drawImage(120,80, 0,0, title_Map, 0);                            //draw title screen
    if(KEY_STATE != 0x03FF){ init(); gameState=1;}                   //any button pressed
   } 

   if(gameState==1)  //play the game-------------------------------------------- 
   {
    clearBackground();                                               //clear the background
    playSong(1,1);                                                   //play game song
    gameUpdates();                                                   //animate and collision
    buttons();                                                       //Buttons pressed  
    drawImage(16,16, P.x,P.y, P.map, 0);                             //draw player
    drawImage( 8, 6, B.x,B.y, ball_Map, B.frame);                    //draw ball
   } 

   if(gameState==2)  //end screen-----------------------------------------------
   { 
    playSong(2,0);                                                   //play end song once
    drawImage(120,80, 0,0, end_Map, 0);                              //draw end screen
    endScreenTimer+=1; if(endScreenTimer>15*3){ gameState=0;}        //hold for 3 seconds
   }  	

   //frames per second---------------------------------------------------------- 
   //VRAM[15]=0; VRAM[FPS]=RGB(31,31,0);                               //draw fps 
   FPS+=1; if(lastFr>REG_TM2D>>12){ FPS=0;}                          //increase frame
   lastFr=REG_TM2D>>12;                                              //reset counter

   //swap buffers---------------------------------------------------------------
   while(*Scanline<160){}	                                         //wait all scanlines 
   if  ( DISPCNT&BACKB){ DISPCNT &= ~BACKB; VRAM=(u16*)VRAM_B;}      //back  buffer
   else{                 DISPCNT |=  BACKB; VRAM=(u16*)VRAM_F;}      //front buffer  
  }
 }
}

