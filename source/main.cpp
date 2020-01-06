#include <memory>
#include <3ds.h>
#include "ui.hpp"
#include "ssh.hpp"
uiFuncs_s ui;
/* Main function */
int main(int argc, char *argv[]) 
{
	//ui.debug = true;
	APT_SetAppCpuTimeLimit(30);
	aptSetSleepAllowed(false);
	aptSetHomeAllowed(false);
	
	threadCreate((ThreadFunc)&uiThread, nullptr, 0x1000, 0x28, 1, true);
	svcSleepThread(1e+9);
	
	auto ssho = std::make_unique<ssh>();
	ssho->init();
	int e = 0;
	while(aptMainLoop())
	{
		e = ssho->mainLoop();
		if (!e) break;
	}
	return 0;
}