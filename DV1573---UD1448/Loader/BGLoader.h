#pragma once
#include <Pch/Pch.h>
#include "LoaderFormat.h"

//Example usage:
/*
	BGLoader testLoader;
	testLoader.LoadMesh("Assets/Meshes/SexyCube.meh");
	testLoader.Unload();
*/


// Temporary Mesh Data, might delete 
struct BGMesh
{
	std::string name;

	float translation[3];
	float rotation[3];
	float scale[3];

	std::vector<BGLoading::Vertex> vertices;
	std::vector<BGLoading::Face> faces;
};

class BGLoader
{
private:
	std::string	fileName;

	std::vector<BGMesh> Meshes;

	// Temporary paths to load data into
	BGLoading::BGHeader	fileHeader;
	BGLoading::MeshGroup* meshGroup;
	BGLoading::PhongMaterial* material;
	BGLoading::LoaderMesh* mesh;
	BGLoading::DirLight* dirLight;
	BGLoading::PointLight* pointLight;
	BGLoading::MeshVert* meshVert;
	BGLoading::MeshFace* meshFace;

	std::vector<BGLoading::MeshAnis> animationsD;
	std::vector<BGLoading::MeshSkeleton> skeletonsD;

public:
	BGLoader();
	BGLoader(std::string filesName);		// Default constructor to directly load a file
	~BGLoader();
	void Unload();

	bool LoadMesh(std::string fileName);	// Load a file
	std::string GetFileName() { return fileName; }

	int GetMeshCount() { return fileHeader.meshCount; }	// Get meshes
	char* GetMeshName(int meshId);

	// Returns all the verices in format posX/posY/PosZ/uvX/uvY/normalX/normalY/normalZ/posX/posY.... 
	// Next vertex starts where the last vertex ends (8 floats)
	float* GetVertices(int meshId);
	int GetVertexCount(int meshId) { return mesh[meshId].vertexCount; }

	// Returns all the faces in format index1/index2/index3/index1/index2.... 
	// Next face starts where the last face ends (3 ints)
	int* GetFaces(int meshId);
	int GetFaceCount(int meshId)  { return mesh[meshId].faceCount; }



	// Ignore below, will be reformated or removed
	BGLoading::Skeleton GetSkeleton(int index) const { return mesh[index].skeleton; }
	BGLoading::Joint GetJoint(int mIndex, int jIndex) const { return skeletonsD[mIndex].joint[jIndex]; }
	BGLoading::Animation GetAnimation(int mIndex, int aIndex) const { return animationsD[mIndex].animations[aIndex].ani; }
	BGLoading::KeyFrame GetKeyFrame(int mIndex, int aIndex, int kIndex) const { return animationsD[mIndex].animations[aIndex].keyFrames[kIndex].key; }
	BGLoading::Transform GetTransform(int mIndex, int aIndex, int kIndex, int tIndex) const { return animationsD[mIndex].animations[aIndex].keyFrames[kIndex].transforms[tIndex].t; }


	float* GetDirLightPos(int index) const { return dirLight[index].position; }
	float GetDirLightIntensity(int index) const { return dirLight[index].intensity; }
	float* GetDirLightRotation(int index) const { return dirLight[index].rotation; }
	float* GetDirLightColor(int index) const { return dirLight[index].color; }
	int GetDirLightCount() const { return fileHeader.dirLightCount; }

	//float GetPointLightPos(int index, int posIndex								) const { return pointLight[index].position[posIndex]; }
	float* GetPointLightPos(int index) const { return pointLight[index].position; }
	float* GetPointLightColor(int index) const { return pointLight[index].color; }

	float GetPointLightIntensity(int index) const { return pointLight[index].intensity; }
	int GetPointLightCount() const { return fileHeader.pointLightCount; }

	unsigned int GetMaterialID(int meshIndex) const { return mesh[meshIndex].materialID; }
	BGLoading::PhongMaterial GetMaterial(int index) const { return material[index]; }
	int GetMaterialCount() const { return fileHeader.materialCount; }

	int GetMeshGroupCount() const { return fileHeader.groupCount; }
	BGLoading::MeshGroup GetMeshGroup(int index) const { return meshGroup[index]; }
};

