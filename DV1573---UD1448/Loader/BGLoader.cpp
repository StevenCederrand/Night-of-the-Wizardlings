#include <Pch/Pch.h>
#include "BGLoader.h"

BGLoader::BGLoader()
{
	fileName = "";
	meshCount = 0;

	meshGroup = nullptr;
	material = nullptr;
	loaderMesh = nullptr;
	dirLight = nullptr;
	pointLight = nullptr;

}

BGLoader::BGLoader(std::string fileName)
{
	this->fileName = fileName;
	meshCount = 0;

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
	bggMaterials.clear();

	for (std::vector<Vertices> vector : bggVertices)
		vector.clear();
	bggVertices.clear();

	for (std::vector<Face> vector : bggFaces)
		vector.clear();
	bggFaces.clear();

	for (BGLoading::Vertex* v : meshVert)
		delete[] v;
	meshVert.clear();

	for (BGLoading::Face* f : meshFace)
		delete[] f;
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


	meshGroup = nullptr;
	material = nullptr;
	loaderMesh = nullptr;
	dirLight = nullptr;
	pointLight = nullptr;

	fileHeader.meshCount = 0;
	fileHeader.groupCount = 0;
	fileHeader.materialCount = 0;
	fileHeader.pointLightCount = 0;
	fileHeader.dirLightCount = 0;


	fileName = "";
}

void BGLoader::BGFormatData()
{
	meshCount = fileHeader.meshCount;


	bggVertices.resize(meshCount);
	bggFaces.resize(meshCount);
	bggMaterials.resize(meshCount);

	for (int meshId = 0; meshId < meshCount; meshId++)
	{
		bggVertices[meshId].resize(GetVertexCount(meshId));
		bggFaces[meshId].resize(GetFaceCount(meshId));

		// Vertices
		for (size_t v = 0; v < bggVertices[meshId].size(); v++)
		{
			bggVertices[meshId][v].position[0] = meshVert[meshId][v].position[0];
			bggVertices[meshId][v].position[1] = meshVert[meshId][v].position[1];
			bggVertices[meshId][v].position[2] = meshVert[meshId][v].position[2];

			bggVertices[meshId][v].UV[0] = meshVert[meshId][v].uv[0];
			bggVertices[meshId][v].UV[1] = meshVert[meshId][v].uv[1];

			bggVertices[meshId][v].Normals[0] = meshVert[meshId][v].normal[0];
			bggVertices[meshId][v].Normals[1] = meshVert[meshId][v].normal[1];
			bggVertices[meshId][v].Normals[2] = meshVert[meshId][v].normal[2];
		}

		// Faces
		for (size_t f = 0; f < bggFaces[meshId].size(); f++)
		{
			bggFaces[meshId][f].indices[0] = meshFace[meshId][f].indices[0];
			bggFaces[meshId][f].indices[1] = meshFace[meshId][f].indices[1];
			bggFaces[meshId][f].indices[2] = meshFace[meshId][f].indices[2];
		}

		// Material
		bggMaterials[meshId].name = (std::string)material[meshId].name;
		bggMaterials[meshId].ambient = glm::vec3(*material[meshId].ambient);
		bggMaterials[meshId].diffuse = glm::vec3(*material[meshId].diffuse);
		bggMaterials[meshId].specular = glm::vec3(*material[meshId].specular);
		bggMaterials[meshId].ambient = glm::vec3(*material[meshId].ambient);
	}

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

		logTrace("Opening file " + (std::string)fileName + 
			", Meshes: " + std::to_string(fileHeader.meshCount) + 
			", Materials: " + std::to_string(fileHeader.materialCount));

		
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

	// Parse data to a more comfortable format
	BGFormatData();

	return true;
}
