INSERT INTO `azerothcore_clean_world`.`command` (`name`, `security`, `help`) VALUES('azth maxskill', '0', 'Max all skill of targeted player');
INSERT INTO `azerothcore_clean_world`.`command` (`name`, `security`, `help`) VALUES('azth xp', '0', 'Set a custom xp rate');
INSERT INTO `azerothcore_clean_world`.`command` (`name`, `security`, `help`) VALUES('bank', '1', 'Syntax: .bank\r\n\r\nShow your bank inventory.');
INSERT INTO `azerothcore_clean_world`.`command` (`name`, `security`, `help`) VALUES('debug send chatmessage', '3', 'Syntax: ');
INSERT INTO `azerothcore_clean_world`.`command` (`name`, `security`, `help`) VALUES('instance stats', '1', 'Syntax: .instance stats\r\n Shows statistics about instances.');
INSERT INTO `azerothcore_clean_world`.`command` (`name`, `security`, `help`) VALUES('npc evade', '2', 'Syntax: .npc evade [reason] [force]\nMakes the targeted NPC enter evade mode.\nDefaults to specifying EVADE_REASON_OTHER, override this by providing the reason string (ex.: .npc evade EVADE_REASON_BOUNDARY).\nSpecify \'force\' to clear any pre-existing evade state before evading - this may cause weirdness, use at your own risk.');
INSERT INTO `azerothcore_clean_world`.`command` (`name`, `security`, `help`) VALUES('qc', '0', 'Syntax: .qc [Quest]');
