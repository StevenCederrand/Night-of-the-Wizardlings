#pragma once
#include <Pch/Pch.h>
#include "MeshFormat.h"

//Example usage:
/*
	BGLoader testLoader;
	testLoader.LoadMesh("C:/Users/fisk0/Documents/GitHub/DV1573---UD1448/Assets/Meshes/SexyCube.meh");
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
	BGLoader(std::string filesName);
	~BGLoader();
	void Unload();

	bool LoadMesh(std::string fileName);

	int GetMeshCount() { return fileHeader.meshCount; }
	char* GetMeshName(int meshId);

	float* GetVertices(int meshId);
	int GetVertexCount(int meshId) { return mesh[meshId].vertexCount; }

	float* GetFaces(int meshId);
	int GetFaceCount(int meshId)  { return mesh[meshId].faceCount; }




	BGLoading::LoaderMesh GetMesh(int index) const { return mesh[index]; }
	BGLoading::Skeleton GetSkeleton(int index) const { return mesh[index].skeleton; }
	BGLoading::Joint GetJoint(int mIndex, int jIndex) const { return skeletonsD[mIndex].joint[jIndex]; }
	BGLoading::Animation GetAnimation(int mIndex, int aIndex) const { return animationsD[mIndex].animations[aIndex].ani; }
	BGLoading::KeyFrame GetKeyFrame(int mIndex, int aIndex, int kIndex) const { return animationsD[mIndex].animations[aIndex].keyFrames[kIndex].key; }
	BGLoading::Transform GetTransform(int mIndex, int aIndex, int kIndex, int tIndex) const { return animationsD[mIndex].animations[aIndex].keyFrames[kIndex].transforms[tIndex].t; }


	int GetHeader() { return fileHeader.meshCount; }
	std::string GetFileName() { return fileName; }

	BGLoading::Vertex* GetVerticies(int meshId) { return meshVert[meshId].vertices; }

	int GetType(int index) const { return mesh[index].type; }
	int GetLink(int index) const { return mesh[index].link; }
	int GetCollectIndex(int index) const { return mesh[index].collect; }

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

