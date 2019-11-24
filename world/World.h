#pragma once
#include "pch.h"

#include "math/Vector2.h"
using math::Vector2;

#include "ecs/EntityRepository.h"
using ecs::EntityRepository;

#include "Position.h"
#include "Model.h"

namespace world {
	class World
	{
	public:
		World();
		void Generate();
		void Render();
		void Update(double elapsed);
		void UpdateKeyState(char key, bool state);
		void UpdateMousePosition(Vector2 position);
	private:
		map<char, bool> m_keys;
		EntityRepository<
			Position,
			Model
		> ER;
	};
}
