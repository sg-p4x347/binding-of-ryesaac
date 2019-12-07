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

void MultimediaPlayer::startAudio()
{
	if (m_playASync && m_playLoop)
	{
		PlaySound(getFilePath().c_str(), // The filepath of the sound file
			NULL, // Only not NULL if SND_RESOURCE is specified in the following parameters
			SND_FILENAME // Specifies that the first parameter is a filename
			| SND_ASYNC // Specifies asynchronous playing of the audio
			| SND_LOOP); // Specifies audio should loop
	}
	else if (m_playASync && !m_playLoop)
	{
		PlaySound(getFilePath().c_str(), // The filepath of the sound file
			NULL, // Only not NULL if SND_RESOURCE is specified in the following parameters
			SND_FILENAME // Specifies that the first parameter is a filename
			| SND_ASYNC); // Specifies asynchronous playing of the audio
	}
	else if (!m_playASync && m_playLoop)
	{
		PlaySound(getFilePath().c_str(), // The filepath of the sound file
			NULL, // Only not NULL if SND_RESOURCE is specified in the following parameters
			SND_FILENAME // Specifies that the first parameter is a filename
			| SND_LOOP); // Specifies audio should loop
	}
}

void MultimediaPlayer::stopAudio()
{
	PlaySound(NULL, 0, 0);
}
