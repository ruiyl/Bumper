#pragma once

#include "TextBox.h"
#include "InputHandler.h"

#define MAINMENUCOUNT 3
#define HOST_GAME 0
#define JOIN_GAME 1
#define QUIT_GAME 2

class MainMenu
{
public:
	MainMenu();
	MainMenu(DX::DeviceResources* resource);
	~MainMenu();

	void Update(InputHandler* input);
	void Render(DX::DeviceResources* resource);

	bool			isDone() const		{ return m_isDone; }
	int				getResult() const	{ return m_result; }
	std::wstring	getMessage() const	{ return m_textBoxResult; }

	void resetMenu();

private:
	void Choose();

	std::vector<Text2D>		m_menu;
	bool					m_useTextbox = false;
	TextBox*				m_pTextBox = nullptr;
	std::wstring			m_textBoxResult = L"";

	int				m_currentMenu;

	bool			m_isDone = false;
	int				m_result;
};

