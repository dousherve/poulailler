#include <Arduino.h>
#include <SoftwareSerial.h>

// Constantes & Enumérations

#define RX_RFID_PIN 2
#define TX_RFID_PIN 3

enum Badge { ROUGE, JAUNE, BLEU, BLEU2, BLEU3, CARTE, NON_RECONNU };

const unsigned char rouge[] = {2,'0','E','0','0','4','C','5','7','0','A','1','F',3};
const unsigned char jaune[] = {2,'0','9','0','0','2','E','1','9','8','0','B','E',3};
const unsigned char bleu[]  = {2,'1','5','0','0','9','1','9','0','8','5','9','1',3};
const unsigned char bleu2[] = {2,'0','0','0','0','1','A','C','3','1','3','C','A',3};
const unsigned char bleu3[] = {2,'1','5','0','0','8','9','1','7','4','8','C','3',3};
const unsigned char carte[] = {2,'3','8','0','0','7','1','2','6','1','F','7','0',3};

// Déclaration des variables globales

Badge b;
bool poules[6];
unsigned int nbPoules = 0;

SoftwareSerial RFID(RX_RFID_PIN, TX_RFID_PIN);
unsigned char buffer[64];
int count = 0;

// Prototypes des fonctions

void readRfidBuffer();
Badge getBadge();
void clearBufferArray();
bool arrEquals(unsigned char arr[]);
String getChickenName();

void setup() {
    RFID.begin(9600);
    Serial.begin(9600);
}

void loop() {

  if (RFID.available()) {

    readRfidBuffer();

    b = getBadge();

    if (b != NON_RECONNU) {

      poules[b] = !poules[b];
      poules[b] ? nbPoules++ : nbPoules--;

      Serial.println(getChickenName() + " est " + (poules[b] ? "rentrée" : "sortie")
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

void clearBufferArray() {

  for (int i=0; i<count; i++)
  {
    buffer[i] = NULL;
  }

}

bool arrEquals(unsigned char arr[]) {

  for (int i = 0; i < 14; i++) {
    if (buffer[i] != arr[i])
      return false;
  }

  return true;

}

String getChickenName() {

  switch (getBadge()) {
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
