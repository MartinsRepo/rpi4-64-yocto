cmake_minimum_required(VERSION 3.22.2)

file(READ "VERSION_NUMBER" ver)
string(REGEX MATCH "v([01]?[0-9][0-9]?.[01]?[0-9][0-9]?.[01]?[0-9][0-9]?)" _ ${ver})
set(version ${CMAKE_MATCH_1})
message("Project Version Number : ${version}")

project(dmap-het-core VERSION ${version})
set (CMAKE_CXX_STANDARD 14)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/")

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set(EXECUTABLE_NAME app_het_core)
set(LIB_NAME _het_core)

FIND_PATH(TF_HEADER NAMES tensorflow/lite/c/c_api.h PATHS ${CMAKE_SOURCE_DIR}/dep/ARM64/tensorflow_lite/include/ NO_DEFAULT_PATH)
FIND_LIBRARY(TF_LIB NAMES libtensorflowlite_c.so)
find_package(OpenCV REQUIRED core imgproc calib3d)

# import tensorflowlite and OpenCV
#if (CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
#    FIND_PATH(TF_HEADER NAMES tensorflow/lite/c/c_api.h PATHS ${CMAKE_SOURCE_DIR}/dep/ARM64/tensorflow_lite/include/ NO_DEFAULT_PATH)
#    FIND_LIBRARY(TF_LIB NAMES libtensorflowlite_c.so PATHS ${CMAKE_SOURCE_DIR}/dep/ARM64/tensorflow_lite/lib  NO_DEFAULT_PATH)
#    find_package(OpenCV REQUIRED core imgproc calib3d PATHS ${CMAKE_SOURCE_DIR}/dep/ARM64/opencv  NO_DEFAULT_PATH)
#else() 
#    FIND_PATH(TF_HEADER NAMES tensorflow/lite/c/c_api.h PATHS ${CMAKE_SOURCE_DIR}/dep/x86_64/tensorflow_lite/include/ NO_DEFAULT_PATH)
#    FIND_LIBRARY(TF_LIB NAMES libtensorflowlite_c.so PATHS ${CMAKE_SOURCE_DIR}/dep/x86_64/tensorflow_lite/lib  NO_DEFAULT_PATH)
 #   find_package(OpenCV REQUIRED core imgproc calib3d PATHS ${CMAKE_SOURCE_DIR}/dep/x86_64/opencv  NO_DEFAULT_PATH)
                  
#endif() 

configure_file("${PROJECT_SOURCE_DIR}/include/version.h.in"
                    "${PROJECT_BINARY_DIR}/version.h")


add_library(${LIB_NAME} SHARED)

set(LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/lib/HETCoreVersion.cpp
            ${CMAKE_CURRENT_LIST_DIR}/src/lib/HETDModule.cpp
            ${CMAKE_CURRENT_LIST_DIR}/src/lib/face_detection/CocoveiaFaceDetector.cpp 
            ${CMAKE_CURRENT_LIST_DIR}/src/lib/HETModule.cpp 
            ${CMAKE_CURRENT_LIST_DIR}/src/lib/faciallandmark_detection/PIPNetFacialLandmarkDetector.cpp
            ${CMAKE_CURRENT_LIST_DIR}/src/lib/headpose_estimation/PnPHeadPoseEstimation.cpp
            ${CMAKE_CURRENT_LIST_DIR}/src/lib/HETCoreInit.cpp
            ${CMAKE_CURRENT_LIST_DIR}/src/lib/HETSModule.cpp
            ${CMAKE_CURRENT_LIST_DIR}/src/lib/eye_opening_detection/FLMEyeOpeningDetector.cpp
            ${CMAKE_CURRENT_LIST_DIR}/src/common/Utils.cpp
            ${CMAKE_CURRENT_LIST_DIR}/src/common/CppLogger.cpp
            ${CMAKE_CURRENT_LIST_DIR}/src/common/ImageProcessing.cpp
            ${CMAKE_CURRENT_LIST_DIR}/src/common/AlgoFactory.cpp
)

target_sources(${LIB_NAME} PRIVATE
               ${LIB_SOURCES}
)

target_include_directories(${LIB_NAME} PUBLIC
                            ${CMAKE_CURRENT_LIST_DIR}/include/
                            PRIVATE
                            ${PROJECT_BINARY_DIR}
                            ${TF_HEADER}
                            ${OpenCV_INCLUDE_DIRS}
)



# copy model to build folder
add_custom_command(TARGET ${LIB_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/models/ $<TARGET_FILE_DIR:${LIB_NAME}>/models)

# change to loading dependencies from
target_link_libraries(${LIB_NAME}
                        ${OpenCV_LIBS} 
                        ${TF_LIB}) 
                        
target_compile_options(${LIB_NAME} PRIVATE -DHET_CORE_EXPORTS)

add_executable(${EXECUTABLE_NAME})

set(APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/app/CSVOutputWritter.cpp
                ${CMAKE_CURRENT_LIST_DIR}/src/app/RawImageReader.cpp
)


target_sources(${EXECUTABLE_NAME} PRIVATE
                            ${CMAKE_CURRENT_LIST_DIR}/src/app/main.cpp
                            ${APP_SOURCES})
                                                     
target_link_libraries(${EXECUTABLE_NAME} ${LIB_NAME} )

# install required libraries (remove if the cmake_install.cmake is used)
execute_process(  # Get needed dynamic libraries from binary
WORKING_DIRECTORY .
COMMAND ldd ./${EXECUTABLE_NAME}
COMMAND nawk "{print $3}"
COMMAND sort -u
COMMAND egrep "tensorflow|opencv" # Only libs I want to copy
OUTPUT_VARIABLE INSTALL_FILES
ERROR_VARIABLE INSTALL_FILES_ERROR
OUTPUT_STRIP_TRAILING_WHITESPACE
)
    
string(REGEX REPLACE "\n" ";" INSTALL_FILES "${INSTALL_FILES}") # Convert into list

message(" ----> ${INSTALL_FILES}")

if (${INSTALL_FILES_ERROR})
    message("----> ${INSTALL_FILES_ERROR}")
endif (${INSTALL_FILES_ERROR})

foreach (_file ${INSTALL_FILES}) # Resolve links
    get_filename_component(_resolvedFile "${_file}" REALPATH)
    list (APPEND INSTALL_RESOLVED_FILES ${_resolvedFile} )
endforeach()

install(FILES                               # Copy all libraries and symlinks pointing to real paths
    ${INSTALL_FILES}
    DESTINATION lib
    COMPONENT core
    )
install(FILES                               # Copy all libraries real paths
    ${INSTALL_RESOLVED_FILES}
    DESTINATION lib
    COMPONENT core
    )

message(" ----> Installing lib-dmap-het-core lib")
install(TARGETS ${LIB_NAME}
        LIBRARY DESTINATION lib)
#install(TARGETS ${TF_LIB}
#        LIBRARY DESTINATION lib)
message(" ----> Installing model")
install(DIRECTORY models DESTINATION bin)


install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)

#
# Unit testing setup
#
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86" AND BUILD_TESTING)
    enable_testing()
    message(STATUS "Build unit tests for the project. Tests should always be found in the test folder\n")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/test)  
endif()
