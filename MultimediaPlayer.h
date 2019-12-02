#pragma once
#include "pch.h"
#include <Windows.h>
#include <mmsystem.h>
class MultimediaPlayer
{
private:
	string m_filePath;
	bool m_playASync;
	bool m_playLoop;

public:
	MultimediaPlayer();
	MultimediaPlayer(string filePath, bool playASync, bool playLoop);

	void setFilePath(string filePath);
	string getFilePath();
	void setPlayASync(bool playASync);
	bool getPlayASync();
	void setPlayLoop(bool playLoop);
	bool getPlayLoop();

	void startMusic();
};

