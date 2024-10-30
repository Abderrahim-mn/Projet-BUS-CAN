# Projet de Réseau CAN avec Capteurs Multiples

## Description

Ce projet vise à recueillir et transmettre les informations de différents capteurs vers un ordinateur central (PC Host) via un bus CAN. Les capteurs sont répartis sur plusieurs cartes microcontrôleurs ST-Nucleo, qui lisent les données et les transmettent dans des trames CAN. L'ordinateur central utilise une sonde PEAK PCAN-USB pour lire les informations du bus CAN, visualisées via une interface utilisateur développée avec Qt.

## Objectifs

- Acquérir des données de capteurs variés (pression, humidité, luminosité, etc.).
- Transmettre les informations via un bus CAN.
- Contrôler des dispositifs tels qu'un servomoteur et un anémomètre pour des mesures dynamiques.

## Architecture du Système

- **Carte 1 (ST-Nucleo) :**
  - Contrôle un servomoteur Dynamixel XL-430 pour le mouvement.
  - Mesure la **vitesse du vent** à l'aide d'un anémomètre SOMFY via RS232.
  
- **Carte 2 (ST-Nucleo) :**
  - Capteur de **pression** (LPS22HB) et **humidité/température** (HTS221) via I2C.
  - Capteur de **luminosité** et de **distance** (VL6180X) via I2C.
  
- **Carte 3 (ST-Nucleo) :**
  - Capteur **IMU 9 Axes** (MPU9250) pour l'accélération, le gyroscope, et le magnétomètre via I2C.
  
- **PC Host :**
  - Connecté au bus CAN via une sonde PEAK PCAN-USB pour lire les données.
  - Affichage des données en temps réel via une IHM (interface homme-machine) Qt.

## Matériel Requis

- **Microcontrôleurs ST-Nucleo** : pour la connexion des capteurs et l'envoi des données sur le bus CAN.
- **Sonde PEAK PCAN-USB** : pour l'interface entre le bus CAN et le PC.
- **Capteurs** : 
  - Anémomètre SOMFY (vitesse du vent)
  - Servomoteur Dynamixel XL-430
  - Capteur de pression (LPS22HB)
  - Capteur d'humidité et température (HTS221)
  - Capteur de luminosité et distance (VL6180X)
  - Accéléromètre/Gyroscope/Magnétomètre (MPU9250)
- **PC avec logiciel Qt** : pour afficher et visualiser les données.

## Installation et Configuration

1. **Configuration des Cartes ST-Nucleo** :
   - Connecter les capteurs aux ports I2C/RS232 correspondants sur chaque carte ST-Nucleo.
   - Configurer le bus CAN pour la communication inter-carte et avec le PC.
   
2. **PC Host** :
   - Installer les pilotes nécessaires pour la sonde PEAK PCAN-USB.
   - Développer l'interface Qt pour l'affichage des données.

3. **Logiciel** :
   - Programmer les microcontrôleurs ST-Nucleo pour lire les valeurs des capteurs et les transmettre via le bus CAN.
   - Développer l'IHM avec Qt pour afficher les informations recueillies.

## Utilisation

1. Lancer l'IHM Qt sur le PC Host.
2. Connecter les cartes ST-Nucleo avec leurs capteurs au bus CAN.
3. Utiliser la sonde PEAK PCAN-USB pour transmettre les données CAN vers le PC Host.
4. Observer et analyser les données des capteurs en temps réel via l'interface Qt.

## Auteur

Ce projet a été réalisé dans le cadre d'une étude sur l'acquisition de données de capteurs multiples en utilisant le protocole CAN.
