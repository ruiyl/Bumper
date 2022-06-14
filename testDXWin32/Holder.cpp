#include "pch.h"
#include "Holder.h"
#include "IEntity.h"

Holder::Holder()
{
}


Holder::~Holder()
{
	for (auto i = m_entityHolder.begin(); i != m_entityHolder.end(); i++) {
		delete *i;
	}
}

void Holder::updateAll(InputHandler * input, float elapsedTime)
{
	for (auto i = m_entityHolder.begin(); i != m_entityHolder.end(); i++) {
		(*i)->update(m_pResource, input, elapsedTime);
	}

	for (unsigned int i = 0; i < m_entityHolder.size() - 1; i++) {
		if (m_entityHolder[i]->getCollider() == nullptr)
			continue;
		for (unsigned int j = i + 1; j < m_entityHolder.size(); j++) {
			if (m_entityHolder[j]->getCollider() == nullptr)
				continue;
			if (m_entityHolder[i]->getCollider()->Collide(m_entityHolder[j]->getCollider()))
			{
				//OutputDebugString(TEXT("Collide\n"));
				m_entityHolder[i]->onCollision(m_entityHolder[j]->getCollider());
			}
			else
			{
				//OutputDebugString(TEXT("Not Collide\n"));
			}
		}
	}

	for (auto i = m_entityHolder.begin(); i != m_entityHolder.end(); i++) {
		if ((*i)->isDestroyed()) {
			i = m_entityHolder.erase(i);
		}
		if (i == m_entityHolder.end())
			break;
	}
}

void Holder::renderAll(Renderer * renderer, Camera * camera)
{
	for (auto i = m_entityHolder.begin(); i != m_entityHolder.end(); i++) {
		renderer->render((*i), camera, m_pResource);
	}
}

int Holder::findEntityResource(IEntity * e)
{
	for (unsigned int i = 0; i < m_resourceIndex.size(); i++) {
		if (typeid(*e).name() == m_resourceIndex[i].name)
			return i;
	}
	return -1;
}

void Holder::initEntity(IEntity * e)
{
	_RPT1(0, "initialize %s\t", typeid(*e).name());
	int result = findEntityResource(e);
	if (result >= 0) {
		OutputDebugString(L"resource found\n");
		e->setMesh(m_modelHolder[m_resourceIndex[result].index]);
		e->initialize(m_pResource, m_resourceHolder[m_resourceIndex[result].index]);
	}
	else {
		OutputDebugString(L"resource not found\n");
		e->initialize(m_pResource);

		ResourceIndex ri;
		ri.index = m_resourceHolder.size();
		ri.name = typeid(*e).name();

		m_modelHolder.push_back(e->getMesh());
		m_resourceHolder.push_back(Utility::GraphicsResource(e->getGraphicsComponent().getGraphicsResource()));
		m_resourceIndex.push_back(ri);
	}
}

void Holder::registerPlayer(int id, IEntity * e)
{
	m_playerHolder[id] = e;
}

int Holder::getPlayerLeft()
{
	int count = 0;
	for (auto i = m_playerHolder.begin(); i != m_playerHolder.end(); i++) {
		if (!(*i).second->isDestroyed()) {
			count++;
		}
	}
	return count;
}

IEntity * Holder::getPlayerById(int id)
{
	auto result = m_playerHolder.find(id);
	if (result != m_playerHolder.end()) {
		return (*result).second;
	}
	else {
		return nullptr;
	}
}

void Holder::destroyEntity(std::vector<IEntity*>::iterator e)
{
	m_entityHolder.erase(e);
}

void Holder::Reset()
{
	m_entityHolder.clear();
	m_modelHolder.clear();
	m_playerHolder.clear();
	m_resourceHolder.clear();
	m_resourceIndex.clear();
}
