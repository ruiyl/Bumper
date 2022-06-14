#pragma once

#include "Utility.h"

class IEntity;

class IComponent
{
public:
	IComponent();
	virtual ~IComponent();

	IEntity* getEntity() { return m_pEntity; };

protected:
	IEntity * m_pEntity = nullptr;
};

