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

DELETE FROM `smartstone_commands`;
INSERT INTO `smartstone_commands` (id, `text`, item, icon, parent_menu, `type`, `action`, charges, duration, `comment`) VALUES 
(1, "Teletrasportami al Mercato Nero", NULL, 2, 1, 1, 0, 2, 0, "Teletrasporto al Mercato Nero"),
(2, "Change Faction", 987890, 0, 2, 1, 0, 1, 0, "Change Faction 1 charge"),
(3, "Rename", 987891, 0, 2, 1, 0, 1, 0, "Rename character 1 charge"),
(4, "Menù character", NULL, 1, 1, 2, 2, -1, 0, "submenu character"),
(5, "Change Race", 987892, 0, 2, 1, 3, 1, 0, "Change character race 1 charge");