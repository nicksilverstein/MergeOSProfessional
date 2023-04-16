
# small trick to get the real source directory at this stage
STRING(REPLACE "/PreLoad.cmake" "" MERGEOS_HOME_DIR ${CMAKE_CURRENT_LIST_FILE})

#message("/PreLoad.cmake ... ${MERGEOS_HOME_DIR}")

SET(CMAKE_MODULE_PATH "${MERGEOS_HOME_DIR}/sdk/cmake" CACHE INTERNAL "")

#message("CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

