#include "NewScript.hpp"
NewScript *create_object() {
    return new NewScript;
}

void NewScript::Start() {
	auto *gameobject = f_GameObject::FindGameObjectByID(objId);
	std::cout << "hello!" << std::endl;
}
void NewScript::Update() {
	Log deez("hi", LOG_INFO);
}
                            




