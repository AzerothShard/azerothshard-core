UPDATE `item_template` SET `name` = "Smartstone", `Flags` = 64, `ScriptName` = 'azth_smart_stone', stackable = 1, `spellid_1` = 36177, maxcount = 1 WHERE (entry = 32547);


DELETE FROM item_template WHERE entry IN (987890, 987891, 987892, 987893, 987894, 987895);
INSERT INTO item_template (entry, class, subclass, NAME, displayid, quality, description, scriptname, flags, stackable, `spellid_1`, buyprice) VALUES
(987890, 0, 8, "Change Faction!", 1542, 5, "Aggiungi alla smartstone il cambio fazione! (1 carica)", "smart_stone_command", 64, 1, 36177, 1000),
(987891, 0, 8, "Rename!", 7629, 5, "Aggiungi alla smartstone il rename! (1 carica)", "smart_stone_command", 64, 1, 36177, 100000),
(987892, 0, 8, "Change Race!", 472, 5, "Aggiungi alla smartstone il cambio razza! (1 carica)", "smart_stone_command", 64, 1, 36177, 10000),
(987893, 0, 8, "Jukebox!", 41449, 5, "Divertiti con la tua musica! (1 carica)", "smart_stone_command", 64, 1, 36177, 10000),
(987894, 0, 8, "Herbalism Bonus Loot!", 7396, 5, "Aumenta la chance di trovare materiali usando Herbalism! (passivo)", "smart_stone_command", 64, 1, 36177, 10000),
(987895, 0, 8, "Mining Bonus Loot!", 6568, 5, "Aumenta la chance di trovare materiali usando Mining! (passivo)", "smart_stone_command", 64, 1, 36177, 10000);


DELETE FROM creature_template WHERE entry = 170000;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid1`, `modelid2`, `modelid3`, `modelid4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `unit_flags2`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `VehicleId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `HoverHeight`, `Health_mod`, `Mana_mod`, `Armor_mod`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`, `VerifiedBuild`) VALUES
('170000','0','0','0','0','0','1060','0','0','0','Smartstone Vendor','AzerothShard',NULL,'0','10','10','0','31','4224','1','1.14286','1','0','13','17','0','42','1','2000','2000','1','512','2048','0','0','0','0','0','0','9','13','100','7','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','','0','3','1','1','1','1','0','0','1','0','2','','12340');


DELETE FROM npc_vendor WHERE entry = 170000;
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987890','0','0','0');
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987891','0','0','0');
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987892','0','0','0');
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987893','0','0','0');
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987894','0','0','0');
INSERT INTO `npc_vendor` (`entry`, `slot`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES('170000','0','987895','0','0','0');

