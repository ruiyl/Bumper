#include "pch.h"
#include "TextBox.h"


TextBox::TextBox()
{
	m_pos = { 0.0f,0.0f };
	m_message = L"127.0.0.1";
}


TextBox::~TextBox()
{
}

void TextBox::Update(InputHandler * input)
{
	input->setTextListen(true);
	if (input->hasCharEvent()) {
		wchar_t key = input->consumeCharEvent();
		switch (key)
		{
		case L'\b': // backspace
			if (!m_message.empty())
				m_message.pop_back();
			break;
		case L'\r': // Enter
			input->setTextListen(false);
			m_done = true;
			break;
		default:
			m_message.push_back(key);
			break;
		}
	}
}

void TextBox::Render(DX::DeviceResources * resource)
{
	Text2D msg(m_message, resource);
	ID2D1SolidColorBrush* brush;
	auto context = resource->GetD2DDeviceContext();

	context->CreateSolidColorBrush(msg.m_color, &brush);

	context->BeginDraw();

	context->DrawTextLayout(D2D1::Point2F(m_pos.x, m_pos.y), msg.m_textLayout.Get(), brush);

	context->EndDraw();

	brush->Release();
}
