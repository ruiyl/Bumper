#pragma once

#include <map>
#include <deque>
#include <DirectXMath.h>

class InputHandler
{
public:
	enum class PressedState {
		unpressed,
		depressed
	};

	InputHandler();
	~InputHandler();

	//update keyboard and mouse state
	void OnKeyPressed(UINT vk);
	void OnKeyReleased(UINT vk);
	void OnCharacter(wchar_t c);
	void OnMouseMove(LPARAM& lParam);
	void OnLMousePressed();
	void OnLMouseReleased();
	void OnRMousePressed();
	void OnRMouseReleased();

	//get keyboard and mouse state
	PressedState getKeyPress(UINT vk);
	PressedState getMousePress(short mouseButton);
	DirectX::XMINT2 getMousePos();

	void setTextListen(bool val) { m_isListenForChar = val; }
	bool hasCharEvent() { return m_characterEvent.size() != 0; };
	bool hasKeyEvent()	{ return m_keyEvent.size() != 0; }
	wchar_t consumeCharEvent();
	UINT	consumeKeyEvent();

	DirectX::XMINT2 getMovement();
	DirectX::XMFLOAT2 getTurnCamera();

private:
	static std::map<UINT, PressedState>		InputHandler::m_keyboardState;
	static PressedState						InputHandler::m_mouseState[2];
	static DirectX::XMINT2					InputHandler::m_mousePos;
	static DirectX::XMINT2					InputHandler::m_lastMousePos;
	static std::deque<wchar_t>				InputHandler::m_characterEvent;
	static std::deque<UINT>					InputHandler::m_keyEvent;
	static bool								m_isListenForChar;
};

