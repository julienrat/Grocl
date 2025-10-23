# broadcast_receiver_OSC_sender (MIDI sender)

Petit projet pour ESP32 : récepteur ESP-NOW qui convertit les messages reçus en messages MIDI envoyés via BLE (BLE-MIDI).

Caractéristiques
- Réception de messages via ESP-NOW
- Conversion des valeurs reçues en notes MIDI (Note On)
- Envoi MIDI via BLE (BLE-MIDI library)

Dépendances
- PlatformIO (framework: Arduino)
- Bibliothèque BLE-MIDI (installée via `platformio.ini`)

Fichiers importants
- `src/main.cpp` : logique principale (réception ESP-NOW, mapping en note, envoi Note On/Off)
- `platformio.ini` : configuration PlatformIO et dépendances

Build et flash
1. Connecte ton ESP32 en USB
2. Dans le dossier du projet, compile et téléverse :

```bash
platformio run --target upload
```

Test et utilisation
1. Allume l'ESP32 ; il démarre en mode station et écoute ESP-NOW.
2. Connecte-toi à l'ESP32 en BLE depuis une application supportant BLE-MIDI (par ex. une app smartphone, ou un Mac/PC compatible).
3. Envoie depuis l'émetteur des messages ESP-NOW contenant une valeur (int ou 1 octet). À chaque message reçu, le récepteur envoie une Note On mappée sur la plage C2..C7.

Format des messages attendus
- 1 octet : valeur (0..180) — le récepteur mappe la valeur en note + vélocité
- int (4 octets) : valeur (0..180)

Comportement MIDI
- À chaque message reçu, le programme envoie systématiquement un Note On pour la note mappée (il coupe d'abord la note précédente avec Note Off pour retrigger proprement).

Personnalisation
- Modifier la plage des notes, le canal MIDI (`MIDI_CHANNEL` dans `src/main.cpp`) ou la logique de vélocité directement dans `src/main.cpp`.

Dépannage
- Si le firmware dépasse la taille, vérifie `platformio.ini` (partition `huge_app` peut être utilisée pour augmenter l'espace de flash).
- Si tu ne reçois pas de notes : vérifie que l'émetteur envoie des messages ESP-NOW et que le BLE-MIDI est connecté depuis l'app destinataire.

Licence
- MIT
