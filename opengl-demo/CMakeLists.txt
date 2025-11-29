add_executable(opengl-demo
    main.cpp
)

target_link_libraries(opengl-demo
    PRIVATE
    glad::glad
    glfw::glfw
)