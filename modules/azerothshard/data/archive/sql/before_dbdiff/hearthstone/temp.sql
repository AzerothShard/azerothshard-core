DROP TABLE IF EXISTS sw_characters.`character_smartstone_commands`;
CREATE TABLE sw_characters.`character_smartstone_commands`
( `playerGuid` INT(10) NOT NULL COMMENT 'player guid', 
`command` INT(10) NOT NULL COMMENT 'player command id',
  `dateExpired` BIGINT(10) NOT NULL DEFAULT '-1',
  `charges` INT(10) NOT NULL DEFAULT '-1'); 

ALTER TABLE sw_characters.`character_smartstone_commands`
  ADD CONSTRAINT `character_smartstone_commands` UNIQUE(playerGuid, command);

UPDATE sw_world.`item_template` SET `Flags` = 64, `ScriptName` = 'azth_smart_stone', stackable = 1, `spellid_1` = 36177, maxcount = 5 WHERE (entry = 32547);

DROP TABLE IF EXISTS `sw_extra`.`smartstone_commands`;
CREATE TABLE `sw_extra`.`smartstone_commands` (
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

DELETE FROM `sw_extra`.`smartstone_commands`;
INSERT INTO `sw_extra`.`smartstone_commands` (id, `text`, item, icon, parent_menu, `type`, `action`, `comment`) VALUES 
(1, "Teletrasportami al Mercato Nero", 987890, 2, 1, 1, 0, "Teletrasporto al Mercato Nero"),
(2, "Demorph", NULL, 0, 1, 1, 0, "Demorpha il player"),
(3, "Mucca Morph", 987891, 0, 2, 1, 0, "Morpha il player in mucca"),
(4, "Prova menu", NULL, 1, 1, 2, 2, "test"),
(5, "Prova menu 2", NULL, 2, 2, 2, 3, "test"),
(6, "Prova menu 3", NULL, 2, 3, 2, 1, "test");

DELETE FROM sw_world.item_template WHERE entry IN (987890, 987891);
INSERT INTO sw_world.item_template (entry, class, subclass, NAME, displayid, quality, description, scriptname, flags, stackable, `spellid_1`, buyprice) VALUES
(987890, 0, 8, "Teletrasporto al Mercato Nero", 1542, 5, "Aggiungi alla smartstone", "smart_stone_command", 64, 1, 36177, 1000),
(987891, 0, 8, "Mucca!", 472, 5, "Aggiungi alla smartstone la muccaAaAAaaaaAAAaa", "smart_stone_command", 64, 1, 36177, 100000);

DELETE FROM sw_world.creature_template WHERE entry = 170000;
INSERT INTO sw_world.`creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
('170000','0','0','0','0','0','1060','0','0','0','Smartstone','AzerothShard',NULL,'0','10','10','0','31','4224','1','1.14286','1','0','13','17','0','42','1','2000','2000','1','512','2048','0','0','0','0','0','0','9','13','100','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','1','1','1','1','0','0','1','0','2','','12340');

DELETE FROM sw_world.npc_vendor WHERE entry = 170000;
INSERT INTO sw_world.`npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987890','0','0','0');
INSERT INTO sw_world.`npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987891','0','0','0');
