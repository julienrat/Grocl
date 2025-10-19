# Broadcast Receiver (ESP32) — Guide pour débutant (FR)

Ce projet contient un code Arduino pour un récepteur basé sur une carte ESP32 (configuration PlatformIO). Ce README explique, pas à pas et en termes simples, comment ouvrir, compiler, téléverser et dépanner le projet — même si vous débutez avec Arduino.

## Contenu du projet
- `platformio.ini` : configuration du projet PlatformIO (plateforme, carte, port série, dépendances).
- `src/main.cpp` : code principal Arduino (sketch) pour le récepteur.
- `lib/` et `include/` : dossiers pour bibliothèques et en-têtes si utilisés.
- `test/` : dossier pour tests (vide ou exemples).

> Remarque : Ce README suppose que vous utilisez PlatformIO (extension VS Code) pour développer et téléverser le code sur une carte ESP32.

## Prérequis
- Un ordinateur Windows, macOS ou Linux.
- Visual Studio Code installé.
- Extension PlatformIO IDE installée dans VS Code.
- Un câble USB pour connecter la carte ESP32 à votre ordinateur.
- La carte ESP32 (par exemple `esp32dev`).

## Installer PlatformIO (si nécessaire)
1. Ouvrez Visual Studio Code.
2. Allez dans l'onglet Extensions (icône des blocs) et recherchez "PlatformIO IDE".
3. Installez l'extension, puis redémarrez VS Code si demandé.

PlatformIO gère les outils de compilation et les bibliothèques pour vous.

## Structure importante du projet
- Le fichier `platformio.ini` contient :
  - la plateforme (`espressif32`),
  - la carte (`esp32dev`),
  - le framework (`arduino`),
  - le port série (`COM3` dans cet exemple — adaptez-le à votre machine),
  - une dépendance : `ESP32Servo` (pour contrôler des servomoteurs si le code l'utilise).

Si votre carte est connectée sur un autre port série (par exemple `COM4`), modifiez `upload_port` et `monitor_port` dans `platformio.ini`.

## Ouvrir le projet
1. Dans VS Code, choisissez `File` → `Open Folder...` et ouvrez le dossier du projet (le dossier qui contient `platformio.ini`).
2. PlatformIO devrait détecter automatiquement le projet. Si ce n'est pas le cas, ouvrez la palette de commandes (Ctrl+Shift+P) et lancez "PlatformIO: Home".

## Compiler le projet
1. Dans la barre d'état (ou via la palette de commandes PlatformIO), cliquez sur le bouton "Build" (ou utilisez la commande "PlatformIO: Build").
2. PlatformIO téléchargera les bibliothèques nécessaires et compilera le code.
3. Si la compilation se termine sans erreurs, vous verrez "SUCCESS".

## Téléverser (Upload) sur la carte ESP32
1. Connectez votre ESP32 via USB.
2. Vérifiez le port COM utilisé (sur Windows, regardez dans le Gestionnaire de périphériques → Ports COM...).
3. Si nécessaire, modifiez `upload_port` dans `platformio.ini` pour mettre le port correct.
4. Dans PlatformIO, cliquez sur "Upload" (ou utilisez la commande "PlatformIO: Upload").
5. PlatformIO compilera (si nécessaire) puis téléversera le code sur la carte.

Note : Certaines cartes ESP32 demandent d'appuyer sur un bouton EN/RST ou BOOT pendant le téléversement. Si le téléversement échoue, essayez d'appuyer brièvement sur Reset ou d'appuyer sur Boot au moment opportun.

## Ouvrir le moniteur série (Serial Monitor)
1. Après le téléversement, ouvrez le moniteur série pour lire les messages de la carte : utilisez "PlatformIO: Monitor" ou cliquez sur l'icône de moniteur.
2. Assurez-vous que la vitesse (baud) correspond à la vitesse définie dans le code (ex : 115200). Le `platformio.ini` contient `monitor_speed = 115200` par défaut.
3. Vous devriez voir des messages de debug ou d'état envoyés par `Serial.println()` dans `src/main.cpp`.

## Dépannage rapide
- Erreur de compilation liée à une bibliothèque manquante :
  - PlatformIO installe automatiquement `lib_deps` définies dans `platformio.ini`. Si une bibliothèque manque, vérifiez le nom correct et la version dans `platformio.ini`.
- Le port COM n'est pas détecté :
  - Débranchez/rebranchez le câble USB.
  - Installez les drivers USB-serial (ex : CP210x, CH340) si nécessaire.
- Téléversement échoue ou la carte ne répond pas :
  - Appuyez sur le bouton Reset/EN pendant le téléversement.
  - Tentez d'ouvrir le moniteur série et d'appuyer sur Reset pour voir les messages.
- Messages étranges dans le moniteur : vérifiez la vitesse de transmission (baud).

## Où regarder dans le code
- `src/main.cpp` contient la boucle Arduino (`setup()` et `loop()`).
- Recherchez les appels à `Serial.begin(...)` pour connaître la vitesse série.
- Si le projet contrôle des servos, la dépendance `ESP32Servo` est listée dans `platformio.ini`.

## Personnalisation simple pour débutants
- Changer le port série : éditez `platformio.ini` et remplacez `COM3` par le port vu dans le Gestionnaire de périphériques.
- Changer la vitesse du moniteur : modifiez `monitor_speed` dans `platformio.ini` ou changez la valeur dans `Serial.begin(...)` dans `src/main.cpp`.

## Ressources utiles (en français)
- PlatformIO : https://platformio.org/
- Tutoriel Arduino (FR) : https://www.arduino.cc/en/Tutorial/HomePage (site officiel, option langue FR disponible)
- ESP32 et Arduino : recherchez "ESP32 Arduino PlatformIO" pour des guides pas à pas.

## Résumé rapide
1. Ouvrez le dossier dans VS Code.
2. Build (compiler) avec PlatformIO.
3. Connectez l'ESP32, ajustez `upload_port` si nécessaire.
4. Upload pour téléverser le code.
5. Ouvrez le Serial Monitor à 115200 bauds pour voir les logs.

