//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "InputHandler.h"
#include "Renderer.h"
#include "Car.h"
#include "Stage.h"
#include "Holder.h"
#include "OBJ_Loader.h"
#include "ClientGame.h"
#include "MainMenu.h"
#include <fstream>


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

	InputHandler							m_inputHandler;
	Renderer								m_renderer;
	Camera									m_camera;
	Holder									m_holder;
	ClientGame								m_clientManager;
	MainMenu								m_mainMenu;

private:

	void Spawn();
	void SetUp();
	void StartGame();
    void Update(DX::StepTimer const& timer);
	void CheckGameOver();
    void Render();
	void ResetGame();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

	enum class GameState
	{
		Starting,
		SettingUp,
		Waiting,
		Playing,
		Ending
	};

	GameState								m_state;
	std::wstring							m_text;

	std::ofstream file;
	bool doneLoading = false;
	float counter = 0.0f;
};