
cmake_minimum_required(VERSION 2.8)
project(echo)
# Set OSCC_API_INSTALL with `cmake -DOSCC_API_INSTALL=<path>` or hardcode it below
# set(OSCC_API_INSTALL <path>)
set(OSCC_INCLUDES ${OSCC_API_INSTALL}/include ${OSCC_API_INSTALL}/src)
set(SOURCES ${CMAKE_SOURCE_DIR}/echo.c ${OSCC_API_INSTALL}/src/oscc.c)
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DKIA_NIRO=ON")
target_include_directories(${PROJECT_NAME} PUBLIC ${OSCC_INCLUDES})

