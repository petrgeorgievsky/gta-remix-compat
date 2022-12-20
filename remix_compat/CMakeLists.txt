
cmake_minimum_required(VERSION 3.20)

add_library(remix_compat SHARED
        main.cpp
        executable_injection.ixx
        gta_remix_fixes.ixx
        gta_sa_fixes.cpp)

target_include_directories(remix_compat PRIVATE ${DirectX_INCLUDE_DIRS})
target_compile_features(remix_compat PRIVATE cxx_std_20)
target_compile_options(remix_compat PRIVATE /experimental:module)
set_target_properties(remix_compat PROPERTIES
        SUFFIX ".asi"
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_link_libraries(remix_compat PRIVATE ${DirectX_D3DX9D_LIBRARY}  WIL::WIL)