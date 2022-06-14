#include <vector>
#include <iterator>
#include <typeinfo>
#include "DeviceResources.h"
#include "InputHandler.h"
#include "Camera.h"
#include "Renderer.h"
#include "ClientGame.h"

class IEntity;

class Holder
{
public:
	Holder();
	~Holder();

	std::vector<IEntity*>* getEntityHolder() { return &m_entityHolder; }

	void setDeviceResource(DX::DeviceResources* resource)	{ m_pResource = resource; }
	void setClientManager(ClientGame* client)				{ m_pClientManager = client; }

	void updateAll(InputHandler* input, float elapsedTime);
	void renderAll(Renderer* renderer, Camera* camera);
	void initEntity(IEntity* e);

	template<class T>
	IEntity* makeEntity();
	template<class T>
	IEntity* makeEntity(float x, float y, float z, float rotX, float rotY, float rotZ);
	template<class T>
	IEntity* makeEntity(float x, float y, float z, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ);

	void		registerPlayer(int id, IEntity* e);
	int			getPlayerLeft();
	IEntity*	getPlayerById(int id);

	void destroyEntity(std::vector<IEntity*>::iterator e);
	void Reset();

private:
	int findEntityResource(IEntity* e);

	struct ResourceIndex
	{
		const char*		name;
		int				index;
	};

	std::vector<IEntity*>						m_entityHolder;
	std::map<int, IEntity*>						m_playerHolder;
	std::vector<std::vector<Mesh*>>				m_modelHolder;
	std::vector<Utility::GraphicsResource>		m_resourceHolder;
	std::vector<ResourceIndex>					m_resourceIndex;
	DX::DeviceResources*						m_pResource;
	ClientGame*									m_pClientManager;
	IEntity*									m_localPlayer;
};

template<class T>
IEntity* Holder::makeEntity()
{
	m_entityHolder.push_back(new T());
	initEntity(m_entityHolder.back());
	return m_entityHolder.back();
}

template<class T>
IEntity* Holder::makeEntity(float x, float y, float z, float rotX, float rotY, float rotZ)
{
	m_entityHolder.push_back(new T(x, y, z, rotX, rotY, rotZ));
	initEntity(m_entityHolder.back());
	return m_entityHolder.back();
}

template<class T>
IEntity* Holder::makeEntity(float x, float y, float z, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ)
{
	m_entityHolder.push_back(new T(x, y, z, rotX, rotY, rotZ, scaleX, scaleY, scaleZ));
	initEntity(m_entityHolder.back());
	return m_entityHolder.back();
}