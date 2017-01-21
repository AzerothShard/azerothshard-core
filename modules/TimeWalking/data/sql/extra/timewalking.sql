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

-- Dump della struttura di tabella extra.timewalking
CREATE TABLE IF NOT EXISTS `timewalking` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL DEFAULT '0',
  `exp` int(11) NOT NULL DEFAULT '0',
  `phase` int(3) NOT NULL DEFAULT '0',
  `level` int(11) NOT NULL DEFAULT '0',
  `bonus` int(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8;

-- Dump dei dati della tabella extra.timewalking: ~14 rows (circa)
/*!40000 ALTER TABLE `timewalking` DISABLE KEYS */;
INSERT INTO `timewalking` (`id`, `name`, `exp`, `phase`, `level`, `bonus`) VALUES
	(1, 'Ragefire Chasm', 1, 1, 10, 1),
	(2, 'Deadmines', 1, 1, 10, 0),
	(3, 'Blackfathom Deeps', 1, 2, 15, 0),
	(4, 'The Stockade', 1, 2, 15, 0),
	(5, 'Razorfen Kraul', 1, 3, 25, 1),
	(6, 'Uldaman', 1, 4, 30, 0),
	(7, 'Razorfen Downs', 1, 5, 35, 0),
	(8, 'Blackrock Depths: Detention Block', 1, 6, 42, 0),
	(9, 'Lower Blackrock Spire', 1, 7, 48, 0),
	(10, 'Molten Core', 1, 8, 50, 0),
	(11, 'Hellfire Citadel: Hellfire Ramparts', 2, 9, 57, 1),
	(12, 'Coilfang Reservoir: The Steamvault', 2, 10, 65, 0),
	(13, 'Coilfang Reservoir: Serpentshrine Cavern', 2, 11, 70, 0),
	(14, 'Onyxia\'s Lair', 3, 12, 80, 0);
/*!40000 ALTER TABLE `timewalking` ENABLE KEYS */;

-- Dump della struttura di tabella extra.timewalking_levels
CREATE TABLE IF NOT EXISTS `timewalking_levels` (
  `level` int(11) NOT NULL,
  `health` int(11) NOT NULL,
  `resistance` int(11) NOT NULL,
  `healing` int(11) NOT NULL,
  `damage` int(11) NOT NULL,
  `power_cost` int(11) NOT NULL,
  `all_stat` int(11) NOT NULL,
  `mana` float NOT NULL,
  `crit_chance` int(11) NOT NULL,
  `miss_chance` int(11) NOT NULL,
  `dodge_chance` int(11) NOT NULL,
  `parry_chance` int(11) NOT NULL,
  `block_chance` int(11) NOT NULL,
  PRIMARY KEY (`level`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Dump dei dati della tabella extra.timewalking_levels: ~0 rows (circa)
/*!40000 ALTER TABLE `timewalking_levels` DISABLE KEYS */;
INSERT INTO `timewalking_levels` (`level`, `health`, `resistance`, `healing`, `damage`, `power_cost`, `all_stat`, `mana`, `crit_chance`, `miss_chance`, `dodge_chance`, `parry_chance`, `block_chance`) VALUES
	(50, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20);
/*!40000 ALTER TABLE `timewalking_levels` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
