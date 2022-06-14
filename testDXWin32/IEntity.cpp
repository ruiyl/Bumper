#include "pch.h"
#include "IEntity.h"

IEntity::IEntity()
{
	m_pBody = new Body(this);
	m_pBody->setPosition(0.0f, 0.0f, 0.0f);
	m_pBody->setRotation(0.0f, 0.0f, 0.0f);
	m_pBody->setScale(1.0f, 1.0f, 1.0f);

	m_graphicsComponent = GraphicsComponent(this);
}

IEntity::IEntity(float x, float y, float z, float rotX, float rotY, float rotZ)
{
	m_pBody = new Body(this);
	m_pBody->setPosition(x, y, z);
	m_pBody->setRotation(rotX, rotY, rotZ);
	m_pBody->setScale(1.0f, 1.0f, 1.0f);

	m_graphicsComponent = GraphicsComponent(this);
}

IEntity::IEntity(float x, float y, float z, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ)
{
	m_pBody = new Body(this);
	m_pBody->setPosition(x, y, z);
	m_pBody->setRotation(rotX, rotY, rotZ);
	m_pBody->setScale(scaleX, scaleY, scaleZ);

	m_graphicsComponent = GraphicsComponent(this);
}

void IEntity::onCollision(ICollider * other)
{
	m_pBody->collide(other->getEntity()->getBody());
}
