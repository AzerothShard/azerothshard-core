





DROP TABLE IF EXISTS `quest_bugged`;


CREATE TABLE `quest_bugged` (
  `ID` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `bugged` tinyint(1) NOT NULL DEFAULT '1',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;








