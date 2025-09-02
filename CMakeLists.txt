cmake_minimum_required(VERSION 3.13)
project(ethercat_motor VERSION 0.1.6 LANGUAGES C CXX DESCRIPTION "hand_sdk 包含所有手")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#生成调试信息
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -rdynamic -fno-omit-frame-pointer -funwind-tables")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -fno-omit-frame-pointer -funwind-tables")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
# set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -s")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -flto")
# set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
# set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -flto")

file(GLOB_RECURSE SRC_SOURCE
    src/**/*.cpp
)

find_package(soem REQUIRED)
find_package(fmt REQUIRED)
find_library(ZMQ_LIBRARY zmq REQUIRED)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/SOEM)
add_executable(${PROJECT_NAME} ${SRC_SOURCE} ${SERIAL_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt ${ZMQ_LIBRARY} soem boost_stacktrace_backtrace backtrace dl)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-sign-compare)
# target_link_options(${PROJECT_NAME} PRIVATE -rdynamic -fno-omit-frame-pointer)
# 添加头文件路径
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/SOEM/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME ${PROJECT_NAME})

include(CTest)
enable_testing()
include(GoogleTest)

file(GLOB_RECURSE test_srcs
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     "tests/*.cpp")
foreach(test_src ${test_srcs})
    message("${test_src}")
    get_filename_component(test_exe ${test_src} NAME_WE)
    add_executable(${test_exe} ${test_src})
    target_link_libraries (${test_exe} gtest gflags ${PROJECT_NAME} hand_soem boost_stacktrace_backtrace backtrace dl)
    gtest_discover_tests(${test_exe} ${test_src} AUTO)
    # target_link_options(${test_exe} PRIVATE -rdynamic -fno-omit-frame-pointer -funwind-tables)
endforeach()
#1
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE # 运行时文件（可执行文件）
    LIBRARY DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE # 动态库（Linux/macOS）
    ARCHIVE DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE # 静态库（Linux/macOS）
    INCLUDES DESTINATION include
)

# 安装头文件到 include 目录
install(DIRECTORY include/
    DESTINATION include  # 目标路径（相对于 CMAKE_INSTALL_PREFIX）
)

# 安装头文件到 include 目录
install(DIRECTORY serial/include/
    DESTINATION include  # 目标路径（相对于 CMAKE_INSTALL_PREFIX）
)
# 3. 导出 Targets
install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# 4. 配置 package config 文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# 5. 安装 Config 文件
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)


install(FILES setup.sh DESTINATION . PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)

include (InstallRequiredSystemLibraries)
# set (CPACK_PACKAGING_INSTALL_PREFIX /usr/local)
# cmake -DCMAKE_INSTALL_PREFIX=$PWD ..

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "hand_sdk")
include(CPack)

