# Base de données du WebCentral

Voici le modèle conceptuel

![MCD!](WebCentral-Database-MCD.png "MCD")

## Description des entités de la base de données

### Groups

Cette entité contient des groupes de cinémas. Dans notre utilisation, les occurrences de l'entité 'Groups' seront structurées sous la forme d'un unique arbre. Un nœud sera donc le nœud racine de tous les autres. L'association réflexive HasParentGroup décrit la relation de parentalité entre les nœuds de l'arbre. Les nœuds directement enfants aux nœuds racines décriront des espaces géographiques (Inde, Espagne, Estonie, etc ...). Les nœuds du niveau suivant décriront les exploitant de salles locales. Cette structure ne permet donc pas de modéliser la présence d'exploitants internationale, car ceux-ci devront partager plusieurs parents (donc plusieurs lieux géographiques). Or, une structure en arbre ne le permet pas. Cependant, si cela s'avère nécessaire, il sera possible de faire évoluer la structure en arbre vers une structure en graphe cyclique et ainsi permettre à un nœud d'exploitant de salle d'avoir plusieurs parents. Pour cela, il faudra modifier les cardinalités de l'association "Has Parent Group" vers 0,n-0,n .

### Site

Un site est un cinéma ayant une ou plusieurs salles ICEs. Un site ne peut être relier qu'a un seul groupe.

### Connection

Cette entité définie la méthode de connexion sur un site (proxy, ssh, ftp ).

### Server Pair Configuration

Cette entité décrit les informations unique pour chaque server auditorium, elle peut également décrire les information pour un couple de serveur (auditorium + hallway) dans les cas d'héritage avec Hallway server configuration.

### Auditorium Par, Auditorium Panel, Auditorium BackLight & Auditorium MovingHead

Liste du matériel présent dans l'auditorium, hors éclairage ménage.

### Hallway Server Configuration

Cette entité contient les informations unique du serveur couloir.

### Hallway Par & Hallway Panel

Ces 2 entités constituent la liste du matériel présent dans le Hallway Server Configuration.

### Default Server Configuration Hallway & Default Server Configuration Auditorium

Default Server Configuration Hallway contient la liste des informations communes entre les serveurs couloirs.
Default Server Configuration Auditorium contient la liste des informations communes entre les serveurs auditoriums.

### Type, Localisation & Movie

Ces 3 entités contiennent des informations relatives à une release. Elles forment une clé primaire composée pour l'entité Releases.

### Releases

La release représente un ensemble de CPL partageant des propriétés communes (même film, même Localisation, même type). Pour un même film cible, il peut exister plusieurs type.

### CPL

Cette association résulte sur la création d'une table d'association. L'association CPL représente un fichier installé sur le serveur auditorium.

### Sync

Cette association résulte sur la création d'une table d'association. Elle contient les informations sur le fichier sync. Elle a une contrainte d'inclusion sur l'association CPL, car un fichier sync est toujours en lien avec un CPL.
Cette contrainte d'inclusion est représentée par un trigger dans le modèle phisyque de données.

```
CREATE TRIGGER `cpl_exists`
BEFORE INSERT ON `sync`
FOR EACH ROW
BEGIN
IF NOT EXISTS (SELECT * FROM `cpl` WHERE id_serv_pair_config = NEW.id_serv_pair_config && id_type = NEW.id_type && id_movie = NEW.id_movie &   id_localisation = NEW.id_localisation) THEN
SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT='Aucun CPL correspondant';
END IF;
END;
```

## Annexes

### MLD

![MLD!](WebCentral-Database-MLD.png "MLD")
