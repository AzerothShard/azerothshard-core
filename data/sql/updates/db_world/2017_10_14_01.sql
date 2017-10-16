-- DB update 2017_10_14_00 -> 2017_10_14_01
DROP PROCEDURE IF EXISTS `updateDb`;
DELIMITER //
CREATE PROCEDURE updateDb ()
proc:BEGIN DECLARE OK VARCHAR(100) DEFAULT 'FALSE';
SELECT COUNT(*) INTO @COLEXISTS
FROM information_schema.COLUMNS
WHERE TABLE_SCHEMA = DATABASE() AND TABLE_NAME = 'version_db_world' AND COLUMN_NAME = '2017_10_14_00';
IF @COLEXISTS = 0 THEN LEAVE proc; END IF;
START TRANSACTION;
ALTER TABLE version_db_world CHANGE COLUMN 2017_10_14_00 2017_10_14_01 bit;
SELECT sql_rev INTO OK FROM version_db_world WHERE sql_rev = '1504704582101308200'; IF OK <> 'FALSE' THEN LEAVE proc; END IF;
--
-- START UPDATING QUERIES
--
--
-- END UPDATING QUERIES
--
COMMIT;
END;
//
DELIMITER ;
CALL updateDb();
DROP PROCEDURE IF EXISTS `updateDb`;
