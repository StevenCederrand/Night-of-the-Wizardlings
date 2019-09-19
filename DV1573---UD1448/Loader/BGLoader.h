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


class BGLoader
{
private:
	std::string	fileName;

	int meshCount;

	std::vector<std::vector<Vertices>> bggVertices;
	std::vector<std::vector<Face>> bggFaces;
	std::vector<Material> bggMaterials;

	// TODO: Direction lights, Point Lights, Skeletons, and Animations

	// Temporary shared format to load data into
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
	// Default constructor to directly load a file
	BGLoader(std::string filesName);		
	~BGLoader();
	void Unload();
	void BGFormatData();

	bool LoadMesh(std::string fileName);	// Load a file

	// Returns the name of the file
	std::string GetFileName() const { return fileName; }

	//  Returns the meshcount of the file
	const int GetMeshCount() const { return fileHeader.meshCount; }

	//  Returns the name of a specifc mesh
	const std::string GetMeshName(int meshID) { return (std::string)loaderMesh[meshID].name; }
	//  Returns the name of the first mesh
	const std::string GetMeshName() { return (std::string)loaderMesh[0].name; }

	// Returns the vertices of a specific mesh
	const std::vector<Vertices> GetVertices(int meshId) { return bggVertices[meshId];  }
	// Returns the vertices of the first mesh
	const std::vector<Vertices> GetVertices() { return bggVertices[0]; }

	// Returns the faces of a specific mesh
	const std::vector<Face> GetFaces(int meshId) { return bggFaces[meshId]; }
	// Returns the faces of the first mesh
	const std::vector<Face> GetFaces() { return bggFaces[0]; }

	// Returns a specific material
	const Material GetMaterial(int meshId) { return bggMaterials[meshId]; }
	// Returns the first material in the file
	const Material GetMaterial() { return bggMaterials[0]; }

	// Returns a specific albedo map
	const std::string GetAlbedo(int meshId) { return (std::string)material[meshId].albedo; }
	// Returns the first albedo map
	const std::string GetAlbedo() { return (std::string)material[0].albedo; }

	// Returns a specific normal map
	const std::string GetNormalMap(int meshId) { return (std::string)material[meshId].normal; }
	// Returns the first normal map
	const std::string GetNormalMap() { return (std::string)material[0].normal; }

	// Returns the vertexcount of a specific mesh
	const int GetVertexCount(int meshId) { return loaderMesh[meshId].vertexCount; }
	// Returns the vertexcount of the first mesh
	const int GetVertexCount() { return loaderMesh[0].vertexCount; }

	// Returns the facecount of a specific mesh
	const int GetFaceCount(int meshId)  { return loaderMesh[meshId].faceCount; }
	// Returns the face count of the first mesh
	const int GetFaceCount()  { return loaderMesh[0].faceCount; }


	// Ignore below, will be reformated or removed
	BGLoading::LoaderMesh GetLoaderMesh(int meshId) const { return loaderMesh[meshId]; }

	const BGLoading::PhongMaterial GetMaterial(int meshID) const { return material[meshID]; }
	const BGLoading::PhongMaterial GetMaterial() const { return material[0]; }

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

	float* GetPointLightPos(int index) const { return pointLight[index].position; }
	float* GetPointLightColor(int index) const { return pointLight[index].color; }

	float GetPointLightIntensity(int index) const { return pointLight[index].intensity; }
	int GetPointLightCount() const { return fileHeader.pointLightCount; }

	unsigned int GetMaterialID(int meshIndex) const { return loaderMesh[meshIndex].materialID; }
	int GetMaterialCount() const { return fileHeader.materialCount; }

	int GetMeshGroupCount() const { return fileHeader.groupCount; }
};

