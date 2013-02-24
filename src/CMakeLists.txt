add_definitions(-DSCALE=2 -DCANACE_VERSION=\"0.1\")
add_executable(canace canace.c tkkeys.c tkspooler.c tktape.c acescreen.c z80.c tape.c keyboard.c spooler.c)

add_custom_command(
    PRE_BUILD
    OUTPUT canace.tcl.h
    WORKING_DIRECTORY ${CanAce_SOURCE_DIR}/src
    COMMAND xxd -i canace.tcl canace.tcl.h
    DEPENDS canace.tcl
)

add_library(tkwin STATIC tkwin.c ${CanAce_SOURCE_DIR}/src/canace.tcl.h)

find_package(TCL)
find_package(SDL)

include_directories(${TK_INCLUDE_PATH})
include_directories(${SDL_INCLUDE_DIR})

target_link_libraries(canace tkwin ${SDL_LIBRARY} ${TCL_LIBRARY} ${TK_LIBRARY})

install(PROGRAMS canace DESTINATION bin)
install(FILES ../ace.rom DESTINATION share/canace)
