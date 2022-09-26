#include "scripts.hpp"
#include "api.hpp"

using namespace HyperAPI;
using namespace HyperAPI::Experimental;

void InitScripts() {
    // Add scripts here, the scripts and the manager do not have a Graphical User Interface so it wont show up in the editor
    // this will get called when you press the "Play" button in the editor or launching the built game

    /*
    heres how you add it

    GameObject *object = f_GameObject::FindGameObjectByName("GameObject Name");
    object->AddComponent<NativeScriptManager>().AddScript<MyScript>();
    */
}