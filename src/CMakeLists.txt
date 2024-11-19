
project(main)

add_subdirectory(base)

add_subdirectory(test/testA)

# 查找当前目录下的所有源文件
# 并将名称保存到 SOURCEFILES 变量
aux_source_directory(. SOURCEFILES)
aux_source_directory(../lib/cJSON SOURCEFILES)

# 打印源文件
message(STATUS "main srcs: ${SOURCEFILES}")

add_executable(${PROJECT_NAME} ${SOURCEFILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/base)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/manager)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/map)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/enemy)

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
