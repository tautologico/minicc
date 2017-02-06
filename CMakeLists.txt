cmake_minimum_required(VERSION 3.6)
project(minicc)

# Configuração do LLVM ##############

# comentar linha abaixo se o sistema consegue encontrar o módulo do LLVM automaticamente
set(LLVM_DIR "/usr/local/opt/llvm/lib/cmake/llvm")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(CMAKE_CXX_STANDARD 14)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(SOURCE_FILES main.cpp lexer.cpp lexer.h)
add_executable(minicc ${SOURCE_FILES})

# copia arquivos de teste para diretorio de build
add_custom_command(TARGET minicc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/test/print.mc
        ${CMAKE_SOURCE_DIR}/test/cond.mc
        ${CMAKE_CURRENT_BINARY_DIR})

# llvm_map_components_to_libnames(llvm_libs core)
# target_link_libraries(minicc ${llvm_libs})