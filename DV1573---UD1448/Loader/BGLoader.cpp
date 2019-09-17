#include <Pch/Pch.h>
#include "BGLoader.h"

BGLoader::BGLoader()
{
	fileName = "";

	meshGroup = nullptr;
	material = nullptr;
	loaderMesh = nullptr;
	dirLight = nullptr;
	pointLight = nullptr;

}

BGLoader::BGLoader(std::string fileName)
{
	this->fileName = fileName;

	meshGroup = nullptr;
	material = nullptr;
	loaderMesh = nullptr;
	dirLight = nullptr;
	pointLight = nullptr;

	LoadMesh(fileName);
}

BGLoader::~BGLoader()
{
	Unload();
}

void BGLoader::Unload()
{	
	animationsD.clear();
	skeletonsD.clear();
	meshVert.clear();
	meshFace.clear();


	if (meshGroup)
		delete[] meshGroup;
	if (material)
		delete[] material;
	if (loaderMesh)	
		delete[] loaderMesh;
	if (dirLight)
		delete[] dirLight;
	if (pointLight)
		delete[] pointLight;
	if (vertices)
		delete[] vertices;
	if (faces)
		delete[] faces;


	meshGroup = nullptr;
	material = nullptr;
	loaderMesh = nullptr;
	dirLight = nullptr;
	pointLight = nullptr;


	vertices = nullptr;
	faces = nullptr;

	fileHeader.meshCount = 0;
	fileHeader.groupCount = 0;
	fileHeader.materialCount = 0;
	fileHeader.pointLightCount = 0;
	fileHeader.dirLightCount = 0;


	fileName = "";
}

const float* BGLoader::GetVertices(int meshNr)
{
	BGLoading::LoaderMesh tempMesh = loaderMesh[meshNr];
	int vertexCount = tempMesh.vertexCount;

	int vertexSize = vertexCount * 8 + 1;
	vertices = new float[vertexSize];

	int j = 0;
	for (int i = 0; i < vertexCount; i++)
	{
		BGLoading::Vertex tempVertices = meshVert[meshNr][i];

		vertices[j] = (float)tempVertices.position[0]; j++;
		vertices[j] = (float)tempVertices.position[1]; j++;
		vertices[j] = (float)tempVertices.position[2]; j++;

		vertices[j] = (float)tempVertices.uv[0]; j++;
		vertices[j] = (float)tempVertices.uv[1]; j++;

		vertices[j] = (float)tempVertices.normal[0]; j++;
		vertices[j] = (float)tempVertices.normal[1]; j++;
		vertices[j] = (float)tempVertices.normal[2]; j++;

	}


	return vertices;
}

const float* BGLoader::GetVertices()
{
	BGLoading::LoaderMesh tempMesh = loaderMesh[0];
	int vertexCount = tempMesh.vertexCount;

	int vertexSize = vertexCount * 8 + 1;
	vertices = new float[vertexSize];

	int j = 0;
	for (int i = 0; i < vertexCount; i++)
	{
		BGLoading::Vertex tempVertices = meshVert[0][i];

		vertices[j] = (float)tempVertices.position[0]; j++;
		vertices[j] = (float)tempVertices.position[1]; j++;
		vertices[j] = (float)tempVertices.position[2]; j++;

		vertices[j] = (float)tempVertices.uv[0]; j++;
		vertices[j] = (float)tempVertices.uv[1]; j++;

		vertices[j] = (float)tempVertices.normal[0]; j++;
		vertices[j] = (float)tempVertices.normal[1]; j++;
		vertices[j] = (float)tempVertices.normal[2]; j++;

	}


	return vertices;
}

const int* BGLoader::GetFaces(int meshNr)
{
	BGLoading::LoaderMesh tempMesh = loaderMesh[meshNr];
	int faceCount = tempMesh.faceCount;

	int faceSize = faceCount * 3;
	faces = new int[faceSize];

	int j = 0;
	for (int i = 0; i < faceCount; i++)
	{
		BGLoading::Face tempFace = meshFace[meshNr][i];

		faces[j] = (int)tempFace.indices[0]; j++;
		faces[j] = (int)tempFace.indices[1]; j++;
		faces[j] = (int)tempFace.indices[2]; j++;
	}


	return faces;
}

const int* BGLoader::GetFaces()
{
	BGLoading::LoaderMesh tempMesh = loaderMesh[0];
	int faceCount = tempMesh.faceCount;

	int faceSize = faceCount * 3;
	faces = new int[faceSize];

	int j = 0;
	for (int i = 0; i < faceCount; i++)
	{
		BGLoading::Face tempFace = meshFace[0][i];

		faces[j] = (int)tempFace.indices[0]; j++;
		faces[j] = (int)tempFace.indices[1]; j++;
		faces[j] = (int)tempFace.indices[2]; j++;
	}


	return faces;
}

const Material BGLoader::GetMaterial(int meshID)
{
	Material tempMat;
	tempMat.name = (std::string)material[meshID].name;

	tempMat.ambient = glm::vec3(*material[meshID].ambient);
	tempMat.diffuse = glm::vec3(*material[meshID].diffuse);
	tempMat.specular = glm::vec3(*material[meshID].specular);
	tempMat.ambient = glm::vec3(*material[meshID].ambient);


	return tempMat;
}

const Material BGLoader::GetMaterial()
{
	Material tempMat;
	tempMat.name = (std::string)material[0].name;

	tempMat.ambient = glm::vec3(*material[0].ambient);
	tempMat.diffuse = glm::vec3(*material[0].diffuse);
	tempMat.specular = glm::vec3(*material[0].specular);
	tempMat.ambient = glm::vec3(*material[0].ambient);


	return tempMat;
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
		loaderMesh = new BGLoading::LoaderMesh[fileHeader.meshCount];
		meshVert.resize(fileHeader.meshCount);
		meshFace.resize(fileHeader.meshCount);
		material = new BGLoading::PhongMaterial[fileHeader.materialCount];
		dirLight = new BGLoading::DirLight[fileHeader.dirLightCount];
		pointLight = new BGLoading::PointLight[fileHeader.pointLightCount];

		
		// Fill data
		for (int i = 0; i < fileHeader.groupCount; i++)
		{
			binFile.read((char*)&meshGroup[i], sizeof(BGLoading::MeshGroup));
		}

		for (int i = 0; i < fileHeader.meshCount; i++)
		{
			binFile.read((char*)&loaderMesh[i], sizeof(BGLoading::LoaderMesh));

			// Allocate memory for the array of vertex arrays
			meshVert[i] = new BGLoading::Vertex[loaderMesh[i].vertexCount];

			//Read data for all the vertices, this includes pos, uv, normals, tangents and binormals.
			for (int v = 0; v < loaderMesh[i].vertexCount; v++)
			{
				binFile.read((char*)&meshVert[i][v], sizeof(BGLoading::Vertex));
			}

			// Allocate memory for the array of face arrays
			meshFace[i] = new BGLoading::Face[loaderMesh[i].faceCount];
			//Read data for all the vertices, this includes pos, uv, normals, tangents and binormals.
			for (int f = 0; f < loaderMesh[i].faceCount; f++)
			{
				binFile.read((char*)&meshFace[i][f], sizeof(BGLoading::Face));
			}

			// 3.3 Joints
			BGLoading::MeshSkeleton newSkeleton;
			// Allocate memory for the joint vector inside
			newSkeleton.joint.resize(loaderMesh[i].skeleton.jointCount);
			for (int j = 0; j < loaderMesh[i].skeleton.jointCount; j++)
			{
				BGLoading::Joint newJoint;
				binFile.read((char*)&newJoint, sizeof(BGLoading::Joint));
				newSkeleton.joint[j] = newJoint;
			}


			BGLoading::MeshAnis newAnimations;
			// Allocate memory for the animation vector inside
			newAnimations.animations.resize(loaderMesh[i].skeleton.aniCount);
			for (int a = 0; a < loaderMesh[i].skeleton.aniCount; a++)
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

			logTrace("Mesh " + (std::string)loaderMesh[i].name + " imported," +
				" Vertices: " + std::to_string(loaderMesh[i].vertexCount) +
				" Faces: " + std::to_string(loaderMesh[i].faceCount)
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
