#pragma once
#include "scene.hpp"
namespace HyperAPI
{
    namespace Experimental {
        class GameObject;
    }

    class StaticScript {
    public:
        Experimental::GameObject *gameObject;
        StaticScript() {}

        virtual void OnStart() {}
        virtual void OnUpdate() {}
        virtual void Collision2D(Experimental::GameObject *other) {}
        virtual void CollisionExit2D(Experimental::GameObject *other) {}

        virtual void Collision3D(Experimental::GameObject *other) {}
        virtual void CollisionExit3D(Experimental::GameObject *other) {}
    };
}

/*
Add your class definitions here,
DO NOT Forget to rebuild the api code!!!

for example:
class MyScript : public StaticScript
{
public:
    // variables

    MyScript() : StaticScript("My Script") { }

    void OnStart() override
    {
        // code
    }

    void OnUpdate() override
    {
        // code
    }
}
*/

void InitScripts();