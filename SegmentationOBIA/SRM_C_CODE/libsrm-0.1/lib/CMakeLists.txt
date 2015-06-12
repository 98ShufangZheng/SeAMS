set(CMAKE_C_FLAGS "-std=gnu99 -pedantic -Wall -g")
add_library(srm SHARED unionfind srm)

INSTALL(FILES srm.h DESTINATION include)
INSTALL(FILES libsrm.so DESTINATION lib)

