use `ice`;
INSERT INTO `type` (`name`) VALUES 
("Long Métrage"),
("Court Métrage"),
("Trailer"),
("Test"),
("Transitionnel"),
("Classification"),
("Teaser"),
("Code de conduite"),
("Annonce d'intérêt public"),
("Publicité"),
("Clip"),
("Promo"),
("Carte stéréo 3D"),
("Episode"),
("Points forts"),
("Evenements");
INSERT INTO `localisation` (`name`) VALUES 
('France'),
('USA'),
('Espagne'),
('Estonie'),
('Inde'),
('AR-KSA'),
('AR-AE');

# ------------------------------------------ Données de Test ------------------------------------------
INSERT INTO `groups` (`name`) VALUES
("Europe"),
("Amerique"),
("Asie"),
("Afrique"),
("Oceanie");
INSERT INTO `groups` (`name`, `id_group_1`) VALUES
("France", 1),
("USA", 2),
("Espagne", 1),
("Estonie", 1),
("Inde", 3);
INSERT INTO connection (`name`) VALUES
("SSH"),
("VPN");
INSERT INTO site (`name`, `id_connection`, `id_group`) VALUES
("Paris", 1, 6),
("New York", 2, 7),
("Madrid", 1, 8),
("Tallinn", 2, 9),
("Mumbai", 1, 10);
INSERT INTO default_server_configuration_auditorium () VALUES ();
INSERT INTO default_server_configuration_hallway () VALUES ();
INSERT INTO server_pair_configuration (`projection_server_ip`,`auditorium_server_ip`,`id_default_auditorium`,`id_site`,`name`) VALUES
("128.168.0.1","128.168.0.2",1,1,"Salle ICE 1"),
("128.168.0.3","128.168.0.4",1,1,"Salle ICE 2"),
("128.168.1.1","128.168.1.2",1,2,"Salle ICE 1"),
("128.168.1.3","128.168.1.4",1,2,"Salle ICE 2"),
("128.168.1.5","128.168.1.6",1,3,"Salle ICE 1");

INSERT INTO `hallway_server_configuration` (`id_serv_pair_config`, `id_dafault_hallway`) VALUES 
(1,1),
(2,1),
(3,1),
(4,1),
(5,1);

INSERT INTO `movie` (`title`) VALUES 
("test|Toy Story"),
("test|Toy Story 2"),
("test|Toy Story 3"),
("test|Toy Story 4"),
("test|Toy Story 5");
INSERT INTO `releases` (`id_movie`,`id_type`,`id_localisation`,`release_cpl_ref_path`) VALUES 
(1,7,1,"/"),
(2,7,2,"/"),
(2,7,1,"/"),
(5,1,2,"/"),
(3,1,2,"/"),
(3,1,1,"/");
INSERT INTO `cpl` (`uuid`,`name`,`sha1_sync`,`path_cpl`,`id_serv_pair_config`,`id_movie`,`id_type`,`id_localisation` ) VALUES
("123456","Toy Story","123456","/",1,1,7,1),
("123457","Toy Story 2","123457","/",2,2,7,1),
("123458","Toy Story 2","123458","/",3,2,7,2),
("123459","Toy Story 5","123459","/",4,5,1,2),
("123455","Toy Story 3","123455","/",5,5,1,2);

INSERT INTO `sync` (`id_serv_pair_config`,`id_movie`,`id_type`,`id_localisation`,`path_sync`) VALUES
(1,1,7,1,"/1"),
(2,2,7,1,"/2"),
(3,2,7,2,"/2"),
(4,5,1,2,"/5"),
(5,5,1,2,"/3");