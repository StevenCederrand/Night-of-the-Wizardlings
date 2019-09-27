#include <Pch/Pch.h>
#include "BGLoader.h"

BGLoader::BGLoader()
{
	fileName = "";
	meshCount = 0;
	matCount = 0;

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
	matCount = 0;

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

	for (std::vector<Vertex> vector : bggVertices)
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
	matCount = fileHeader.materialCount;


	// Synced mesh vectors
	bggVertices.resize(meshCount);
	bggFaces.resize(meshCount);
	bggPositions.resize(meshCount);
	bggRotation.resize(meshCount);
	bggScale.resize(meshCount);
	bggTransforms.resize(meshCount);

	//bggSkeleton.resize(skeletonsD.size());
	//bggAnimation.resize(animationsD.size());
	bggSkeleton.resize(meshCount);
	bggAnimation.resize(meshCount);

	// Material vector
	bggMaterials.resize(matCount);

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

		
		// Transforms
		bggPositions[meshId] = glm::make_vec3(loaderMesh[meshId].translation);
		bggRotation[meshId] = glm::quat(glm::radians(glm::make_vec3(loaderMesh[meshId].rotation)));
		bggScale[meshId] = glm::make_vec3(loaderMesh[meshId].scale);

		Transform newTransform;
		newTransform.position = bggPositions[meshId];
		newTransform.rotation = bggRotation[meshId];
		newTransform.scale = bggScale[meshId];
		bggTransforms[meshId] = newTransform;

	}

	for (int i = 0; i < matCount; i++)
	{
		// Material
		bggMaterials[i].name = (std::string)material[i].name;
		bggMaterials[i].ambient = glm::vec3(*material[i].ambient);
		bggMaterials[i].diffuse = glm::vec3(*material[i].diffuse);
		bggMaterials[i].specular = glm::vec3(*material[i].specular);
		bggMaterials[i].ambient = glm::vec3(*material[i].ambient);
	}


	// TODO: Improve importer to make this step simpler and better
	for (int i = 0; i < bggSkeleton.size(); i++)
	{
		if (loaderMesh[i].skeleton.jointCount > 0)
		{
			//bggSkeleton[i].name = loaderMesh[i].skeleton.name;
			bggSkeleton[i].name = skeletonsD[i].joint[0].name;
			bggSkeleton[i].joints.resize(loaderMesh[i].skeleton.jointCount);

			for (int j = 0; j < bggSkeleton[i].joints.size(); j++)
			{
				bggSkeleton[i].joints[j].name = skeletonsD[i].joint[j].name;
				bggSkeleton[i].joints[j].invBindPose = glm::make_mat4(skeletonsD[i].joint[j].invBindPose);
				bggSkeleton[i].joints[j].parentIndex = skeletonsD[i].joint[j].parentIndex;
			}
		}
	}


	for(int i = 0; i < bggAnimation.size(); i++)
	{
		//TODO: fix
		if (loaderMesh[i].skeleton.aniCount > 0)
		{
			bggAnimation[i].name = animationsD[i].animations[0].ani.name;
			
			bggAnimation[i].duration = animationsD[i].animations[0].ani.duration;
			bggAnimation[i].rate = animationsD[i].animations[0].ani.rate;
			bggAnimation[i].keyframeFirst = animationsD[i].animations[0].ani.keyframeFirst;
			bggAnimation[i].keyframeLast = animationsD[i].animations[0].ani.keyframeLast;

			bggAnimation[i].keyframes.resize(animationsD[i].animations[0].ani.keyframeCount);
			for (int k = 0; k < bggAnimation[i].keyframes.size(); k++)
			{
				bggAnimation[i].keyframes[k].id = animationsD[i].animations[0].keyFrames[k].key.id;

				int transformCount = animationsD[i].animations[0].keyFrames[k].transforms.size();
				bggAnimation[i].keyframes[k].local_joints_T.resize(transformCount);
				bggAnimation[i].keyframes[k].local_joints_R.resize(transformCount);
				bggAnimation[i].keyframes[k].local_joints_S.resize(transformCount);
				for (int t = 0; t < transformCount; t++)
				{
					bggAnimation[i].keyframes[k].local_joints_T[t] = glm::make_vec3(animationsD[i].animations[0].keyFrames[k].transforms[t].t.transform);
					bggAnimation[i].keyframes[k].local_joints_R[t] = glm::make_vec3(animationsD[i].animations[0].keyFrames[k].transforms[t].t.rotate);
					bggAnimation[i].keyframes[k].local_joints_S[t] = glm::make_vec3(animationsD[i].animations[0].keyFrames[k].transforms[t].t.scale);
				}
			}
		}
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
