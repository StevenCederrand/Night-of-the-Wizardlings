#include "Pch/Pch.h"
#include "AnimatedObject.h"

AnimatedObject::AnimatedObject()
{
	currentTime = 0;
}

AnimatedObject::AnimatedObject(std::string name) : GameObject(name)
{
	currentTime = 0;
}

AnimatedObject::~AnimatedObject()
{
}

void AnimatedObject::update(float dt)
{
	currentTime += dt;

	for (size_t i = 0; i < m_meshes.size(); i++)
	{
		size_t animSize = MeshMap::getInstance()->getMesh(m_meshes[i].name)->getAnimations().size();
		for (size_t a = 0; a < animSize; a++)
		{
			std::string animName = MeshMap::getInstance()->getMesh(m_meshes[i].name)->getAnimations()[a];
			ComputeMatrix(m_meshes[i].name, animName, dt);
		}
	}
}

void AnimatedObject::ComputeMatrix(std::string meshn, std::string animation, float dt)
{
	// Mesh, animation, and skeleton
	const Animation* anim = AnimationMap::getInstance()->getAnimation(animation);
	const Mesh* mesh = MeshMap::getInstance()->getMesh(meshn);
	const Skeleton* skeleton = SkeletonMap::getInstance()->getSkeleton(mesh->getSkeleton());

	// time must be less than duration.
	if (currentTime > anim->duration)
		currentTime = 0;

	// keyframes involved.
	int k1 = (int)(currentTime * anim->rate);
	k1 = (int)fmaxf((float)k1, (float)anim->keyframeFirst);
	int k2 = (int)fminf((float)k1 + 1, (float)anim->keyframeLast);

	// keyframes in currentTime terms
	float k1_time = k1 / anim->rate;
	float k2_time = k2 / anim->rate;
	// time rescaled into [0..1] as a percentage between k1 and k2
	float t = (currentTime - k1_time) / (k2_time - k1_time);

	int boneCount = (int)skeleton->joints.size();

	glm::mat4 bones_global_pose[MAX_BONES]{ glm::mat4(1.0f) };
	for (int i = 0; i < MAX_BONES; i++)
		bones_global_pose[i] = glm::mat4(1.0f);

	glm::vec3 translation_r = glm::vec3(anim->keyframes[k1].local_joints_T[0] * (1 - t) + anim->keyframes[k2].local_joints_T[0] * t);
	glm::vec3 scaling_r = glm::vec3(anim->keyframes[k1].local_joints_S[0] * (1 - t) + anim->keyframes[k2].local_joints_S[0] * t);
	glm::quat quaternion_r = glm::slerp(anim->keyframes[k1].local_joints_R[0], anim->keyframes[k2].local_joints_R[0], t);

	glm::mat4 MODEL_MAT = glm::mat4(1.0f);
	glm::mat4 translationMatrix_r = glm::translate(MODEL_MAT, translation_r);
	glm::mat4 rotationMatrix_r = glm::mat4_cast(quaternion_r);
	glm::mat4 scaleMatrix_r = glm::scale(MODEL_MAT, scaling_r);
	glm::mat4 local_r = translationMatrix_r * rotationMatrix_r * scaleMatrix_r;

	bones_global_pose[0] = local_r;

	skinData.bones[0] = bones_global_pose[0] * skeleton->joints[0].invBindPose;
	//boneList->bones[0] = glm::inverse(mesh->GetSkeleton().joints[0].invBindPose);
	for (int bone = 1; bone < boneCount; bone++)
	{
		glm::vec3 translation = glm::vec3(anim->keyframes[k1].local_joints_T[bone] * (1 - t) + anim->keyframes[k2].local_joints_T[bone] * t);
		glm::vec3 scaling = glm::vec3(anim->keyframes[k1].local_joints_S[bone] * (1 - t) + anim->keyframes[k2].local_joints_S[bone] * t);
		glm::quat quaternion = glm::slerp(anim->keyframes[k1].local_joints_R[bone], anim->keyframes[k2].local_joints_R[bone], t);

		MODEL_MAT = glm::mat4(1.0f);
		glm::mat4 translationMatrix = glm::translate(MODEL_MAT, translation);
		glm::mat4 rotationMatrix = glm::mat4_cast(quaternion);
		glm::mat4 scaleMatrix = glm::scale(MODEL_MAT, scaling);
		glm::mat4 local = translationMatrix * rotationMatrix * scaleMatrix;

		bones_global_pose[bone] = bones_global_pose[skeleton->joints[bone].parentIndex] * local;
		skinData.bones[bone] = bones_global_pose[bone] * skeleton->joints[bone].invBindPose;
		//boneList->bones[bone]		= glm::inverse(mesh->GetSkeleton().joints[bone].invBindPose);
	}
}
