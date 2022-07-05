# ICE-CORE API

Ce document décrit les commandes permettant de contrôler ICE-CORE.
La solution possède deux canaux de commande : 

- un module HTTP permettant l'exécution de l'ensemble des commandes (largement utilisé par l'interface web)
- un module UDP permettant l'exécution des commandes via des macros définis par le vidéoprojecteur.

## UDP

Ce module regroupe peu de commande car son utilisation doit être limitée.<br>
Le port découte est le **9011**.

### STOP SCRIPT

- *syntaxe* : `STOP_SCRIPT`
- *description* : permet de stopper le script en cours

> Attention : si le script en cours est dû à une synchronisation avec le serveur de projection. Cette commande n'aura aucun effet !

### START SCRIPT

- *syntaxe* : `START_SCRIPT name [force]`
- *description* : permet de démarrer un script à partir de son nom (exemple : **VibSas_Venom2.lcp**).
- *complément* : le paramètre *force* permet de forcer le jeu du script et d'empêcher le système à changer de script si une nouvelle synchro pourrait démarrer.

> Attention : le script doit être un script qui doit être joué en boucle pour que cela fonctionne (*loop* ou *sas*) !

### DEVICES

Il est possible dans ICE-CORE de **forcer** l'état d'un device (panel, PAR, Backlight ou movingHead). Cela implique que tant que le device est en état forcé, aucun script n'a d'effets sur le device. 

- *syntaxe* : `UNFORCE_ALL_DEVICES`
- *description* : tous les devices en états forcés sont maintenant libres et peuvent de nouveau évoluer en fonction d'un script.
---
- *syntaxe* : `UNFORCE_DEVICE name`
- *description* : le device qui a pour nom *name* (défini dans la configuration du serveur) est maintenant libre et peut de nouveau évoluer en fonction d'un script.

### PAR SCENE

- *syntaxe* : `SET_PAR_SCENE_ON`
- *description* : permet de forcer l'allumage des PARs scène. Cette commande n'est utilisable que sur le serveur ICE SAS, là où des PARs de type **scene** ont été définis dans la configuration. Elle permet de s'affranchir des différentiation de script SAS.
- *complément* : cette commande ne fonctionne que si le mode du service ICE est **MANUAL** ou **SYNC** et à pour effet de forcer l'état du device.

> Aide : l'équivalent `SET_PAR_SCENE_OFF` existe.

### PAR AMBIANCE

- *syntaxe* : `SET_PAR_AMBIANCE_ON`
- *description* : permet de forcer l'allumage des PARs SAS. Cette commande n'est utilisable que sur le serveur ICE SAS, là où des PARs de type **ambiance** ont été définis dans la configuration. Elle permet de s'affranchir des différentiation de script SAS.
- *complément* : cette commande ne fonctionne que si le mode du service ICE est **MANUAL** ou **SYNC** et à pour effet de forcer l'état du device.

> Aide : l'équivalent `SET_PAR_AMBIANCE_OFF` existe.

### MODE

Le mode définit les actions possibles par le système.<br>
Le mode **OFF** ne permet aucun contrôle des devices.<br>
Le mode **MANUAL** permet de contrôler manuellement les devices (un à un ou via le mode scene).<br>
Le mode **SYNC** permet de laisser le système lancer les scripts automatiquement en fonction des DCP joués sur le serveur de projection.<br>
Le mode **EXTERNAL** permet de contrôler les devices via un plugin dans Premiere Pro.

- *syntaxe* : `SWITCH_MODE mode`
- *description* : permet de changer le mode du service ICE-CORE.

### PROJECTION

Le mode de projection permet de définir la manière de plaquer les images wall d'un script dans les panneaux.<br>
Ce mode peut être forcé par script (dans ses fichiers de synchro) mais s'il n'est pas définit, c'est le mode de projection global qui fait foi.

- *syntaxe* : `SWITCH_PROJECTION projection`
- *description* : permet de changer le mode de projection global du système.

> #### Pour comprendre :
> La taille et la position (dans un repère absolu) des panneaux ainsi que de l'écran sont définis dans la configuration du système.<br>
>
> Dans la projection **NO_FIT**, la hauteur de l'image *wall* représente la hauteur de l'écran. Ce ratio est utilisé pour calculer, tout en gardant la MIRE de l'écran prolongé sur le panneaux, la sous-image pour chaque panneau. Le même ratio est gardé pour la largeur des images.
> Ce mode conserve les proportions !
> ---
> Dans la projection **FIT_X**, la hauteur de l'image *wall* représente la hauteur de l'écran. Ce ratio est utilisé pour calculer, tout en gardant la MIRE de l'écran prolongé sur le panneaux, la sous-image pour chaque panneau. Le ratio gardé pour la largeur des images est cette fois-ci, le rapport entre la largeur de l'image et la longeur totale des panneaux afin d'optimiser l'affichage d'un maximum de matière ainsi qu'éviter un manque de matière en largeur.
> Ce mode ne respecte pas les proportions ! 
> ---
> Dans la projection **FIT_Y**, la hauteur de l'image *wall* représente la hauteur de l'écran. Ce ratio est utilisé pour calculer, tout en gardant la MIRE de l'écran prolongé sur le panneaux, la sous-image pour chaque panneau. Le ratio gardé pour la hauteur des images est cette fois-ci, le rapport entre la hauteur de l'image et le double de l'écart le plus grand entre tous les panneaux et la MIRE de l'écran afin d'optimiser l'affichage d'un maximum de matière ainsi qu'éviter un manque de matière en hauteur.
> Ce mode ne respecte pas les proportions ! 
> ---
> Dans la projection **FIT_X_PROP_Y**, le ratio pour optilmiser la matière en largeur (*FIT_X*) est également utilisé en hauteur afin de garder les proportions des contenus dans les images.
> Ce mode conserve les proportions ! 
> ---
> Dans la projection **FIT_Y_PROP_X**, le ratio pour optilmiser la matière en hauteur (*FIT_Y*) est également utilisé en largeur afin de garder les proportions des contenus dans les images.
> Ce mode conserve les proportions ! 
> ---
> Dans la projection **FIT_X_FIT_Y**, toute la matière est optimisée. On utilise les ratios de *FIT_X* et *FIT_Y* combinés.
> Ce mode ne respecte pas les proportions !

## HTTP
								   
Le port découte est le **8300**. Toutes les requêtes sont de type **POST**.

### LOGIN/LOGOUT

- *chemin* : `http://IP:8300/login`
- *syntaxe* : 

```xml
	<command>
		<username>admin</username>
		<password>ice17180</password>
	</command>
```
- *description* : permet de s'authentifier auprès du service ICE-CORE. La réponse contient un cookie de connexion qu'il faudra renvoyer à chaque nouvelle requête pour autoriser l'exécution de cette commande.

> Voir le chemin : `http://IP:8300/logout`

### COMMANDES

Chaque commande est formatée de la manière suivante : 

```xml
	<command>
		<type>typeCommand</type>
		<parameters>
		</parameters>
	</command>
```

Le type permet de définir la commande en question. 
Pour chaque paramètre, il faut créer une balise du nom du paramètre dans la balise *parameters* et mettre en texte la valeur du paramètre. 

Exemple : 

```xml
	<command>
		<type>SWITCH_MODE</type>
		<parameters>
			<mode>MANUAL</mode>
		</parameters>
	</command>
```

#### STATUS_IMS

Elle permet de retourner le status de l'IMS : 

- le timecode
- le nom du contenu joué
- l'UUID du contenu joué

<ins>Paramètres</ins>

Aucun

---

#### LIST_DEVICES

Elle permet de retourner la configuration et l'état des devices ICE : 

- les Panneaux
- les PARs
- les MovingHeads
- les Backlights

<ins>Paramètres</ins>

Aucun

---

#### GET_CONFIGURATION

Elle permet de retourner la configuration de la salle ICE : 

- le positionnement et la taille des panneaux
- le positionnement et la taille de l'écran
- le positionnement des movingHeads
- le positionnement de la zone de siège

<ins>Paramètres</ins>

Aucun

---

#### UNFORCE_ALL_DEVICES

Elle permet de désactiver l'état forcé de tous les devices attachés au serveur.

<ins>Paramètres</ins>

Aucun

---

#### UNFORCE_DEVICE

Elle permet de désactiver l'état forcé d'un device attaché au serveur.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| device      | nom         |

---

#### GET_MODE

Elle permet de récupérer le mode actuel du service ICE-CORE.

<ins>Paramètres</ins>

Aucun

---

#### SWITCH_MODE

Elle permet de changer le mode actuel du service ICE-CORE.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| mode      | nom         |

---

#### GET_CURRENT_PROJECTION

Elle permet de récupérer le mode de projection actuel du service ICE-CORE.

<ins>Paramètres</ins>

Aucun

---

#### SWITCH_PROJECTION

Elle permet de changer le mode de projection actuel du service ICE-CORE.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| projection      | nom         |

---

#### STOP_SCRIPT

Elle permet de stopper le script courant s'il est en état forcé et si c'est un script de loop.

<ins>Paramètres</ins>

Aucun

---

#### LAUNCH_SCRIPT

Elle permet de démarrer un script s'il est en état forcé et si c'est un script de loop.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| script      | nom du dossier lcp        |
| force      | état forcé (pas de resynchro tant que le scriot n'est pas arrêté manuellement)         |

---

#### SET_PAR_SCENE_ON/SET_PAR_SCENE_OFF

Elle permet d'activer/désactiver l'éclairage des PAR scene sur le serveur SAS.

<ins>Paramètres</ins>

Aucun

---

#### SET_PAR_AMBIANCE_ON/SET_PAR_AMBIANCE_OFF

Elle permet d'activer/désactiver l'éclairage des PAR ambiance sur le serveur SAS.

<ins>Paramètres</ins>

Aucun

---

#### SET_COLOR

Elle permet contrôler manuellement un device de la salle ICE : 

- une couleur uni pour un panneau
- une couleur d'un backlight
- une couleur, intensité et un zoom pour un PAR
- une couleur, une intensité, un zoom et l'orientation angulaire d'une movingHead

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| device      | nom |
| red      | couleur rouge (0 à 255) |
| green      | couleur verte (0 à 255) |
| blue      | couleur bleue (0 à 255) |
| white      | couleur blanche (0 à 255) : uniquement PAR & Moving |
| intensity      | intensité (0 à 255) : uniquement PAR & Moving |
| zoom      | zoom (0 à 255) : uniquement PAR & Moving |
| force      | force le maintien de la valeur |
| pan      | orientation horizontale de la movingHead en degré |
| tilt      | orientation verticale de la movingHead en degré |

---

#### SET_POINTER

Elle permet contrôler manuellement une movingHead en positionnant sur les sièges de la salle ICE.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| device      | nom |
| red      | couleur rouge (0 à 255) |
| green      | couleur verte (0 à 255) |
| blue      | couleur bleue (0 à 255) |
| white      | couleur blanche (0 à 255) : uniquement PAR & Moving |
| intensity      | intensité (0 à 255) : uniquement PAR & Moving |
| zoom      | zoom (0 à 255) : uniquement PAR & Moving |
| force      | force le maintien de la valeur |
| x      | ratio en 0 et 1 du positionnement sur la largeur de la zone des sièges |
| y      | ratio en 0 et 1 du positionnement sur la longueur de la zone des sièges |

---

#### GET_CPL_INFOS

Elle permet de récupérer le liste des DCP présent sur le serveru de projection avec toutes les données utiles associées.

<ins>Paramètres</ins>

Aucun

---

#### GET_CPL_CONTENT

Elle permet de récupérer le contenu du fichier CPL d'un DCP donné.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| cplId      | UUID de la CPL |

---

#### GET_CPL_CONTENT_NAME

Elle permet de récupérer les contenus des fichiers CPL dont le nom contient une string donnée.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| search      | string à retrouver dans le nom de la CPL |

---

#### GET_DOWNLOADED_SCRIPTS

Elle permet de retourner la liste des scripts ICE actuellement téléchargés ou en cours de téléchargement.

<ins>Paramètres</ins>

Aucun

---

#### GET_CURRENT_SYNC

Elle permet de récupérer les informations de synchronisation courante. Le script et la synchro associée.

<ins>Paramètres</ins>

Aucun

---

#### CHANGE_PRIORITY_SCRIPT

Elle permet de forcer/déforcer un script à être gardé même si plus de place disque ou plus présent sur le FTP.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| scriptName      | nom du script |
| priority      | bool : true si doit être gardé, false sinon |

---

#### SYNCHRONIZE_SCRIPT

Elle permet de synchronier un script avec une CPL présente sur le serveur de projection.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| sync      | UUID de la CPL de référence (doit être déjà synchronisée) |
| cpl      | UUID de la CPL à synchroniser |
| method      | algorithme de synchronisation à utiliser (*byId* ou *byDuration*) |

---

#### GET_SYNC_CONTENT

Elle permet de récupérer le contenu du fichier de synchronisation pour une UUID de cpl donnée.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| syncId      | UUID de la CPL synchronisée |

---

#### GET_SCENES

Une scène représente l'état des tous les équipements d'une salle ICE à un instant T. Il est possible de configurer une scène pour ensuite la rejouer.
Cette commande retourne toutes les scènes définies sur le serveur.

<ins>Paramètres</ins>

Aucun

#### PLAY_SCENE

Elle permet de jouer une scène sur le serveur. Le mode du serveru doit être en MANUAL.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| scene      | nom de scène |

#### CREATE_UPDATE_SCENE

Elle permet de créer ou dee modifier une scène existante selon le même format que la réponse de la commande GET_SCENES.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| scene      | nom de scène |
| content      | définition de la scène |

#### REMOVE_SCENE

Elle permet de supprimer une scène du serveur.

<ins>Paramètres</ins>

| Nom         | Description |
| ----------- | ----------- |
| scene      | nom de scène |