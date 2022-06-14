#pragma once
#include "ICollider.h"
class CircleCollider :
	public ICollider
{
public:
	CircleCollider();
	CircleCollider(IEntity* e, float radius);
	~CircleCollider();

	bool Collide(ICollider* rhs);

	float getRadius() const		{ return m_radius; }

private:
	float m_radius;
};

