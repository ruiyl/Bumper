#pragma once

#include "pch.h"
#include "IEntity.h"

class IEntity;

class Camera
{
public:
	struct MatrixBuffer
	{
		DirectX::XMMATRIX matrix;
	};

	Camera();
	//Camera(float x, float y, float z);
	Camera(IEntity* target);
	~Camera();

	void update(DX::DeviceResources* resource, InputHandler* input);
	HRESULT initialize(DX::DeviceResources* resource);
	//void setTarget(IEntity* target);
	

	const DirectX::XMVECTOR* getEye() const		{ return &m_eye; }
	const DirectX::XMVECTOR* getAt() const		{ return &m_eye; }
	const DirectX::XMVECTOR* getUp() const		{ return &m_eye; }
	ID3D11Buffer* getViewBuffer() const			{ return m_pViewBuffer; }
	ID3D11Buffer* getProjectionBuffer() const	{ return m_pProjectionBuffer; }

private:
	float m_rollAngle;
	float m_pitchAngle;
	float m_yawAngle;

	DirectX::XMVECTOR						m_eye;
	DirectX::XMVECTOR						m_at;
	DirectX::XMVECTOR						m_up;
	DirectX::XMVECTOR						m_right;
	DirectX::XMMATRIX						m_viewMatrix;
	DirectX::XMMATRIX						m_projectionMatrix;
	ID3D11Buffer*							m_pViewBuffer = nullptr;
	ID3D11Buffer*							m_pProjectionBuffer = nullptr;
	IEntity*								m_pTarget = nullptr;

	void updateBuffer(DX::DeviceResources* resource);
};

