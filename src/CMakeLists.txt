add_definitions(-DSCALE=2 -DCANACE_VERSION=\"0.1\")
add_executable(canace canace.c tkwin.c tkkeys.c tkspooler.c tktape.c acescreen.c z80.c tape.c keyboard.c spooler.c)

find_package(TCL)
find_package(SDL)

include_directories(${TK_INCLUDE_PATH})
include_directories(${SDL_INCLUDE_DIR})

target_link_libraries(canace ${SDL_LIBRARY} ${TCL_LIBRARY} ${TK_LIBRARY})

install(PROGRAMS canace DESTINATION bin)
install(FILES ../ace.rom DESTINATION share/canace)
