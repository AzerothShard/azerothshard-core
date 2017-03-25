CU_SET_PATH("CMAKE_MOD_TOURNAMENTSET_DIR" "${CMAKE_CURRENT_LIST_DIR}")

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/scripts/TournamentSet.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/scripts/AzthGearScalingSocket.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/scripts/AzthGearScaling.cpp")
AC_ADD_SCRIPT_LOADER("TournamentSet" "${CMAKE_CURRENT_LIST_DIR}/src/scripts/loader.h")

CU_ADD_HOOK(BEFORE_GAME_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/src/cmake/before_game.cmake")
