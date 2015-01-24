#include <stdio.h>
#include <Windows.h>
#pragma comment(lib, "Winmm.lib")

int PlayMusic(const char *NameOfFile)
{
	if(!PlaySound(NameOfFile, 0, SND_ASYNC))// SND_ASYNC - продолжать выполнение
	{
		printf ("Error occured!");
		return -1;
	}
	return 0;
}

int StopMusic()
{
	PlaySound(0, 0, 0);
}