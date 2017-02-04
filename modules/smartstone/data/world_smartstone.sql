UPDATE `item_template` SET `name` = "Smartstone", `Flags` = 64, `ScriptName` = 'azth_smart_stone', stackable = 1, `spellid_1` = 36177, maxcount = 1 WHERE (entry = 32547);


DELETE FROM item_template WHERE entry IN (987890, 987891, 987892, 987893, 987894, 987895);
INSERT INTO item_template (entry, class, subclass, NAME, displayid, quality, description, scriptname, flags, stackable, `spellid_1`, buyprice) VALUES
(987890, 0, 8, "Change Faction!", 1542, 5, "Aggiungi alla smartstone il cambio fazione! (1 carica), ATTENZIONE: COL CAMBIO FAZIONE SI PERDE ARENA RATING. NON È POSSIBILE EFFETTUARLO SE SI HANNO ITEM IN ASTA O IN MAIL", "smart_stone_command", 64, 1, 36177, 1000),
(987891, 0, 8, "Rename!", 7629, 5, "Aggiungi alla smartstone il rename! (1 carica)", "smart_stone_command", 64, 1, 36177, 100000),
(987892, 0, 8, "Change Race!", 472, 5, "Aggiungi alla smartstone il cambio razza! (1 carica)", "smart_stone_command", 64, 1, 36177, 10000),
(987893, 0, 8, "Jukebox!", 41449, 5, "Divertiti con la tua musica! (1 carica)", "smart_stone_command", 64, 1, 36177, 10000),
(987894, 0, 8, "Herbalism Bonus Loot!", 7396, 5, "Aumenta la chance di trovare materiali usando Herbalism! (passivo)", "smart_stone_command", 64, 1, 36177, 10000),
(987895, 0, 8, "Mining Bonus Loot!", 6568, 5, "Aumenta la chance di trovare materiali usando Mining! (passivo)", "smart_stone_command", 64, 1, 36177, 10000);


DELETE FROM creature_template WHERE entry = 170000;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
('170000','0','0','0','0','0','1060','0','0','0','Smartstore','AzerothShard',NULL,'0','82','83','0','31','1','1','1.14286','1','0','13','17','0','42','1','2000','2000','1','512','2048','0','0','0','0','0','0','9','13','100','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','1','1','1','1','0','0','1','0','2','smartstone_vendor','12340');

UPDATE creature_template SET npcflag = 129 WHERE entry = 170000;

DELETE FROM npc_vendor WHERE entry = 170000;
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987890','0','0','0');
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987891','0','0','0');
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987892','0','0','0');
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987893','0','0','0');
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987894','0','0','0');
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987895','0','0','0');

DELETE FROM playercreateinfo_item WHERE itemid = 32547;
INSERT INTO playercreateinfo_item (race, class, itemid, amount) VALUES
(0,0,32547,1),
(0,1,32547,1),
(0,2,32547,1),
(0,3,32547,1),
(0,4,32547,1),
(0,5,32547,1),
(0,6,32547,1),
(0,7,32547,1),
(0,8,32547,1),
(0,9,32547,1),
(0,11,32547,1),
(1,0,32547,1),
(1,1,32547,1),
(1,2,32547,1),
(1,3,32547,1),
(1,4,32547,1),
(1,5,32547,1),
(1,6,32547,1),
(1,7,32547,1),
(1,8,32547,1),
(1,9,32547,1),
(1,11,32547,1),
(2,0,32547,1),
(2,1,32547,1),
(2,2,32547,1),
(2,3,32547,1),
(2,4,32547,1),
(2,5,32547,1),
(2,6,32547,1),
(2,7,32547,1),
(2,8,32547,1),
(2,9,32547,1),
(2,11,32547,1),
(3,0,32547,1),
(3,1,32547,1),
(3,2,32547,1),
(3,3,32547,1),
(3,4,32547,1),
(3,5,32547,1),
(3,6,32547,1),
(3,7,32547,1),
(3,8,32547,1),
(3,9,32547,1),
(3,11,32547,1),
(4,0,32547,1),
(4,1,32547,1),
(4,2,32547,1),
(4,3,32547,1),
(4,4,32547,1),
(4,5,32547,1),
(4,6,32547,1),
(4,7,32547,1),
(4,8,32547,1),
(4,9,32547,1),
(4,11,32547,1),
(5,0,32547,1),
(5,1,32547,1),
(5,2,32547,1),
(5,3,32547,1),
(5,4,32547,1),
(5,5,32547,1),
(5,6,32547,1),
(5,7,32547,1),
(5,8,32547,1),
(5,9,32547,1),
(5,11,32547,1),
(6,0,32547,1),
(6,1,32547,1),
(6,2,32547,1),
(6,3,32547,1),
(6,4,32547,1),
(6,5,32547,1),
(6,6,32547,1),
(6,7,32547,1),
(6,8,32547,1),
(6,9,32547,1),
(6,11,32547,1),
(7,0,32547,1),
(7,1,32547,1),
(7,2,32547,1),
(7,3,32547,1),
(7,4,32547,1),
(7,5,32547,1),
(7,6,32547,1),
(7,7,32547,1),
(7,8,32547,1),
(7,9,32547,1),
(7,11,32547,1),
(8,0,32547,1),
(8,1,32547,1),
(8,2,32547,1),
(8,3,32547,1),
(8,4,32547,1),
(8,5,32547,1),
(8,6,32547,1),
(8,7,32547,1),
(8,8,32547,1),
(8,9,32547,1),
(8,11,32547,1),
(10,0,32547,1),
(10,1,32547,1),
(10,2,32547,1),
(10,3,32547,1),
(10,4,32547,1),
(10,5,32547,1),
(10,6,32547,1),
(10,7,32547,1),
(10,8,32547,1),
(10,9,32547,1),
(10,11,32547,1),
(11,0,32547,1),
(11,1,32547,1),
(11,2,32547,1),
(11,3,32547,1),
(11,4,32547,1),
(11,5,32547,1),
(11,6,32547,1),
(11,7,32547,1),
(11,8,32547,1),
(11,9,32547,1),
(11,11,32547,1);

-- new commands
DELETE FROM smartstone_commands WHERE id IN(10,11);
INSERT INTO smartstone_commands VALUES (10, "Max Skill", NULL,9, 2, 1, 0, -1, 0, "Max Skill Command");
INSERT INTO smartstone_commands VALUES (11, "Change exp", NULL,4, 2, 3, 0, -1, 0, "Azth xp command");
