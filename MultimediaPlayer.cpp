#include "pch.h"
#include "MultimediaPlayer.h"

MultimediaPlayer::MultimediaPlayer()
{
	setFilePath("");
	setPlayASync(false);
	setPlayLoop(false);
}

MultimediaPlayer::MultimediaPlayer(string filePath, bool playASync, bool playLoop)
{
	setFilePath(filePath);
	setPlayASync(playASync);
	setPlayLoop(playLoop);
}

void MultimediaPlayer::setFilePath(string filePath)
{
	m_filePath = filePath;
}

string MultimediaPlayer::getFilePath()
{
	return m_filePath;
}

void MultimediaPlayer::setPlayASync(bool playASync)
{
	m_playASync = playASync;
}

bool MultimediaPlayer::getPlayASync()
{
	return m_playASync;
}

void MultimediaPlayer::setPlayLoop(bool playLoop)
{
	m_playLoop = playLoop;
}

bool MultimediaPlayer::getPlayLoop()
{
	return m_playLoop;
}

void MultimediaPlayer::startMusic()
{
	PlaySound(getFilePath().c_str(), // The filepath of the sound file
		NULL, // Only not NULL if SND_RESOURCE is specified in the following parameters
		SND_FILENAME // Specifies that the first paranmeter is a filename
		| SND_ASYNC // Specifies asynchronous playing of the audio
		| SND_LOOP); // Specifies audio should loop
}
