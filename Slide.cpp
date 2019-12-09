#include"pch.h"
#include "Slide.h"

Slide::Slide()
{
}

Slide::Slide(double durationInSec, string texName)
{
	setDuration(durationInSec);
	setTexName(texName);
}

void Slide::setDuration(double dur)
{
	m_durationInSec = dur;
}

double Slide::getDuration()
{
	return m_durationInSec;
}

void Slide::setTexName(string tex)
{
	m_textureName = tex;
}

string Slide::getTexName()
{
	return m_textureName;
}