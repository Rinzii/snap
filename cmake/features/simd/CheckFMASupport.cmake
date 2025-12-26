include(CheckCXXSourceCompiles)

check_cxx_source_compiles("
        #include <immintrin.h>
        int main() {
            __m128 fma_test = _mm_fmadd_ps(_mm_set1_ps(1.0f), _mm_set1_ps(2.0f), _mm_set1_ps(3.0f));
            return 0;
        }
    " SNAP_SIMD_HAS_FMA_SUPPORT)

if (SNAP_SIMD_HAS_FMA_SUPPORT)
    add_compile_definitions(SNAP_CONFIG_RT_SIMD_HAS_FMA)
endif ()
