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