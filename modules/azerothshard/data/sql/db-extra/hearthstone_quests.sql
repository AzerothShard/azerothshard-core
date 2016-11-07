DROP TABLE IF EXISTS `hearthstone_quests`;
CREATE TABLE `hearthstone_quests`(  
  `id` BIGINT(10) NOT NULL,
  `flag` INT(10) DEFAULT 0, -- 1 pve, 2 pvp
  PRIMARY KEY (`id`)
);

-- pve quests
DELETE FROM hearthstone_quests WHERE id >= 100000 AND id <= 100080;
INSERT INTO hearthstone_quests (id) VALUES
(100000),(100001),(100002),(100003),(100004),(100005),(100006),(100007),(100008),(100009),(100010),(100011),(100012),(100013),(100014),(100015),(100016),(100017),(100018),(100019),(100020),(100021),(100022),(100023),(100024),(100025),(100026),(100027),(100028),(100029),(100030),(100031),(100032),(100033),(100034),(100035),(100036),(100037),(100038),(100039),(100040),(100041),(100042),(100043),(100044),(100045),(100046),(100047),(100048),(100049),(100050),(100051),(100052),(100053),(100054),(100055),(100056),(100057),(100058),(100059),(100060),(100061),(100062),(100063),(100064),(100065),(100066),(100067),(100068),(100069),(100070),(100071),(100072),(100073),(100074),(100075),(100076),(100077),(100078),(100079),(100080);

UPDATE hearthstone_quests SET flag = 1 WHERE id >= 100000 AND id <= 100080;

-- pvp quests
DELETE FROM hearthstone_quests WHERE id >= 110000 AND id <= 110039;
INSERT INTO hearthstone_quests (id) VALUES
(110000),(110001),(110002),(110003),(110004),(110005),(110006),(110007),(110008),(110009),(110010),(110011),(110012),(110013),(110014),(110015),(110016),(110017),(110018),(110019),(110020),(110021),(110022),(110023),(110024),(110025),(110026),(110027),(110028),(110029),(110030),(110031),(110032),(110033),(110034),(110035),(110036),(110037),(110038),(110039);

UPDATE hearthstone_quests SET flag = 2 WHERE id >= 110000 AND id <= 110039;