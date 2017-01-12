-- --------------------------------------------------------
-- Host:                         25.121.200.252
-- Versione server:              5.5.53-0ubuntu0.14.04.1 - (Ubuntu)
-- S.O. server:                  debian-linux-gnu
-- HeidiSQL Versione:            9.4.0.5125
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- Dump della struttura di tabella azth_1_extra.jukebox_music
CREATE TABLE IF NOT EXISTS `jukebox_music` (
  `author` varchar(50) DEFAULT NULL,
  `title` varchar(50) DEFAULT NULL,
  `sound_ID` int(11) DEFAULT NULL,
  `genre` varchar(50) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Dump dei dati della tabella azth_1_extra.jukebox_music: ~46 rows (circa)
/*!40000 ALTER TABLE `jukebox_music` DISABLE KEYS */;
INSERT INTO `jukebox_music` (`author`, `title`, `sound_ID`, `genre`) VALUES
	('ACDC', 'Highway To Hell', 18022, 'Rock'),
	('Animals', 'House Of The Rising Sun', 18033, 'Rock'),
	('Blink182', 'All The Small Things', 18034, 'Pop punk'),
	('Blur', 'Song 2', 18023, 'Rock'),
	('Coldplay', 'Hymn For The Weekend', 18035, 'Pop'),
	('Deorro', 'Five More Hours', 18036, 'Electronic dance'),
	('Good Charlotte', 'The Anthem', 18037, 'Pop punk'),
	('Gorillaz', ' Feel Good Inc', 18025, 'Rock'),
	('Green Day', '21 Guns', 18026, 'Pop punk'),
	('Green Day', 'Boulevard of Broken Dreams', 18027, 'Pop punk'),
	('Iggy Pop', 'The Passenger', 18038, 'Rock'),
	('Imagine Dragons', 'Radioactive', 18028, 'Rock'),
	('James Blunt', 'Stay The Night', 18039, 'Rock'),
	('Kansas', 'Carry on My Wayward Son', 18029, 'Rock'),
	('The Proclaimers', 'I\'m Gonna Be', 18030, 'Rock'),
	('Twenty One Pilots', 'Heathens', 18031, 'Rock'),
	('X-Ambassadors', 'Renegades', 18032, 'Rock'),
	('Metallica', 'Enter Sandman', 18050, 'Metal'),
	('ACDC', 'You Shook Me All Night Long', 18040, 'Rock'),
	('Iron Maiden', 'Fear of The Dark', 18044, 'Metal'),
	('Iron Maiden', 'The Trooper', 18046, 'Metal'),
	('Avenged Sevenfold', 'Afterlife', 18041, 'Metal'),
	('Leo Moracchioli', 'Eye of the Tiger', 18048, 'Rock'),
	('Iron Maiden', 'Run to the Hills', 18045, 'Metal'),
	('Iron Maiden', 'These Color Don\'t Run', 18047, 'Metal'),
	('Sonata Arctica', 'Fullmoon', 18051, 'Metal'),
	('Avenged Sevenfold', 'M.I.A.', 18042, 'Metal'),
	('Dire Straits', 'Sultans of Swing', 18052, 'Rock'),
	('Megadeth', 'Tornado of Souls', 18049, 'Metal'),
	('Green Day', 'Holiday', 18043, 'Pop punk'),
	('Alan Walker', 'Faded', 18053, 'Electronic dance'),
	('Blink182', 'Anthem Part Two', 18054, 'Pop punk'),
	('Dropkick Murphys', 'The State of Massachusetts', 18055, 'Rock'),
	('Eiffel 65', 'Blue', 18056, 'Electronic dance'),
	('Five Horse Johnson', 'Cherry Red', 18057, 'Rock'),
	('Gabry Ponte', 'Geordie', 18058, 'Electronic dance'),
	('Gigi D\'Agostino', 'L\'Amour Toujours', 18059, 'Electronic dance'),
	('Guns N\' Roses', 'Knockin On Heavens Door', 18060, 'Rock'),
	('Guns N\' Roses', 'Paradise City', 18061, 'Rock'),
	('Guns N\' Roses', 'Sweet Child O\' Mine', 18062, 'Rock'),
	('Jack Holiday & Mike Candys', 'The Riddle Anthem', 18063, 'Pop'),
	('Krewella', 'Alive', 18064, 'Electronic dance'),
	('Martin Garrix', 'In The Name of Love', 18065, 'Future bass'),
	('Martin Garrix', 'Lions In The Wild', 18066, 'House'),
	('Rhapsody', 'Dawn of Victory', 18067, 'Metal'),
	('The Chainsmokers', 'Don\'t Let Me Down', 18068, 'Pop');
/*!40000 ALTER TABLE `jukebox_music` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
