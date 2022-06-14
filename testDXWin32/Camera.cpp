#include "pch.h"
#include "Camera.h"

using namespace DirectX;
Camera::Camera()
{
	m_eye = { 0.0f, 0.0f, 0.0f };
	m_at = { 0.0f,0.0f,1.0f };
	m_up = { 0.0f, 1.0f, 0.0f };
	m_right = { 1.0f,0.0f,0.0f };

	m_rollAngle = 0.0f;
	m_pitchAngle = 0.0f;
	m_yawAngle = 0.0f;

	m_viewMatrix = XMMatrixLookAtLH(m_eye, m_at, m_up);
	m_projectionMatrix = XMMatrixIdentity();
}

Camera::Camera(IEntity* target)
{
	m_pTarget = target;
	XMFLOAT3 targetPos = m_pTarget->getPosition();
	XMFLOAT3 targetRot = m_pTarget->getRotation();
	XMVECTOR targetPosition = XMLoadFloat3(&targetPos);
	XMVECTOR targetRotation = XMLoadFloat3(&targetRot);

	auto targetRotationMatrix = XMMatrixRotationRollPitchYawFromVector(targetRotation);

	m_eye = { 0.0f,5.0f,-5.0f };
	m_at = { 0.0f,-5.0f,5.0f };
	m_eye = XMVectorSubtract(targetPosition, XMVector3Transform(m_at, targetRotationMatrix));
	m_right = { 1.0f,0.0f,0.0f };
	m_up = { 0.0f,1.0f,0.0f };

	m_at = XMVectorSubtract(targetPosition, m_eye);

	m_rollAngle = 0.0f;
	m_pitchAngle = 0.0f;
	m_yawAngle = 0.0f;

	m_viewMatrix = XMMatrixLookAtLH(m_eye, m_eye + m_at, m_up);
	m_projectionMatrix = XMMatrixIdentity();
}

Camera::~Camera()
{
}

void Camera::update(DX::DeviceResources * resource, InputHandler* input)
{
	if (!m_pTarget->isDestroyed()) {
		XMFLOAT3 targetPos = m_pTarget->getPosition();
		XMFLOAT3 targetRot = m_pTarget->getRotation();
		XMVECTOR targetPosition = XMLoadFloat3(&targetPos);
		XMVECTOR targetRotation = XMLoadFloat3(&targetRot);

		XMFLOAT2 cameraRotation = input->getTurnCamera();
		m_pitchAngle += cameraRotation.y;
		m_yawAngle += cameraRotation.x;

		if (cameraRotation.x == 0) {
			if (m_yawAngle > 0) m_yawAngle--;
			if (m_yawAngle < 0) m_yawAngle++;
		}
		if (cameraRotation.y == 0) {
			if (m_pitchAngle > 0) m_pitchAngle--;
			if (m_pitchAngle < 0) m_pitchAngle++;
		}

		if (std::fabs(m_yawAngle) > 60)
			m_yawAngle = (m_yawAngle >= 0) ? 60.0f : -60.0f;
		if (std::fabs(m_pitchAngle) > 30)
			m_pitchAngle = (m_pitchAngle >= 0) ? 30.0f : -30.0f;

		auto targetRotationMatrix = XMMatrixRotationRollPitchYawFromVector(targetRotation);
		auto cameraYaw = XMMatrixRotationAxis({ 0.0f,1.0f,0.0f }, XMConvertToRadians(m_yawAngle));
		m_right = XMVector3Transform({ 1.0f,0.0f,0.0f }, targetRotationMatrix * cameraYaw);
		auto cameraPitch = XMMatrixRotationAxis(m_right, XMConvertToRadians(m_pitchAngle));

		m_eye = XMVectorSubtract(targetPosition, XMVector3Transform({ 0.0f,-5.0f,5.0f }, targetRotationMatrix * cameraYaw * cameraPitch));
		m_at = XMVectorSubtract(targetPosition, m_eye);
		m_up = XMVector3Transform({ 0.0f,1.0f,1.0f }, targetRotationMatrix * cameraYaw * cameraPitch);

		// Vector Regeneration
		//m_at = XMVector3Normalize(m_at);
		//m_right = XMVector3Cross(m_up, m_at);
		//m_right = XMVector3Normalize(m_right);
		//m_up = XMVector3Cross(m_at, m_right);
		//m_up = XMVector3Normalize(m_up);

		//_RPT1(0, "%f,%f,%f\t", m_eye.m128_f32[0], m_eye.m128_f32[1], m_eye.m128_f32[2]);
		//_RPT1(0, "%f,%f,%f\n", m_at.m128_f32[0], m_at.m128_f32[1], m_at.m128_f32[2]);
		//_RPT1(0, "%f,%f,%f\t", m_up.m128_f32[0], m_up.m128_f32[1], m_up.m128_f32[2]);
		//_RPT1(0, "%f,%f,%f\n", m_right.m128_f32[0], m_right.m128_f32[1], m_right.m128_f32[2]);
		//_RPT1(0, "%f\n", XMVector3Dot(m_at, m_up));

		m_viewMatrix = XMMatrixLookAtLH(m_eye, m_eye + m_at, m_up);
		this->updateBuffer(resource);
	}
}

HRESULT Camera::initialize(DX::DeviceResources * resource)
{
	HRESULT hr;
	auto device = resource->GetD3DDevice();
	D3D11_VIEWPORT viewport = resource->GetScreenViewport();

	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI * 0.4f, viewport.Width / viewport.Height, 0.01f, 100.0f);
	auto transposeProjectionMatrix = DirectX::XMMatrixTranspose(m_projectionMatrix);
	auto transposeViewMatrix = XMMatrixTranspose(m_viewMatrix);

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(MatrixBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = &transposeViewMatrix;
	hr = device->CreateBuffer(&bd, &InitData, &m_pViewBuffer);
	if (FAILED(hr))
		return hr;

	InitData.pSysMem = &transposeProjectionMatrix;
	hr = device->CreateBuffer(&bd, &InitData, &m_pProjectionBuffer);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void Camera::updateBuffer(DX::DeviceResources * resource)
{
	auto context = resource->GetD3DDeviceContext();

	auto transposeViewMatrix = XMMatrixTranspose(m_viewMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//	Disable GPU access to the vertex buffer data.
	context->Map(m_pViewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	//	Update the vertex buffer here.
	memcpy(mappedResource.pData, &transposeViewMatrix, sizeof(transposeViewMatrix));

	//	Reenable GPU access to the vertex buffer data.
	context->Unmap(m_pViewBuffer, 0);
}
