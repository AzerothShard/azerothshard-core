CU_SET_PATH("CMAKE_MOD_GUILDHOUSE_DIR" "${CMAKE_CURRENT_LIST_DIR}")

AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/scripts/npc_guildhouse.cpp")
AC_ADD_SCRIPT_LOADER("GuildHouse" "${CMAKE_CURRENT_LIST_DIR}/src/scripts/guildhouse_loader.h")

CU_ADD_HOOK(BEFORE_GAME_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/src/cmake/before_game.cmake")
