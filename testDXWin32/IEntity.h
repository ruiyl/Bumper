#pragma once
#include "GraphicsComponent.h"
#include "InputHandler.h"
#include "IController.h"
#include "ICollider.h"
#include "Body.h"
#include "Mesh.h"
#include <vector>

class IEntity
{
public:
	IEntity();
	IEntity(float x, float y, float z, float rotX, float rotY, float rotZ);
	IEntity(float x, float y, float z, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ);
	virtual ~IEntity() {};

	void virtual initialize(DX::DeviceResources* resource) = 0;
	void virtual initialize(DX::DeviceResources* resource, Utility::GraphicsResource& sharedResource) = 0;
	void virtual update(DX::DeviceResources* resource, InputHandler* input, float elapsedTime) = 0;
	void virtual onCollision(ICollider* other);

	void setDestroy(bool isDestroyed)			{ m_isDestroy = isDestroyed; }
	void setMesh(std::vector<Mesh*>& meshes)	{ m_pMesh = meshes; }
	void setChild(IEntity* child)				{ child->m_pParent = this; }
	void setController(IController* controller) { m_pController = controller; }

	bool							isDestroyed() const				{ return m_isDestroy; }
	ICollider*						getCollider()					{ return m_pCollider; }
	GraphicsComponent&				getGraphicsComponent()			{ return m_graphicsComponent; }
	Body*							getBody()						{ return m_pBody; }
	std::vector<Mesh*>&				getMesh()						{ return m_pMesh; }
	unsigned int					getMeshCount() const			{ return m_pMesh.size(); }
	std::wstring					getModelPath() const			{ return m_modelPath; }
	std::wstring					getTexturePath() const			{ return m_texturePath; }
	DirectX::XMFLOAT3				getPosition() const				{ return m_pBody->getPosition(); }
	DirectX::XMFLOAT3				getRotation() const				{ return m_pBody->getRotation(); }
	DirectX::XMFLOAT3				getScale() const				{ return m_pBody->getScale(); }
	IEntity*						getParent()						{ return m_pParent; }

protected:
	bool									m_isDestroy = false;
	bool									m_isActive = true;

	GraphicsComponent						m_graphicsComponent;
	std::vector<Mesh*>						m_pMesh;
	std::wstring							m_modelPath = L"";
	std::wstring							m_texturePath = L"";

	IEntity*								m_pParent = nullptr;
	IController*							m_pController = nullptr;
	ICollider*								m_pCollider = nullptr;
	Body*									m_pBody = nullptr;
};

