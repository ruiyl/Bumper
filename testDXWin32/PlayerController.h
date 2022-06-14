#pragma once
#include "IController.h"
#include "InputHandler.h"

class PlayerController :
	public IController
{
public:
	PlayerController();
	PlayerController(IEntity* e, InputHandler* input);
	~PlayerController();

	void update(float elapsedTime);

private:
	InputHandler * m_pInput;
};

