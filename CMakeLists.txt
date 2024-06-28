cmake_minimum_required(VERSION 3.10)

set(PATCH_VERSION "2" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.1.${PATCH_VERSION})

project(custom_allocator VERSION ${PROJECT_VESRION})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

configure_file(version.h.in version.h)

add_executable(custom_allocator_appl main.cpp)
add_library(custom_allocator lib.cpp)
add_executable(test_custom_allocator test_version.cpp)

set_target_properties(custom_allocator_appl custom_allocator test_custom_allocator PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_include_directories(custom_allocator
    PRIVATE "${CMAKE_BINARY_DIR}"
)
set_target_properties(test_custom_allocator PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

target_link_libraries(custom_allocator_appl PRIVATE
    custom_allocator
)
target_link_libraries(test_custom_allocator
    ${Boost_LIBRARIES}
    custom_allocator
)

if (MSVC)
    target_compile_options(custom_allocator_appl PRIVATE
        /W4
    )
    target_compile_options(custom_allocator PRIVATE
        /W4
    )
    target_compile_options(test_custom_allocator PRIVATE
        /W4
    )
else ()
    target_compile_options(custom_allocator_appl PRIVATE
        -Wall -Wextra -pedantic -Werror -std=c++17
    )
    target_compile_options(custom_allocator PRIVATE
        -Wall -Wextra -pedantic -Werror -std=c++17
    )
    target_compile_options(test_custom_allocator PRIVATE
        -Wall -Wextra -pedantic -Werror -std=c++17
    )
endif()



install(TARGETS custom_allocator_appl RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

enable_testing()

add_test(test_custom_allocator test_custom_allocator)
