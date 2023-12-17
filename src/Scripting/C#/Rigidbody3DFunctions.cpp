#include "Rigidbody3DFunctions.hpp"
#include "box2d/b2_body.h"
#include "mono/metadata/object.h"

namespace HyperAPI::CsharpScriptEngine::Functions {
    void Rigidbody3D_SetPosition(float x, float y, float z, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();

        btRigidBody *body = (btRigidBody *)rigidbody.body;
        body->getWorldTransform().setOrigin(btVector3(x, y, z));
    }

    void Rigidbody3D_AddForce(float x, float y, float z, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();

        rigidbody.AddForce(glm::vec3(x, y, z));
    }

    void Rigidbody3D_AddTorque(float x, float y, float z, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();

        rigidbody.AddTorque(glm::vec3(x, y, z));
    }

    void Rigidbody3D_AddForceAtPosition(float x, float y, float z, float px, float py, float pz, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();

        rigidbody.AddForceAtPosition(glm::vec3(x, y, z), glm::vec3(px, py, pz));
    }

    void Rigidbody3D_SetVelocity(float x, float y, float z, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();
        rigidbody.body->setActivationState(DISABLE_DEACTIVATION);
        rigidbody.SetVelocity(glm::vec3(x, y, z));
    }

    void Rigidbody3D_SetAngularVelocity(float x, float y, float z, MonoString *id) {
        using namespace Experimental;
        using namespace CsharpVariables;

        const std::string m_id = mono_string_to_utf8(id);
        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();

        rigidbody.SetAngularVelocity(glm::vec3(x, y, z));
    }

    void Rigidbody3D_GetKey(MonoString *key, MonoString *id, MonoString **result) {
        using namespace Experimental;
        using namespace CsharpVariables;
        using namespace std;

        const std::string m_key = mono_string_to_utf8(key);
        const std::string m_id = mono_string_to_utf8(id);

        auto *gameObject = f_GameObject::FindGameObjectByID(m_id);
        if (!gameObject) {
            Log log(("C#: Couldn't find game object with ID: " + m_id), LOG_ERROR);
            return;
        }

        auto &rigidbody = gameObject->GetComponent<Rigidbody3D>();

        if (m_key == "velocity") {
            *result = mono_string_new(appDomain, (to_string((rigidbody.GetVelocity().x)) + " " + to_string(rigidbody.GetVelocity().y) + " " + to_string((rigidbody.GetVelocity().z))).c_str());
        }
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions