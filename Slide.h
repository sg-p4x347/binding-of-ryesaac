#pragma once
#include "pch.h"

class Slide
{
private:
	double m_durationInSec;
	string m_textureName;
public:
	Slide();
	Slide(double durationInSec, string texName);
	
	void setDuration(double dur);
	double getDuration();
	void setTexName(string tex);
	string getTexName();
};

