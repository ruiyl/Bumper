#pragma once

#include "DeviceResources.h"
#include <string>

class Text2D
{
	friend class Renderer;
	friend class TextBox;
	friend class MainMenu;
public:
	Text2D();
	Text2D(const std::wstring& text, DX::DeviceResources* resource);
	~Text2D();

	void setColor(D2D1::ColorF color) { m_color = color; }

private:
	std::wstring								m_text = L"";
	D2D1::ColorF								m_color = D2D1::ColorF::White;
	Microsoft::WRL::ComPtr<IDWriteTextLayout>	m_textLayout;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>	m_textFormat;
};

