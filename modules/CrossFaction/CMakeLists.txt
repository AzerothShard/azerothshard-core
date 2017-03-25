CU_SET_PATH("CMAKE_MOD_CROSSFACTION_DIR" "${CMAKE_CURRENT_LIST_DIR}")

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/scripts/CrossFaction.cpp")
AC_ADD_SCRIPT_LOADER("CrossFaction" "${CMAKE_CURRENT_LIST_DIR}/src/scripts/crossfaction_loader.h")

CU_ADD_HOOK(BEFORE_GAME_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/src/cmake/before_game.cmake")


