add_executable(opengl-demo
    main.cpp
)

add_shaders(opengl-demo
    demo.frag
    demo.vert
)

target_link_libraries(opengl-demo
    PRIVATE
    glad::glad
    glfw::glfw
    glm::glm
    demo::shared
)
