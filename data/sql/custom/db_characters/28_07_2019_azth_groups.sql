INSERT INTO `version_db_characters` (`sql_rev`) VALUES ('1564268153379319400');

-- Create new table
CREATE TABLE IF NOT EXISTS `azth_groups` (
  `guid` int(10) unsigned NOT NULL,
  `leaderGuid` int(10) unsigned NOT NULL,
  `MaxlevelGroup` smallint(8) unsigned NOT NULL DEFAULT 0,
  `MaxGroupSize` tinyint(3) unsigned NOT NULL DEFAULT 1,
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Insert rows
INSERT INTO `azth_groups` (`guid`, `leaderGuid`, `MaxlevelGroup`, `MaxGroupSize`)
SELECT guid, leaderGuid, MaxlevelGroup, MaxGroupSize FROM groups;

-- Change default table to default
ALTER TABLE `groups` 
DROP COLUMN `MaxlevelGroup`,
DROP COLUMN `MaxGroupSize`;