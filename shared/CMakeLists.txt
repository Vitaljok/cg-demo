add_library(demo_shared 
    STATIC
    demo/utils.cpp
)
target_link_libraries(demo_shared)
target_include_directories(demo_shared PUBLIC . )
add_library(demo::shared ALIAS demo_shared)