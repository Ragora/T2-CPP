/*
 *	CRigidBody.cpp
 *	Rigid Body Nodes for Tribes 2
 *	Copyright (c) 2013 Robert MacGregor
*/

#include <stdafx.h>
#include <CRigidBody.h>

CRigidBody::CRigidBody(btRigidBody *body) : rigid_body(body)
{
}

CRigidBody::~CRigidBody(void)
{
}