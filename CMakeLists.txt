cmake_minimum_required(VERSION 3.6)
project(SuperStation)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(cmake/TargetArch.cmake)
target_architecture(TARGET_ARCH)

message("Building for ${TARGET_ARCH}")


#set(CMAKE_FIND_DEBUG_MODE true)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Add SDL
if(WIN32)
  if(TARGET_ARCH STREQUAL "x86_32")
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sdl2/win32/x86)
  endif()
  if(TARGET_ARCH STREQUAL "x86_64")
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sdl2/win32/x64)
  endif()
  #set(SDL2_BUILDING_LIBRARY on)
endif()

find_package(SDL2 REQUIRED)

# Add GLES2
if(WIN32)
  if(TARGET_ARCH STREQUAL "x86_32")
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/angle/win32/x86)
    set(EGL_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/angle/win32/x86/bin/libEGL.dll.lib)
    set(OPENGLES2_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/angle/win32/x86/bin/libGLESv2.dll.lib)
  endif()
  if(TARGET_ARCH STREQUAL "x86_64")
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/angle/win32/x64)
    set(EGL_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/angle/win32/x64/bin/libEGL.dll.lib)
    set(OPENGLES2_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/angle/win32/x64/bin/libGLESv2.dll.lib)
  endif()
endif()

find_package(EGL REQUIRED)
find_package(OpenGLES2 REQUIRED)

# Add STB libs
add_library(stb_image INTERFACE)
add_library(stb_ds INTERFACE)
add_library(stb_rect_pack INTERFACE)
#add_library(stb_image_write INTERFACE)
#add_library(stb_image_resize INTERFACE)
target_include_directories(stb_image INTERFACE thirdparty/stb)
target_include_directories(stb_ds INTERFACE thirdparty/stb)
target_include_directories(stb_rect_pack INTERFACE thirdparty/stb)
#target_include_directories(stb_image_write INTERFACE thirdparty/stb)
#target_include_directories(stb_image_resize INTERFACE thirdparty/stb)

# Add jsmn
add_library(jsmn INTERFACE)
target_include_directories(jsmn INTERFACE thirdparty/jsmn)

# Add Wren
file(GLOB_RECURSE wren_sources thirdparty/wren/src/*.c)
file(GLOB_RECURSE wren_headers thirdparty/wren/src/*.h)

add_library(wren STATIC ${wren_sources} ${wren_headers})
target_include_directories(wren PRIVATE thirdparty/wren/src/vm thirdparty/wren/src/optional)
target_include_directories(wren PUBLIC thirdparty/wren/src/include)
target_link_libraries(wren m)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(wren PRIVATE DEBUG)
endif()
  
# Add zip
add_subdirectory(thirdparty/zip)

# Add Superstation executable
file(GLOB_RECURSE sources src/*.c)
file(GLOB_RECURSE headers src/*.h)

add_executable(superstation ${headers} ${sources})

target_link_libraries(superstation PRIVATE EGL::EGL OpenGLES2::OpenGLES2 SDL2::Main stb_image stb_ds stb_rect_pack wren jsmn zip)

option(SST_INCLUDE_ASSETS "Include built-in scripts and shaders" OFF)
if(SST_INCLUDE_ASSETS)
  include(cmake/bin2h.cmake)
  target_include_directories(superstation PRIVATE include)
  target_compile_definitions(superstation PRIVATE SST_INCLUDE_ASSETS)
  
  bin2h(SOURCE_FILE glsl/fbo.fragment.glsl HEADER_FILE include/resources.in VARIABLE_NAME RES_PROG_FBO_FRAGMENT NULL_TERMINATE)
  bin2h(SOURCE_FILE glsl/fbo.vertex.glsl HEADER_FILE include/resources.in APPEND VARIABLE_NAME RES_PROG_FBO_VERTEX NULL_TERMINATE)
  bin2h(SOURCE_FILE glsl/tile.fragment.glsl HEADER_FILE include/resources.in APPEND VARIABLE_NAME RES_PROG_TILE_FRAGMENT NULL_TERMINATE)
  bin2h(SOURCE_FILE glsl/tile.vertex.glsl HEADER_FILE include/resources.in APPEND VARIABLE_NAME RES_PROG_TILE_VERTEX NULL_TERMINATE)
  bin2h(SOURCE_FILE glsl/sprite.fragment.glsl HEADER_FILE include/resources.in APPEND VARIABLE_NAME RES_PROG_SPRITE_FRAGMENT NULL_TERMINATE)
  bin2h(SOURCE_FILE glsl/sprite.vertex.glsl HEADER_FILE include/resources.in APPEND VARIABLE_NAME RES_PROG_SPRITE_VERTEX NULL_TERMINATE)
  bin2h(SOURCE_FILE wren/bootstrap.wren HEADER_FILE include/resources.in APPEND VARIABLE_NAME RES_SCRIPT_BOOTSTRAP NULL_TERMINATE)
  bin2h(SOURCE_FILE wren/gfx.wren HEADER_FILE include/resources.in APPEND VARIABLE_NAME RES_SCRIPT_GFX NULL_TERMINATE)
  bin2h(SOURCE_FILE wren/input.wren HEADER_FILE include/resources.in APPEND VARIABLE_NAME RES_SCRIPT_INPUT NULL_TERMINATE)
  bin2h(SOURCE_FILE wren/io.wren HEADER_FILE include/resources.in APPEND VARIABLE_NAME RES_SCRIPT_IO NULL_TERMINATE)
  bin2h(SOURCE_FILE wren/json.wren HEADER_FILE include/resources.in APPEND VARIABLE_NAME RES_SCRIPT_JSON NULL_TERMINATE)
endif()

# Add emscripten configuration
if(EMSCRIPTEN)
  target_include_directories(superstation PRIVATE thirdparty/emscripten)
  # set(CMAKE_EXECUTABLE_SUFFIX ".html")
  target_compile_options(superstation PRIVATE "-O3")
  #target_link_libraries(superstation PRIVATE "-s ASYNCIFY -O3 -lidbfs.js -s EXPORTED_FUNCTIONS='[\"_sst_web_vm_interpret\",\"_sst_web_vm_set_paused\",\"_sst_web_vram_read_pixel\",\"_main\"]' -s EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\"]' --preload-file ../glsl --preload-file ../example --preload-file ../wren")
  target_link_libraries(superstation PRIVATE "-s ASYNCIFY -s 'ASYNCIFY_IMPORTS=[\"do_fetch\",\"do_fetch_zip\"]' -O3 -lidbfs.js -s EXPORTED_FUNCTIONS='[\"_sst_web_vm_interpret\",\"_sst_web_vm_set_paused\",\"_sst_web_vram_read_pixel\",\"_sst_web_system_loadGame\",\"_main\"]' -s EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\"]'")
endif()

# Needed by SDL for console by mingw
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
  target_link_options(superstation PRIVATE "-mconsole")
endif()

# On windows copy dlls to bin dir
if(WIN32)
  configure_file(${SDL2_INCLUDE_DIRS}/../../bin/SDL2.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/SDL2.dll COPYONLY)
  configure_file(${EGL_INCLUDE_DIR}/../bin/libEGL.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libEGL.dll COPYONLY)
  configure_file(${EGL_INCLUDE_DIR}/../bin/libGLESv2.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libGLESv2.dll COPYONLY)
endif()
