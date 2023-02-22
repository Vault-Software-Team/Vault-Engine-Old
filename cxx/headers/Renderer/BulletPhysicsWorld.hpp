#pragma once
#include <libs.hpp>

namespace HyperAPI::BulletPhysicsWorld {
    extern btDiscreteDynamicsWorld *dynamicsWorld;
    extern btBroadphaseInterface *broadphase;
    extern btDefaultCollisionConfiguration *collisionConfiguration;
    extern btCollisionDispatcher *dispatcher;
    extern btSequentialImpulseConstraintSolver *solver;
    extern btGhostPairCallback *ghostPairCallback;
    extern btAlignedObjectArray<btCollisionShape *> collisionShapes;
    extern btAlignedObjectArray<btRigidBody *> rigidBodies;
    extern btAlignedObjectArray<btPairCachingGhostObject *> ghostObjects;
    extern btAlignedObjectArray<btTypedConstraint *> constraints;
    extern btAlignedObjectArray<btCollisionObject *> collisionObjects;

    void Delete();
    void Init();
    void UpdatePhysics();

    void CollisionCallback(
        std::function<void(const std::string &, const std::string &)>
            HandleEntities);
} // namespace HyperAPI::BulletPhysicsWorld