


  UNIQUE KEY `entry` (`entry`)
  `aura` int(11) DEFAULT '-1',
  `comment` text,
  `default_name` text,
  `entry` int(11) NOT NULL,
  `flags` int(11) DEFAULT '0',
  `scale` float DEFAULT '1',
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40000 ALTER TABLE `azth_morph_template` DISABLE KEYS */;
/*!40000 ALTER TABLE `azth_morph_template` ENABLE KEYS */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET character_set_client = utf8 */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;
CREATE TABLE `azth_morph_template` (
DROP TABLE IF EXISTS `azth_morph_template`;
INSERT INTO `azth_morph_template` (`entry`, `flags`, `default_name`, `scale`, `aura`, `comment`) VALUES (1060,0,'MUCCA!',3,17,'prova muccaaa');
INSERT INTO `azth_morph_template` (`entry`, `flags`, `default_name`, `scale`, `aura`, `comment`) VALUES (30721,0,'The Lich King',3,17,'Lich King Morph');
LOCK TABLES `azth_morph_template` WRITE;
UNLOCK TABLES;
