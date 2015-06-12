find_package(OpenCV REQUIRED)

set(CMAKE_C_FLAGS "-std=gnu99 -pedantic -Wall -g")

if(${OpenCV_VERSION} VERSION_GREATER 2.3.0)
  add_definitions(-DOpenCV_OLD)
endif()

include_directories(${PROJECT_SOURCE_DIR}/lib)
link_directories(${SRM_BINARY_DIR}/lib)

add_executable(webcam webcam.c)
target_link_libraries(webcam srm ${OpenCV_LIBS})

add_executable(video video.c)
target_link_libraries(video srm ${OpenCV_LIBS})

add_executable(video_sub video_sub.c)
target_link_libraries(video_sub srm ${OpenCV_LIBS})

add_executable(image image.c)
target_link_libraries(image srm ${OpenCV_LIBS})

add_executable(uf uf.c)
target_link_libraries(uf srm m)

add_executable(stats stats.c)
target_link_libraries(stats srm m ${OpenCV_LIBS})

