
project(main)

add_subdirectory(base)

add_subdirectory(test/testA)

# 查找当前目录下的所有源文件
# 并将名称保存到 SOURCEFILES 变量
aux_source_directory(. SOURCEFILES)
aux_source_directory(${Manager_Dir} SOURCEFILES)
aux_source_directory(${Map_Dir} SOURCEFILES)
aux_source_directory(${Enemy_Dir} SOURCEFILES)
aux_source_directory(${Bullet_Dir} SOURCEFILES)
aux_source_directory(${Tower_Dir} SOURCEFILES)

aux_source_directory(${cJson_Dir} SOURCEFILES)

message(STATUS "Manager_Dir: ${Manager_Dir}")
message(STATUS "Map_Dir: ${Map_Dir}")
message(STATUS "Enemy_Dir: ${Enemy_Dir}")
message(STATUS "cJson_Dir: ${cJson_Dir}")

message(STATUS "find srcs: ${SOURCEFILES}") # 打印源文件

add_executable(${PROJECT_NAME} ${SOURCEFILES})

# 添加头文件
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${Base_Dir})
target_include_directories(${PROJECT_NAME} PUBLIC ${Manager_Dir})
target_include_directories(${PROJECT_NAME} PUBLIC ${Map_Dir})
target_include_directories(${PROJECT_NAME} PUBLIC ${Enemy_Dir})
target_include_directories(${PROJECT_NAME} PUBLIC ${Bullet_Dir})
target_include_directories(${PROJECT_NAME} PUBLIC ${Tower_Dir})

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
    base

    # SDL2
    SDL2main
    SDL2_image
    SDL2_mixer
    SDL2_ttf
)
