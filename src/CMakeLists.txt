
project(main)

add_subdirectory(base)

add_subdirectory(test/testA)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/base)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/manager)

# 添加头文件
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_Dir}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_image_Dir}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_ttf_Dir}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_mixer_Dir}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_gfx_Dir})
target_include_directories(${PROJECT_NAME} PUBLIC ${cJson_Dir})

# 添加链接库路径
target_link_directories(${PROJECT_NAME} PUBLIC ${SDL2_Dir}/lib/x64)
target_link_directories(${PROJECT_NAME} PUBLIC ${SDL2_image_Dir}/lib/x64)
target_link_directories(${PROJECT_NAME} PUBLIC ${SDL2_ttf_Dir}/lib/x64)
target_link_directories(${PROJECT_NAME} PUBLIC ${SDL2_mixer_Dir}/lib/x64)

# 添加链接库
target_link_libraries(${PROJECT_NAME}
    SDL2
    SDL2main
    SDL2_image
    SDL2_mixer
    SDL2_ttf
)
