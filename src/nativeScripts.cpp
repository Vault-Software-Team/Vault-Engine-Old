#include "lib/nativeScripts.hpp"
#include <iostream>

namespace HyperAPI::NativeScriptEngine {
    void LoadScript(const std::string &scriptName) {
//        std::cout << "Loading script: " << scriptName << std::endl;
//        // load the .so file
//        // call "HelloWorld" function
//
//         void *handle = dlopen(scriptName.c_str(), RTLD_LAZY);
//         if (!handle) {
//             std::cerr << "Cannot open library: " << dlerror() << '\n';
//             return;
//         }
//         // load the symbol
//         typedef void (*hello_t)();
//         // reset errors
//         dlerror();
//         hello_t hello = (hello_t) dlsym(handle, "HelloWorld");
//         const char *dlsym_error = dlerror();
//         if (dlsym_error) {
//             std::cerr << "Cannot load symbol 'HelloWorld': " << dlsym_error << '\n';
//             dlclose(handle);
//             return;
//         }
//         hello();
//         // close the library
//         dlclose(handle);
    }
}
