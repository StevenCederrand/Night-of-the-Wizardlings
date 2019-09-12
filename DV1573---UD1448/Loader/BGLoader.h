#include <Pch/Pch.h>

#include "MeshFormat.h"


struct MeshSkeleton
{
	std::vector<BGLoading::Joint> joint;
};

struct MeshAnis
{
	struct MeshAnimation
	{
		struct KeyFrameL
		{
			struct TransformL
			{
				BGLoading::Transform t;
			};

			BGLoading::KeyFrame key;
			std::vector<TransformL> transforms;
		};


		BGLoading::Animation ani;
		std::vector<KeyFrameL> keyFrames;
	};

	std::vector<MeshAnimation> animations;
};

class BGLoader
{
private:
	std::string	fileName;
	std::string errorMessage;


	// Temporary paths to load data into
	BGLoading::BGHeader		fileHeader;
	BGLoading::MeshGroup* meshGroup;
	BGLoading::PhongMaterial* material;
	BGLoading::LoaderMesh* mesh;
	BGLoading::DirLight* dirLight;
	BGLoading::PointLight* pointLight;
	BGLoading::MeshVert* meshVert;

	std::vector<MeshAnis> animationsD;
	std::vector<MeshSkeleton> skeletonsD;

public:
	BGLoader();
	BGLoader(std::string filesName);
	~BGLoader();

	std::string GetLastError();
	bool LoadMesh(std::string fileName);

	int GetMeshCount() { return fileHeader.meshCount; }
	char* GetMeshName(int meshID);

	float* GetVertices(int meshID);
	int GetVertexCount(int meshID) { return mesh[meshID].vertexCount; }







	BGLoading::LoaderMesh GetMesh(int index) const { return mesh[index]; }
	BGLoading::Skeleton GetSkeleton(int index) const { return mesh[index].skeleton; }
	BGLoading::Joint GetJoint(int mIndex, int jIndex) const { return skeletonsD[mIndex].joint[jIndex]; }
	BGLoading::Animation GetAnimation(int mIndex, int aIndex) const { return animationsD[mIndex].animations[aIndex].ani; }
	BGLoading::KeyFrame GetKeyFrame(int mIndex, int aIndex, int kIndex) const { return animationsD[mIndex].animations[aIndex].keyFrames[kIndex].key; }
	BGLoading::Transform GetTransform(int mIndex, int aIndex, int kIndex, int tIndex) const { return animationsD[mIndex].animations[aIndex].keyFrames[kIndex].transforms[tIndex].t; }


	int GetHeader() { return fileHeader.meshCount; }
	std::string GetFileName() { return fileName; }

	BGLoading::Vertex* GetVerticies(int meshID) { return meshVert[meshID].vertices; }

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

