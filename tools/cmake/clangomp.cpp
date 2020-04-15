#include <omp.h>
#include <stdio.h>

int main() {

    #ifdef _OPENMP
        printf("Hello from _OPENMP\n");
    #else
      breaks_on_purpose
    #endif

    #pragma omp parallel
    printf("Hello from thread %d, nthreads %d\n", omp_get_thread_num(), omp_get_num_threads());
    return 0;
}
