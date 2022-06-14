#pragma once

#include "Utility.h"
#include "Text2D.h"
#include "InputHandler.h"

class TextBox
{
public:
	TextBox();
	~TextBox();

	void Update(InputHandler* input);
	void Render(DX::DeviceResources* resource);

	std::wstring getMessage() { return m_message; }
	bool isDone() const { return m_done; }

private:
	DirectX::XMFLOAT2	m_pos;
	float				m_width = 100.0f;
	float				m_height = 50.0f;
	float				m_wPadding;
	float				m_hPadding;

	bool				m_done = false;

	std::wstring		m_message = L"";
};

