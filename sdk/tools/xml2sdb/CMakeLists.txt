
add_definitions(-DSDBWRITE_HOSTTOOL=1)

list(APPEND SOURCE
    main.cpp
    tinyxml2.cpp
    xml2sdb.cpp
    xml2sdb.h
    ${MERGEOS_SOURCE_DIR}/dll/appcompat/apphelp/sdbwrite.c
    ${MERGEOS_SOURCE_DIR}/dll/appcompat/apphelp/sdbstringtable.c)

include_directories(
    ${MERGEOS_SOURCE_DIR}/dll/appcompat/apphelp
    ${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/appcompat)
add_host_tool(xml2sdb ${SOURCE})
target_link_libraries(xml2sdb PRIVATE host_includes)

if(MSVC)
    # Disable warning "'=': conversion from 'a' to 'b', possible loss of data"
    target_compile_options(xml2sdb PRIVATE "/wd4244")
else()
    target_compile_options(xml2sdb PRIVATE "-fshort-wchar")
endif()
