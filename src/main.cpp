#include <Arduino.h>

#include <Ethernet.h>
#include <SoftwareSerial.h>
#include <SPI.h>

// -------- Constantes & Enumérations --------
#define RX_RFID_PIN 4
#define TX_RFID_PIN 5

const byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

enum Badge {
  ROUGE = 0,
  JAUNE,
  BLEU,
  BLEU2,
  BLEU3,
  CARTE,
  NON_RECONNU
};

const unsigned char rouge[] = {2,'0','E','0','0','4','C','5','7','0','A','1','F',3};
const unsigned char jaune[] = {2,'0','9','0','0','2','E','1','9','8','0','B','E',3};
const unsigned char bleu [] = {2,'1','5','0','0','9','1','9','0','8','5','9','1',3};
const unsigned char bleu2[] = {2,'0','0','0','0','1','A','C','3','1','3','C','A',3};
const unsigned char bleu3[] = {2,'1','5','0','0','8','9','1','7','4','8','C','3',3};
const unsigned char carte[] = {2,'3','8','0','0','7','1','2','6','1','F','7','0',3};
// -------------------------------------------

// ----------- Variables globales ------------
Badge b;

bool poules[6];
int nbPoules = 0;

SoftwareSerial RFID(RX_RFID_PIN, TX_RFID_PIN);
unsigned char buffer[64];
int count = 0;

IPAddress ip(192, 168, 1, 48);
EthernetServer server(80);

// ======= DEBUG =======
int debugIndex = 0;
// =====================

// -------- Prototypes des fonctions ----------

void readRfidBuffer();
void clearBufferArray();
Badge readBadge();
bool bufferEquals(unsigned char arr[]);
String getChickenName(int index);

String generateRequestResponse();
void handleHttpRequests();
// -------------------------------------------

void setup()
{
    for (int i=0; i < 6; i++)
    {
      poules[i] = false;
    }

    Serial.begin(9600);
    RFID.begin(9600);

    Ethernet.begin(mac, ip);

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Shield Ethernet non détecté. Impossible de continuer.");
      while (true) {
        delay(1);
      }
    }

    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Le câble Ethernet est déconnecté. Impossible de continuer.");
      while (true) {
        delay(1);
      }
    }

    server.begin();
    Serial.print("Le serveur est à l'adresse ");
    Serial.println(Ethernet.localIP());
}

void loop()
{
  // Lecture du badge, gestion du nombre de poules

  if (RFID.available())
  {
    readRfidBuffer();

    b = readBadge();

    if (b != NON_RECONNU)
    {
      poules[b] = !poules[b];
      poules[b] ? nbPoules++ : nbPoules--;

      Serial.println(getChickenName(b) + " est " + (poules[b] ? "rentrée" : "sortie")
                     + "! Il y a maintenant " + nbPoules + " poule" + (nbPoules > 1 ? "s." : "."));

      if (nbPoules == 6)
      {
        Serial.println("Toutes les poules sont dans le poulailler!");
      }

    }
    else if (b == NON_RECONNU)
    {
      Serial.println("Poule non reconnue.");
    }
    else
    {
      Serial.println("Badge inconnu.");
    }

    clearBufferArray();
    count = 0;
  }

  // Gestion des requêtes HTTP
  handleHttpRequests();

  // DEBUG
  if (Serial.available())
  {
    char c = Serial.read();
    if (c >= 48 && c <= 53)
    {
      int i = (int) c - 48;
      poules[i] = !poules[i];
      for (int i=0; i < 6; i++)
      {
        Serial.print(poules[i] ? "Oui " : "Non ");
      }
      Serial.println("");
      poules[i] ? nbPoules++ : nbPoules--;
    }
    else
    {
      for (int i=0; i < 6; i++)
      {
        Serial.print(poules[i] ? "Oui " : "Non ");
      }
      Serial.println("");
    }
  }

  // Délai avant la prochaine boucle (erreurs de lecture du RFID sinon, 50ms minimum)
  delay(200);
}

// --------- Implémentations des fonctions ---------

void readRfidBuffer()
{
  while(RFID.available())
  {
    buffer[count] = RFID.read();
    count++;
    if(count == 64)
      break;
  }
}

void clearBufferArray()
{
  for (int i=0; i<count; i++)
  {
    buffer[i] = NULL;
  }
}

bool bufferEquals(unsigned char arr[])
{
  for (int i = 0; i < 14; i++)
  {
    if (buffer[i] != arr[i])
      return false;
  }

  return true;
}

Badge readBadge()
{
  if (bufferEquals(rouge)) {
    return ROUGE;
  } else if (bufferEquals(jaune)) {
    return JAUNE;
  } else if (bufferEquals(bleu)) {
    return BLEU;
  } else if (bufferEquals(bleu2)) {
    return BLEU2;
  } else if (bufferEquals(bleu3)) {
    return BLEU3;
  } else if (bufferEquals(carte)) {
    return CARTE;
  } else {
    return NON_RECONNU;
  }
}

String getChickenName(int index)
{
  switch (index)
  {
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
    default:
      return "Poule inconnue.";
      break;
  }
}

String generateRequestResponse()
{
  String response = "{\"missing\": [";
  bool isFirst = true;

  for (int i = 0; i < 6; i++)
  {
    if (!poules[i]) {
      if (!isFirst)
      {
        response += ", ";
      }
      response += "\"";
      response += getChickenName(i);
      response += "\"";
      isFirst = false;
    }
  }

  response += "], \"count\": ";
  response += String(6 - nbPoules) + "}";

  return response;
}

void handleHttpRequests()
{
  EthernetClient client = server.available();
  if (client)
  {
    Serial.println("New client");
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        if (c == '\n' && currentLineIsBlank)
        {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Connection: close");
          client.println(); // Nouvelle ligne nécessaire (fin de l'en-tête de la réponse HTTP)
          String r = generateRequestResponse();
          Serial.println(r);
          client.println(r);
          break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    delay(1);
    client.stop();
    Serial.println("Client disconnected");
  }
}
