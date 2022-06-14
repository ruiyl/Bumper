#include "pch.h"
#include "Car.h"


std::wstring Car::m_privateModelPath = L"Car.obj";
std::wstring Car::m_privateTexturePath = L"braynzar.jpg";

Car::Car()
{
	m_modelPath = m_privateModelPath;
	m_pCollider = new CircleCollider(this, 2);
}

Car::Car(float x, float y, float z, float rotX, float rotY, float rotZ) : IEntity::IEntity(x, y, z, rotX, rotY, rotZ)
{
	m_modelPath = m_privateModelPath;
	m_pCollider = new CircleCollider(this, 2);
}

Car::Car(float x, float y, float z, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ) : IEntity::IEntity(x, y, z, rotX, rotY, rotZ, scaleX, scaleY, scaleZ)
{
	m_modelPath = m_privateModelPath;
	m_pCollider = new CircleCollider(this, 1 * ((scaleX + scaleY) / 2));
}


Car::~Car()
{
}

void Car::initialize(DX::DeviceResources * resource)
{
	Mesh* tempMesh;
	if (!m_modelPath.empty()) {
		objl::Loader::g_loader.LoadFile(m_modelPath);
		for (objl::Mesh mesh : objl::Loader::g_loader.LoadedMeshes) {
			tempMesh = new Mesh(this);
			tempMesh->setMesh(mesh);
			m_pMesh.push_back(tempMesh);
		}
	}
	m_graphicsComponent.initialize(resource);
}

void Car::initialize(DX::DeviceResources * resource, Utility::GraphicsResource & sharedResource)
{
	m_graphicsComponent.initialize(resource, sharedResource);
}

void Car::update(DX::DeviceResources * resource, InputHandler * input, float elapsedTime)
{
	if (m_isActive && !m_isDestroy) {
		if (m_pParent != nullptr) {

		}
		if (m_pController != nullptr) {
			m_pController->update(elapsedTime);
			//_RPT1(0, "%f,%f,%f\n", XMVectorGetX(this->getPosition()), XMVectorGetY(this->getPosition()), XMVectorGetZ(this->getPosition()));
		}
		else {
			m_pBody->update(elapsedTime);
		}
		m_graphicsComponent.updateResource(resource);
	}
}
