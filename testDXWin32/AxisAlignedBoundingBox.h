#pragma once
#include "ICollider.h"
class AxisAlignedBoundingBox :
	public ICollider
{
public:
	AxisAlignedBoundingBox();
	AxisAlignedBoundingBox(IEntity* e);
	~AxisAlignedBoundingBox();
	
	void update();
	bool Collide(ICollider* rhs);

private:
	XMVECTOR	m_minPoint;
	XMVECTOR	m_maxPoint;
	XMVECTOR	m_boundingBox[8];
};

