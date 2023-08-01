#pragma once
#include <dllapi.hpp>
#include <libs.hpp>

namespace HyperAPI {
    namespace CppScripting {
        class DLL_API Script {
        public:
            std::string objId;
            std::string name;

            virtual void Start(){};
            virtual void Update(){};
        };

        struct DLL_API SharedObject {
            std::string name;
            std::string typeName;
            void *handle;
            Script *(*create)();
        };
        DLL_API extern std::vector<SharedObject> cpp_scripts;

        void LoadScripts();
        void CompileLinuxScripts();
        void CompileWindowsScripts();
    } // namespace CppScripting
} // namespace HyperAPI