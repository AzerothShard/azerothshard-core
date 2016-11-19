


  PRIMARY KEY (`id`)
  `comment` varchar(255) NOT NULL DEFAULT '',
  `faction` int(8) unsigned NOT NULL DEFAULT '3',
  `guildId` bigint(20) NOT NULL DEFAULT '0',
  `id` int(16) unsigned NOT NULL AUTO_INCREMENT,
  `map` int(16) NOT NULL,
  `minguildsize` int(16) unsigned NOT NULL DEFAULT '1',
  `price` bigint(20) NOT NULL DEFAULT '0',
  `x` double NOT NULL,
  `y` double NOT NULL,
  `z` double NOT NULL,
) ENGINE=InnoDB AUTO_INCREMENT=68 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;
/*!40000 ALTER TABLE `guildhouses` DISABLE KEYS */;
/*!40000 ALTER TABLE `guildhouses` ENABLE KEYS */;
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
CREATE TABLE `guildhouses` (
DROP TABLE IF EXISTS `guildhouses`;
INSERT INTO `guildhouses` (`id`, `guildId`, `x`, `y`, `z`, `map`, `comment`, `price`, `faction`, `minguildsize`) VALUES (18,10,-11365.9,-4713.16,6.16,1,'guildhouses grande, south sea (north isle), entrambe le fazioni',90000,3,90);
INSERT INTO `guildhouses` (`id`, `guildId`, `x`, `y`, `z`, `map`, `comment`, `price`, `faction`, `minguildsize`) VALUES (36,4,-2925.269,38.0056,189.911,1,'guildhouse grande, orda, Mulgore',90000,3,90);
INSERT INTO `guildhouses` (`id`, `guildId`, `x`, `y`, `z`, `map`, `comment`, `price`, `faction`, `minguildsize`) VALUES (67,127,-13017.33,-2198.73,9.11,0,'guildhouses grande, Forbidding Sea, Dark Battleship orda',90000,3,90);
INSERT INTO `guildhouses` (`id`, `guildId`, `x`, `y`, `z`, `map`, `comment`, `price`, `faction`, `minguildsize`) VALUES (8,80,-5371.08,-1932.1,87.22,1,'guildhouse media, thousand needles, orda',45000,3,50);
INSERT INTO `guildhouses` (`id`, `guildId`, `x`, `y`, `z`, `map`, `comment`, `price`, `faction`, `minguildsize`) VALUES (9,91,-2263.09,-1903.57,3.13,0,'guildhouses media, Arathi Highlands, entrambe le fazioni, \"We are pirates\" ',45000,3,50);
LOCK TABLES `guildhouses` WRITE;
UNLOCK TABLES;
