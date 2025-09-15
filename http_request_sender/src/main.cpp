#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiManager.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// Structure du message à envoyer
typedef struct struct_message {
  int angle;
} struct_message;

// Crée une variable de type struct_message
struct_message myData;

// Configuration WiFi
String jsonUrl = ""; // URL du fichier JSON à télécharger
bool shouldSaveConfig = false; // Flag pour sauvegarder la config

// Canal WiFi pour ESP-NOW
#define WIFI_CHANNEL 1

// Callback quand les données sont envoyées via ESP-NOW
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Statut d'envoi: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Succès" : "Échec");
}

// Callback pour sauvegarder la configuration
void saveConfigCallback() {
  Serial.println("Configuration doit être sauvegardée");
  shouldSaveConfig = true;
}

// Fonction pour sauvegarder la configuration dans SPIFFS
void saveConfig() {
  Serial.println("Sauvegarde de la configuration...");
  DynamicJsonDocument doc(1024);
  doc["json_url"] = jsonUrl;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Échec de l'ouverture du fichier config en écriture");
    return;
  }

  serializeJson(doc, configFile);
  configFile.close();
}

// Fonction pour charger la configuration depuis SPIFFS
void loadConfig() {
  if (SPIFFS.begin(true)) {
    Serial.println("SPIFFS monté");
    if (SPIFFS.exists("/config.json")) {
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, configFile);
        if (!error) {
          jsonUrl = doc["json_url"].as<String>();
        }
        configFile.close();
      }
    }
  } else {
    Serial.println("Échec du montage SPIFFS");
  }
}

// Fonction pour configurer ESP-NOW
void setupEspNow() {
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erreur pendant l'initialisation d'ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
  
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  for (int i = 0; i < 6; i++) {
    peerInfo.peer_addr[i] = (uint8_t)0xFF;
  }
  peerInfo.channel = WIFI_CHANNEL;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Échec de l'ajout du peer");
    return;
  }
}

// Fonction pour télécharger et parser le JSON
int getAngleFromJson() {
  if (jsonUrl.length() == 0) {
    return -1;
  }

  HTTPClient http;
  http.begin(jsonUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("Réponse JSON reçue : " + payload); // Affiche la réponse JSON
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      return doc["angle"].as<int>();
    } else {
      Serial.println("Erreur de parsing JSON");
    }
  } else {
    Serial.printf("Erreur HTTP : %d\n", httpCode); // Affiche le code d'erreur HTTP
  }

  http.end();

  // Si la requête échoue, retourne en mode point d'accès
  Serial.println("Échec de la requête JSON. Retour au mode point d'accès.");
  WiFiManager wm;
  wm.startConfigPortal("ESP32_Config_AP");

  return -1;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Monte le système de fichiers SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Erreur SPIFFS");
    return;
  }

  // Charge la configuration existante
  loadConfig();

  // Configure WiFiManager
  WiFiManager wm;
  wm.setSaveConfigCallback(saveConfigCallback);

  // Ajoute un champ personnalisé pour l'URL du JSON avec un exemple
  WiFiManagerParameter custom_json_url(
    "json_url", 
    "URL du fichier JSON (ex: https://julienrat.github.io/Grocl/http_request_sender/angle.json)", 
    jsonUrl.c_str(), 
    100
  );
  wm.addParameter(&custom_json_url);

  // Essaie de se connecter au WiFi ou crée un point d'accès
  if (!wm.autoConnect("ESP32_Config_AP")) {
    Serial.println("Échec de connexion");
    ESP.restart();
  }

  // Sauvegarde la nouvelle configuration si nécessaire
  if (shouldSaveConfig) {
    jsonUrl = custom_json_url.getValue();
    saveConfig();
  }

  // Configure ESP-NOW après la connexion WiFi
  setupEspNow();
}

// Adresse de broadcast pour ESP-NOW
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void loop() {
  static int failureCount = 0; // Compteur d'échecs consécutifs

  // Récupère l'angle depuis le fichier JSON
  int angle = getAngleFromJson();

  // Si l'angle est valide (-1 indique une erreur)
  if (angle >= 0 && angle <= 180) {
    failureCount = 0; // Réinitialise le compteur en cas de succès
    myData.angle = angle;

    // Affiche l'angle à envoyer
    Serial.printf("Angle à envoyer via ESP-NOW : %d\n", angle);

    // Déconnecte du WiFi avant d'envoyer via ESP-NOW
    WiFi.disconnect();
    delay(100); // Petit délai pour s'assurer de la déconnexion

    // Réinitialise ESP-NOW avant d'envoyer
    esp_now_deinit();
    setupEspNow();

    // Envoie le message via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

    if (result == ESP_OK) {
      Serial.printf("Angle %d envoyé avec succès\n", angle);
    } else {
      Serial.println("Erreur d'envoi ESP-NOW");
    }

    // Reconnecte au WiFi après l'envoi
    WiFi.begin();
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
    }
    Serial.println("\nReconnecté au WiFi");
  } else {
    failureCount++;
    Serial.printf("Échec de récupération de l'angle (%d/10)\n", failureCount);

    // Si 10 échecs consécutifs, retourne en mode point d'accès
    if (failureCount >= 10) {
      Serial.println("10 échecs consécutifs. Retour au mode point d'accès.");
      WiFiManager wm;
      wm.startConfigPortal("ESP32_Config_AP");
      failureCount = 0; // Réinitialise le compteur après retour au mode AP
    }
  }

  // Attend 5 secondes avant la prochaine mise à jour
  delay(5000);
}
