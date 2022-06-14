#pragma once
#include "IComponent.h"
class Mesh :
	public IComponent
{
public:
	Mesh();
	Mesh(IEntity* e);
	~Mesh();

	void setPrimitive(D3D_PRIMITIVE_TOPOLOGY primitive)		{ m_primitiveTopology = primitive; }
	void setMesh(objl::Mesh& mesh);

	D3D_PRIMITIVE_TOPOLOGY		getPrimitive() const			{ return m_primitiveTopology; }
	objl::Material&				getMaterial()					{ return m_material; }

	std::vector<Utility::Vertex>&	getVertices()			{ return m_pVertices; }
	std::vector<unsigned int>&		getIndices()			{ return m_pIndices; }
	int								getVertexCount() const	{ return m_pVertices.size(); }
	int								getIndexCount() const	{ return m_pIndices.size(); }

private:
	objl::Material							m_material;
	D3D_PRIMITIVE_TOPOLOGY					m_primitiveTopology;
	std::vector<Utility::Vertex>			m_pVertices;
	std::vector<unsigned int>				m_pIndices;
};

