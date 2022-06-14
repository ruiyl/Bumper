#include "pch.h"
#include "CircleCollider.h"
#include "IEntity.h"


CircleCollider::CircleCollider()
{
}

CircleCollider::CircleCollider(IEntity * e, float radius) : ICollider(e)
{
	m_radius = radius;
}


CircleCollider::~CircleCollider()
{
}

bool CircleCollider::Collide(ICollider * rhs)
{
	CircleCollider* _rhs = (CircleCollider*)rhs;
	XMFLOAT3 fpos = m_pEntity->getPosition();
	XMFLOAT3 fpos2 = _rhs->m_pEntity->getPosition();
	XMVECTOR pos = XMLoadFloat3(&fpos);
	XMVECTOR pos2 = XMLoadFloat3(&fpos2);

	float distantSq = XMVectorGetX(XMVector2LengthSq(XMVectorSubtract({ XMVectorGetX(pos),XMVectorGetZ(pos) }, { XMVectorGetX(pos2),XMVectorGetZ(pos2) })));
	float limit = (m_radius + _rhs->getRadius()) * (m_radius + _rhs->getRadius());
	if (distantSq > limit)
		return false;
	else 
		return true;
}
