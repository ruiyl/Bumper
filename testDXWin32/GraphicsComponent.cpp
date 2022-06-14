
#include "pch.h"
#include "GraphicsComponent.h"
#include "IEntity.h"
#pragma comment(lib, "C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Lib\\x86\\d3dx11.lib")
#include "C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\D3DX11tex.h"

GraphicsComponent::GraphicsComponent()
{
}

GraphicsComponent::GraphicsComponent(IEntity* entity)
{
	m_textureIndex.resize(0);

	m_pEntity = entity;

	XMFLOAT3 translation = m_pEntity->getPosition();
	XMFLOAT3 rotation = m_pEntity->getRotation();
	XMFLOAT3 scale = m_pEntity->getScale();

	auto translationVector = XMLoadFloat3(&translation);
	auto rotationVector = XMLoadFloat3(&rotation);
	auto scalingVector = XMLoadFloat3(&scale);
	XMStoreFloat4x4(&m_worldMatrix, 
		DirectX::XMMatrixScalingFromVector(scalingVector) *
		DirectX::XMMatrixRotationRollPitchYawFromVector(rotationVector) *
		DirectX::XMMatrixTranslationFromVector(translationVector));
}

GraphicsComponent::~GraphicsComponent()
{
	//m_pWorldBuffer->Release();
	//m_pVertexBuffer->Release();
	//m_pIndexBuffer->Release();
	//for each (ID3D11ShaderResourceView* srv in m_pTextureRV)
	//{
	//	srv->Release();
	//}
	//for each (ID3D11SamplerState* ss in m_pSamplerState)
	//{
	//	ss->Release();
	//}
}

HRESULT GraphicsComponent::initialize(DX::DeviceResources* resource)
{
	HRESULT hr;

	ID3D11Device* device = resource->GetD3DDevice();
	auto meshes = m_pEntity->getMesh();
	int numMesh = m_pEntity->getMeshCount();

	D3D11_BUFFER_DESC bd = {};
	D3D11_SUBRESOURCE_DATA InitData = {};

	DirectX::XMMATRIX transposeWorldMatrix = DirectX::XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(m_worldMatrix);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	InitData.pSysMem = &transposeWorldMatrix;
	hr = device->CreateBuffer(&bd, &InitData, &m_worldBuffer);
	if (FAILED(hr))
		return hr;

	ID3D11SamplerState* ss;
	// Create default Sampler State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampDesc, &ss);
	if (FAILED(hr))
		return hr;
	m_graphicsResouce.samplerState.push_back(ss);

	// Create vertex and index buffer by the number of meshes and put their pointers into vector
	for (int i = 0; i < numMesh; i++) {
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(Utility::Vertex) * meshes[i]->getVertexCount();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ID3D11Buffer* temp;
		InitData.pSysMem = &meshes[i]->getVertices()[0];
		hr = device->CreateBuffer(&bd, &InitData, &temp);
		if (FAILED(hr))
			return hr;
		m_graphicsResouce.vertexBuffer.push_back(temp);

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * meshes[i]->getIndexCount();
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		InitData.pSysMem = &meshes[i]->getIndices()[0];
		hr = device->CreateBuffer(&bd, &InitData, &temp);
		if (FAILED(hr))
			return hr;
		m_graphicsResouce.indexBuffer.push_back(temp);

		// Find texture of each mesh, if found, set its pointer to SRV, if not, load new texture
		bool foundTexture = false;
		objl::Material material = meshes[i]->getMaterial();
		if (material.map_Kd.empty()) {

			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = sizeof(ColorBuffer);
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;
			
			ColorBuffer col;
			col.color = { material.Kd.X, material.Kd.Y, material.Kd.Z, 1.0f };
			InitData.pSysMem = &col;

			ID3D11Buffer* tempCol;
			hr = device->CreateBuffer(&bd, &InitData, &tempCol);
			if (FAILED(hr)) {
				return hr;
			}

			m_graphicsResouce.textureRV.push_back(nullptr);
			m_graphicsResouce.colorBuffer.push_back(tempCol);
		}
		else {
			for (TextureIndex t : m_textureIndex) {
				if (material.name == t.textureName) {
					foundTexture = true;
					m_graphicsResouce.textureRV[i] = m_graphicsResouce.textureRV[t.index];
					break;
				}
			}
			if (!foundTexture) {
				ID3D11ShaderResourceView* tempSRV;
				TextureIndex tempTI;

				try {
					DX::ThrowIfFailed(
						D3DX11CreateShaderResourceViewFromFile(device, material.map_Kd.c_str(), NULL, NULL, &tempSRV, NULL)
					);
				}
				catch (DX::com_exception e) {
					_RPT1(0, "%s\n", e.what());
				}
				m_graphicsResouce.textureRV.push_back(tempSRV);
				m_graphicsResouce.colorBuffer.push_back(nullptr);

				tempTI.index = m_graphicsResouce.textureRV.size() - 1;
				tempTI.textureName = material.map_Kd;
				m_textureIndex.push_back(tempTI);
			}
		}
		m_graphicsResouce.samplerState.push_back(m_graphicsResouce.samplerState[0]);
	}

	return S_OK;
}

HRESULT GraphicsComponent::initialize(DX::DeviceResources * resource, Utility::GraphicsResource & sharedResource)
{
	HRESULT hr;

	ID3D11Device* device = resource->GetD3DDevice();

	D3D11_BUFFER_DESC bd = {};
	D3D11_SUBRESOURCE_DATA InitData = {};

	DirectX::XMMATRIX transposeWorldMatrix = DirectX::XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(m_worldMatrix);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	InitData.pSysMem = &transposeWorldMatrix;
	hr = device->CreateBuffer(&bd, &InitData, &m_worldBuffer);
	if (FAILED(hr))
		return hr;

	m_graphicsResouce = sharedResource;

	return S_OK;
}

HRESULT GraphicsComponent::updateResource(DX::DeviceResources * resource)
{
	auto context = resource->GetD3DDeviceContext();

	XMFLOAT3 translation = m_pEntity->getPosition();
	XMFLOAT3 rotation = m_pEntity->getRotation();
	XMFLOAT3 scale = m_pEntity->getScale();

	auto translationVector = XMLoadFloat3(&translation);
	auto rotationVector = XMLoadFloat3(&rotation);
	auto scalingVector = XMLoadFloat3(&scale);

	XMStoreFloat4x4(&m_worldMatrix,
		DirectX::XMMatrixScalingFromVector(scalingVector) *
		DirectX::XMMatrixRotationRollPitchYawFromVector(rotationVector) *
		DirectX::XMMatrixTranslationFromVector(translationVector));
	
	DirectX::XMMATRIX transposeWorldMatrix = DirectX::XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix));

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	
	//	Disable GPU access to the vertex buffer data.
	context->Map(m_worldBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	
	//	Update the vertex buffer here.
	memcpy(mappedResource.pData, &transposeWorldMatrix, sizeof(transposeWorldMatrix));

	//	Reenable GPU access to the vertex buffer data.
	context->Unmap(m_worldBuffer, 0);

	return S_OK;
}
