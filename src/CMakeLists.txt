
project(main)

add_subdirectory(base)
add_subdirectory(manager)

add_subdirectory(test/testA)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_LIB_SRCS)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/base)

target_link_libraries(${PROJECT_NAME} base)
