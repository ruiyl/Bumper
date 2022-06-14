#include "pch.h"
#include "Text2D.h"


Text2D::Text2D()
{
}

Text2D::Text2D(const std::wstring & text, DX::DeviceResources * resource) :
	m_text(text)
{
	IDWriteFactory1* dWriteFactory = resource->GetDWriteFactory();

	dWriteFactory->CreateTextFormat(
		L"Segoe UI",
		nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		32.0f,
		L"en-US",
		&m_textFormat
	);
	
	m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	dWriteFactory->CreateTextLayout(
		m_text.c_str(),
		m_text.length(),
		m_textFormat.Get(),
		240.0f, // Max width of the input text.
		50.0f, // Max height of the input text.
		&m_textLayout
	);
}


Text2D::~Text2D()
{
}
