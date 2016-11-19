


  PRIMARY KEY (`add_type`)
  `add_type` int(16) unsigned NOT NULL,
  `comment` varchar(255) NOT NULL DEFAULT '',
  `minguildsize` int(16) unsigned NOT NULL DEFAULT '1',
  `price` bigint(20) NOT NULL DEFAULT '1000',
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40000 ALTER TABLE `guildhouses_addtype` DISABLE KEYS */;
/*!40000 ALTER TABLE `guildhouses_addtype` ENABLE KEYS */;
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
CREATE TABLE `guildhouses_addtype` (
DROP TABLE IF EXISTS `guildhouses_addtype`;
INSERT INTO `guildhouses_addtype` (`add_type`, `comment`, `price`, `minguildsize`) VALUES (2,'Guardia',8000,99999999);
INSERT INTO `guildhouses_addtype` (`add_type`, `comment`, `price`, `minguildsize`) VALUES (3,'Professioni',1500,30);
INSERT INTO `guildhouses_addtype` (`add_type`, `comment`, `price`, `minguildsize`) VALUES (4,'PvP',1000,30);
INSERT INTO `guildhouses_addtype` (`add_type`, `comment`, `price`, `minguildsize`) VALUES (5,'Manichini',1500,30);
INSERT INTO `guildhouses_addtype` (`add_type`, `comment`, `price`, `minguildsize`) VALUES (6,'Vendors',2500,30);
INSERT INTO `guildhouses_addtype` (`add_type`, `comment`, `price`, `minguildsize`) VALUES (7,'Ambiente',4000,99999999);
INSERT INTO `guildhouses_addtype` (`add_type`, `comment`, `price`, `minguildsize`) VALUES (8,'Custom',6500,99999999);
INSERT INTO `guildhouses_addtype` (`add_type`, `comment`, `price`, `minguildsize`) VALUES (9,'Utilità Varie',8500,99999999);
LOCK TABLES `guildhouses_addtype` WRITE;
UNLOCK TABLES;
