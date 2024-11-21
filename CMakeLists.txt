
cmake_minimum_required(VERSION 3.20.0)
project(test)

set(CMAKE_CXX_STANDARD 23)

enable_testing()

# 添加链接库路径
link_directories(${CMAKE_SOURCE_DIR}/lib)

set(SDL2_Dir ${CMAKE_SOURCE_DIR}/lib/SDL2)
set(SDL2_image_Dir ${CMAKE_SOURCE_DIR}/lib/SDL2_image)
set(SDL2_ttf_Dir ${CMAKE_SOURCE_DIR}/lib/SDL2_ttf)
set(SDL2_mixer_Dir ${CMAKE_SOURCE_DIR}/lib/SDL2_mixer)
set(SDL2_gfx_Dir ${CMAKE_SOURCE_DIR}/lib/SDL2_gfx)
set(cJson_Dir ${CMAKE_SOURCE_DIR}/lib/cJson)

set(Base_Dir ${CMAKE_SOURCE_DIR}/src/base)
set(Manager_Dir ${CMAKE_SOURCE_DIR}/src/manager)
set(Map_Dir ${CMAKE_SOURCE_DIR}/src/map)
set(Enemy_Dir ${CMAKE_SOURCE_DIR}/src/enemy)
set(Bullet_Dir ${CMAKE_SOURCE_DIR}/src/bullet)
set(Tower_Dir ${CMAKE_SOURCE_DIR}/src/tower)
set(Ui_Dir ${CMAKE_SOURCE_DIR}/src/ui)

# 设置输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(src)
