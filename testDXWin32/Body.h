#pragma once
#include "IComponent.h"

using namespace DirectX;
class Body :
	public IComponent
{
	friend class PlayerController;
	friend class NetworkController;
public:
	Body();
	Body(IEntity* e);
	~Body();

	void update(float elapsedTime);
	void move(float elapsedTime);
	void afterMove(float elapsedTime);
	void collide(Body* other);

	XMFLOAT3		getPosition() const		{ return m_position; }
	XMFLOAT3		getRotation() const		{ return m_rotation; }
	XMFLOAT3		getVelocity() const		{ return m_velocity; }
	XMFLOAT3		getAcceleration() const { return m_acceleration; }
	XMFLOAT3		getScale() const		{ return m_scale; }

	Utility::EntityState getState();

	void					setPosition(float x, float y, float z)						{ m_position = { x,y,z }; }
	void					setRotation(float rotX, float rotY, float rotZ)				{ m_rotation = { rotX,rotY,rotZ }; }
	void					setVelocity(float x, float y, float z)						{ m_velocity = { x,y,z }; }
	void					setAcceleration(float x, float y, float z)					{ m_acceleration = { x,y,z }; }
	void					setScale(float scaleX, float scaleY, float scaleZ)			{ m_scale = { scaleX,scaleY,scaleZ }; }
	void					setSpeed(float speed)										{ m_speed = speed;}
	void					setTurnSpeed(float turnSpeed)								{ m_turnSpeed = turnSpeed; }

private:
	XMFLOAT3							m_position;
	XMFLOAT3							m_rotation;
	XMFLOAT3							m_scale;
	XMFLOAT3							m_lastMove = { 0.0f,0.0f,0.0f };
	XMFLOAT3							m_lastRotation = { 0.0f,0.0f,0.0f };
	XMFLOAT3							m_velocity = { 0.0f,0.0f,0.0f };
	XMFLOAT3							m_acceleration = { 0.0f,0.0f,0.0f };
	float								m_speed = 50.0f;
	float								m_turnSpeed = 2.0f;
	float								m_mass = 1.0f;

	bool								m_isColliding = false;
};

