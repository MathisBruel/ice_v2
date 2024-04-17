-- MySQL dump 10.13  Distrib 8.0.28, for Win64 (x86_64)
--
-- Host: localhost    Database: ice
-- ------------------------------------------------------
-- Server version 5.7.30-0ubuntu0.18.04.1

use ice;

CREATE USER IF NOT EXISTS "cinelight"@"localhost" IDENTIFIED BY "R@qazwsx2";
GRANT ALL PRIVILEGES ON *.* TO `cinelight`@`localhost` WITH GRANT OPTION;

INSERT INTO `group` (`name`,`description`)
  VALUES ('Charente', 'Les cinemas du 17');
INSERT INTO `cinema` (`name`)
  VALUES ('La Rochelle');
INSERT INTO `link_cinema_group` (`id_group`, `id_cinema`)
  VALUES (5, 4);
INSERT INTO `auditorium` (`id_cinema`, `name`, `room`, `type_ims`, `ip_ims`, `port_ims`, `user_ims`, `pass_ims`)
  VALUES (4, 'Salle 1', 2, 1,'127.0.0.1', 8080, 'admin', 'admin');
INSERT INTO `server` (`id_auditorium`, `type_server`, `ip`)
  VALUES (4,1,"192.167.50.10"),(4,2,"192.167.50.20");
INSERT INTO `cpl` (`name`, `uuid`, `path_cpl`, `path_sync`, `type_cpl`)
  VALUES ("CPL1", "2b79b259-384d-478c-adec-f19f01bbc574", "path1", "path2", 1), ("CPL2", "2b79b259-384d-478c-adec-f19f01bbc575", "path1", "path2", 2),
("CPL3", "2b79b259-384d-478c-adec-f19f01bbc576", "path1", "", 3);