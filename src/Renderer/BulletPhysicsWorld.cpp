#include "BulletPhysicsWorld.hpp"

namespace HyperAPI::BulletPhysicsWorld {
    DLL_API btDiscreteDynamicsWorld *dynamicsWorld;
    DLL_API btBroadphaseInterface *broadphase;
    DLL_API btDefaultCollisionConfiguration *collisionConfiguration;
    DLL_API btCollisionDispatcher *dispatcher;
    DLL_API btSequentialImpulseConstraintSolver *solver;
    DLL_API btAlignedObjectArray<btCollisionShape *> collisionShapes;
    DLL_API btAlignedObjectArray<btRigidBody *> rigidBodies;
    DLL_API btAlignedObjectArray<btPairCachingGhostObject *> ghostObjects;
    DLL_API btAlignedObjectArray<btTypedConstraint *> constraints;
    DLL_API btAlignedObjectArray<btCollisionObject *> collisionObjects;

    void Delete() {
        delete dynamicsWorld;
        delete solver;
        delete dispatcher;
        delete collisionConfiguration;
        delete broadphase;
    }

    void Init() {
        broadphase = new btDbvtBroadphase();
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        solver = new btSequentialImpulseConstraintSolver;
        dynamicsWorld = new btDiscreteDynamicsWorld(
            dispatcher, broadphase, solver, collisionConfiguration);
        dynamicsWorld->setGravity(btVector3(0, -9.81, 0));
    }

    void UpdatePhysics() {
        dynamicsWorld->stepSimulation(1.f / 60.f, 10);
    }

    void CollisionCallback(
        std::function<void(const std::string &, const std::string &)>
            HandleEntities) {
        int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
        // do collision callback on all rigid bodies
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold *contactManifold =
                dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
            const auto *obA = static_cast<const btCollisionObject *>(
                contactManifold->getBody0());
            const auto *obB = static_cast<const btCollisionObject *>(
                contactManifold->getBody1());

            // get the entity names
            auto *entityA = static_cast<std::string *>(obA->getUserPointer());
            auto *entityB = static_cast<std::string *>(obB->getUserPointer());

            // call the callback
            HandleEntities(*entityA, *entityB);
        }
    }
} // namespace HyperAPI::BulletPhysicsWorld