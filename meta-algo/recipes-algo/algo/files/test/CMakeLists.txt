cmake_minimum_required(VERSION 3.22.2)

# Locate GTest
find_package(GTest REQUIRED)

set(TEST_SOURCES src/face_detection/TestFaceDetection.cpp
                 src/common/Metrics.cpp
)

# Link runTests with what we want to test and the GTest and pthread library
add_executable(run_tests 
                ${TEST_SOURCES}
                ${LIB_SOURCES}
                ${APP_SOURCES}
)

target_include_directories(run_tests PUBLIC
                                    ${CMAKE_CURRENT_LIST_DIR}/include/
                                    ${CMAKE_CURRENT_LIST_DIR}/../include/
                                    ${PROJECT_BINARY_DIR}
                                    ${TF_HEADER}
                                    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(run_tests  
                    GTest::GTest 
                    GTest::Main
                    ${OpenCV_LIBS} 
                    ${TF_LIB}                     
)
