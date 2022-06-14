#include "pch.h"
#include "Mesh.h"
#include "IEntity.h"


Mesh::Mesh()
{
}

Mesh::Mesh(IEntity* e)
{
	m_pEntity = e;
}


Mesh::~Mesh()
{
}

void Mesh::setMesh(objl::Mesh & mesh)
{
	for (objl::Vertex v : mesh.Vertices) {
		m_pVertices.push_back(v);
	}
	for (unsigned int i : mesh.Indices) {
		m_pIndices.push_back(i);
	}
	m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_material = mesh.MeshMaterial;
}
