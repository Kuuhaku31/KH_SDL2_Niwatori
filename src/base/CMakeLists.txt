
project(base)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

# 生成链接库
add_library(${PROJECT_NAME} SHARED ${DIR_LIB_SRCS})

# 添加头文件
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_Dir}/include)

# 添加链接库路径
target_link_directories(${PROJECT_NAME} PUBLIC ${SDL2_Dir}/lib/x64)

target_link_libraries(${PROJECT_NAME} SDL2)
