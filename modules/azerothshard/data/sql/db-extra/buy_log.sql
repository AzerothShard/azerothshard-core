DROP TABLE IF EXISTS `buy_log`;
CREATE TABLE `buy_log` (
  `playerGuid` BIGINT(10) NOT NULL,
  `item` INT(10) NOT NULL,
  `vendor` BIGINT(10) NOT NULL,
  `price` BIGINT(10) NOT NULL COMMENT 'if < 0 it is an extendedCost'
);