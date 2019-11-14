#include "Pch/Pch.h"
#include "AnimatedObject.h"

AnimatedObject::AnimatedObject(std::string name) : GameObject(name)
{
	m_type = 1;
	currentTime = 0;
	boneBuffer = 0;


	//Binds buffer for the bone matrices on the gpu
	glGenBuffers(1, &boneBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, boneBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 64, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

AnimatedObject::~AnimatedObject()
{
	glDeleteBuffers(1, &boneBuffer);
}

void AnimatedObject::update(float dt)
{
	// Update animation time
	currentTime += dt;

	// Basic animation update for testing
	// TODO: Only update 1 animation, Choose animation to update
	// Should come naturally with specific animation implementation

	// (DEBUG) This loop temporarily updates all the animations for all the meshes based on time,
	// looping with the shortest animation and always replacing the pallete.
	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		size_t animSize = MeshMap::getInstance()->getMesh(m_meshes[i].name)->getAnimations().size();
		for (size_t a = 0; a < animSize; a++)
		{
			std::string animName = MeshMap::getInstance()->getMesh(m_meshes[i].name)->getAnimations()[a];
			ComputeMatrix((int)i, m_meshes[i].name, animName);
		}
	}
}

//TODO: Optimize if laggy, possibly move to GPU
//Less allocations will speed up the algorithm
void AnimatedObject::ComputeMatrix(int meshId, std::string meshn, std::string animation)
{
	// Mesh, animation, and skeleton
	const Animation& anim = *AnimationMap::getInstance()->getAnimation(animation);
	const Mesh& mesh = *MeshMap::getInstance()->getMesh(meshn);
	const Skeleton& skeleton = *SkeletonMap::getInstance()->getSkeleton(mesh.getSkeleton());

	// time must be less than duration. Also resets animation.
	if (currentTime > anim.duration)
		currentTime = 0;

	// keyframes involved.
	int k1 = (int)(currentTime * anim.rate);
	k1 = (int)fmaxf((float)k1, (float)anim.keyframeFirst);
	int k2 = (int)fminf((float)k1 + 1, (float)anim.keyframeLast);

	// keyframes in currentTime terms
	float k1_time = k1 / anim.rate;
	float k2_time = k2 / anim.rate;
	// time rescaled into [0..1] as a percentage between k1 and k2
	float t = (currentTime - k1_time) / (k2_time - k1_time);

	glm::mat4 bones_global_pose[MAX_BONES]{ glm::mat4(1.0f) };
	for (int i = 0; i < MAX_BONES; i++)
		bones_global_pose[i] = glm::mat4(1.0f);
	const Transform& keyTransform1 = anim.keyframes[k1].local_joint_t[0].transform;
	const Transform& keyTransform2 = anim.keyframes[k2].local_joint_t[0].transform;

	glm::vec3 translation_r = glm::vec3(keyTransform1.position * (1 - t) + keyTransform2.position * t);
	glm::quat quaternion_r	= glm::slerp(keyTransform1.rotation, keyTransform2.rotation, t);
	glm::vec3 scaling_r		= glm::vec3(keyTransform1.scale * (1 - t) + keyTransform2.scale * t);

	glm::mat4 MODEL_MAT = glm::mat4(1.0f);
	glm::mat4 translationMatrix_r	= glm::translate(MODEL_MAT, translation_r);
	glm::mat4 rotationMatrix_r		= glm::mat4_cast(quaternion_r);
	glm::mat4 scaleMatrix_r			= glm::scale(MODEL_MAT, scaling_r);
	glm::mat4 local_r				= translationMatrix_r * rotationMatrix_r * scaleMatrix_r;

	bones_global_pose[0] = local_r;
	bonePallete.bones[0] = bones_global_pose[0] * skeleton.joints[0].invBindPose;

	int boneCount = (int)skeleton.joints.size();
	int transformCount = (int)anim.keyframes[0].local_joint_t.size();	// Assuming same size for all
	for (int i = 1; i < boneCount; i++)									// First bone already assigned (i = 1)
	{
		glm::vec3 translation	= glm::vec3();
		glm::vec3 scaling		= glm::vec3(1.0f);
		glm::quat quaternion	= glm::quat();

		if(i < transformCount)
		{
			const Transform& keyTransform1 = anim.keyframes[k1].local_joint_t[i].transform;
			const Transform& keyTransform2 = anim.keyframes[k2].local_joint_t[i].transform;

			translation = glm::vec3(keyTransform1.position * (1 - t) + keyTransform2.position * t);
			quaternion	= glm::slerp(keyTransform1.rotation, keyTransform2.rotation, t);
			scaling		= glm::vec3(keyTransform1.scale * (1 - t) + keyTransform2.scale * t);
		}
		MODEL_MAT = glm::mat4(1.0f);
		glm::mat4 translationMatrix = glm::translate(MODEL_MAT, translation);
		glm::mat4 rotationMatrix	= glm::mat4_cast(quaternion);
		glm::mat4 scaleMatrix		= glm::scale(MODEL_MAT, scaling);
		glm::mat4 localTransform	= translationMatrix * rotationMatrix * scaleMatrix;

		if (i < transformCount)
			bones_global_pose[i] = bones_global_pose[skeleton.joints[i].parentIndex] * localTransform;
		bonePallete.bones[i] = bones_global_pose[i] * skeleton.joints[i].invBindPose;
	}
}

void AnimatedObject::BindAnimation(int meshId)
{
	GLint aniShader = ShaderMap::getInstance()->getShader(ANIMATION)->getShaderID();
	unsigned int boneDataIndex = glGetUniformBlockIndex(aniShader, "SkinDataBlock");
	glUniformBlockBinding(aniShader, boneDataIndex, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, boneBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(BonePalleteBuffer), &bonePallete, GL_STATIC_DRAW);
}
