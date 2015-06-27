/*
 *	CRigidBody.h
 *	Rigid Body Nodes for Tribes 2
 *	Copyright (c) 2013 Robert MacGregor
*/

#include <stdafx.h>

class CRigidBody
{
public:
	CRigidBody(btRigidBody *body);
	~CRigidBody(void);
private:
	btRigidBody *rigid_body;
};