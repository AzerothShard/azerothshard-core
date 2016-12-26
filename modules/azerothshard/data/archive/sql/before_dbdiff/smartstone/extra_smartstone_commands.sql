DROP TABLE IF EXISTS `smartstone_commands`;
CREATE TABLE `smartstone_commands` (
  `id` INT(10) NOT NULL COMMENT 'command id',
  `text` TEXT NOT NULL COMMENT 'text that will be shown to the player',
  `item` INT(10) DEFAULT NULL COMMENT 'item that will provide this command',
  `icon` INT(10) DEFAULT NULL COMMENT 'icon id for the command',
  `parent_menu` INT(10) DEFAULT NULL COMMENT 'menu that will contain the command',
  `type` INT(10) DEFAULT '1' COMMENT 'type of command',
  `action` INT(10) DEFAULT NULL COMMENT 'based on type',
  `charges` INT(10) DEFAULT '-1' COMMENT 'number of times it can be used',
  `duration` INT(10) DEFAULT '0' COMMENT 'duration in minutes',
  `comment` TEXT COMMENT 'description of the command',
  PRIMARY KEY (`id`),
  UNIQUE KEY `item` (`item`)
);

/* debug stuff*/
DELETE FROM `smartstone_commands`;
INSERT INTO `smartstone_commands` (id, `text`, item, icon, parent_menu, `type`, `action`, charges, duration, `comment`) VALUES 
(1, "Teletrasportami al Mercato Nero", 987890, 2, 1, 1, 0, 2, 0, "Teletrasporto al Mercato Nero"),
(2, "Demorph", 987892, 0, 1, 1, 0, -1, 2, "Demorpha il player"),
(3, "Mucca Morph", 987891, 0, 2, 1, 0, -1, 2, "Morpha il player in mucca"),
(4, "Prova menu", 987893, 1, 1, 2, 2, -1, 0, "test"),
(5, "Prova menu 2", NULL, 2, 2, 2, 3, -1, 0, "test"),
(6, "Prova menu 3", NULL, 2, 3, 2, 1, -1, 0, "test");