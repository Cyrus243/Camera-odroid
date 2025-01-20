# Camera-odroid

## Description

Ce projet consiste en la **conception et l'implémentation d'un système de vision basé sur une caméra USB** sur une plateforme Linux (Odroid-C2). Il combine des fonctionnalités avancées telles que la capture et le traitement vidéo en temps réel, la transmission vidéo via TCP/IP, le décodage de codes QR et l'infrastructure conteneurisée pour une meilleure portabilité.

## Fonctionnalités

- **Capture et traitement vidéo en temps réel** :
  - Utilisation de **OpenCV** pour capturer et traiter les flux vidéo en temps réel.
- **Transmission vidéo via TCP/IP** :
  - Envoi des flux vidéo via un réseau TCP/IP.
  - Gestion dynamique du flux vidéo basée sur les données des capteurs.
- **Décodage de codes QR** :
  - Intégration de la bibliothèque **ZBar** pour analyser et décoder les codes QR.
  - Traitement asynchrone grâce à la création et gestion de processus enfants.

## Prérequis

- **Matériel** :
  - Odroid-C2 (ou tout autre appareil Linux compatible).
  - poste de travail client (Linux)
  - Caméra USB.

- **Logiciels** :
  - Vscode
  - Linux (distribution recommandée : CentOS).
  - Compilateur C++ (GCC).
  - Bibliothèques : OpenCV, ZBar.

## Installation

1. Clonez le dépôt :
   ```bash
   git clone https://github.com/Cyrus243/Camera-odroid.git
   cd Camera-odroid
   ```

2. Ouvrez les parties poste (g1-vlsi407-poste) et odroid (g1-vlsi407-odroid) dans Vscode

3. Compilez le projet à l'aide de la commande Vscode: Crtl+Shift+B ou faites run build task dans le menu terminal

4. Lancez l'application sur l'odroid puis sur le poste de travail:
   ```bash
   ./cameraApp
   ```

## Utilisation

- **Décodage des codes QR** : Les codes QR présents dans le flux vidéo sont automatiquement décodés après la capture d'une photo et les informations sont affichées sur la console.
- **Contrôle du flux vidéo** : Le système ajuste dynamiquement la qualité et le débit vidéo en fonction des données des capteurs. La résolution peut être choisi manuellement en appuyant sur la touche r.

## Licence

Ce projet est sous licence MIT. Consultez le fichier [LICENSE](LICENSE) pour plus d'informations.

---
