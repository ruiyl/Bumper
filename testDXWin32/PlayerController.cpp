#include "pch.h"
#include "PlayerController.h"
#include "IEntity.h"


PlayerController::PlayerController()
{
}

PlayerController::PlayerController(IEntity * e, InputHandler* input)
{
	m_pEntity = e;
	m_pInput = input;
}


PlayerController::~PlayerController()
{
}

void PlayerController::update(float elapsedTime)
{
	Body* body = m_pEntity->getBody();
	XMFLOAT3 frotation = m_pEntity->getRotation();
	XMINT2 move = m_pInput->getMovement();
	bool boost = m_pInput->getKeyPress(0x10) == InputHandler::PressedState::depressed;

	float turn = XMConvertToDegrees(frotation.y);

	float turnAmount = move.x * body->m_turnSpeed;
	turn += turnAmount;

	if (std::fabs(turn) > 360)
		turn -= (turn > 0) ? 360 : -360;

	XMVECTOR direction = XMVector3Transform({ 0.0f,0.0f,1.0f }, XMMatrixRotationY(XMConvertToRadians(turn)));

	if (boost)
		XMStoreFloat3(&body->m_acceleration, 2.0f * body->m_speed * move.y * direction);
	else 
		XMStoreFloat3(&body->m_acceleration, body->m_speed * move.y * direction);

	body->m_rotation.y = XMConvertToRadians(turn);

	body->update(elapsedTime);
}
