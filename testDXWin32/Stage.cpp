#include "pch.h"
#include "Stage.h"

using namespace DirectX;
std::vector<Utility::Vertex> Stage::m_privateVertex =
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
std::vector<unsigned int> Stage::m_privateIndex =
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

std::wstring Stage::m_privateModelPath = L"";
std::wstring Stage::m_privateTexturePath = L"braynzar.jpg";

Stage::Stage()
{
	m_modelPath = m_privateModelPath;
	m_texturePath = m_privateTexturePath;
}

Stage::Stage(float x, float y, float z, float rotX, float rotY, float rotZ) : IEntity::IEntity(x, y, z, rotX, rotY, rotZ)
{
	m_modelPath = m_privateModelPath;
	m_texturePath = m_privateTexturePath;
}

Stage::Stage(float x, float y, float z, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ) : IEntity::IEntity(x, y, z, rotX, rotY, rotZ, scaleX, scaleY, scaleZ)
{
	m_modelPath = m_privateModelPath;
	m_texturePath = m_privateTexturePath;
}


Stage::~Stage()
{
}

void Stage::initialize(DX::DeviceResources * resource)
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
		objl::Material m;
		m.map_Kd = m_texturePath;
		tempMesh->getMaterial() = m;
		tempMesh->setPrimitive(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_pMesh.push_back(tempMesh);
	}
	m_graphicsComponent.initialize(resource);
}

void Stage::initialize(DX::DeviceResources * resource, Utility::GraphicsResource & sharedResource)
{
	m_graphicsComponent.initialize(resource, sharedResource);
}

void Stage::update(DX::DeviceResources * resource, InputHandler * input, float elapsedTime)
{
}
