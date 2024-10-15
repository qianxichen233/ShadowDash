#include <dlfcn.h>
#include <iostream>

#include "ninja.h"

using namespace std;

typedef shadowdash::buildGroup (*ManifestFunc)(); // Define the function pointer type

int main(int argc, char** argv) {
    // Load the shared library
    void* handle = dlopen("./manifest.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Failed to load library: %s\n", dlerror());
        return 1;
    }

    // Get the manifest function
    ManifestFunc manifest_func = nullptr;
    // *(void**)(&manifest_func) = dlsym(handle, "manifest");
    manifest_func = reinterpret_cast<ManifestFunc>(dlsym(handle, "manifest"));

    if (!manifest_func) {
        fprintf(stderr, "Failed to find manifest: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }

    // Call the manifest function and get the returned value
    shadowdash::buildGroup builds = manifest_func();
    
    for(const shadowdash::build& build : builds.builds) {
        // cout << "build" << endl;
        // for(const shadowdash::str& str : build.outputs_.values_) {
        //     cout << "outputs" << endl;
        //     for (const shadowdash::Token& token: str.tokens_) {
        //         cout << "token: " << token.value_ << endl;
        //     }
        // }
        cout << build << endl;
    }

    // Close the library
    dlclose(handle);

    ninja::shadowdash_compile(argc, argv, builds);
}
