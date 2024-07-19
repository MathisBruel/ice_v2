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
INSERT INTO `groups` (`name`, `id_group_1`) VALUES
("France", NULL),
("USA", NULL),
("Espagne", NULL),
("Estonie", NULL),
("Inde", NULL);
/*
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
(1,1,1,"/"),
(5,1,2,"/"),
(3,1,2,"/"),
(3,1,1,"/");
INSERT INTO `connection` (``) VALUES 
();
INSERT INTO `site` (``) VALUES 
();
INSERT INTO `server_pair_configuration` (``) VALUES 
();
INSERT INTO `cpl` (``) VALUES 
();
*/