#pragma once
#include "IController.h"
#include "ClientGame.h"
class NetworkController :
	public IController
{
public:
	NetworkController();
	NetworkController(IEntity* e, ClientGame* manager, int id);
	~NetworkController();

	void update(float elapsedTime);
	void saveStartState();

private:
	ClientGame *			m_pClientManager;
	int						m_id;

	bool					m_useInterpolation = false;
	DirectX::XMFLOAT3		m_startPosition;
	DirectX::XMFLOAT3		m_startVelocity;
};

