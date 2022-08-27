//--------------------------By:3DSage-------------------------------------------
//               https://www.youtube.com/c/3DSage
//------------------------------------------------------------------------------

//---#defines---
#include "gba.h"
//---Math functions---
#include <math.h> 

//---Global variables---
#define  GBA_SW 160                                        //actual gba screen width
#define  SW     120                                        //game screen width
#define  SH      80                                        //game screen height
#define RGB(r,g,b) ((r)+((g)<<5)+((b)<<10))                //15 bit, 0-31, 5bit=r, 5bit=g, 5bit=b 
int lastFr=0,FPS=0;                                        //for frames per second

typedef struct                                             //player
{
 int x,y;                                                  //position
}Player; Player P;

void clearBackground()
{int x,y;
 for(x=0;x<SW;x++)
 {                                                         //rgb values 0-31
  for(y=0;y<SH;y++){ VRAM[y*GBA_SW+x]=RGB(8,12,16);}       //clear all 120x80 pixels
 }
}

void buttons()                                             //buttons to press
{
 if(KEY_R ){ P.x+=3; if(P.x>SW-1){ P.x=SW-1;}}             //move right
 if(KEY_L ){ P.x-=3; if(P.x<   0){ P.x=   0;}}             //move left
 if(KEY_U ){ P.y-=3; if(P.y<   0){ P.y=   0;}}             //move up
 if(KEY_D ){ P.y+=3; if(P.y>SH-1){ P.y=SH-1;}}             //move down
 if(KEY_A ){ } 
 if(KEY_B ){ } 
 if(KEY_LS){ } 
 if(KEY_RS){ } 
 if(KEY_ST){ } 
 if(KEY_SL){ } 
}

void init()
{
 P.x=70; P.y=35;                                                     //init player
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
   clearBackground();//clear background-----------------------------------------
   buttons();                                                        //button input
   VRAM[P.y*GBA_SW+P.x]=RGB(0,31,0);                                 //draw player dot

   //frames per second---------------------------------------------------------- 
   VRAM[15]=0; VRAM[FPS]=RGB(31,31,0);                               //draw fps 
   FPS+=1; if(lastFr>REG_TM2D>>12){ FPS=0;}                          //increase frame
   lastFr=REG_TM2D>>12;                                              //reset counter

   //swap buffers---------------------------------------------------------------
   while(*Scanline<160){}	                                         //wait all scanlines 
   if  ( DISPCNT&BACKB){ DISPCNT &= ~BACKB; VRAM=(u16*)VRAM_B;}      //back  buffer
   else{                 DISPCNT |=  BACKB; VRAM=(u16*)VRAM_F;}      //front buffer  
  }
 }
}

