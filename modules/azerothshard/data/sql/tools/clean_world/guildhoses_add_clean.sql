# remove unexistant objects from guildhouses_add

DELETE FROM guildhouses_add WHERE guid NOT IN ( SELECT guid FROM gameobjects );
