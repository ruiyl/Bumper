#pragma once
#include "IEntity.h"
#include "PlayerController.h"
#include "NetworkController.h"
#include "CircleCollider.h"

class Car :
	public IEntity
{
public:
	Car();
	Car(float x, float y, float z, float rotX, float rotY, float rotZ);
	Car(float x, float y, float z, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ);
	~Car();

	void initialize(DX::DeviceResources* resource);
	void initialize(DX::DeviceResources* resource, Utility::GraphicsResource& sharedResource);
	void update(DX::DeviceResources* resource, InputHandler* input, float elapsedTime);

private:
	static std::wstring					m_privateModelPath;
	static std::wstring					m_privateTexturePath;
};

