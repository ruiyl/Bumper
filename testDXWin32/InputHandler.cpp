#include "pch.h"
#include "InputHandler.h"
#include <Windowsx.h>

// Initialize static member
std::map<UINT, InputHandler::PressedState>	InputHandler::m_keyboardState;
InputHandler::PressedState					InputHandler::m_mouseState[2];
DirectX::XMINT2								InputHandler::m_mousePos;
DirectX::XMINT2								InputHandler::m_lastMousePos;
std::deque<wchar_t>							InputHandler::m_characterEvent;
std::deque<UINT>							InputHandler::m_keyEvent;
bool										InputHandler::m_isListenForChar = false;

InputHandler::InputHandler()
{
}


InputHandler::~InputHandler()
{
}

void InputHandler::OnKeyPressed(UINT vk)
{
	InputHandler::m_keyboardState[vk] = PressedState::depressed;
	if (!m_isListenForChar)
		m_keyEvent.push_back(vk);
}

void InputHandler::OnKeyReleased(UINT vk)
{
	InputHandler::m_keyboardState[vk] = PressedState::unpressed;
}

void InputHandler::OnCharacter(wchar_t c)
{
	if (m_isListenForChar)
		InputHandler::m_characterEvent.push_back(c);
}

void InputHandler::OnMouseMove(LPARAM & lParam)
{
	InputHandler::m_mousePos = DirectX::XMINT2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
}

void InputHandler::OnLMousePressed()
{
	InputHandler::m_mouseState[0] = PressedState::depressed;
}

void InputHandler::OnLMouseReleased()
{
	InputHandler::m_mouseState[0] = PressedState::unpressed;
}

void InputHandler::OnRMousePressed()
{
	InputHandler::m_mouseState[1] = PressedState::depressed;
}

void InputHandler::OnRMouseReleased()
{
	InputHandler::m_mouseState[1] = PressedState::unpressed;
}

InputHandler::PressedState InputHandler::getKeyPress(UINT vk)
{
	return InputHandler::m_keyboardState[vk];
}

InputHandler::PressedState InputHandler::getMousePress(short mouseButton)
{
	return InputHandler::m_mouseState[mouseButton];
}

DirectX::XMINT2 InputHandler::getMousePos()
{
	return InputHandler::m_mousePos;
}

wchar_t InputHandler::consumeCharEvent()
{
	wchar_t c = L'\0';
	if (m_characterEvent.size() > 0) {
		c = m_characterEvent.front();
		m_characterEvent.pop_front();
	}
	return c;
}

UINT InputHandler::consumeKeyEvent()
{
	UINT key = 0;
	if (m_keyEvent.size() > 0) {
		key = m_keyEvent.front();
		m_keyEvent.pop_front();
	}
	return key;
}

DirectX::XMINT2 InputHandler::getMovement()
{
	DirectX::XMINT2 move = { 0,0 };
	if (m_keyboardState[0x41] == PressedState::depressed) { // A key
		move.x += -1;
	}
	if (m_keyboardState[0x44] == PressedState::depressed) { // D key
		move.x += 1;
	}
	if (m_keyboardState[0x53] == PressedState::depressed) { // S key
		move.y += -1;
	}
	if (m_keyboardState[0x57] == PressedState::depressed) { // W key
		move.y += 1;
	}
	return move;
}

DirectX::XMFLOAT2 InputHandler::getTurnCamera()
{
	DirectX::XMFLOAT2 rotate = { 0,0 };
	if (m_keyboardState[0x4A] == PressedState::depressed) { // J key
		rotate.x += 1;
	}
	if (m_keyboardState[0x4C] == PressedState::depressed) { // L key
		rotate.x += -1;
	}
	if (m_keyboardState[0x4B] == PressedState::depressed) { // K key
		rotate.y += -1;
	}
	if (m_keyboardState[0x49] == PressedState::depressed) { // I key
		rotate.y += 1;
	}
	return rotate;
}
