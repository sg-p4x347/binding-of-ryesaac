#include "pch.h"

#include "Position.h"

namespace world {
	Position::Position() {}
	Position::Position(Vector3 position, Vector3 rotation) : Pos(position), Rot(rotation) {}
	M4 Position::GetTransform()
	{
		return math::CreateTranslation(Pos) * math::CreateRotation(Rot);
	}
}