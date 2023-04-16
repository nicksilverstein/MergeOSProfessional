
# NTVDM Default Configuration Files

list(APPEND NTVDM_CONFIG_FILES
     AUTOEXEC.NT
     CONFIG.NT)

foreach(item ${NTVDM_CONFIG_FILES})
    add_cd_file(FILE "${CMAKE_CURRENT_SOURCE_DIR}/${item}" DESTINATION mergeos/system32 FOR all)
    add_cd_file(FILE "${CMAKE_CURRENT_SOURCE_DIR}/${item}" DESTINATION mergeos/repair FOR all)
endforeach(item)

# system.ini
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/system.ini DESTINATION mergeos FOR all)

# win.ini
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/win.ini DESTINATION mergeos FOR all)
