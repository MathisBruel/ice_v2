# ICE TOOLS

Ce document a pour but de décrire la configuration et l'utilisation des outils pour ICE nouvelle mouture.
Les outils sont  : 

- Base64Decoder
- Base64Encoder
- CALIB-DMX
- CALIB-MH
- CALIB-MH-POS
- CALIB-PANEL
- SYNCER
- SYNCHRO-TEMP

## Base64Decoder

Cet outil windows fonctionne en ligne de commande et permet de décoder une chaine base64.
Il a pour but de retrouver les login et password de connection au FTP.

## Base64Encoder

Cet outil windows fonctionne en ligne de commande et permet de doublement encoder une chaine en base64.
Il permet de créer la chaine de login et password de connection au FTP.

## CALIB-DMX

Cet outil Windows permet de calibrer la balance des couleurs des équipements DMXs Par & MovingHead.

### Configuration

Il faut spécifier le nom du device tel que renseigner dans la conf du serveur ICE ainsi que l'adresse IP du server ICE.

### Utilisation

Il faut pour 2 niveaux de composantes de blanc (une valeur au dessus de 200 et une inférieure à 200) l'équivalent RGB le plus proche. 
Pour cela, mettre le curseur sur un niveau de blanc : l'outil va alterner entre rendre la composante blanche et rendre l'association des composantes RGB.
Il faut ensuite régler les valeurs de R, G et B pour obtenir le blanc désiré.

Enfin, il faut remplir un fichier *.calib* de la sorte : 

```
CALIB WHITE
223 142 205 43
128 86 118 24
```

On donne pour chaque blanc corrigé, le ligne suivante : `WHITE RED GREEN BLUE`

## CALIB-MH

Cet outil Windows permet de régler le défaut de positionnement des movingHeads.

### Configuration

Via l'interface de l'outil, il faut renseigner l'adresse IP du server ICE contrôlant la movingHead ainsi que son nom tel que renseigné dans les fichiers de configuration du server ICE.

### Utilisation

Les panneaux sont montés de manière symétrique dans une salle. Nous allons utiliser cette caractéristique à notre avantage.

#### Pan : selon la profondeur de la salle

- Mettre la valeur de pan à 0°
- Changer la valeur de tilt pour que la movingHead concernée pointe sur le mur d'en face
- Vérifier que le pointage de la movingHead (vis-à-vis des panneaux du mur d'en face) correspond à la position de la movinghead (vis-à-vis des panneaux situés sous la movinghead)
- Modifier la valeur du pan pour que le positionnement coincide. On peut alors lire la valeur du biasPan et la reporter dans le fichier de configuration ICE

#### Tilt : selon la hauteur de la salle

- Garder la valeur de pan corrigée
- Vérifier que le pointage de la movingHead (vis-à-vis des panneaux du mur d'en face) correspond à la position de la movinghead (vis-à-vis des panneaux situés sous la movinghead)
- Modifier la valeur du tilt pour que le positionnement coincide. On peut alors lire la valeur du biasTilt et la reporter dans le fichier de configuration ICE

## CALIB-MH-POS

Cet outil Windows permet de régler les valeurs limites de pointage de chaque movingHead.

### Configuration

Via l'interface de l'outil, il faut renseigner l'adresse IP du server ICE contrôlant la movingHead ainsi que son nom tel que renseigné dans les fichiers de configuration du server ICE.
Il faut renseigner la valeur de zoom de l'équipement (adaptation selon la salle) ainsi que son positionnement.

### Utilisation

Il faut alors switcher la valeur de "pointer" : 

- gauche => la moving pointe le premier siège gauche devant l'écran
- droite => la moving pointe le dernier siège droit au fond de la salle

tout en modifiant les valeurs min et max selon les 2 dimensions X et Y.

> Régler les valeurs de min/max en Z à une valeur fixe (le haut des sièges).
> X représente la largeur de la salle et Y la longueur de la salle.

## CALIB-PANEL

TODO

## SYNCHRO-TEMP

Cet outil permet de télécharger toutes les CPLS des serveurs ICE-CORE qui respectent la recherche.

### Configuration

Il faut fournir un fichier *servers.xml* qui liste tous les servers concernés ; formaté de la manière suivante : 

```
<Servers>
	<Server>192.168.99.10</Server>
	<Server>192.168.33.12</Server>
</Servers>
```

## SYNCER

TODO