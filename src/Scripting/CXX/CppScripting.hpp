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
            virtual void OnCollisionEnter2D(Experimental::GameObject *collided_with){};
            virtual void OnCollisionExit2D(Experimental::GameObject *collided_with){};
            virtual void OnCollisionEnter3D(Experimental::GameObject *collided_with){};
        };

#ifdef _WIN32
        struct DLL_API SharedObject {
            std::string name;
            std::string typeName;
            HINSTANCE handle;
            Script *(*create)();
        };
#else
        struct DLL_API SharedObject {
            std::string name;
            std::string typeName;
            void *handle;
            Script *(*create)();
        };
#endif
        DLL_API extern std::vector<SharedObject> cpp_scripts;
        DLL_API extern std::vector<filewatch::FileWatch<std::string> *> FileWatches;

        void CompileScripts_Windows(fs::path dirEntry);
        void CompileScripts_Linux(fs::path dirEntry);
        void LoadScripts();
        void CompileLinuxScripts();
        void CompileWindowsScripts();
    } // namespace CppScripting
} // namespace HyperAPI