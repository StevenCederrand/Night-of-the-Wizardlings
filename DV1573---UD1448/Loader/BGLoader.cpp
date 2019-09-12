#include <Pch/Pch.h>
#include "BGLoader.h"

BGLoader::BGLoader()
{
	fileName = "";

	meshGroup = nullptr;
	material = nullptr;
	mesh = nullptr;
	dirLight = nullptr;
	pointLight = nullptr;
	meshVert = nullptr;
	meshFace = nullptr;
}

BGLoader::BGLoader(std::string fileName)
{
	this->fileName = fileName;

	meshGroup = nullptr;
	material = nullptr;
	mesh = nullptr;
	dirLight = nullptr;
	pointLight = nullptr;
	meshVert = nullptr;
	meshFace = nullptr;

	LoadMesh(fileName);
}

BGLoader::~BGLoader()
{
	Unload();
}

void BGLoader::Unload()
{	
	Meshes.clear();
	animationsD.clear();
	skeletonsD.clear();


	if (meshGroup)
		delete[] meshGroup;
	if (material)
		delete[] material;
	if (mesh)
	{
		for (int i = 0; i < fileHeader.meshCount; i++)
		{
			delete[] meshVert[i].vertices;
			delete[] meshFace[i].faces;
		}
			
		delete[] mesh;
	}
	if (dirLight)
		delete[] dirLight;
	if (pointLight)
		delete[] pointLight;
	if(meshVert)
		delete[] meshVert;
	if(meshFace)
		delete[] meshFace;


	meshGroup = nullptr;
	material = nullptr;
	mesh = nullptr;
	dirLight = nullptr;
	pointLight = nullptr;
	meshVert = nullptr;
	meshFace = nullptr;

	fileHeader.meshCount = 0;
	fileHeader.groupCount = 0;
	fileHeader.materialCount = 0;
	fileHeader.pointLightCount = 0;
	fileHeader.dirLightCount = 0;


	fileName = "";
	errorMessage = "";
}

char* BGLoader::GetMeshName(int meshID)
{
	char* meshName = mesh[meshID].name;

	return meshName;
}

float* BGLoader::GetVertices(int meshNr)
{
	BGLoading::LoaderMesh tempMesh = mesh[meshNr];
	int vertexCount = tempMesh.vertexCount;
	size_t vertexSize = vertexCount * 8 + 1;

	float* vertices = new float[vertexSize];
	int vertexOffset = 0;

	vertices[0] = (float)vertexCount;
	vertices += 1;

	for (int i = 1; i < vertexCount; i++)
	{
		BGLoading::Vertex tempVertices = meshVert[0].vertices[i];

		vertices[0] = tempVertices.position[0];
		vertices[1] = tempVertices.position[1];
		vertices[2] = tempVertices.position[2];
		vertices += 3;

		vertices[0] = tempVertices.uv[0];
		vertices[1] = tempVertices.uv[1];
		vertices += 2;

		vertices[0] = tempVertices.normal[0];
		vertices[0] = tempVertices.normal[1];
		vertices[0] = tempVertices.normal[2];
		vertices += 3;

		vertexOffset += 8;
	}
	vertices -= vertexOffset;


	return vertices;
}

float* BGLoader::GetFaces(int meshId)
{
	return nullptr;
}

std::string BGLoader::GetLastError()
{
	return errorMessage;
}

bool BGLoader::LoadMesh(std::string fileName)
{
	Unload();

	std::ifstream binFile(fileName, std::ios::binary);
	if (!binFile)
	{
		std::cout << "Error! Could not find importer file: " << (char*)& fileName << std::endl;
		errorMessage = "Error: Could not find importer file: " + fileName;
		return false;
	}
	else
	{
		// Allocate memory based on header
		binFile.read((char*)&fileHeader, sizeof(BGLoading::BGHeader));
		meshGroup = new BGLoading::MeshGroup[fileHeader.groupCount];
		mesh = new BGLoading::LoaderMesh[fileHeader.meshCount];
		meshVert = new BGLoading::MeshVert[fileHeader.meshCount];
		meshFace = new BGLoading::MeshFace[fileHeader.meshCount];
		material = new BGLoading::PhongMaterial[fileHeader.materialCount];
		dirLight = new BGLoading::DirLight[fileHeader.dirLightCount];
		pointLight = new BGLoading::PointLight[fileHeader.pointLightCount];

		
		Meshes.resize(fileHeader.meshCount);

		// Fill data
		for (int i = 0; i < fileHeader.groupCount; i++)
		{
			binFile.read((char*)&meshGroup[i], sizeof(BGLoading::MeshGroup));
		}

		for (int i = 0; i < fileHeader.meshCount; i++)
		{

			binFile.read((char*)&mesh[i], sizeof(BGLoading::LoaderMesh));


			Meshes[i].name = (std::string)mesh[i].name;
			for (int p = 0; p < 3; p++)
			{
				Meshes[i].translation[p] = mesh[i].translation[p];
				Meshes[i].rotation[p] = mesh[i].rotation[p];
				Meshes[i].scale[p] = mesh[i].scale[p];
			}

			// Allocate memory for the array of vertex arrays
			meshVert[i].vertices = new BGLoading::Vertex[mesh[i].vertexCount];
			Meshes[i].vertices.resize(mesh[i].vertexCount);

			//Read data for all the vertices, this includes pos, uv, normals, tangents and binormals.
			for (int v = 0; v < mesh[i].vertexCount; v++)
			{
				binFile.read((char*)&meshVert[i].vertices[v], sizeof(BGLoading::Vertex));
				Meshes[i].vertices[v] = meshVert[i].vertices[v];
			}

			// Allocate memory for the array of face arrays
			meshFace[i].faces = new BGLoading::Face[mesh[i].faceCount];
			Meshes[i].faces.resize(mesh[i].faceCount);
			//Read data for all the vertices, this includes pos, uv, normals, tangents and binormals.
			for (int f = 0; f < mesh[i].faceCount; f++)
			{
				binFile.read((char*)&meshFace[i].faces[f], sizeof(BGLoading::Face));
			}

			// 3.3 Joints
			BGLoading::MeshSkeleton newSkeleton;
			// Allocate memory for the joint vector inside
			newSkeleton.joint.resize(mesh[i].skeleton.jointCount);
			for (int j = 0; j < mesh[i].skeleton.jointCount; j++)
			{
				BGLoading::Joint newJoint;
				std::cout << "Writing joint " << j << "..." << std::endl;
				binFile.read((char*)&newJoint, sizeof(BGLoading::Joint));
				newSkeleton.joint[j] = newJoint;
			}


			BGLoading::MeshAnis newAnimations;
			// Allocate memory for the animation vector inside
			newAnimations.animations.resize(mesh[i].skeleton.aniCount);
			for (int a = 0; a < mesh[i].skeleton.aniCount; a++)
			{
				// 3.4.1 Animations
				BGLoading::Animation newAni;
				std::cout << "Writing animation " << a << "..." << std::endl;
				binFile.read((char*)&newAni, sizeof(BGLoading::Animation));
				// Apply the data about the animation and
				// Allocate memory for the keyframe vector inside
				newAnimations.animations[a].ani = newAni;
				newAnimations.animations[a].keyFrames.resize(newAni.keyframeCount);
				for (int k = 0; k < newAni.keyframeCount; k++)
				{
					// 3.4.2 Keyframes
					BGLoading::KeyFrame newKey;
					std::cout << "Writing keyframe " << k << "..." << std::endl;
					binFile.read((char*)&newKey, sizeof(BGLoading::KeyFrame));
					// Apply the data about the keyframe and
					// Allocate memory for the transform vector inside 
					newAnimations.animations[a].keyFrames[k].key = newKey;
					newAnimations.animations[a].keyFrames[k].transforms.resize(newKey.transformCount);
					for (int t = 0; t < newKey.transformCount; t++)
					{
						// 3.4.3 Transforms
						BGLoading::Transform newTr;
						binFile.read((char*)&newTr, sizeof(BGLoading::Transform));
						// Apply the data about the transform
						newAnimations.animations[a].keyFrames[k].transforms[t].t = newTr;
					}
				}
			}

			animationsD.push_back(newAnimations);
			skeletonsD.push_back(newSkeleton);



		}

		for (int i = 0; i < fileHeader.materialCount; i++)
		{
			binFile.read((char*)&material[i], sizeof(BGLoading::PhongMaterial));

			std::cout << "Albedo name: " << material[i].albedo << std::endl;
			std::cout << "Normal Name: " << material[i].normal << std::endl;
		}

		for (int i = 0; i < fileHeader.dirLightCount; i++)
		{
			binFile.read((char*)&dirLight[i], sizeof(float) * 10);
			std::cout << "Directional light" << std::endl;
			std::cout << "Position: " << dirLight[i].position[0] << "  " << dirLight[i].position[1]
				<< "  " << dirLight[i].position[2] << std::endl;

			std::cout << "Rotation: " << dirLight[i].rotation[0] << "  " << dirLight[i].rotation[1]
				<< "  " << dirLight[i].rotation[2] << std::endl;

			std::cout << "Color: " << dirLight[i].color[0] << "  " << dirLight[i].color[1]
				<< "  " << dirLight[i].color[2] << std::endl;

			std::cout << "Intensity: " << dirLight[i].intensity << std::endl;
			//Pos xyz/ rotation xyz/ color xyz/ intensity
		}
		for (int i = 0; i < fileHeader.pointLightCount; i++)
		{
			binFile.read((char*)&pointLight[i], sizeof(float) * 7);
			std::cout << "Point light" << std::endl;
			std::cout << "Position: " << pointLight[i].position[0] << "  " << pointLight[i].position[1]
				<< "  " << pointLight[i].position[2] << std::endl;

			std::cout << "Color: " << pointLight[i].color[0] << "  " << pointLight[i].color[1]
				<< "  " << pointLight[i].color[2] << std::endl;

			std::cout << "Intensity: " << pointLight[i].intensity << std::endl;
			//Pos xyz/ color xyz/ intensity
		}
	}
	binFile.close();


	return true;
}
