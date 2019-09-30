#include "Pch/Pch.h"
#include "AnimatedObject.h"

AnimatedObject::AnimatedObject()
{
}

AnimatedObject::AnimatedObject(std::string name) : GameObject(name)
{
}

AnimatedObject::~AnimatedObject()
{
}

void AnimatedObject::update(float dt)
{
	ComputeMatrix(dt);
}

void AnimatedObject::ComputeMatrix(int animId, float dt)
{
	// use animation 0 for now....
	Animation& anim = mesh->GetSkeleton().animations[0];


	// time must be less than duration.
	if (anim_time > anim.duration) return;

	//anim_time = 0.0f;
	// keyframes involved.
	int k1 = (int)(anim_time * anim.rate);
	k1 = fmaxf(k1, anim.keyframeFirst);

	int k2 = fminf(k1 + 1, anim.keyframeLast);

	// keyframes in anim_time terms
	float k1_time = k1 / anim.rate;
	float k2_time = k2 / anim.rate;
	// time rescaled into [0..1] as a percentage between k1 and k2
	float t = (anim_time - k1_time) / (k2_time - k1_time);

	int boneCount = (int)mesh->GetSkeleton().joints.size();

	glm::mat4 bones_global_pose[MAXBONES]{ glm::mat4(1.0f) };
	for (int i = 0; i < MAXBONES; i++)
		bones_global_pose[i] = glm::mat4(1.0f);

	glm::vec3 translation_r = glm::vec3(anim.keyframes[k1].local_joints_T[0] * (1 - t) + anim.keyframes[k2].local_joints_T[0] * t);
	glm::vec3 scaling_r = glm::vec3(anim.keyframes[k1].local_joints_S[0] * (1 - t) + anim.keyframes[k2].local_joints_S[0] * t);
	glm::quat quaternion_r = glm::slerp(anim.keyframes[k1].local_joints_R[0], anim.keyframes[k2].local_joints_R[0], t);

	MODEL_MAT = glm::mat4(1.0f);
	glm::mat4 translationMatrix_r = glm::translate(MODEL_MAT, translation_r);
	glm::mat4 rotationMatrix_r = glm::mat4_cast(quaternion_r);
	glm::mat4 scaleMatrix_r = glm::scale(MODEL_MAT, scaling_r);
	glm::mat4 local_r = translationMatrix_r * rotationMatrix_r * scaleMatrix_r;

	bones_global_pose[0] = local_r;

	boneList->bones[0] = bones_global_pose[0] * mesh->GetSkeleton().joints[0].invBindPose;
	//boneList->bones[0] = glm::inverse(mesh->GetSkeleton().joints[0].invBindPose);
	for (int bone = 1; bone < boneCount; bone++)
	{
		glm::vec3 translation = glm::vec3(anim.keyframes[k1].local_joints_T[bone] * (1 - t) + anim.keyframes[k2].local_joints_T[bone] * t);
		glm::vec3 scaling = glm::vec3(anim.keyframes[k1].local_joints_S[bone] * (1 - t) + anim.keyframes[k2].local_joints_S[bone] * t);
		glm::quat quaternion = glm::slerp(anim.keyframes[k1].local_joints_R[bone], anim.keyframes[k2].local_joints_R[bone], t);

		MODEL_MAT = glm::mat4(1.0f);
		glm::mat4 translationMatrix = glm::translate(MODEL_MAT, translation);
		glm::mat4 rotationMatrix = glm::mat4_cast(quaternion);
		glm::mat4 scaleMatrix = glm::scale(MODEL_MAT, scaling);
		glm::mat4 local = translationMatrix * rotationMatrix * scaleMatrix;

		bones_global_pose[bone] = bones_global_pose[mesh->GetSkeleton().joints[bone].parentIndex] * local;
		boneList->bones[bone] = bones_global_pose[bone] * mesh->GetSkeleton().joints[bone].invBindPose;
		//boneList->bones[bone]		= glm::inverse(mesh->GetSkeleton().joints[bone].invBindPose);
	}
}
