cmake_minimum_required(VERSION 3.16.3)

project(GZModder)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_libraries(stdc++fs)
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})
add_executable(gzm ../src/animation.cpp ../src/leng.cpp ../src/zscript.cpp ../src/files.cpp ../src/main.cpp ../src/decorate.cpp ../src/load_pk3.cpp  ../src/gzdoom.cpp ../src/set_offset.cpp ../src/editors.cpp ../src/load_wad.cpp)
add_executable(prev ../src/preview.cpp ../src/files.cpp)

target_link_libraries(prev ${GTK3_LIBRARIES})
target_link_libraries(gzm ${GTK3_LIBRARIES})
add_compile_options(-Wall)
add_compile_options(-export-dynamic)
add_compile_options(-Wmultichar)
