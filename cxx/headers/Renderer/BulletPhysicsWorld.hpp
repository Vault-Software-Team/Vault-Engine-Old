#pragma once
#include <dllapi.hpp>
#include <libs.hpp>

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