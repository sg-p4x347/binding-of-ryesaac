#pragma once
#include "pch.h"

#include "ecs/Component.h"

struct Collision :
	public ecs::Component
{
	struct Contact {
		ecs::EntityID Collider;
	};
	vector<Contact> Contacts;
};

