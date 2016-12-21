-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Versione server:              5.6.34-log - MySQL Community Server (GPL)
-- S.O. server:                  Win64
-- HeidiSQL Versione:            9.3.0.4984
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- Dump della struttura di tabella extra.jukebox_music
CREATE TABLE IF NOT EXISTS `jukebox_music` (
  `author` varchar(50) DEFAULT NULL,
  `title` varchar(50) DEFAULT NULL,
  `sound_ID` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Dump dei dati della tabella extra.jukebox_music: ~17 rows (circa)
/*!40000 ALTER TABLE `jukebox_music` DISABLE KEYS */;
INSERT INTO `jukebox_music` (`author`, `title`, `sound_ID`) VALUES
	('ACDC', 'Highway To Hell', 18022),
	('Animals', 'House Of The Rising Sun', 18033),
	('Blink 182', 'All The Small Things', 18034),
	('Blur', 'Song 2', 18023),
	('Coldplay', 'Hymn For The Weekend', 18035),
	('Deorro', 'Five More Hours', 18036),
	('Good Charlotte', 'The Anthem', 18037),
	('Gorillaz', ' Feel Good Inc', 18025),
	('Green Day', '21 Guns', 18026),
	('Green Day', 'Boulevard of Broken Dreams', 18027),
	('Iggy Pop', 'The Passenger', 18038),
	('Imagine Dragons', 'Radioactive', 18028),
	('James Blunt', 'Stay The Night', 18039),
	('Kansas', 'Carry on My Wayward Son', 18029),
	('The Proclaimers', 'I\'m Gonna Be', 18030),
	('Twenty One Pilots', 'Heathens', 18031),
	('X-Ambassadors', 'Renegades', 18032);
/*!40000 ALTER TABLE `jukebox_music` ENABLE KEYS */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
