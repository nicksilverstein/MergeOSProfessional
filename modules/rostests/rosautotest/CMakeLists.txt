
list(APPEND SOURCE
    CConfiguration.cpp
    CFatalException.cpp
    CInvalidParameterException.cpp
    CJournaledTestList.cpp
    CPipe.cpp
    CPipedProcess.cpp
    CProcess.cpp
    CSimpleException.cpp
    CTestList.cpp
    CVirtualTestList.cpp
    CWebService.cpp
    CWineTest.cpp
    main.cpp
    misc.cpp
    shutdown.cpp
    tools.cpp
    precomp.h)

add_executable(rosautotest ${SOURCE} ${CMAKE_CURRENT_BINARY_DIR}/rosautotestmsg.rc)
target_link_libraries(rosautotest cppstl)
set_target_cpp_properties(rosautotest WITH_EXCEPTIONS)
set_module_type(rosautotest win32cui UNICODE)
add_importlibs(rosautotest advapi32 shell32 user32 wininet msvcrt kernel32 ntdll)
add_pch(rosautotest precomp.h SOURCE)
add_message_headers(ANSI rosautotestmsg.mc)
add_dependencies(rosautotest rosautotestmsg)
add_cd_file(TARGET rosautotest DESTINATION mergeos/system32 FOR all)
