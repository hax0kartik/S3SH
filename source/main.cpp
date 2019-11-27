#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <3ds.h>
#include "ui.hpp"
#include "ssh.hpp"
uiFuncs_s ui;
/* Main function */
int main(int argc, char *argv[]) 
{
//	ui.debug = true;
	APT_SetAppCpuTimeLimit(30);
	threadCreate((ThreadFunc)&uiThread, nullptr, 0x1000, 0x28, 1, true);
	svcSleepThread(1e+9);
	ssh ssho;
	ssho.init();
	ssho.mainLoop();
	ssho.deinit();

	while(aptMainLoop())
	{
		if(keysDown() & KEY_START) break;
	}
	return 0;
}