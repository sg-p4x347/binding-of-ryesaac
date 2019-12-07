#pragma once
#include "pch.h"
#include <Windows.h>
#include <Mmsystem.h>

namespace game
{
	class MultimediaPlayer
	{
	private:
		string m_filePath;
		bool m_playASync;
		bool m_playLoop;

		MultimediaPlayer();

	public:
		static MultimediaPlayer& GetInstance();
		static void SetUp(string filePath, bool playASync, bool playLoop);

		void setFilePath(string filePath);
		string getFilePath();
		void setPlayASync(bool playASync);
		bool getPlayASync();
		void setPlayLoop(bool playLoop);
		bool getPlayLoop();

		void startAudio();
		void stopAudio();
	};
}
