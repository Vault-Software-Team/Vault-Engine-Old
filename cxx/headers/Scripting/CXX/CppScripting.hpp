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
        };

        struct SharedObject {
            std::string name;
            std::string typeName;
            void *handle;
            Script *(*create)();
        };
        extern std::vector<SharedObject> cpp_scripts;

        void LoadScripts();
        void CompileLinuxScripts();
        void CompileWindowsScripts();
    } // namespace CppScripting
} // namespace HyperAPI