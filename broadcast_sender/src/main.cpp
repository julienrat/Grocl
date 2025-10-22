#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_wifi.h>

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
//Rouge canal 1 // Vert Canal 2 // Blanc Canal 3 // Noir Canal 4
#define WIFI_CHANNEL 3

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000); // Attendre que le moniteur série soit prêt
  
  Serial.println("Démarrage de l'émetteur ESP-NOW...");
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Pour être sûr de ne pas être connecté à un point d'accès
  
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
  
  // Configuration pour la compatibilité avec ESP8266
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  for (int i = 0; i < 6; i++) {
    peerInfo.peer_addr[i] = (uint8_t)0xFF;
  }
  peerInfo.channel = WIFI_CHANNEL;
  peerInfo.encrypt = false;
  
  // Force le canal WiFi avant d'ajouter le peer
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  delay(100);
  
  // Ajoute le peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Échec de l'ajout du peer");
    return;
  }
  Serial.println("Peer ajouté avec succès");
}

// Adresse de broadcast pour ESP-NOW
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void loop() {
  // Fait varier l'angle de 0 à 180 degrés
  static int angle = 0;
  myData.angle = angle;
  
  // Envoie le message
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  if (result == ESP_OK) {
    Serial.print("Envoi réussi de l'angle : ");
    Serial.println(angle);
  } else {
    Serial.println("Erreur d'envoi");
  }

  // Incrémente l'angle
  angle = (angle + 10) % 181;  // Incrémente par pas de 10, reste entre 0 et 180
  //angle=90;
  // Attend 5 secondes
  delay(50);
}
