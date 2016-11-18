-- Max Skill
DELETE FROM command WHERE name = 'azth maxskill';
INSERT INTO command VALUES('azth maxskill', 0, 'Max all skill of targeted player');
DELETE FROM trinity_string WHERE entry = 90001;
INSERT INTO trinity_string(entry, content_default) VALUES(90001, 'The target level must be level %u');

-- xp rate
DELETE FROM command WHERE name = 'azth xp';
INSERT INTO command VALUES('azth xp', 0, 'Set a custom xp rate');

-- guild houses

INSERT INTO `command` (`name`, `security`, `help`) VALUES ('npc guildadd', '2', 'Sintassi: \r\n\r\nnpc guildadd '); 
UPDATE `command` SET `help` = 'Syntax: \r\n\r\nnpc guildadd [creature] [guild_house_id] [guild_house_add_id]' WHERE `name` = 'npc guildadd'; 
INSERT INTO `command` (`name`, `security`) VALUES ('gobject guildadd', '2'); 
UPDATE `command` SET `help` = 'Syntax: \r\n\r\ngobject guildadd [object] [guild_house_id] [guild_house_add_id]' WHERE `name` = 'gobject guildadd'; 
