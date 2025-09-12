#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Servo.h>

// Pin pour le servo moteur (D7 sur le Wemos D1 Mini = GPIO13)
#define SERVO_PIN D7

// Structure du message à recevoir
typedef struct struct_message {
  int angle;
} struct_message;

// Crée une variable de type struct_message
struct_message myData;

// Crée un objet servo
Servo myServo;

// Callback qui s'exécute quand on reçoit des données
void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len) {
  // Adresse MAC de l'expéditeur
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  
  // Copie les données reçues dans notre structure
  memcpy(&myData, incomingData, sizeof(myData));
  
  // Affiche les informations reçues
  Serial.print("Reçu de: ");
  Serial.print(macStr);
  Serial.print(" | Angle reçu: ");
  Serial.println(myData.angle);

  // Applique l'angle au servo moteur
  myServo.write(myData.angle);
  Serial.print("Servo positionné à : ");
  Serial.println(myData.angle);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000); // Attendre que le moniteur série soit prêt
  
  Serial.println("Démarrage du récepteur ESP-NOW pour Wemos D1 Mini...");
  
  // Initialise le servo moteur
  myServo.attach(SERVO_PIN);
  myServo.write(90);  // Position initiale à 90 degrés
  Serial.println("Servo moteur initialisé");
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Configure le canal WiFi
  wifi_set_channel(1);
  Serial.println("Canal WiFi configuré sur : 1");
  
  Serial.print("Adresse MAC du récepteur: ");
  Serial.println(WiFi.macAddress());

  // Initialise ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Erreur pendant l'initialisation d'ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW initialisé avec succès");

  // Enregistre la callback qui sera appelée pour recevoir les données
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  
  Serial.println("En attente de messages...");
}

void loop() {
  // Rien à faire ici - tout est géré dans la callback OnDataRecv
  delay(10);
}
