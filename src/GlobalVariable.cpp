#include "GlobalVariable.h"
#include "FilePath.h"

namespace Global {
	/*  测试  */

	bool DrawLinesMode = false;
	bool MistSwitch = true;

	int GamePlayerX;   
	int GamePlayerY;

	bool GameResourceLoadingBool = false;
	bool GameResourceUninstallBool = false;


	/*  测试  */

	bool ConsoleBool = false;
	extern unsigned int CommandBufferSize = 0;
	bool* MainCommandBufferS = nullptr;
	void MainCommandBufferUpdateRequest() {
		for (size_t i = 0; i < CommandBufferSize; ++i)
		{
			MainCommandBufferS[i] = true;
		}
	}

	bool MultiplePeopleMode = false;
	bool ServerOrClient = true;

	unsigned int mWidth;
	unsigned int mHeight;

	int ServerPort;
	int ClientPort;
	std::string ClientIP;

	bool VulKanValidationLayer;
	bool Monitor;
	bool MonitorCompatibleMode;
	bool FullScreen;
	float MusicVolume;
	float SoundEffectsVolume;

	unsigned char KeyW;
	unsigned char KeyS;
	unsigned char KeyA;
	unsigned char KeyD;
}