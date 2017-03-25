CU_SET_PATH("CMAKE_MOD_HEARTHSTONE_MODE_DIR" "${CMAKE_CURRENT_LIST_DIR}")

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/scripts/azth_custom_hearthstone_mode.cpp")
AC_ADD_SCRIPT_LOADER("HearthStoneMode" "${CMAKE_CURRENT_LIST_DIR}/src/scripts/hearthstone_mode_loader.h")

CU_ADD_HOOK(BEFORE_GAME_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/src/cmake/before_game.cmake")
