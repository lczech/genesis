#include <stdio.h>
#include <iostream>
#include <omp.h>

int main() {

    #ifdef _OPENMP
        printf("Hello from _OPENMP\n");
    #else
      breaks_on_purpose
    #endif

    #pragma omp parallel
    std::cout << "Hello from thread " << omp_get_thread_num() << "/" << omp_get_num_threads() << "\n";
    // printf("Hello from thread %d, with threads %d\n", omp_get_thread_num(), omp_get_num_threads());
    return 0;
}
