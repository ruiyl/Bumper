#include "pch.h"
#include "NetworkController.h"
#include "IEntity.h"


NetworkController::NetworkController()
{
}

NetworkController::NetworkController(IEntity * e, ClientGame * manager, int id)
{
	m_pEntity = e;
	m_pClientManager = manager;
	m_id = id;
}


NetworkController::~NetworkController()
{
}

void NetworkController::update(float elapsedTime)
{
	int id = m_id;
	Body* body = m_pEntity->getBody();

	if (!m_useInterpolation)
	{
		if (m_pClientManager->isUpdated[id])
		{
			XMFLOAT3 position = { m_pClientManager->State[id].position[0], m_pClientManager->State[id].position[1], m_pClientManager->State[id].position[2] };
			XMFLOAT3 rotation = { m_pClientManager->State[id].rotation[0], m_pClientManager->State[id].rotation[1], m_pClientManager->State[id].rotation[2] };
			XMFLOAT3 velocity = { m_pClientManager->State[id].velocity[0], m_pClientManager->State[id].velocity[1], m_pClientManager->State[id].velocity[2] };
			XMFLOAT3 acceleration = { m_pClientManager->State[id].acceleration[0], m_pClientManager->State[id].acceleration[1], m_pClientManager->State[id].acceleration[2] };

			body->setPosition(position.x, position.y, position.z);
			body->setRotation(rotation.x, rotation.y, rotation.z);
			body->setVelocity(velocity.x, velocity.y, velocity.z);
			body->setAcceleration(acceleration.x, acceleration.y, acceleration.z);
			m_pEntity->setDestroy(m_pClientManager->State[id].isDestroyed);
			m_pClientManager->isUpdated[id] = false;
			//_RPT1(0, "client %d pp a %f,%f,%f\n", id, body->getPosition().x, body->getPosition().y, body->getPosition().z);
		}
		body->update(elapsedTime);
	}
	else
	{
		if (m_pClientManager->isUpdated[id])
		{
			float lastKnownTime = m_pClientManager->StateAge[id];
			float factor = lastKnownTime / UPDATE_INTERVAL;
			if (factor > 1.0f)
				factor = 1.0f;

			XMVECTOR startPosition = XMLoadFloat3(&body->m_position);
			XMVECTOR startVelocity = XMLoadFloat3(&body->m_velocity);
			XMVECTOR receivedPosition = { m_pClientManager->State[id].position[0], m_pClientManager->State[id].position[1], m_pClientManager->State[id].position[2] };
			XMVECTOR receivedVelocity = { m_pClientManager->State[id].velocity[0], m_pClientManager->State[id].velocity[1], m_pClientManager->State[id].velocity[2] };
			XMVECTOR receivedRotation = { m_pClientManager->State[id].rotation[0], m_pClientManager->State[id].rotation[1], m_pClientManager->State[id].rotation[2] };

			XMVECTOR blendedVelocity = startVelocity + (receivedVelocity - startVelocity)*factor;
			XMVECTOR currentPosition = startPosition + blendedVelocity * elapsedTime;
			XMVECTOR realPosition = receivedPosition + receivedVelocity * lastKnownTime;
			XMVECTOR blendedPosition = currentPosition + (realPosition - currentPosition)*factor;
			
			XMStoreFloat3(&body->m_velocity, blendedPosition - startPosition);
			XMStoreFloat3(&body->m_position, blendedPosition);
			XMStoreFloat3(&body->m_rotation, receivedRotation);
			m_pEntity->setDestroy(m_pClientManager->State[id].isDestroyed);
		}
		else 
		{
			body->update(elapsedTime);
		}
	}
}

void NetworkController::saveStartState()
{
	Body* body = m_pEntity->getBody();
	m_startPosition = body->m_position;
	m_startVelocity = body->m_velocity;
}
