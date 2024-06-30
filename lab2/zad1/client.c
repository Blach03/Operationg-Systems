#include <stdio.h>
#include <dlfcn.h>
#include "library.h"



void use_static_library() {
    int number = 8;
    int max_iter = 1000;

    int result = test_collatz_convergence(number, max_iter);
    if (result != -1) {
        printf("Static Library: Converged to 1 in %d iterations.\n", result);
    } else {
        printf("Static Library: Did not converge to 1 within the limit.\n");
    }
}

void use_shared_library() {
    void* handle = dlopen("./liblibrary.so", RTLD_LAZY);

    int (*test_collatz_convergence_shared)(int, int) = dlsym(handle, "test_collatz_convergence");

    int number = 27;
    int max_iter = 1000;

    int result = test_collatz_convergence_shared(number, max_iter);
    if (result != -1) {
        printf("Shared Library: Converged to 1 in %d iterations.\n", result);
    } else {
        printf("Shared Library: Did not converge to 1 within the limit.\n");
    }

    dlclose(handle);
}

void use_dynamic_loading() {
    int number = 13;
    int max_iter = 1000;

    void* handle = dlopen("./liblibrary.so", RTLD_LAZY);

    int (*test_collatz_convergence_dynamic)(int, int) = dlsym(handle, "test_collatz_convergence");

    int result = test_collatz_convergence_dynamic(number, max_iter);
    if (result != -1) {
        printf("Dynamic Loading: Converged to 1 in %d iterations.\n", result);
    } else {
        printf("Dynamic Loading: Did not converge to 1 within the limit.\n");
    }

    dlclose(handle);
}

int main(int argc, char *argv[]) {

    
    //use_static_library();
    

    //#ifdef SHARED
    use_shared_library();
    //#endif

    #ifdef DYNAMIC
    use_dynamic_loading();
    #endif

    return 0;
}
