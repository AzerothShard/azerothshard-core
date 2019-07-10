CU_SET_PATH("CMAKE_3V3SOLOQ_DIR" "${CMAKE_CURRENT_LIST_DIR}")

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/scripts/npc_solo3v3.cpp")
AC_ADD_SCRIPT_LOADER("3v3" "${CMAKE_CURRENT_LIST_DIR}/src/scripts/loader.h")

CU_ADD_HOOK(BEFORE_GAME_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/src/cmake/before_game.cmake")

# Add config file
AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/conf/3v3_soloq.conf.dist")
