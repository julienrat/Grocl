# Grocl — Broadcast sender
Ce dépôt contient un firmware ESP32 qui lit des données de vent depuis la SPIFFS (`data/wind.json`) et diffuse des angles via ESP-NOW.
 # Grocl — Broadcast sender 🚀
 
 Firmware ESP32 minimal qui lit des données de vent depuis la SPIFFS (`data/wind.json`) et diffuse des angles via ESP-NOW.
 
 ## 🔌 Canal Wi‑Fi et ESP-NOW
 
 ESP-NOW fonctionne sur un canal Wi‑Fi 2.4 GHz (1–13 selon la région). Le canal utilisé par l'émetteur est défini dans `src/main.cpp` via la constante `WIFI_CHANNEL`.
 
 🎨 Mapping couleur → canal (convention du projet) :
 
 - 🔴 Rouge  — canal 1
 - 🟢 Vert   — canal 5
 - ⚪ Blanc  — canal 9
 - ⚫ Noir   — canal 13
 
 ⚠️ Remarque : les canaux 2.4 GHz se chevauchent
 
 Les canaux adjacents sur la bande 2.4 GHz se recouvrent partiellement. Par exemple, le canal 1 chevauche les canaux 2, 3 et 4. Cela peut provoquer des interférences si deux appareils sont réglés sur des canaux proches.
 
 ✅ Recommandation : pour réduire les interférences dans des environnements chargés, préférez des canaux non-chevauchants comme 1, 6 et 11 (séparés et généralement plus stables).
 
 ## 🛠️ Conseils pratiques
 
 - Assurez-vous que tous les appareils ESP-NOW qui doivent communiquer utilisent le même `WIFI_CHANNEL`.
 - Si vous observez des pertes ou des perturbations, essayez un autre canal (1, 6 ou 11).
 - Vérifiez la réglementation locale : certains canaux peuvent être restreints selon votre pays.
 
 Pour modifier le canal : ouvrez `src/main.cpp` et changez la ligne :
 
 ```cpp
 #define WIFI_CHANNEL 1
 ```
 
 Puis rebuild/flash le firmware 
## Canal Wi‑Fi et ESP-NOW

ESP-NOW fonctionne sur un canal Wi‑Fi 2.4 GHz précis (1–13 selon la région). Dans `src/main.cpp` la constante `WIFI_CHANNEL` définit le canal utilisé par l'émetteur.

Mapping de couleurs (convention utilisée dans le projet) :

- Rouge → canal 1
- Vert  → canal 5
- Blanc → canal 9
- Noir  → canal 13

Important — chevauchement des canaux :

Les canaux 2.4 GHz se chevauchent. Par exemple, le canal 1 couvre une bande qui recouvre partiellement les canaux 2, 3 et 4. Cela signifie que deux appareils réglés sur des canaux différents mais proches (ex. 1 et 5) peuvent s'interférer. Pour minimiser les interférences sur des réseaux densément peuplés, on recommande d'utiliser des canaux non-chevauchants comme 1, 6 et 11 (ces trois-là sont séparés et ont un recouvrement minimal en pratique).

Remarques pratiques :

- Si vous utilisez plusieurs émetteurs/récepteurs ESP-NOW, assurez-vous qu'ils utilisent le même `WIFI_CHANNEL` (ESP-NOW peer et broadcast exigent le même canal pour communiquer).
- Si vous observez des pertes de messages ou des interférences, essayez de changer `WIFI_CHANNEL` vers 1, 6 ou 11.
- Le choix de canal peut être contraint par la réglementation locale — sur certaines régions seuls certains canaux sont autorisés.

Pour modifier le canal, ouvrez `src/main.cpp` et changez la définition :

```cpp
#define WIFI_CHANNEL 1
```

Puis rebuild/flash le firmware.
