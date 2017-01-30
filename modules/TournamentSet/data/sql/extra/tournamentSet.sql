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

-- Dump della struttura di tabella extra.tournamentset_characters_active
CREATE TABLE IF NOT EXISTS `tournamentset_characters_active` (
  `id` int(11) NOT NULL,
  `season` int(11) NOT NULL,
  `spec` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Dump dei dati della tabella extra.tournamentset_characters_active: ~1 rows (circa)
DELETE FROM `tournamentset_characters_active`;
/*!40000 ALTER TABLE `tournamentset_characters_active` DISABLE KEYS */;
INSERT INTO `tournamentset_characters_active` (`id`, `season`, `spec`) VALUES
	(15, 7, 201);
/*!40000 ALTER TABLE `tournamentset_characters_active` ENABLE KEYS */;

-- Dump della struttura di tabella extra.tournament_set
CREATE TABLE IF NOT EXISTS `tournament_set` (
  `season` int(11) NOT NULL,
  `class` int(11) NOT NULL,
  `spec` int(11) NOT NULL,
  `name` text NOT NULL,
  `head` int(11) DEFAULT NULL,
  `neck` int(11) DEFAULT NULL,
  `shoulders` int(11) DEFAULT NULL,
  `body` int(11) DEFAULT NULL,
  `chest` int(11) DEFAULT NULL,
  `waist` int(11) DEFAULT NULL,
  `legs` int(11) DEFAULT NULL,
  `feet` int(11) DEFAULT NULL,
  `wrists` int(11) DEFAULT NULL,
  `hands` int(11) DEFAULT NULL,
  `finger1` int(11) DEFAULT NULL,
  `finger2` int(11) DEFAULT NULL,
  `trinket1` int(11) DEFAULT NULL,
  `trinket2` int(11) DEFAULT NULL,
  `back` int(11) DEFAULT NULL,
  `mainHand` int(11) DEFAULT NULL,
  `offHand` int(11) DEFAULT NULL,
  `ranged` int(11) DEFAULT NULL,
  `tabard` int(11) DEFAULT NULL,
  PRIMARY KEY (`season`,`class`,`spec`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Dump dei dati della tabella extra.tournament_set: ~2 rows (circa)
DELETE FROM `tournament_set`;
/*!40000 ALTER TABLE `tournament_set` DISABLE KEYS */;
INSERT INTO `tournament_set` (`season`, `class`, `spec`, `name`, `head`, `neck`, `shoulders`, `body`, `chest`, `waist`, `legs`, `feet`, `wrists`, `hands`, `finger1`, `finger2`, `trinket1`, `trinket2`, `back`, `mainHand`, `offHand`, `ranged`, `tabard`) VALUES
	(7, 2, 1, 'Paladino Retribution', 51277, 0, 51279, 0, 51275, 0, 51278, 0, 0, 51276, 0, 0, 0, 0, 0, 0, 0, 0, 0),
	(7, 2, 2, 'Paladino Holy', 51277, 0, 51279, 0, 51275, 0, 51278, 0, 0, 51276, 0, 0, 0, 0, 0, 0, 0, 0, 0);
/*!40000 ALTER TABLE `tournament_set` ENABLE KEYS */;

-- Dump della struttura di tabella extra.tournament_socket_enchants
CREATE TABLE IF NOT EXISTS `tournament_socket_enchants` (
  `itemId` int(11) NOT NULL,
  `socket1` int(11) DEFAULT '0',
  `socket2` int(11) DEFAULT '0',
  `socket3` int(11) DEFAULT '0',
  `enchant` int(11) DEFAULT '0',
  PRIMARY KEY (`itemId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Dump dei dati della tabella extra.tournament_socket_enchants: ~1 rows (circa)
DELETE FROM `tournament_socket_enchants`;
/*!40000 ALTER TABLE `tournament_socket_enchants` DISABLE KEYS */;
INSERT INTO `tournament_socket_enchants` (`itemId`, `socket1`, `socket2`, `socket3`, `enchant`) VALUES
	(51277, 40118, 0, 0, 0);
/*!40000 ALTER TABLE `tournament_socket_enchants` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
