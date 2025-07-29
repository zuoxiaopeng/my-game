# 标题
cmake_minimum_required(VERSION 3.10.0)
project(SDLShooter VERSION 0.1.0 LANGUAGES C CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置编译器选项
if (MSVC)
    add_compile_options(/W4) # MSVC编译器 加 /WX 报错 
else()
    add_compile_options(-Wall -Wextra -Wpedantic) # GCC编译器 加 -Werror 报错
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)  # 默认使用 Debug 模式
endif()

# 设置字符集为UTF-8
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# 设置可执行文件目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

set(TARGET ${PROJECT_NAME}-${CMAKE_SYSTEM_NAME})

# 无预设Cmake文件
# include_directories("C:/CODE/library/SDL2/include")
# link_directories("C:/CODE/library/SDL2/lib/x64")

# 查找并载入Cmake预设
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# 添加可执行文件
add_executable(${TARGET} 
                src/main.cpp
                src/Game.cpp
                src/Scene.cpp
                src/SceneMain.cpp
                src/SceneTitle.cpp
                src/SceneEnd.cpp
                )

# 链接库
target_link_libraries(${TARGET} 
                        SDL2::SDL2main 
                        SDL2::SDL2
                        SDL2_image::SDL2_image
                        SDL2_ttf::SDL2_ttf
                        SDL2_mixer::SDL2_mixer
                        )
