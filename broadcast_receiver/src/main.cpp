#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ESP32Servo.h>

// Pin pour le servo moteur
#define SERVO_PIN 13

// Crée un objet servo
Servo myServo;

// Structure pour recevoir les données
typedef struct struct_message {
  int angle;
} struct_message;

// Crée une variable pour stocker les données reçues
struct_message myData;

// Callback qui s'exécute quand on reçoit des données
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
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

#define WIFI_CHANNEL 1

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000); // Attendre que le moniteur série soit prêt
  
  Serial.println("Démarrage du récepteur ESP-NOW...");
  
  // Initialise le servo moteur
  ESP32PWM::allocateTimer(0);
  myServo.setPeriodHertz(50);    // Fréquence standard pour les servos
  myServo.attach(SERVO_PIN, 500, 2400); // min/max pulse width
  myServo.write(90);  // Position initiale à 90 degrés
  Serial.println("Servo moteur initialisé");
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Pour être sûr de ne pas être connecté à un point d'accès
  
  // Configure le canal WiFi
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  Serial.print("Canal WiFi configuré sur : ");
  Serial.println(WIFI_CHANNEL);
  
  Serial.print("Adresse MAC du récepteur: ");
  Serial.println(WiFi.macAddress());

  // Initialise ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Erreur pendant l'initialisation d'ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW initialisé avec succès");
  Serial.println("En attente de messages...");

  // Enregistre la callback qui sera appelée pour recevoir les données
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Rien à faire ici - tout est géré dans la callback OnDataRecv
  delay(10);
}
