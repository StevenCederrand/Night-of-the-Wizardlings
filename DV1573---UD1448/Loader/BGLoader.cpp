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
	int vertexSize = vertexCount * (int)8 + 1;

	float* vertices = new float[vertexSize];
	int vertexOffset = 0;

	vertices[0] = (float)vertexCount;
	vertices += 1;

	for (int i = 1; i < vertexCount; i++)
	{
		BGLoading::Vertex tempVertices = meshVert[meshNr].vertices[i];

		vertices[0] = tempVertices.position[0];
		vertices[1] = tempVertices.position[1];
		vertices[2] = tempVertices.position[2];
		vertices += 3;

		vertices[0] = tempVertices.uv[0];
		vertices[1] = tempVertices.uv[1];
		vertices += 2;

		vertices[0] = tempVertices.normal[0];
		vertices[1] = tempVertices.normal[1];
		vertices[2] = tempVertices.normal[2];
		vertices += 3;

		vertexOffset += 8;
	}
	vertices -= vertexOffset;


	return vertices;
}

int* BGLoader::GetFaces(int meshNr)
{
	BGLoading::LoaderMesh tempMesh = mesh[meshNr];
	int faceCount = tempMesh.vertexCount;
	int faceSize = 3;

	int* faces = new int[faceCount * (int)3];
	int faceOffset = 0;

	for (int i = 0; i < faceCount; i++)
	{
		BGLoading::Face tempFace = meshFace[meshNr].faces[i];

		faces[0] = tempFace.indices[0];
		faces[1] = tempFace.indices[1];
		faces[2] = tempFace.indices[2];
		faces += 3;
		faceOffset += 3;
	}
	faces -= faceOffset;


	return faces;
}

bool BGLoader::LoadMesh(std::string fileName)
{
	Unload();

	std::ifstream binFile(fileName, std::ios::binary);
	if (!binFile)
	{
		logError("Error! Could not find importer file: ", fileName);
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
				binFile.read((char*)&newAni, sizeof(BGLoading::Animation));
				// Apply the data about the animation and
				// Allocate memory for the keyframe vector inside
				newAnimations.animations[a].ani = newAni;
				newAnimations.animations[a].keyFrames.resize(newAni.keyframeCount);
				for (int k = 0; k < newAni.keyframeCount; k++)
				{
					// 3.4.2 Keyframes
					BGLoading::KeyFrame newKey;
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

			logTrace("Mesh " + Meshes[i].name + " imported," + 
				" Vertices: " + std::to_string(Meshes[i].vertices.size()) +
				" Faces: " + std::to_string(Meshes[i].faces.size())
				);
		}

		for (int i = 0; i < fileHeader.materialCount; i++)
		{
			binFile.read((char*)&material[i], sizeof(BGLoading::PhongMaterial));
		}

		for (int i = 0; i < fileHeader.dirLightCount; i++)
		{
			binFile.read((char*)&dirLight[i], sizeof(float) * 10);
		}
		for (int i = 0; i < fileHeader.pointLightCount; i++)
		{
			binFile.read((char*)&pointLight[i], sizeof(float) * 7);
		}
	}
	binFile.close();



	return true;
}
