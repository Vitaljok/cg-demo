Include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glfw)
target_compile_options(glfw PRIVATE -w)
add_library(glfw::glfw ALIAS glfw)


# GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.2
  GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(glm)

# stb
add_library(stb 
  STATIC
  stb/stb_impl.cpp
)
target_include_directories(stb PUBLIC stb/)
add_library(stb::stb ALIAS stb)


# GLAD (downloaded from https://gen.glad.sh/)
add_library(glad 
    STATIC
    glad/src/gl.c
)
target_include_directories(glad
    PUBLIC
    glad/include
)
add_library(glad::glad ALIAS glad)

# Assimp
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG        v6.0.2
  GIT_SHALLOW    TRUE
)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_OBJ_IMPORTER ON)
set(ASSIMP_BUILD_TESTS OFF)
FetchContent_MakeAvailable(assimp)
target_compile_options(assimp PUBLIC -w)