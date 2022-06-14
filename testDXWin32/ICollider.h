#pragma once
#include <DirectXMath.h>
#include "IComponent.h"

using namespace DirectX;

class ICollider :
	public IComponent
{
public:
	ICollider();
	ICollider(IEntity* e);
	~ICollider();

	virtual void update();
	virtual bool Collide(ICollider* rhs) = 0;

};

