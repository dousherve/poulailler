#include <Arduino.h>
#include <SoftwareSerial.h>

// Constantes & Enumérations

#define RX_RFID_PIN 4
#define TX_RFID_PIN 5

#define RX_BT_PIN 2
#define TX_BT_PIN 3

enum Badge { ROUGE = 0, JAUNE, BLEU, BLEU2, BLEU3, CARTE, NON_RECONNU };

const unsigned char rouge[] = {2,'0','E','0','0','4','C','5','7','0','A','1','F',3};
const unsigned char jaune[] = {2,'0','9','0','0','2','E','1','9','8','0','B','E',3};
const unsigned char bleu [] = {2,'1','5','0','0','9','1','9','0','8','5','9','1',3};
const unsigned char bleu2[] = {2,'0','0','0','0','1','A','C','3','1','3','C','A',3};
const unsigned char bleu3[] = {2,'1','5','0','0','8','9','1','7','4','8','C','3',3};
const unsigned char carte[] = {2,'3','8','0','0','7','1','2','6','1','F','7','0',3};

// Déclaration des variables globales

Badge b;
bool poules[6] = {false, false, false, false, false, false};
int nbPoules = 0;

SoftwareSerial RFID(RX_RFID_PIN, TX_RFID_PIN);
unsigned char buffer[64];
int count = 0;

SoftwareSerial BT(RX_BT_PIN, TX_BT_PIN);
char bt_buffer[64];
int bt_count = 0;

String response = "";

// Prototypes des fonctions

void readRfidBuffer();
void readBtBuffer();
Badge getBadge();
void clearBufferArray();
void clearBtBufferArray();
bool arrEquals(unsigned char arr[]);
String getChickenName(int b);
String processRequest();

void setup() {
    RFID.begin(9600);
    BT.begin(9600);
    Serial.begin(9600);
}

void loop() {

  // Lecture du badge, gestion du nombre de poules

  if (RFID.available()) {

    readRfidBuffer();

    b = getBadge();

    if (b != NON_RECONNU) {

      poules[b] = !poules[b];
      poules[b] ? nbPoules++ : nbPoules--;

      Serial.println(getChickenName(b) + " est " + (poules[b] ? "rentrée" : "sortie")
                     + "! Il y a maintenant " + nbPoules + " poule" + (nbPoules > 1 ? "s." : "."));

      if (nbPoules == 6) {
        Serial.println("Toutes les poules sont dans le poulailler!");
      }

    } else if (b == NON_RECONNU) {
      Serial.println("Poule non reconnue.");
    } else {
      Serial.println("Badge inconnu.");
    }

    clearBufferArray();
    count = 0;
  }

  // Gestion des requêtes Bluetooth

  if (BT.available()) {

    readBtBuffer();

    if (strcmp(bt_buffer, "Allo") == 0)
    {
      BT.println(processRequest());
    } else {
      Serial.println(bt_buffer);
    }

    clearBtBufferArray();
    bt_count = 0;

  }

  // Délai avant la prochaine boucle (erreurs de lecture du RFID sinon, 50ms minimum)
  delay(200);

  if (Serial.available())
    RFID.write(Serial.read());
}

// Implémentations des fonctions

void readRfidBuffer() {

  while(RFID.available()) {
    buffer[count] = RFID.read();
    count++;
    if(count == 64)
      break;
  }

}

void readBtBuffer() {

  while(BT.available()) {
    bt_buffer[bt_count] = BT.read();
    bt_count++;
    if(bt_count == 64)
      break;
  }

}

void clearBufferArray() {

  for (int i=0; i<count; i++)
  {
    buffer[i] = NULL;
  }

}

void clearBtBufferArray() {

  for (int i=0; i<bt_count; i++)
  {
    bt_buffer[i] = NULL;
  }

}

bool arrEquals(unsigned char arr[]) {

  for (int i = 0; i < 14; i++) {
    if (buffer[i] != arr[i])
      return false;
  }

  return true;

}

String getChickenName(int b) {

  switch (b) {
    case ROUGE:
      return "Tchiki";
      break;
    case BLEU:
      return "Mama";
      break;
    case JAUNE:
      return "Black Jack";
      break;
    case BLEU2:
      return "Coco";
      break;
    case BLEU3:
      return "Mechmecha";
      break;
    case CARTE:
      return "Bernadette";
      break;
  }

}

Badge getBadge() {

  if (arrEquals(rouge)) {
    return ROUGE;
  } else if (arrEquals(jaune)) {
    return JAUNE;
  } else if (arrEquals(bleu)) {
    return BLEU;
  } else if (arrEquals(bleu2)) {
    return BLEU2;
  } else if (arrEquals(bleu3)) {
    return BLEU3;
  } else if (arrEquals(carte)) {
    return CARTE;
  } else {
    return NON_RECONNU;
  }

}

String processRequest() {

  response = "{\"missing\": [";

  for (int i = 0; i < 6; i++) {
    if (!poules[i]) {
      response += "\"";
      response += getChickenName(i);
      if (i < 5)
      {
        response += "\", ";
      } else {
        response += "\"";
      }
    }
  }

  response += "], \"count\": ";
  response += String(6 - nbPoules) + "}";

  return response;
}
