#include "NewScript.hpp"
NewScript *create_object() {
	Log log("fuck!", LOG_INFO);
    return new NewScript;
}

std::vector<Log> *get_logs() {
	return &Scene::logs;
}

void NewScript::Start() {
	auto *gameobject = f_GameObject::FindGameObjectByID(objId);
	std::cout << "hello!" << std::endl;
}
void NewScript::Update() {
	Log deez("hi", LOG_INFO);
}
                            






