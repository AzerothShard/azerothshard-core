CU_SUBDIRLIST(sub_DIRS  "${CMAKE_MOD_CROSSFACTION_DIR}/src/classes/" TRUE TRUE)
FOREACH(subdir ${sub_DIRS})
        file(GLOB sources "${subdir}/*.cpp" "${subdir}/*.h")

        CU_LIST_ADD_CACHE(game_STAT_SRCS
            "${sources}"
        )

        CU_LIST_ADD_CACHE(game_INCLUDE_DIRS
            ${subdir}
        )

        CU_LIST_ADD_CACHE(scripts_INCLUDE_DIRS
            ${subdir}
        )
ENDFOREACH()
