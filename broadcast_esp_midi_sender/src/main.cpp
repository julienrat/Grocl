#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32.h>

//Rouge canal 1 // Vert Canal 5 // Blanc Canal 9 // Noir Canal 13

#define WIFI_CHANNEL 13
#define MIDI_CHANNEL 1  // Canal MIDI par défaut

// Création de l'instance MIDI BLE
BLEMIDI_CREATE_INSTANCE("Wind_to_midi", MIDI)

// Structure pour recevoir les données
typedef struct struct_message {
  int value;  // Cette valeur sera convertie en valeur MIDI (0-127)
  uint8_t on; // 1 = note on, 0 = note off (optionnel)
} struct_message;

// Crée une variable pour stocker les données reçues
struct_message myData;
// Note courante suivie pour envoyer NoteOff si nécessaire
int lastNote = -1;
int lastVelocity = 0;

// Callback qui s'exécute quand on reçoit des données
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  // Adresse MAC de l'expéditeur
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  
  // Parse les données reçues de façon robuste (messages plus courts possibles)
  int value = 0;
  // Si on a au moins 4 octets, lit un int (little-endian attendu)
  if (len >= (int)sizeof(int)) {
    memcpy(&value, incomingData, sizeof(int));
  } else if (len >= 1) {
    // si seulement 1 octet envoyé, l'interprète comme value
    value = incomingData[0];
  }

  // Affiche les informations reçues
  Serial.print("Reçu de: ");
  Serial.print(macStr);
  Serial.print(" | Valeur reçue: ");
  Serial.println(value);

  // Convertit la valeur (0-180) en note MIDI dans une plage utile
  int midiNote = map(value, 0, 180, 36, 96); // C2..C7 approx
  int velocity = map(value, 0, 180, 0, 127);
  if (velocity < 1) velocity = 64; // garantir une vélocité minimale pour forcer Note On

  // A chaque message reçu, envoie systématiquement une Note On
  // coupe d'abord la note précédente si elle existe (pour retrigger)
  if (lastNote != -1) {
    MIDI.sendNoteOff(lastNote, 0, MIDI_CHANNEL);
    Serial.print("Note Off envoyé pour (précédente) : ");
    Serial.println(lastNote);
  }

  MIDI.sendNoteOn(midiNote, velocity, MIDI_CHANNEL);
  Serial.print("Note On envoyé : ");
  Serial.print(midiNote);
  Serial.print(" vel=");
  Serial.println(velocity);

  lastNote = midiNote;
  lastVelocity = velocity;
}


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000); // Attendre que le moniteur série soit prêt
  
  Serial.println("Démarrage du récepteur ESP-NOW avec MIDI BLE...");
  
  // Initialisation du MIDI BLE
  MIDI.begin(MIDI_CHANNEL);
  Serial.println("MIDI BLE initialisé");
  
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
