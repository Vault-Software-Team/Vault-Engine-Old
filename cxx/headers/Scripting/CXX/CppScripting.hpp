#pragma once
#include <libs.hpp>

namespace HyperAPI {
    namespace CppScripting {
        class Script {
        public:
            std::string objId;
            std::string name;

            virtual void Start(){};
            virtual void Update(){};
            virtual void OnCollisionEnter2D(Experimental::GameObject *collided_with){};
            virtual void OnCollisionExit2D(Experimental::GameObject *collided_with){};
            virtual void OnCollisionEnter3D(Experimental::GameObject *collided_with){};
        };

#ifdef _WIN32
        struct SharedObject {
            std::string name;
            std::string typeName;
            HINSTANCE handle;
            Script *(*create)();
        };
#else
        struct SharedObject {
            std::string name;
            std::string typeName;
            void *handle;
            Script *(*create)();
        };
#endif
        extern std::vector<SharedObject> cpp_scripts;

        void LoadScripts();
        void CompileLinuxScripts();
        void CompileWindowsScripts();
    } // namespace CppScripting
} // namespace HyperAPI