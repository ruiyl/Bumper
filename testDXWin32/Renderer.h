#pragma once

#include "IEntity.h"
#include "Camera.h"
#include "Text2D.h"
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3dx11.lib")
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\D3DX11async.h"

class Renderer
{
public:
	struct MatrixBuffer
	{
		DirectX::XMMATRIX matrix;
	};

	Renderer();
	~Renderer();

	HRESULT initialize(DX::DeviceResources* resource);
	void render(IEntity* entity, Camera* camera, DX::DeviceResources* resource);
	void renderText(Text2D& text, DX::DeviceResources* resource);
	void renderText(Text2D& text, float x, float y, DX::DeviceResources* resource);

	//ID3D11Buffer* getProjectionBuffer() const { return m_pProjectionBuffer; }

private:
	ID3D11VertexShader*		m_pVertexShader = nullptr;
	ID3D11PixelShader*		m_pPlainTexturePixelShader = nullptr;
	ID3D11PixelShader*		m_pPlainColorPixelShader = nullptr;
	ID3D11PixelShader*		m_pBlendPixelShader = nullptr;
	ID3D11InputLayout*		m_pInputLayout = nullptr;
	ID3D11RasterizerState*	m_pRState = nullptr;
	//ID3D11Buffer*			m_pWorldBuffer = nullptr;
	//ID3D11Buffer*			m_pViewBuffer = nullptr;
	//ID3D11Buffer*			m_pProjectionBuffer = nullptr;
	//DirectX::XMMATRIX		m_projectionMatrix;

	ID3D11Buffer*			m_pVertexBuffer;
	ID3D11Buffer*			m_pIndexBuffer;
	ID3D11Buffer*			m_pConstantBuffer;
	//DirectX::XMMATRIX		m_World;
	//DirectX::XMMATRIX		m_View;
	//DirectX::XMMATRIX		m_Projection;
	struct ConstantBuffer
	{
		DirectX::XMMATRIX mWorld;
		DirectX::XMMATRIX mView;
		DirectX::XMMATRIX mProjection;
	};
};

