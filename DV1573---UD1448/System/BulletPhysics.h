#ifndef BULLETPHYSICS_H
#define BULLETPHYSICS_H
#include <Pch/Pch.h>

enum CollisionObject
{
	box,
	sphere,
	capsule

};

class BulletPhysics
{
public:
	BulletPhysics(float gravity);
	~BulletPhysics();

	//objects box, sphere, capsule. (Extend.x is radius) 
	btRigidBody* createObject(CollisionObject object, float inMass,
		glm::vec3 position, glm::vec3 extend, float friction = 0.2f);

	btDiscreteDynamicsWorld* getDynamicsWorld() const;

	void update(float dt);

private:

	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;

	btAlignedObjectArray<btCollisionShape*> m_collisionShapes;
};

#endif
