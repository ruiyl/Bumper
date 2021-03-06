#include "pch.h"
#include "OBJ_Loader.h"

namespace objl {
	Vector3 math::CrossV3(const Vector3 a, const Vector3 b)
	{
		return Vector3(a.Y * b.Z - a.Z * b.Y,
			a.Z * b.X - a.X * b.Z,
			a.X * b.Y - a.Y * b.X);
	}
	float math::MagnitudeV3(const Vector3 in)
	{
		return (sqrtf(powf(in.X, 2) + powf(in.Y, 2) + powf(in.Z, 2)));
	}
	float math::DotV3(const Vector3 a, const Vector3 b)
	{
		return (a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z);
	}
	float math::AngleBetweenV3(const Vector3 a, const Vector3 b)
	{
		float angle = DotV3(a, b);
		angle /= (MagnitudeV3(a) * MagnitudeV3(b));
		return angle = acosf(angle);
	}
	Vector3 algorithm::operator*(const float& left, const Vector3& right)
	{
		return Vector3(right.X * left, right.Y * left, right.Z * left);
	}
	bool algorithm::inTriangle(Vector3 point, Vector3 tri1, Vector3 tri2, Vector3 tri3)
	{
		// Starting vars
		Vector3 u = tri2 - tri1;
		Vector3 v = tri3 - tri1;
		Vector3 w = point - tri1;
		Vector3 n = math::CrossV3(u, v);

		float y = (math::DotV3(math::CrossV3(u, w), n) / math::DotV3(n, n));
		float b = (math::DotV3(math::CrossV3(u, w), n) / math::DotV3(n, n));
		float a = 1 - y - b;

		// Projected point
		Vector3  p = (a * tri1) + (b * tri2) + (y * tri3);

		if (a >= 0 && a <= 1
			&& b >= 0 && b <= 1
			&& y >= 0 && y <= 1)
		{
			return true;
		}
		else
			return false;
	}
	Loader Loader::g_loader;
	Loader::Loader()
	{

	}
	Loader::~Loader()
	{
		LoadedMeshes.clear();
	}
	bool Loader::LoadFile(std::wstring Path)
	{
		// If the file is not an .obj file return false
		if (Path.substr(Path.size() - 4, 4) != L".obj")
			return false;


		std::wifstream file(Path);

		if (!file.is_open())
			return false;

		LoadedMeshes.clear();
		LoadedVertices.clear();
		LoadedIndices.clear();

		std::vector<Vector3> Positions;
		std::vector<Vector2> TCoords;
		std::vector<Vector3> Normals;

		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;

		std::vector<std::wstring> MeshMatNames;

		bool listening = false;
		std::wstring meshname;

		Mesh tempMesh;

#ifdef OBJL_CONSOLE_OUTPUT
		const unsigned int outputEveryNth = 1000;
		unsigned int outputIndicator = outputEveryNth;
#endif

		std::wstring curline;
		while (std::getline(file, curline))
		{
#ifdef OBJL_CONSOLE_OUTPUT
			if ((outputIndicator = ((outputIndicator + 1) % outputEveryNth)) == 1)
			{
				if (!meshname.empty())
				{
					std::wcout
						<< L"\r- " << meshname
						<< L"\t| vertices > " << Positions.size()
						<< L"\t| texcoords > " << TCoords.size()
						<< L"\t| normals > " << Normals.size()
						<< L"\t| triangles > " << (Vertices.size() / 3)
						<< (!MeshMatNames.empty() ? L"\t| material: " + MeshMatNames.back() : L"");
				}
			}
#endif

			// Generate a Mesh Object or Prepare for an object to be created
			if (algorithm::firstToken(curline) == L"o" || algorithm::firstToken(curline) == L"g" || curline[0] == L'g')
			{
				if (!listening)
				{
					listening = true;

					if (algorithm::firstToken(curline) == L"o" || algorithm::firstToken(curline) == L"g")
					{
						meshname = algorithm::tail(curline);
					}
					else
					{
						meshname = L"unnamed";
					}
				}
				else
				{
					// Generate the mesh to put into the array

					if (!Indices.empty() && !Vertices.empty())
					{
						// Create Mesh
						tempMesh = Mesh(Vertices, Indices);
						tempMesh.MeshName = meshname;

						// Insert Mesh
						LoadedMeshes.push_back(tempMesh);

						// Cleanup
						Vertices.clear();
						Indices.clear();
						meshname.clear();

						meshname = algorithm::tail(curline);
					}
					else
					{
						if (algorithm::firstToken(curline) == L"o" || algorithm::firstToken(curline) == L"g")
						{
							meshname = algorithm::tail(curline);
						}
						else
						{
							meshname = L"unnamed";
						}
					}
				}
#ifdef OBJL_CONSOLE_OUTPUT
				std::wcout << std::endl;
				outputIndicator = 0;
#endif
			}
			// Generate a Vertex Position
			if (algorithm::firstToken(curline) == L"v")
			{
				std::vector<std::wstring> spos;
				Vector3 vpos;
				algorithm::split(algorithm::tail(curline), spos, L" ");

				vpos.X = std::stof(spos[0]);
				vpos.Y = std::stof(spos[1]);
				vpos.Z = std::stof(spos[2]);

				Positions.push_back(vpos);
			}
			// Generate a Vertex Texture Coordinate
			if (algorithm::firstToken(curline) == L"vt")
			{
				std::vector<std::wstring> stex;
				Vector2 vtex;
				algorithm::split(algorithm::tail(curline), stex, L" ");

				vtex.X = std::stof(stex[0]);
				vtex.Y = std::stof(stex[1]);

				TCoords.push_back(vtex);
			}
			// Generate a Vertex Normal;
			if (algorithm::firstToken(curline) == L"vn")
			{
				std::vector<std::wstring> snor;
				Vector3 vnor;
				algorithm::split(algorithm::tail(curline), snor, L" ");

				vnor.X = std::stof(snor[0]);
				vnor.Y = std::stof(snor[1]);
				vnor.Z = std::stof(snor[2]);

				Normals.push_back(vnor);
			}
			// Generate a Face (vertices & indices)
			if (algorithm::firstToken(curline) == L"f")
			{
				// Generate the vertices
				std::vector<Vertex> vVerts;
				GenVerticesFromRawOBJ(vVerts, Positions, TCoords, Normals, curline);

				// Add Vertices
				for (int i = 0; i < int(vVerts.size()); i++)
				{
					Vertices.push_back(vVerts[i]);

					LoadedVertices.push_back(vVerts[i]);
				}

				std::vector<unsigned int> iIndices;

				VertexTriangluation(iIndices, vVerts);

				// Add Indices
				for (int i = 0; i < int(iIndices.size()); i++)
				{
					unsigned int indnum = (unsigned int)((Vertices.size()) - vVerts.size()) + iIndices[i];
					Indices.push_back(indnum);

					indnum = (unsigned int)((LoadedVertices.size()) - vVerts.size()) + iIndices[i];
					LoadedIndices.push_back(indnum);

				}
			}
			// Get Mesh Material Name
			if (algorithm::firstToken(curline) == L"usemtl")
			{
				MeshMatNames.push_back(algorithm::tail(curline));

				// Create new Mesh, if Material changes within a group
				if (!Indices.empty() && !Vertices.empty())
				{
					// Create Mesh
					tempMesh = Mesh(Vertices, Indices);
					tempMesh.MeshName = meshname;
					int i = 2;
					while (1) {
						tempMesh.MeshName = meshname + L"_" + std::to_wstring(i);

						for (auto &m : LoadedMeshes)
							if (m.MeshName == tempMesh.MeshName)
								continue;
						break;
					}

					// Insert Mesh
					LoadedMeshes.push_back(tempMesh);

					// Cleanup
					Vertices.clear();
					Indices.clear();
				}

#ifdef OBJL_CONSOLE_OUTPUT
				outputIndicator = 0;
#endif
			}
			// Load Materials
			if (algorithm::firstToken(curline) == L"mtllib")
			{
				// Generate LoadedMaterial

				// Generate a path to the material file
				std::vector<std::wstring> temp;
				algorithm::split(Path, temp, L"/");

				std::wstring pathtomat = L"";

				if (temp.size() != 1)
				{
					for (int i = 0; i < temp.size() - 1; i++)
					{
						pathtomat += temp[i] + L"/";
					}
				}


				pathtomat += algorithm::tail(curline);

#ifdef OBJL_CONSOLE_OUTPUT
				std::wcout << std::endl << L"- find materials in: " << pathtomat << std::endl;
#endif

				// Load Materials
				LoadMaterials(pathtomat);
			}
		}

#ifdef OBJL_CONSOLE_OUTPUT
		std::wcout << std::endl;
#endif

		// Deal with last mesh

		if (!Indices.empty() && !Vertices.empty())
		{
			// Create Mesh
			tempMesh = Mesh(Vertices, Indices);
			tempMesh.MeshName = meshname;

			// Insert Mesh
			LoadedMeshes.push_back(tempMesh);
		}

		file.close();

		// Set Materials for each Mesh
		for (int i = 0; i < MeshMatNames.size(); i++)
		{
			std::wstring matname = MeshMatNames[i];

			// Find corresponding material name in loaded materials
			// when found copy material variables into mesh material
			for (int j = 0; j < LoadedMaterials.size(); j++)
			{
				if (LoadedMaterials[j].name == matname)
				{
					LoadedMeshes[i].MeshMaterial = LoadedMaterials[j];
					break;
				}
			}
		}

		if (LoadedMeshes.empty() && LoadedVertices.empty() && LoadedIndices.empty())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	void Loader::GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
		const std::vector<Vector3>& iPositions,
		const std::vector<Vector2>& iTCoords,
		const std::vector<Vector3>& iNormals,
		std::wstring icurline)
	{
		std::vector<std::wstring> sface, svert;
		Vertex vVert;
		algorithm::split(algorithm::tail(icurline), sface, L" ");

		bool noNormal = false;

		// For every given vertex do this
		for (int i = 0; i < int(sface.size()); i++)
		{
			// See What type the vertex is.
			int vtype = 0;

			algorithm::split(sface[i], svert, L"/");

			// Check for just position - v1
			if (svert.size() == 1)
			{
				// Only position
				vtype = 1;
			}

			// Check for position & texture - v1/vt1
			if (svert.size() == 2)
			{
				// Position & Texture
				vtype = 2;
			}

			// Check for Position, Texture and Normal - v1/vt1/vn1
			// or if Position and Normal - v1//vn1
			if (svert.size() == 3)
			{
				if (svert[1] != L"")
				{
					// Position, Texture, and Normal
					vtype = 4;
				}
				else
				{
					// Position & Normal
					vtype = 3;
				}
			}

			// Calculate and store the vertex
			switch (vtype)
			{
			case 1: // P
			{
				vVert.Position = algorithm::getElement(iPositions, svert[0]);
				vVert.TextureCoordinate = Vector2(0, 0);
				noNormal = true;
				oVerts.push_back(vVert);
				break;
			}
			case 2: // P/T
			{
				vVert.Position = algorithm::getElement(iPositions, svert[0]);
				vVert.TextureCoordinate = algorithm::getElement(iTCoords, svert[1]);
				noNormal = true;
				oVerts.push_back(vVert);
				break;
			}
			case 3: // P//N
			{
				vVert.Position = algorithm::getElement(iPositions, svert[0]);
				vVert.TextureCoordinate = Vector2(0, 0);
				vVert.Normal = algorithm::getElement(iNormals, svert[2]);
				oVerts.push_back(vVert);
				break;
			}
			case 4: // P/T/N
			{
				vVert.Position = algorithm::getElement(iPositions, svert[0]);
				vVert.TextureCoordinate = algorithm::getElement(iTCoords, svert[1]);
				vVert.Normal = algorithm::getElement(iNormals, svert[2]);
				oVerts.push_back(vVert);
				break;
			}
			default:
			{
				break;
			}
			}
		}

		// take care of missing normals
		// these may not be truly acurate but it is the 
		// best they get for not compiling a mesh with normals	
		if (noNormal)
		{
			Vector3 A = oVerts[0].Position - oVerts[1].Position;
			Vector3 B = oVerts[2].Position - oVerts[1].Position;

			Vector3 normal = math::CrossV3(A, B);

			for (int i = 0; i < int(oVerts.size()); i++)
			{
				oVerts[i].Normal = normal;
			}
		}
	}
	void Loader::VertexTriangluation(std::vector<unsigned int>& oIndices,
		const std::vector<Vertex>& iVerts)
	{
		// If there are 2 or less verts,
		// no triangle can be created,
		// so exit
		if (iVerts.size() < 3)
		{
			return;
		}
		// If it is a triangle no need to calculate it
		if (iVerts.size() == 3)
		{
			oIndices.push_back(0);
			oIndices.push_back(1);
			oIndices.push_back(2);
			return;
		}

		// Create a list of vertices
		std::vector<Vertex> tVerts = iVerts;

		while (true)
		{
			// For every vertex
			for (int i = 0; i < int(tVerts.size()); i++)
			{
				// pPrev = the previous vertex in the list
				Vertex pPrev;
				if (i == 0)
				{
					pPrev = tVerts[tVerts.size() - 1];
				}
				else
				{
					pPrev = tVerts[i - 1];
				}

				// pCur = the current vertex;
				Vertex pCur = tVerts[i];

				// pNext = the next vertex in the list
				Vertex pNext;
				if (i == tVerts.size() - 1)
				{
					pNext = tVerts[0];
				}
				else
				{
					pNext = tVerts[i + 1];
				}

				// Check to see if there are only 3 verts left
				// if so this is the last triangle
				if (tVerts.size() == 3)
				{
					// Create a triangle from pCur, pPrev, pNext
					for (int j = 0; j < int(tVerts.size()); j++)
					{
						if (iVerts[j].Position == pCur.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pPrev.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pNext.Position)
							oIndices.push_back(j);
					}

					tVerts.clear();
					break;
				}
				if (tVerts.size() == 4)
				{
					// Create a triangle from pCur, pPrev, pNext
					for (int j = 0; j < int(iVerts.size()); j++)
					{
						if (iVerts[j].Position == pCur.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pPrev.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pNext.Position)
							oIndices.push_back(j);
					}

					Vector3 tempVec;
					for (int j = 0; j < int(tVerts.size()); j++)
					{
						if (tVerts[j].Position != pCur.Position
							&& tVerts[j].Position != pPrev.Position
							&& tVerts[j].Position != pNext.Position)
						{
							tempVec = tVerts[j].Position;
							break;
						}
					}

					// Create a triangle from pCur, pPrev, pNext
					for (int j = 0; j < int(iVerts.size()); j++)
					{
						if (iVerts[j].Position == pPrev.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == pNext.Position)
							oIndices.push_back(j);
						if (iVerts[j].Position == tempVec)
							oIndices.push_back(j);
					}

					tVerts.clear();
					break;
				}

				// If Vertex is not an interior vertex
				float angle = math::AngleBetweenV3(pPrev.Position - pCur.Position, pNext.Position - pCur.Position) * (180 / 3.14159265359);
				if (angle <= 0 && angle >= 180)
					continue;

				// If any vertices are within this triangle
				bool inTri = false;
				for (int j = 0; j < int(iVerts.size()); j++)
				{
					if (algorithm::inTriangle(iVerts[j].Position, pPrev.Position, pCur.Position, pNext.Position)
						&& iVerts[j].Position != pPrev.Position
						&& iVerts[j].Position != pCur.Position
						&& iVerts[j].Position != pNext.Position)
					{
						inTri = true;
						break;
					}
				}
				if (inTri)
					continue;

				// Create a triangle from pCur, pPrev, pNext
				for (int j = 0; j < int(iVerts.size()); j++)
				{
					if (iVerts[j].Position == pCur.Position)
						oIndices.push_back(j);
					if (iVerts[j].Position == pPrev.Position)
						oIndices.push_back(j);
					if (iVerts[j].Position == pNext.Position)
						oIndices.push_back(j);
				}

				// Delete pCur from the list
				for (int j = 0; j < int(tVerts.size()); j++)
				{
					if (tVerts[j].Position == pCur.Position)
					{
						tVerts.erase(tVerts.begin() + j);
						break;
					}
				}

				// reset i to the start
				// -1 since loop will add 1 to it
				i = -1;
			}

			// if no triangles were created
			if (oIndices.size() == 0)
				break;

			// if no more vertices
			if (tVerts.size() == 0)
				break;
		}
	}
	bool Loader::LoadMaterials(std::wstring path)
	{
		// If the file is not a material file return false
		if (path.substr(path.size() - 4, path.size()) != L".mtl")
			return false;

		std::wifstream file(path);

		// If the file is not found return false
		if (!file.is_open())
			return false;

		Material tempMaterial;

		bool listening = false;

		// Go through each line looking for material variables
		std::wstring curline;
		while (std::getline(file, curline))
		{
			// new material and material name
			if (algorithm::firstToken(curline) == L"newmtl")
			{
				if (!listening)
				{
					listening = true;

					if (curline.size() > 7)
					{
						tempMaterial.name = algorithm::tail(curline);
					}
					else
					{
						tempMaterial.name = L"none";
					}
				}
				else
				{
					// Generate the material

					// Push Back loaded Material
					LoadedMaterials.push_back(tempMaterial);

					// Clear Loaded Material
					tempMaterial = Material();

					if (curline.size() > 7)
					{
						tempMaterial.name = algorithm::tail(curline);
					}
					else
					{
						tempMaterial.name = L"none";
					}
				}
			}
			// Ambient Color
			if (algorithm::firstToken(curline) == L"Ka")
			{
				std::vector<std::wstring> temp;
				algorithm::split(algorithm::tail(curline), temp, L" ");

				if (temp.size() != 3)
					continue;

				tempMaterial.Ka.X = std::stof(temp[0]);
				tempMaterial.Ka.Y = std::stof(temp[1]);
				tempMaterial.Ka.Z = std::stof(temp[2]);
			}
			// Diffuse Color
			if (algorithm::firstToken(curline) == L"Kd")
			{
				std::vector<std::wstring> temp;
				algorithm::split(algorithm::tail(curline), temp, L" ");

				if (temp.size() != 3)
					continue;

				tempMaterial.Kd.X = std::stof(temp[0]);
				tempMaterial.Kd.Y = std::stof(temp[1]);
				tempMaterial.Kd.Z = std::stof(temp[2]);
			}
			// Specular Color
			if (algorithm::firstToken(curline) == L"Ks")
			{
				std::vector<std::wstring> temp;
				algorithm::split(algorithm::tail(curline), temp, L" ");

				if (temp.size() != 3)
					continue;

				tempMaterial.Ks.X = std::stof(temp[0]);
				tempMaterial.Ks.Y = std::stof(temp[1]);
				tempMaterial.Ks.Z = std::stof(temp[2]);
			}
			// Specular Exponent
			if (algorithm::firstToken(curline) == L"Ns")
			{
				tempMaterial.Ns = std::stof(algorithm::tail(curline));
			}
			// Optical Density
			if (algorithm::firstToken(curline) == L"Ni")
			{
				tempMaterial.Ni = std::stof(algorithm::tail(curline));
			}
			// Dissolve
			if (algorithm::firstToken(curline) == L"d")
			{
				tempMaterial.d = std::stof(algorithm::tail(curline));
			}
			// Illumination
			if (algorithm::firstToken(curline) == L"illum")
			{
				tempMaterial.illum = std::stoi(algorithm::tail(curline));
			}
			// Ambient Texture Map
			if (algorithm::firstToken(curline) == L"map_Ka")
			{
				tempMaterial.map_Ka = algorithm::tail(curline);
			}
			// Diffuse Texture Map
			if (algorithm::firstToken(curline) == L"map_Kd")
			{
				tempMaterial.map_Kd = algorithm::tail(curline);
			}
			// Specular Texture Map
			if (algorithm::firstToken(curline) == L"map_Ks")
			{
				tempMaterial.map_Ks = algorithm::tail(curline);
			}
			// Specular Hightlight Map
			if (algorithm::firstToken(curline) == L"map_Ns")
			{
				tempMaterial.map_Ns = algorithm::tail(curline);
			}
			// Alpha Texture Map
			if (algorithm::firstToken(curline) == L"map_d")
			{
				tempMaterial.map_d = algorithm::tail(curline);
			}
			// Bump Map
			if (algorithm::firstToken(curline) == L"map_Bump" || algorithm::firstToken(curline) == L"map_bump" || algorithm::firstToken(curline) == L"bump")
			{
				tempMaterial.map_bump = algorithm::tail(curline);
			}
		}

		// Deal with last material

		// Push Back loaded Material
		LoadedMaterials.push_back(tempMaterial);

		// Test to see if anything was loaded
		// If not return false
		if (LoadedMaterials.empty())
			return false;
		// If so return true
		else
			return true;
	}
}
