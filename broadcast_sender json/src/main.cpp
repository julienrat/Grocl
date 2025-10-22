#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <SPIFFS.h>
#include <cctype>
#include <vector>

// Structure du message à envoyer
typedef struct struct_message {
  int angle;
} struct_message;

// Crée une variable de type struct_message
struct_message myData;

// Callback quand les données sont envoyées
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Statut d'envoi: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Succès" : "Échec");
}

// Canal WiFi à utiliser (1-13)
//Rouge canal 1 // Vert Canal 5 // Blanc Canal 9 // Noir Canal 13

#define WIFI_CHANNEL 1

// NOTE: Le pilotage du servo a été retiré : ce firmware n'envoie que des angles via ESP-NOW

// Adresse de broadcast pour ESP-NOW
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Liste des valeurs lues depuis wind.json
std::vector<int> windValues;
// Liste des ids correspondants (même index que windValues)
std::vector<int> windIds;

// servo pilot removed

// Envoie angle via ESP-NOW
void sendAngle(int angle) {
  myData.angle = angle;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
}

// Lecture et parsing de /wind.json dans SPIFFS (streaming, faible empreinte mémoire)
bool loadWindValues(const char* path = "/wind.json") {
  if (!SPIFFS.begin(true)) {
    Serial.println("Erreur: impossible d'initialiser SPIFFS");
    return false;
  }

  if (!SPIFFS.exists(path)) {
    Serial.print("Fichier introuvable: ");
    Serial.println(path);
    return false;
  }

  File file = SPIFFS.open(path, "r");
  if (!file) {
    Serial.println("Impossible d'ouvrir le fichier");
    return false;
  }

  windValues.clear();
  windIds.clear();

  // On va lire le fichier par petits morceaux et extraire les nombres associés à "vitesse_vent"
  // Ce parser est tolérant : il recherche la clé "vitesse_vent" et lit l'entier qui suit.
  const size_t bufSize = 256;
  char buf[bufSize + 1];
  String rem = "";

  while (file.available()) {
    size_t len = file.readBytes(buf, bufSize);
    buf[len] = '\0';
    String chunk = rem + String(buf);

    int idx = 0;
    while (idx >= 0) {
      int keyPos = chunk.indexOf("\"vitesse_vent\"", idx);
      if (keyPos == -1) break;
      // cherche les deux-points après la clé
      int colon = chunk.indexOf(':', keyPos);
      if (colon == -1) {
        // garde le reste pour la prochaine itération
        break;
      }
      // lit l'entier après ':'
      int j = colon + 1;
      // saute espaces
  while (j < (int)chunk.length() && isspace((unsigned char)chunk[j])) j++;
      // supporte nombres négatifs
      int startNum = j;
      bool hasDigit = false;
  if (j < (int)chunk.length() && (chunk[j] == '-' || isdigit((unsigned char)chunk[j]))) {
        if (chunk[j] == '-') j++;
        while (j < (int)chunk.length() && isDigit(chunk[j])) { j++; hasDigit = true; }
      }
      if (hasDigit) {
        String numStr = chunk.substring(startNum, j);
        int v = numStr.toInt();
        v = constrain(v, 0, 180);
        windValues.push_back(v);
        // tentons de récupérer un id précédant dans le même objet en cherchant "id" avant keyPos
        int idVal = -1;
        int idKey = chunk.lastIndexOf("\"id\"", keyPos);
        if (idKey != -1) {
          int colonId = chunk.indexOf(':', idKey);
          if (colonId != -1) {
            int k = colonId + 1;
            while (k < (int)chunk.length() && isspace((unsigned char)chunk[k])) k++;
            int startId = k;
            bool hasIdDigit = false;
            if (k < (int)chunk.length() && (chunk[k] == '-' || isdigit((unsigned char)chunk[k]))) {
              if (chunk[k] == '-') k++;
              while (k < (int)chunk.length() && isDigit(chunk[k])) { k++; hasIdDigit = true; }
            }
            if (hasIdDigit) {
              String idStr = chunk.substring(startId, k);
              idVal = idStr.toInt();
            }
          }
        }
        windIds.push_back(idVal);
        idx = j;
      } else {
        // pas de nombre complet dans ce chunk -> conserver le reste
        break;
      }
    }

  // conserve la fin du chunk pour capturer les clés tronquées sur buffer boundary
    if ((int)chunk.length() > 64) rem = chunk.substring(chunk.length() - 64);
    else rem = chunk;
  }

  file.close();

  Serial.print("Valeurs de vent chargées: ");
  Serial.println(windValues.size());
  return !windValues.empty();
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000); // Attendre que le moniteur série soit prêt

  Serial.println("Démarrage: lecture wind.json et envoi ESP-NOW...");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Configure le canal WiFi
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  Serial.print("Canal WiFi configuré sur : ");
  Serial.println(WIFI_CHANNEL);

  Serial.print("Adresse MAC de l'émetteur: ");
  Serial.println(WiFi.macAddress());

  // Initialise ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erreur pendant l'initialisation d'ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW initialisé avec succès");

  // Enregistre le callback pour l'envoi
  esp_now_register_send_cb(OnDataSent);

  // Configure le peer broadcast
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  for (int i = 0; i < 6; i++) {
    peerInfo.peer_addr[i] = (uint8_t)0xFF;
  }
  peerInfo.channel = WIFI_CHANNEL;
  peerInfo.encrypt = false;
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  delay(100);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Échec de l'ajout du peer");
    // on continue quand même
  } else {
    Serial.println("Peer ajouté avec succès");
  }


  // Charge les valeurs depuis SPIFFS
  if (!loadWindValues()) {
    Serial.println("Aucune valeur de vent chargée; arrêt.");
  }
  sendAngle(90);
  delay(5000);
}

void loop() {
  static int currentAngle = 0;

  if (windValues.empty()) {
    delay(1000);
    return;
  }

  // Parcours cyclique des valeurs
  for (size_t i = 0; i < windValues.size(); ++i) {
    int target = windValues[i];
    

    // Incrémente ou décrémente par pas de 1 degré jusqu'à la cible
    while (currentAngle != target) {
        if (currentAngle < target) currentAngle++;
        else if (currentAngle > target) currentAngle--;

        // envoie uniquement l'angle intermédiaire via ESP-NOW
        sendAngle(currentAngle);

      // Petite pause pour rendre l'incrément visible/souple
      delay(20);
    }

  // Arrivé à la cible : affichage id et maintien court
  int shownId = -1;
  if (i < windIds.size()) shownId = windIds[i];
  Serial.print("ID: ");
  Serial.println(shownId);
  Serial.print("Angle: ");
  Serial.println(currentAngle);
  // envoie l'angle final atteint
  sendAngle(currentAngle);
  randomSeed(analogRead(0) + currentAngle);
  int holdTime = random(2000, 5000); // maintien entre 2 et 5 secondes
  
    delay(holdTime);
  }
}
