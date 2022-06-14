#pragma once
#include "IEntity.h"
#include "PlayerController.h"
#include "NetworkController.h"
#include "CircleCollider.h"

using namespace DirectX;
class Cube :
	public IEntity
{
public:
	Cube();
	Cube(float x, float y, float z, float rotX, float rotY, float rotZ);
	Cube(float x, float y, float z, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ);
	~Cube();

	void initialize(DX::DeviceResources* resource);
	void initialize(DX::DeviceResources* resource, Utility::GraphicsResource& sharedResource);
	void update(DX::DeviceResources* resource, InputHandler* input, float elapsedTime);

private:
	static std::vector<Utility::Vertex>	m_privateVertex;
	static std::vector<unsigned int>	m_privateIndex;
	static std::wstring					m_privateModelPath;
	static std::wstring					m_privateTexturePath;
};

