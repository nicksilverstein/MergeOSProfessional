
list(APPEND SOURCE
    rosperf.c
    lines.c
    fill.c
    scroll.c
    text.c
    alphablend.c
    testlist.c
    gradient.c)

add_executable(rosperf ${SOURCE} rosperf.rc)
set_module_type(rosperf win32cui UNICODE)
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(rosperf PRIVATE -Wno-unused-but-set-variable)
endif()
add_importlibs(rosperf version msimg32 gdi32 shell32 advapi32 user32 ntdll msvcrt kernel32)
add_cd_file(TARGET rosperf DESTINATION mergeos/system32 FOR all)
