cmake_minimum_required(VERSION 2.6)
project(libsrm)
set(VERSION 0.1)

add_subdirectory(lib)
add_subdirectory(examples EXCLUDE_FROM_ALL)

add_custom_command(OUTPUT index.html
                   COMMAND rst2html README>index.html
                   DEPENDS README
                   )
add_custom_command(OUTPUT libsrm-${VERSION}.tar.gz
                   COMMAND darcs dist -d libsrm-${VERSION}
                   )
add_custom_target(dist DEPENDS index.html libsrm-${VERSION}.tar.gz)

