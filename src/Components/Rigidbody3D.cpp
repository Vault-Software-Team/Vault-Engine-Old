#pragma once
#include "Rigidbody3D.hpp"

namespace HyperAPI::Experimental {
    void Rigidbody3D::CreateBody(btCollisionShape *shape) {
        ref = shape;
        bt_transform = new btTransform();
        bt_transform->setIdentity();
        bt_transform->setOrigin(btVector3(transform->position.x,
                                          transform->position.y,
                                          transform->position.z));
        glm::vec3 q = transform->rotation;
        btQuaternion rotation;
        rotation.setEuler(q.y, q.x, q.z);
        HYPER_LOG("Created a Bullet Physics Rigidbody")
        bt_transform->setRotation(rotation);

        motionState = new btDefaultMotionState(*bt_transform);

        if (mass > 0) {
            shape->calculateLocalInertia(mass, inertia);
        }

        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState,
                                                        shape, inertia);
        rbInfo.m_restitution = restitution;
        rbInfo.m_friction = friction;
        body = new btRigidBody(rbInfo);
        // set user data
        body->setUserPointer(&gameObject->ID);
        // set trigger
        body->setCollisionFlags(
            body->getCollisionFlags() |
            btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
        if (trigger) {
            body->setCollisionFlags(
                body->getCollisionFlags() |
                btCollisionObject::CF_NO_CONTACT_RESPONSE);
        }

        if (fixedRotation) {
            body->setAngularFactor(btVector3(0, 0, 0));
        }

        BulletPhysicsWorld::dynamicsWorld->addRigidBody(body);
    }
} // namespace HyperAPI::Experimental