-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Versione server:              5.6.34-log - MySQL Community Server (GPL)
-- S.O. server:                  Win64
-- HeidiSQL Versione:            9.4.0.5125
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- Dump della struttura di tabella characters.azth_tournamentset_active
CREATE TABLE IF NOT EXISTS `azth_tournamentset_active` (
  `id` int(11) NOT NULL,
  `season` int(11) NOT NULL,
  `spec` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT;

-- Dump dei dati della tabella characters.azth_tournamentset_active: ~2 rows (circa)
DELETE FROM `azth_tournamentset_active`;
/*!40000 ALTER TABLE `azth_tournamentset_active` DISABLE KEYS */;
INSERT INTO `azth_tournamentset_active` (`id`, `season`, `spec`) VALUES
	(15, 7, 1),
	(377, 7, 2);
/*!40000 ALTER TABLE `azth_tournamentset_active` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
