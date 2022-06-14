#include "pch.h"
#include "Renderer.h"

using namespace DirectX;
Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

HRESULT Renderer::initialize(DX::DeviceResources * resource)
{
	auto device = resource->GetD3DDevice();
	auto context = resource->GetD3DDeviceContext();

	HRESULT hr;

	ID3DBlob* pVSBlob = nullptr;
	hr = D3DX11CompileFromFile(L"Shader.fx", 0, 0, "VS", "vs_4_0", 0, 0, 0, &pVSBlob, 0, 0);

	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the vertex shader
	hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(), &m_pInputLayout);
	pVSBlob->Release();
	if (FAILED(hr))
		return hr;
	context->IASetInputLayout(m_pInputLayout);
	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;

	hr = D3DX11CompileFromFile(L"Shader.fx", 0, 0, "PS0", "ps_4_0", 0, 0, 0, &pPSBlob, 0, 0);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	// Create the pixel shader
	hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPlainTexturePixelShader);
	if (FAILED(hr))
		return hr;

	hr = D3DX11CompileFromFile(L"Shader.fx", 0, 0, "PS1", "ps_4_0", 0, 0, 0, &pPSBlob, 0, 0);
	if (FAILED(hr))
	{
		MessageBox(nullptr,
			L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		return hr;
	}

	hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPlainColorPixelShader);

	pPSBlob->Release();
	if (FAILED(hr))
		return hr;

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	cmdesc.FrontCounterClockwise = true;
	hr = device->CreateRasterizerState(&cmdesc, &m_pRState);
	if (FAILED(hr))
		return hr;

	return S_OK;
}

void Renderer::render(IEntity* entity, Camera* camera, DX::DeviceResources* resource)
{

	auto context = resource->GetD3DDeviceContext();
	auto graphicsComponent = entity->getGraphicsComponent();
	auto meshes = entity->getMesh();

	context->IASetInputLayout(m_pInputLayout);

	unsigned stride = sizeof(Utility::Vertex);
	unsigned offset = 0;
	auto vertexBuffer = graphicsComponent.getVertexBuffer();
	auto indexBuffer = graphicsComponent.getIndexBuffer();

	auto viewBuffer = camera->getViewBuffer();
	auto worldBuffer = graphicsComponent.getWorldBuffer();
	auto projectionBuffer = camera->getProjectionBuffer();

	context->VSSetShader(m_pVertexShader, nullptr, 0);
	context->VSSetConstantBuffers(0, 1, &viewBuffer);
	context->VSSetConstantBuffers(1, 1, &projectionBuffer);
	context->VSSetConstantBuffers(2, 1, &worldBuffer);

	auto srv = graphicsComponent.getTextureResourceView();
	auto ss = graphicsComponent.getSamplerState();
	auto color = graphicsComponent.getColorBuffer();

	for (unsigned int i = 0; i < entity->getMeshCount(); i++) {
		context->IASetVertexBuffers(0, 1, &vertexBuffer[i], &stride, &offset);
		context->IASetIndexBuffer(indexBuffer[i], DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(meshes[i]->getPrimitive());

		if (srv[i] != nullptr) {
			context->PSSetShader(m_pPlainTexturePixelShader, nullptr, 0);
			context->PSSetShaderResources(0, 1, &srv[i]);
			context->PSSetSamplers(0, 1, &ss[i]);
		}
		else {
			context->PSSetShader(m_pPlainColorPixelShader, nullptr, 0);
			context->PSSetConstantBuffers(3, 1, &color[i]);
		}
		//context->RSSetState(m_pRState);
		context->DrawIndexed(meshes[i]->getIndexCount(), 0, 0);
	}
}

void Renderer::renderText(Text2D & text, DX::DeviceResources * resource)
{
	ID2D1SolidColorBrush* brush;
	auto context = resource->GetD2DDeviceContext();

	context->CreateSolidColorBrush(text.m_color, &brush);
	
	context->BeginDraw();

	context->DrawTextLayout(D2D1::Point2F(0.f, 0.f), text.m_textLayout.Get(), brush);

	context->EndDraw();

	brush->Release();
}

void Renderer::renderText(Text2D & text, float x, float y, DX::DeviceResources * resource)
{
	ID2D1SolidColorBrush* brush;
	auto context = resource->GetD2DDeviceContext();

	context->CreateSolidColorBrush(text.m_color, &brush);

	context->BeginDraw();

	context->DrawTextLayout(D2D1::Point2F(x, y), text.m_textLayout.Get(), brush);

	context->EndDraw();

	brush->Release();
}
