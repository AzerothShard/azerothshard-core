


  `data0` double DEFAULT NULL,
  `data1` double DEFAULT NULL
  `flag` tinyint(3) DEFAULT NULL,
  `id` double DEFAULT NULL,
  `name` varchar(2295) DEFAULT NULL,
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40000 ALTER TABLE `custom_npc_tele_category` DISABLE KEYS */;
/*!40000 ALTER TABLE `custom_npc_tele_category` ENABLE KEYS */;
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
CREATE TABLE `custom_npc_tele_category` (
DROP TABLE IF EXISTS `custom_npc_tele_category`;
INSERT INTO `custom_npc_tele_category` (`id`, `name`, `flag`, `data0`, `data1`) VALUES (1,'[Instances Lvl 1-60]',0,0,0);
INSERT INTO `custom_npc_tele_category` (`id`, `name`, `flag`, `data0`, `data1`) VALUES (2,'[Instances Lvl 70+]',5,70,0);
LOCK TABLES `custom_npc_tele_category` WRITE;
UNLOCK TABLES;
