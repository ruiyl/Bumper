#pragma once
#include "IComponent.h"
#include "InputHandler.h"

class IController :
	public IComponent
{
public:
	IController();
	virtual ~IController();
	virtual void update(float elapsedTime) = 0;
};

