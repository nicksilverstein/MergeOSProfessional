
add_subdirectory(fonts)
add_subdirectory(inf)
add_subdirectory(nls)
add_subdirectory(sdb)
add_subdirectory(themes)
add_subdirectory(vgafonts)

add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/drivers/etc/hosts DESTINATION mergeos/system32/drivers/etc FOR all)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/drivers/etc/KDBinit DESTINATION mergeos/system32/drivers/etc FOR all)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/drivers/etc/networks DESTINATION mergeos/system32/drivers/etc FOR all)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/drivers/etc/protocol DESTINATION mergeos/system32/drivers/etc FOR all)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/drivers/etc/services DESTINATION mergeos/system32/drivers/etc FOR all)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/sounds/MergeOS_LogOn.wav DESTINATION mergeos/media FOR all)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/sounds/MergeOS_Recycle.wav DESTINATION mergeos/media FOR all)

add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/systemcompatible.manifest DESTINATION mergeos/winsxs/manifests FOR all)
add_cd_file(FILE ${CMAKE_CURRENT_SOURCE_DIR}/forwardcompatible.manifest DESTINATION mergeos/winsxs/manifests FOR all)
