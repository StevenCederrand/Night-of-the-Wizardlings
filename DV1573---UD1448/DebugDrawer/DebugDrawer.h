#ifndef DEBUGDRAWER_H
#define DEBUGDRAWER_H
#include "Pch/Pch.h"

class DebugDrawer
{
private:
	Mesh* m_mesh;	

public:
	DebugDrawer();
	~DebugDrawer();

	void setUpMesh(btRigidBody& body);	

	Buffers getBuffers();	
};

#endif
