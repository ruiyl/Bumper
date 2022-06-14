#include "pch.h"
#include "ICollider.h"
#include "IEntity.h"
#include <vector>


ICollider::ICollider()
{
}

ICollider::ICollider(IEntity * e)
{
	m_pEntity = e;
}


ICollider::~ICollider()
{
}

void ICollider::update()
{	
}
