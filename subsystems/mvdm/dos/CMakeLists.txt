
include_directories(${MERGEOS_SOURCE_DIR}/subsystems/mvdm/dos)
add_asm16_bin(command ${CMAKE_CURRENT_BINARY_DIR}/command.com 0x0100 command.S)
add_cd_file(TARGET command FILE ${CMAKE_CURRENT_BINARY_DIR}/command.com DESTINATION mergeos/system32 FOR all)
