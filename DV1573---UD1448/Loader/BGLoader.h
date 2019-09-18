#pragma once
#include <Pch/Pch.h>
#include "LoaderFormat.h"
#include "Mesh/MeshFormat.h"

//Example usage:
/*
	BGLoader testLoader;
	testLoader.LoadMesh("SexyCube.meh");
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
	
	BGLoading::PhongMaterial material;

};

class BGLoader
{
private:
	std::string	fileName;

	std::vector<BGMesh> Meshes;

	float* vertices;
	int* faces;

	// Temporary paths to load data into
	BGLoading::BGHeader	fileHeader;
	BGLoading::MeshGroup* meshGroup;
	BGLoading::PhongMaterial* material;
	BGLoading::LoaderMesh* loaderMesh;
	BGLoading::DirLight* dirLight;
	BGLoading::PointLight* pointLight;
	std::vector<BGLoading::Vertex*> meshVert;
	std::vector<BGLoading::Face*> meshFace;

	std::vector<BGLoading::MeshAnis> animationsD;
	std::vector<BGLoading::MeshSkeleton> skeletonsD;

public:
	BGLoader();
	BGLoader(std::string filesName);		// Default constructor to directly load a file
	~BGLoader();
	void Unload();

	bool LoadMesh(std::string fileName);	// Load a file

	// Returns the name of the opened file
	std::string GetFileName() const { return fileName; }

	//  Returns the meshcount of the file
	const int GetMeshCount() const { return fileHeader.meshCount; }

	//  Returns the name of a specifc mesh
	const char* GetMeshName(int meshId);

	//  Returns the name of the first mesh
	const char* GetMeshName();

	// Returns the vertices of a specific mesh
	// Returns all the verices in format posX/posY/PosZ/uvX/uvY/normalX/normalY/normalZ/posX/posY.... 
	// Next vertex starts where the last vertex ends (8 floats)
	const float* GetVertices(int meshId);

	// Returns the vertices of the first mesh
	// Returns all the verices in format posX/posY/PosZ/uvX/uvY/normalX/normalY/normalZ/posX/posY.... 
	// Next vertex starts where the last vertex ends (8 floats)
	const float* GetVertices();

	// Returns the vertexcount of a specific mesh
	const int GetVertexCount(int meshId) { return loaderMesh[meshId].vertexCount; }

	// Returns the vertexcount of the first mesh
	const int GetVertexCount() { return loaderMesh[0].vertexCount; }

	// Returns the face indicies of a specific mesh
	// Returns all the faces in format index1/index2/index3/index1/index2.... 
	// Next face starts where the last face ends (3 ints)
	const int* GetFaces(int meshId);

	// Returns the face indicies of the first mesh
	// Returns all the faces in format index1/index2/index3/index1/index2.... 
	// Next face starts where the last face ends (3 ints)
	const int* GetFaces();

	// Returns the facecount of a specific mesh
	const int GetFaceCount(int meshId)  { return loaderMesh[meshId].faceCount; }

	// Returns the face count of the first mesh
	const int GetFaceCount()  { return loaderMesh[0].faceCount; }

	// Returns a specific material
	const Material GetMaterial(int meshID);			// Project specific

	// Returns the first material in the file
	const Material GetMaterial();					// Project specific

	// Returns a specific albedo map
	const std::string GetAlbedo(int meshId) { return (std::string)material[meshId].albedo; }

	// Returns the first albedo map
	const std::string GetAlbedo() { return (std::string)material[0].albedo; }

	// Returns a specific normal map
	const std::string GetNormalMap(int meshId) { return (std::string)material[meshId].normal; }

	// Returns the first normal map
	const std::string GetNormalMap() { return (std::string)material[0].normal; }

	const BGLoading::PhongMaterial GetMaterial(int meshID) const { return material[meshID]; }
	const BGLoading::PhongMaterial GetMaterial() const { return material[0]; }



	// Ignore below, will be reformated or removed
	BGLoading::LoaderMesh GetLoaderMesh(int meshId) const { return loaderMesh[meshId]; }
	const BGLoading::Vertex* GetLoaderVertices(int meshId) { return meshVert[meshId]; }
	const BGLoading::Face* GetLoaderFaces(int meshId) { return meshFace[meshId]; }

	BGLoading::Skeleton GetSkeleton(int index) const { return loaderMesh[index].skeleton; }
	BGLoading::Joint GetJoint(int mIndex, int jIndex) const { return skeletonsD[mIndex].joint[jIndex]; }
	BGLoading::MeshAnis GetAnimation(int mIndex) const { return animationsD[mIndex]; }
	BGLoading::Animation GetAnimationHeader(int mIndex, int aIndex) const { return animationsD[mIndex].animations[aIndex].ani; }
	BGLoading::KeyFrame GetKeyFrame(int mIndex, int aIndex, int kIndex) const { return animationsD[mIndex].animations[aIndex].keyFrames[kIndex].key; }
	BGLoading::Transform GetTransform(int mIndex, int aIndex, int kIndex, int tIndex) const { return animationsD[mIndex].animations[aIndex].keyFrames[kIndex].transforms[tIndex].t; }
	BGLoading::MeshGroup GetMeshGroup(int index) const { return meshGroup[index]; }


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

	unsigned int GetMaterialID(int meshIndex) const { return loaderMesh[meshIndex].materialID; }
	int GetMaterialCount() const { return fileHeader.materialCount; }

	int GetMeshGroupCount() const { return fileHeader.groupCount; }
};

