#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include <sstream>
#include <fstream>
#include <istream>
#include "DeviceResources.h"
#include "IComponent.h"

class IEntity;
class Mesh;
class GraphicsComponent : public IComponent
{
public:
	struct MatrixBuffer
	{
		DirectX::XMMATRIX matrix;
	};
	struct ColorBuffer
	{
		DirectX::XMFLOAT4 color;
	};
	struct TextureIndex
	{
		std::wstring textureName;
		int			index;
	};
	GraphicsComponent();
	GraphicsComponent(IEntity* entity);
	~GraphicsComponent();

	HRESULT initialize(DX::DeviceResources* resource);
	HRESULT initialize(DX::DeviceResources* resource, Utility::GraphicsResource& sharedResource);
	HRESULT updateResource(DX::DeviceResources* resource);

	ID3D11Buffer*							getWorldBuffer() const				{ return m_worldBuffer; }
	std::vector<ID3D11Buffer*>&				getVertexBuffer()					{ return m_graphicsResouce.vertexBuffer; }
	std::vector<ID3D11Buffer*>&				getIndexBuffer()					{ return m_graphicsResouce.indexBuffer; }
	std::vector<ID3D11Buffer*>&				getColorBuffer()					{ return m_graphicsResouce.colorBuffer; }
	std::vector<ID3D11ShaderResourceView*>& getTextureResourceView()			{ return m_graphicsResouce.textureRV; }
	std::vector<ID3D11SamplerState*>&		getSamplerState()					{ return m_graphicsResouce.samplerState; }
	Utility::GraphicsResource&				getGraphicsResource()				{ return m_graphicsResouce; }

private:
	Utility::GraphicsResource		m_graphicsResouce;
	std::vector<TextureIndex>		m_textureIndex;

	ID3D11Buffer*					m_worldBuffer = nullptr;
	DirectX::XMFLOAT4X4				m_worldMatrix;
};

