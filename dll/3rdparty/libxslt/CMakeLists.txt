
include_directories(${MERGEOS_SOURCE_DIR}/sdk/include/mergeos/libs/libxslt
                    ${MERGEOS_SOURCE_DIR}/sdk/lib/3rdparty/libwin-iconv)

add_definitions(
    -DHAVE_CONFIG_H
    -DWIN32
    -D_WINDOWS
    -D_MBCS
    -DHAVE_STAT
    -DHAVE_WIN32_THREADS
    -D_REENTRANT
    -DLIBXML_STATIC)

list(APPEND SOURCE
    attributes.c
    attrvt.c
    documents.c
    extensions.c
    extra.c
    functions.c
    imports.c
    keys.c
    namespaces.c
    numbers.c
    pattern.c
    preproc.c
    security.c
    templates.c
    transform.c
    variables.c
    xslt.c
    xsltlocale.c
    xsltutils.c
    precomp.h)

add_library(libxslt MODULE ${SOURCE})
set_module_type(libxslt win32dll)
target_link_libraries(libxslt libxml2 iconv-static zlib)
add_importlibs(libxslt msvcrt ws2_32 kernel32)

if(MSVC)
    add_importlibs(libxslt ntdll)
endif()

if(NOT MSVC)
    target_compile_options(libxslt PRIVATE -Wno-misleading-indentation -Wno-pointer-sign -Wno-unused-function)
endif()

add_pch(libxslt precomp.h SOURCE)
add_cd_file(TARGET libxslt DESTINATION mergeos/system32 FOR all)
