-- character inventory

DELETE FROM `character_inventory` WHERE `item` NOT IN (SELECT `guid` FROM `item_instance`);
