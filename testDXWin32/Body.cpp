#include "pch.h"
#include "Body.h"
#include "IEntity.h"


Body::Body()
{
}

Body::Body(IEntity * e)
{
	m_pEntity = e;
}


Body::~Body()
{
}

void Body::update(float elapsedTime)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	XMVECTOR velocity = XMLoadFloat3(&m_velocity);
	XMVECTOR acceleration = XMLoadFloat3(&m_acceleration);
	XMVECTOR movement = velocity * elapsedTime;

	//_RPT1(0, "p b %f,%f,%f\n", m_position.x, m_position.y, m_position.z);
	//_RPT1(0, "v b %f,%f,%f\n", XMVectorGetX(velocity), XMVectorGetY(velocity), XMVectorGetZ(velocity));
	position += movement;
	velocity += acceleration * elapsedTime;
	//velocity = XMVector3ClampLength(velocity, 0, 20.0f);

	//_RPT1(0, "v a %f,%f,%f\n", XMVectorGetX(velocity), XMVectorGetY(velocity), XMVectorGetZ(velocity));
	//_RPT1(0, "a %f,%f,%f\n", XMVectorGetX(acceleration), XMVectorGetY(acceleration), XMVectorGetZ(acceleration));

	XMVECTOR front, side;
	XMVector3ComponentsFromNormal(&front, &side, velocity, XMVector3Normalize(acceleration));

	side *= 0.95f;
	velocity = front + side;

	XMStoreFloat3(&m_position, position);
	XMStoreFloat3(&m_velocity, velocity);

	//_RPT1(0, "p a %f,%f,%f\n", m_position.x, m_position.y, m_position.z);
}

void Body::move(float elapsedTime)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	XMVECTOR velocity = XMLoadFloat3(&m_velocity);
	XMVECTOR acceleration = XMLoadFloat3(&m_acceleration);
	XMVECTOR movement = velocity * elapsedTime;

	position += movement;
	velocity += acceleration * elapsedTime;

	XMStoreFloat3(&m_position, position);
	XMStoreFloat3(&m_velocity, velocity);
}

void Body::afterMove(float elapsedTime)
{
	XMVECTOR velocity = XMLoadFloat3(&m_velocity);
	XMVECTOR acceleration = XMLoadFloat3(&m_acceleration);
	XMVECTOR position = XMLoadFloat3(&m_position);
	XMVECTOR front, side;

	XMVector3ComponentsFromNormal(&front, &side, velocity, XMVector3Normalize(acceleration));

	side *= 0.95f;
	velocity = front + side;

	XMStoreFloat3(&m_velocity, velocity);
}

void Body::collide(Body * other)
{
	m_isColliding = true;
	XMVECTOR pos1 = XMLoadFloat3(&m_position);
	XMVECTOR pos2 = XMLoadFloat3(&other->m_position);
	XMVECTOR v1 = XMLoadFloat3(&m_velocity);
	XMVECTOR v2 = XMLoadFloat3(&other->m_velocity);
	XMVECTOR n = XMVector3Normalize(pos2 - pos1);

	//if (XMVectorGetX(XMVector3Dot(v1, v2)) < 0)
	//	return;

	XMVECTOR front1, side1, front2, side2;
	XMVector3ComponentsFromNormal(&front1, &side1, v1, n);
	XMVector3ComponentsFromNormal(&front2, &side2, v2, n);

	_RPT1(0, "v1 %f,%f,%f\n", XMVectorGetX(v1), XMVectorGetY(v1), XMVectorGetZ(v1));
	_RPT1(0, "v2 %f,%f,%f\n", XMVectorGetX(v2), XMVectorGetY(v2), XMVectorGetZ(v2));

	v1 = 0.5f * (front2 - front1) + side1;
	v2 = 0.5f * (front1 - front2) - side2;

	_RPT1(0, "v1 %f,%f,%f\n", XMVectorGetX(v1), XMVectorGetY(v1), XMVectorGetZ(v1));
	_RPT1(0, "v2 %f,%f,%f\n", XMVectorGetX(v2), XMVectorGetY(v2), XMVectorGetZ(v2));
	XMStoreFloat3(&m_velocity, v1);
	XMStoreFloat3(&other->m_velocity, v2);
	//OutputDebugString(TEXT("yea\n"));
}

Utility::EntityState Body::getState()
{
	Utility::EntityState state;

	state.acceleration[0] = m_acceleration.x;
	state.acceleration[1] = m_acceleration.y;
	state.acceleration[2] = m_acceleration.z;

	state.velocity[0] = m_velocity.x;
	state.velocity[1] = m_velocity.y;
	state.velocity[2] = m_velocity.z;

	state.position[0] = m_position.x;
	state.position[1] = m_position.y;
	state.position[2] = m_position.z;

	state.rotation[0] = m_rotation.x;
	state.rotation[1] = m_rotation.y;
	state.rotation[2] = m_rotation.z;

	state.isDestroyed = m_pEntity->isDestroyed();

	return state;
}
