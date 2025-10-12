include(CTest)

if (MSVC)
    message(FATAL_ERROR "SNAP fuzzing currently requires Clang or GCC with libFuzzer support.")
endif ()

include(CheckCXXCompilerFlag)

set(SNAP_FUZZ_SANITIZER_FLAGS "-fsanitize=fuzzer,address,undefined")
check_cxx_compiler_flag("${SNAP_FUZZ_SANITIZER_FLAGS}" SNAP_HAS_FUZZING_SANITIZERS)

if (NOT SNAP_HAS_FUZZING_SANITIZERS)
    check_cxx_compiler_flag("-fsanitize=fuzzer" SNAP_HAS_FUZZING_ENGINE)
    if (SNAP_HAS_FUZZING_ENGINE)
        set(SNAP_FUZZ_SANITIZER_FLAGS "-fsanitize=fuzzer")
    else ()
        message(FATAL_ERROR "Compiler does not support libFuzzer (missing -fsanitize=fuzzer).")
    endif ()
endif ()

set_property(GLOBAL APPEND PROPERTY CTEST_LABELS_FOR_SUBPROJECTS "fuzz")

function(snap_add_fuzzer target)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(ARG "" "" "${multiValueArgs}" ${ARGN})

    if (NOT ARG_SOURCES)
        message(FATAL_ERROR "snap_add_fuzzer(${target}): provide SOURCES")
    endif ()

    add_executable(${target} ${ARG_SOURCES})
    target_link_libraries(${target} PRIVATE snap)
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_compile_options(${target} PRIVATE ${SNAP_FUZZ_SANITIZER_FLAGS} -fno-omit-frame-pointer)
    target_link_options(${target} PRIVATE ${SNAP_FUZZ_SANITIZER_FLAGS})
    set_property(TARGET ${target} PROPERTY FOLDER "fuzzing")

    add_test(NAME ${target}_smoke COMMAND $<TARGET_FILE:${target}> -runs=0)
    set_tests_properties(${target}_smoke PROPERTIES LABELS "fuzz")
endfunction()

snap_add_fuzzer(snap_byteswap_fuzzer
        SOURCES ${CMAKE_CURRENT_LIST_DIR}/bit_byteswap_fuzzer.cpp)
