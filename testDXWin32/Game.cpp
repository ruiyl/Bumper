//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame();

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
	//file = std::ofstream("game.txt");
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	//file << "Game Initialized\n";
	//file.close();
	m_state = Game::GameState::Starting;

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);

	m_renderer.initialize(m_deviceResources.get());

	m_holder.setDeviceResource(m_deviceResources.get());
	m_holder.setClientManager(&m_clientManager);

	m_mainMenu = MainMenu(m_deviceResources.get());
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
		//Render();
    });
	Render();
}

void Game::Spawn()
{
	IEntity* entity = nullptr;
	//_RPT1(0, "client number : %d\n", ClientGame::clientNumber);
	entity = m_holder.makeEntity<Stage>(0.0f, -20.0f, 0.0f, 0.0f, 0.0f, 0.0f, 20.0f, 20.0f, 20.0f);
	for (unsigned int i = 0; i < ClientGame::clientNumber; i++) {
		switch (i)
		{
		case 0:
			entity = m_holder.makeEntity<Car>(-10.0f, 0.0f, 0.0f, 0.0f, XM_PIDIV2, 0.0f);
			m_holder.registerPlayer(i, entity);
			m_clientManager.State[i] = entity->getBody()->getState();
			break;
		case 1:
			entity = m_holder.makeEntity<Car>(10.0f, 0.0f, 0.0f, 0.0f, -XM_PIDIV2, 0.0f);
			m_holder.registerPlayer(i, entity);
			m_clientManager.State[i] = entity->getBody()->getState();
			break;
		case 2:
			entity = m_holder.makeEntity<Car>(0.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f);
			m_holder.registerPlayer(i, entity);
			m_clientManager.State[i] = entity->getBody()->getState();
			break;
		case 3:
			entity = m_holder.makeEntity<Car>(0.0f, 0.0f, 10.0f, 0.0f, XM_PI, 0.0f);
			m_holder.registerPlayer(i, entity);
			m_clientManager.State[i] = entity->getBody()->getState();
			break;
		case 4:
			entity = m_holder.makeEntity<Car>(-10.0f, 0.0f, -10.0f, 0.0f, XM_PIDIV4, 0.0f);
			m_holder.registerPlayer(i, entity);
			m_clientManager.State[i] = entity->getBody()->getState();
			break;
		case 5:
			entity = m_holder.makeEntity<Car>(10.0f, 0.0f, 10.0f, 0.0f, -(XM_PIDIV4 + XM_PIDIV2), 0.0f);
			m_holder.registerPlayer(i, entity);
			m_clientManager.State[i] = entity->getBody()->getState();
			break;
		case 6:
			entity = m_holder.makeEntity<Car>(10.0f, 0.0f, -10.0f, 0.0f, -XM_PIDIV4, 0.0f);
			m_holder.registerPlayer(i, entity);
			m_clientManager.State[i] = entity->getBody()->getState();
			break;
		case 7:
			entity = m_holder.makeEntity<Car>(-10.0f, 0.0f, 10.0f, 0.0f, XM_PIDIV4 + XM_PIDIV2, 0.0f);
			m_holder.registerPlayer(i, entity);
			m_clientManager.State[i] = entity->getBody()->getState();
			break;
		}
		if (entity != nullptr && i != m_clientManager.id) {
			entity->setController(new NetworkController(entity, &m_clientManager, i));
		}
	}
	IEntity* player = m_holder.getPlayerById(m_clientManager.id);
	player->setController(new PlayerController(player, &m_inputHandler));
	m_camera = Camera(player);
	m_camera.initialize(m_deviceResources.get());
	m_state = Game::GameState::Waiting;
	doneLoading = true;
	_RPT1(0, "entity count : %d\n", m_holder.getEntityHolder()->size());
}

void Game::SetUp()
{
	//m_state = SettingUp;
	//m_clientManager.initialize();
}

void Game::StartGame()
{
	m_state = Game::GameState::Playing;
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());
	float t = float(timer.GetTotalSeconds());

    // TODO: Add your game logic here.
	IEntity* e;
	switch (m_state)
	{
	case Game::GameState::Starting:
		m_mainMenu.Update(&m_inputHandler);
		if (m_mainMenu.isDone()) {
			switch (m_mainMenu.getResult())
			{
			case HOST_GAME:
				m_clientManager.HostGame();
				m_state = Game::GameState::SettingUp;
				break;
			case JOIN_GAME:
				try {
					m_clientManager.JoinGame(m_mainMenu.getMessage());
				}
				catch (const char* msg) {
					_RPT1(0, "%s\n", msg);
					m_mainMenu.resetMenu();
					break;
				}
				m_state = Game::GameState::SettingUp;
				break;
			case QUIT_GAME:
				ExitGame();
				break;
			default:
				break;
			}
		}
		break;

	case Game::GameState::SettingUp:
		m_clientManager.update(elapsedTime);
		if (m_clientManager.id == HOST_ID) {
			if (m_inputHandler.hasKeyEvent() && m_inputHandler.consumeKeyEvent() == 0x0D) {
				m_clientManager.lock();
			}
		}
		if (!ClientGame::isWaiting) {
			Spawn();
			if (doneLoading)
				m_clientManager.ready();
		}
		
		break;

	case Game::GameState::Waiting:
		m_clientManager.update(elapsedTime);
		if (ClientGame::isPlaying)
			StartGame();
		break;

	case Game::GameState::Playing:
		counter += elapsedTime;
		e = m_holder.getPlayerById(m_clientManager.id);
		XMFLOAT3 pos = e->getPosition();
		if (XMVectorGetX(XMVector3LengthSq(XMLoadFloat3(&pos))) > 500) {
			e->setDestroy(true);
		}
		if (counter >= UPDATE_INTERVAL) {
			m_clientManager.saveState(e->getBody()->getState());
			m_clientManager.sendState();
			counter = 0.0f;
		}
		m_clientManager.update(elapsedTime);

		CheckGameOver();

		m_holder.updateAll(&m_inputHandler, elapsedTime);
		m_camera.update(m_deviceResources.get(), &m_inputHandler);
		break;

	case Game::GameState::Ending:
		if (m_inputHandler.hasKeyEvent() && m_inputHandler.consumeKeyEvent() == 0x0D) {
			ResetGame();
		}
		break;

	default:
		break;
	}
	//_RPT1(0, "%d\n", m_state);
}
void Game::CheckGameOver()
{
	IEntity* player = m_holder.getPlayerById(m_clientManager.id);
	if (player->isDestroyed()) {
		m_text = std::wstring(L"YOU LOSE");
		m_clientManager.saveState(player->getBody()->getState());
		m_clientManager.sendState();
		m_state = Game::GameState::Ending;
	}
	else if (m_holder.getPlayerLeft() <= 1) {
		m_text = std::wstring(L"YOU WIN");
		m_state = Game::GameState::Ending;
	}
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
	RECT screen = m_deviceResources->GetOutputSize();
    // TODO: Add your rendering code here.
	switch (m_state)
	{
	case Game::GameState::Starting:
		m_mainMenu.Render(m_deviceResources.get());
		break;

	case Game::GameState::SettingUp:
		m_renderer.renderText(Text2D(std::wstring(L"Waiting for other players"), m_deviceResources.get()), m_deviceResources.get());
		break;

	case Game::GameState::Waiting:
		break;

	case Game::GameState::Playing:
		m_holder.renderAll(&m_renderer, &m_camera);
		break;

	case Game::GameState::Ending:
		m_holder.renderAll(&m_renderer, &m_camera);
		m_renderer.renderText(Text2D(m_text, m_deviceResources.get()), screen.right / 2, screen.bottom / 2, m_deviceResources.get());

		break;

	default:
		break;
	}

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

void Game::ResetGame()
{
	m_holder.Reset();
	m_clientManager.Reset();
	m_mainMenu.resetMenu();
	m_state = Game::GameState::Starting;
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
	m_holder.setDeviceResource(m_deviceResources.get());
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
