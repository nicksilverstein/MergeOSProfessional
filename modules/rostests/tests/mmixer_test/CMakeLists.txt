
add_definitions(-D_DLL -D__USE_CRTIMP)
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/libs/sound)
include_directories(${MERGEOS_SOURCE_DIR}/sdk/lib/drivers/sound/mmixer)
add_executable(mmixer_test test.c)
set_module_type(mmixer_test win32cui)
target_link_libraries(mmixer_test mmixer)
add_importlibs(mmixer_test advapi32 setupapi winmm ksuser user32 msvcrt kernel32)
