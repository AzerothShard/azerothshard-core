
target_link_libraries(game-interface
  INTERFACE
    modules-interface)

target_link_libraries(game
  PRIVATE
    azth_mod)

