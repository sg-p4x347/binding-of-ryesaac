#pragma once
#include "pch.h"
#include "Vector2.h"
class World
{
public:
	World();
	void Render();
	void Update(double elapsed);
	void UpdateKeyState(char key, bool state);
	void UpdateMousePosition(Vector2 position);
private:
	map<char, bool> m_keys;
};

