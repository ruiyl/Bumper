#pragma once

#include "OBJ_Loader.h"
#include <DirectXMath.h>

namespace Utility {

	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 normal;

		Vertex() : pos(), texCoord(), normal() {}
		Vertex(float x, float y, float z,
			float u, float v,
			float nx, float ny, float nz)
			: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}
		Vertex(float x, float y, float z,
			float u, float v)
			: pos(x, y, z), texCoord(u, v), normal() {}
		Vertex(objl::Vertex& v) :
			pos(v.Position.X, v.Position.Y, v.Position.Z),
			texCoord(v.TextureCoordinate.X, v.TextureCoordinate.Y),
			normal(v.Normal.X, v.Normal.Y, v.Normal.Z) {}
	};

	struct GraphicsResource
	{
		std::vector<ID3D11Buffer*>					vertexBuffer;
		std::vector<ID3D11Buffer*>					indexBuffer;
		std::vector<ID3D11Buffer*>					colorBuffer;
		std::vector<ID3D11ShaderResourceView*>		textureRV;
		std::vector<ID3D11SamplerState*>			samplerState;

		GraphicsResource() {}
		GraphicsResource(GraphicsResource& other) :
			vertexBuffer(other.vertexBuffer),
			indexBuffer(other.indexBuffer),
			colorBuffer(other.colorBuffer),
			textureRV(other.textureRV),
			samplerState(other.samplerState) {}
	};

	struct EntityState
	{
		float	position[3];
		float	rotation[3];
		float	velocity[3];
		float	acceleration[3];
		bool	isDestroyed;

		EntityState() {
			for (int i = 0; i < 3; i++) {
				position[i] = 0.0f;
				rotation[i] = 0.0f;
				velocity[i] = 0.0f;
				acceleration[i] = 0.0f;
			}
			isDestroyed = false;
		}
	};
}