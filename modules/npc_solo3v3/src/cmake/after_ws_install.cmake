if ( MSVC )
  add_custom_command(TARGET worldserver
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_3V3SOLOQ_DIR}/conf/3v3_soloq.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
elseif ( MINGW )
  add_custom_command(TARGET worldserver
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_3V3SOLOQ_DIR}/conf/3v3_soloq.conf.dist ${CMAKE_BINARY_DIR}/bin/
  )
endif()

install(FILES "${CMAKE_3V3SOLOQ_DIR}/conf/3v3_soloq.conf.dist" DESTINATION ${CONF_DIR})
