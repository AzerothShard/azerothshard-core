CU_SET_PATH("CMAKE_MOD_SMARTSTONE_DIR" "${CMAKE_CURRENT_LIST_DIR}")

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/scripts/AzthSmartStone.cpp")
AC_ADD_SCRIPT_LOADER("Smartstone" "${CMAKE_CURRENT_LIST_DIR}/src/scripts/smartstone_loader.h")

CU_ADD_HOOK(BEFORE_GAME_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/src/cmake/before_game.cmake")
