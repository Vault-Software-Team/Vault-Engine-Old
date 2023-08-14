#pragma once
#include <dllapi.hpp>
#include <libs.hpp>
#include "../vendor/bullet/bullet/BulletSoftBody/btSoftBody.h"
#include "../vendor/bullet/bullet/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include "../vendor/bullet/bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "../vendor/bullet/bullet/BulletSoftBody/btSoftBodySolvers.h"
#include "../vendor/bullet/bullet/BulletSoftBody/btDefaultSoftBodySolver.h"
#include "../vendor/bullet/bullet/BulletSoftBody/btSoftBodyHelpers.h"
#include "../vendor/bullet/bullet/BulletSoftBody/btSoftBodyData.h"

namespace HyperAPI::BulletPhysicsWorld {
    DLL_API extern btDiscreteDynamicsWorld *dynamicsWorld;
    DLL_API extern btBroadphaseInterface *broadphase;
    DLL_API extern btDefaultCollisionConfiguration *collisionConfiguration;
    DLL_API extern btCollisionDispatcher *dispatcher;
    DLL_API extern btSequentialImpulseConstraintSolver *solver;
    DLL_API extern btGhostPairCallback *ghostPairCallback;
    DLL_API extern btAlignedObjectArray<btCollisionShape *> collisionShapes;
    DLL_API extern btAlignedObjectArray<btRigidBody *> rigidBodies;
    DLL_API extern btAlignedObjectArray<btPairCachingGhostObject *> ghostObjects;
    DLL_API extern btAlignedObjectArray<btTypedConstraint *> constraints;
    DLL_API extern btAlignedObjectArray<btCollisionObject *> collisionObjects;

    void Delete();
    void Init();
    void UpdatePhysics();

    void CollisionCallback(
        std::function<void(const std::string &, const std::string &)>
            HandleEntities);
} // namespace HyperAPI::BulletPhysicsWorld