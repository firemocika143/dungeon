#include"sound.h"
#include<windows.h>
#include<mmsystem.h>
using namespace std;

void playMenu()
{
	bool startMusic = true;
    PlaySound(TEXT("C:\\Users\\user\\Desktop\\nightmare\\main.wav"), GetModuleHandle(NULL), SND_LOOP | SND_ASYNC);
	
	return;
}
void playBattle()
{
	bool startMusic = true;
    PlaySound(TEXT("C:\\Users\\user\\Desktop\\nightmare\\chaos.wav"), GetModuleHandle(NULL), SND_LOOP | SND_ASYNC);
	
	return;
}
void playWin()
{
	bool startMusic = true;
    PlaySound(TEXT("C:\\Users\\user\\Desktop\\nightmare\\winning.wav"), GetModuleHandle(NULL), SND_LOOP | SND_ASYNC);
	
	return;
}
void playLose()
{
	bool startMusic = true;
    PlaySound(TEXT("C:\\Users\\user\\Desktop\\nightmare\\lose.wav"), GetModuleHandle(NULL), SND_LOOP | SND_ASYNC);
	
	return;
}
