#pragma once
#include "Rigidbody3D.hpp"

namespace HyperAPI::Experimental {
    void Rigidbody3D::GUI() {
        if (ImGui::CollapsingHeader("Rigidbody 3D")) {
            ImGui::DragFloat("Mass", &mass, 0.01f);
            ImGui::DragFloat("Friction", &friction, 0.01f);
            ImGui::DragFloat("Restitution", &restitution, 0.01f);
            ImGui::Checkbox("Trigger", &trigger);
            ImGui::Checkbox("Fixed Rotation", &fixedRotation);

            ImGui::NewLine();
            if (ImGui::Button(ICON_FA_TRASH " Remove Component")) {
                Scene::m_Registry.remove<Rigidbody3D>(entity);
            }
        }
    }

    void Rigidbody3D::CreateBody(btCollisionShape *shape) {
        ref = shape;
        bt_transform = new btTransform();
        bt_transform->setIdentity();
        bt_transform->setOrigin(btVector3(transform->position.x,
                                          transform->position.y,
                                          transform->position.z));
        glm::vec3 q = transform->rotation;
        btQuaternion rotation;
        rotation.setEuler(q.x, q.y, q.z);
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

    void Rigidbody3D::Update() {
        if (!body)
            return;
        btTransform btTrans = body->getWorldTransform();
        glm::mat4 mat = glm::mat4(1.0f);
        btTrans.getOpenGLMatrix(&mat[0][0]);

        // decompose
        glm::vec3 pos, rot, scal;
        DecomposeTransform(mat, pos, rot, scal);
        transform->position = pos;
        if (!fixedRotation) {
            transform->rotation = rot;
        }
    }
} // namespace HyperAPI::Experimental