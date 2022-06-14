#include "pch.h"
#include "MainMenu.h"


MainMenu::MainMenu()
{
}

MainMenu::MainMenu(DX::DeviceResources * resource)
{
	m_currentMenu = 0;

	m_menu.emplace_back(std::wstring(L"Host Game"), resource);
	m_menu.emplace_back(std::wstring(L"Join Game"), resource);
	m_menu.emplace_back(std::wstring(L"Quit Game"), resource);
}


MainMenu::~MainMenu()
{
}

void MainMenu::Update(InputHandler * input)
{
	if (m_useTextbox) {
		if (m_pTextBox == nullptr) {
			m_pTextBox = new TextBox();
		}

		m_pTextBox->Update(input);

		if (m_pTextBox->isDone()) {
			m_textBoxResult = m_pTextBox->getMessage();
			delete m_pTextBox;
			m_pTextBox = nullptr;
			m_useTextbox = false;
			m_isDone = true;
		}
	}
	else {
		if (input->hasKeyEvent()) {
			UINT key = input->consumeKeyEvent();
			switch (key)
			{
			case 0x26: // Up Arrow
				m_currentMenu--;
				m_currentMenu = (m_currentMenu < 0) ? MAINMENUCOUNT - ((-m_currentMenu) % MAINMENUCOUNT) : m_currentMenu % MAINMENUCOUNT;
				break;
			case 0x28: // Down Arrow
				m_currentMenu++;
				m_currentMenu = m_currentMenu % MAINMENUCOUNT;
				break;
			case 0x0D: // Enter
				Choose();
				break;
			default:
				break;
			}
		}
	}
}

void MainMenu::Render(DX::DeviceResources * resource)
{
	RECT screen = resource->GetOutputSize();

	if (m_useTextbox && m_pTextBox != nullptr) {
		m_pTextBox->Render(resource);
	}
	else {

		D2D1::ColorF menuColor = D2D1::ColorF::White;
		D2D1::ColorF currMenuColor = D2D1::ColorF::Yellow;
		ID2D1DeviceContext* context = resource->GetD2DDeviceContext();
		ID2D1SolidColorBrush* brush;
		float x = 20.0f;
		float y = screen.bottom / 2;
		

		context->BeginDraw();

		for (unsigned int i = 0; i < m_menu.size(); i++) {
			if (i == m_currentMenu)
				context->CreateSolidColorBrush(currMenuColor, &brush);
			else
				context->CreateSolidColorBrush(menuColor, &brush);

			context->DrawTextLayout(D2D1::Point2F(x, y), m_menu[i].m_textLayout.Get(), brush);
			y += m_menu[i].m_textLayout->GetMaxHeight();;
			brush->Release();
		}

		context->EndDraw();
	}
}

void MainMenu::resetMenu()
{
	m_isDone = 0;
	m_textBoxResult = L"";
}

void MainMenu::Choose()
{

	switch (m_currentMenu)
	{
	case HOST_GAME:
		m_isDone = true;
		break;
	case JOIN_GAME:
		m_useTextbox = true;
		break;
	case QUIT_GAME:
		m_isDone = true;
		break;
	default:
		break;
	}
	m_result = m_currentMenu;
}
