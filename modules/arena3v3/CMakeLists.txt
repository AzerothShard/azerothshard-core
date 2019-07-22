# 
#
#

# Add SC
AC_ADD_SCRIPTS("${CMAKE_CURRENT_LIST_DIR}/src/scripts/")

# Add SC loader
AC_ADD_SCRIPT_LOADER("3v3" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")

# Add config file
AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/conf/3v3_soloq.conf.dist")
