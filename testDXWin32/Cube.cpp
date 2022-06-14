#include "pch.h"
#include "Cube.h"

using namespace DirectX;
std::vector<Utility::Vertex> Cube::m_privateVertex =
{
	// Front Face
	Utility::Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
	Utility::Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
	Utility::Vertex(1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
	Utility::Vertex(1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

	// Back Face
	Utility::Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
	Utility::Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
	Utility::Vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
	Utility::Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

	// Top Face
	Utility::Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
	Utility::Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
	Utility::Vertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f),
	Utility::Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f),

	// Bottom Face
	Utility::Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
	Utility::Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
	Utility::Vertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f),
	Utility::Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f),

	// Left Face
	Utility::Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
	Utility::Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
	Utility::Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
	Utility::Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

	// Right Face
	Utility::Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
	Utility::Vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
	Utility::Vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
	Utility::Vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f),
};
std::vector<unsigned int> Cube::m_privateIndex =
{
	// Front Face
	0,  1,  2,
	0,  2,  3,

	// Back Face
	4,  5,  6,
	4,  6,  7,

	// Top Face
	8,  9, 10,
	8, 10, 11,

	// Bottom Face
	12, 13, 14,
	12, 14, 15,

	// Left Face
	16, 17, 18,
	16, 18, 19,

	// Right Face
	20, 21, 22,
	20, 22, 23
};

std::wstring Cube::m_privateModelPath = L"car.obj";
std::wstring Cube::m_privateTexturePath = L"braynzar.jpg";

Cube::Cube()
{
	//m_pMesh->setVertices(m_privateVertex, ARRAYSIZE(m_privateVertex));
	//m_pMesh->setIndices(m_privateIndex, ARRAYSIZE(m_privateIndex));
	//m_pMesh->setPrimitive(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//m_modelPath = m_privateModelPath;
	m_texturePath = m_privateTexturePath;

	//m_pController = new PlayerController(this);
	m_pCollider = new CircleCollider(this, 1);
}

Cube::Cube(float x, float y, float z, float rotX, float rotY, float rotZ) : IEntity::IEntity(x, y, z, rotX, rotY, rotZ)
{
	//m_pMesh->setVertices(m_privateVertex, ARRAYSIZE(m_privateVertex));
	//m_pMesh->setIndices(m_privateIndex, ARRAYSIZE(m_privateIndex));
	//m_pMesh->setPrimitive(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_modelPath = m_privateModelPath;
	//m_texturePath = m_privateTexturePath;
	m_pCollider = new CircleCollider(this, 1);
}

Cube::Cube(float x, float y, float z, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ) : IEntity::IEntity(x, y, z, rotX, rotY, rotZ, scaleX, scaleY, scaleZ)
{
	//m_pMesh->setVertices(m_privateVertex, ARRAYSIZE(m_privateVertex));
	//m_pMesh->setIndices(m_privateIndex, ARRAYSIZE(m_privateIndex));
	//m_pMesh->setPrimitive(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//m_pMesh->setTexturePath(m_privateTexturePath);

	//m_pCollider = new CircleCollider(this, scaleX);
}

Cube::~Cube()
{
}

void Cube::initialize(DX::DeviceResources* resource)
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
	else if (!m_texturePath.empty()) {
		tempMesh = new Mesh(this);
		tempMesh->getVertices() = m_privateVertex;
		tempMesh->getIndices() = m_privateIndex;
		tempMesh->getMaterial().map_Kd = m_texturePath;
		tempMesh->setPrimitive(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pMesh.push_back(tempMesh);
	}
	m_graphicsComponent.initialize(resource);
}

void Cube::initialize(DX::DeviceResources * resource, Utility::GraphicsResource & sharedResource)
{
	m_graphicsComponent.initialize(resource, sharedResource);
}

void Cube::update(DX::DeviceResources* resource, InputHandler* input, float elapsedTime)
{
	if (m_isActive && !m_isDestroy) {
		if (m_pParent != nullptr) {

		}
		if (m_pController != nullptr) {
			m_pController->update(elapsedTime);
			//_RPT1(0, "%f,%f,%f\n", XMVectorGetX(this->getPosition()), XMVectorGetY(this->getPosition()), XMVectorGetZ(this->getPosition()));
		}
		m_pBody->update(elapsedTime);
		m_graphicsComponent.updateResource(resource);
	}
	XMFLOAT3 pos = m_pBody->getPosition();
	if (XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&pos))) > 100) {
		m_isDestroy = true;
	}
}
