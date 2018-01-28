#include <SD.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6

#define CS  4     // chip select

#define MASTER_KEY      "MASTRKEY.txt"
#define T_OF_C          "TABLECOL.txt"

Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, PIN, NEO_GRB + NEO_KHZ800);


struct  Message{
  char type;
  uint16_t length;
  char *message;
};

// Fills m with master key data
struct Message *read_mkey(struct Message *m)
{ 
  File master_key = SD.open(MASTER_KEY, FILE_READ);
  if (!master_key) {
    return NULL;
  }

  m->type = 'K';
  m->length = 0;
  m->message = malloc(33);
  while (master_key.available() && m->length <= 33) {
    m->message[m->length++] = master_key.read();
  }
  m->message[m->length] = '\0';

  master_key.close();

  return m;
}

struct Message *read_index(struct Message *m, int index, char type)
{
  char filename[10];
  sprintf(filename, "%c%d", type, index);

  File f = SD.open(filename, FILE_READ);
  if (!f) {
    return NULL;
  }

  m->type = type;
  m->length = 0;
  m->message = malloc(33);
  while (f.available() && m->length <= 32) {
    m->message[m->length++] = f.read();
  }

  f.close();

  return m;
}

// Returns number of bytes read
int read_toc(struct Message *m, File f)
{
  if (!m->message) {
    m->message = malloc(128);
  }

  m->length = 0;

  while (f.available() && m->length <= 128) {
    m->message[m->length++] = f.read();
  }

  return m->length;
}

void send_message(struct Message *m) {

  Serial.print(m->type);

  Serial.write((uint8_t *) &m->length, 2);
  Serial.print(m->message);
  // for (int i = 0; i < message.length; i++) {
  //   Serial.print(m.message[i])
  // }
}

void recv_message(struct Message *m) {
  // struct Message m;
  // Serial.read((uint8_t *)&m->type, 1);
  int len = 4;
  // Serial.print("hello");
  // delay(100);
  m->type = (char)Serial.read();
  // // Serial.print(m->type);
  char byte1,byte2;
  byte1 = Serial.read();
  byte2 = Serial.read();
  uint16_t sho = (byte1 << 8) + byte2;
  // m->length = sho;
  // m->length = (byte1 << 8) | byte2;
  while (Serial.available() > 0) {
    Serial.read();
    delay(100);
  }
  // // m->length[0] = (void)Serial.read();
  // // m->length[1] = (void)Serial.read();
  // // Serial.print(m->type);
  // // Serial.print(m->length);
  // // Serial.print("world");
  // m->message = malloc(len * sizeof(char));
  // for (int i = 0; i < len; i++) {
  //   m->message[i] = Serial.read();
  // }

  Serial.print("world");
  // Serial.flush();
  // Serial.println("heylot");
  // Serial.print(m->length);
  // Serial.print(sho);
  // m->length = sho;
  // Serial.write((uint8_t *)&sho, 2);

  // delay(100);
  // Serial.flush();
  // Serial.print(m->type);
  // Serial.print(m->length);
  // Serial.print(m->message);

  // Serial.read((uint8_t *)&m->length, 2);
  // Serial.read(&m->message, m->length);
  
  send_message(m);
  Serial.print("hello");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);  // busy spinning while Serial starts

  if (!SD.begin(CS)) {
    Serial.println("SD card says no");
    return;
  }

  Serial.println("Setup complete!");
}

void loop() {
  // put your main code here, to run repeatedly:
  struct Message master_key;
  if (!read_mkey(&master_key)) {
    Serial.println("Can't read master key!");
    return;
  }
  
  Serial.println(master_key.message);
  Serial.println(master_key.length);
  
  free(master_key.message);

  struct Message toc = {0};
  File toc_f = SD.open(T_OF_C, FILE_READ);
  if (!toc_f) {
    Serial.println("Can't read table of contents");
    return;
  }
  
  while (read_toc(&toc, toc_f) > 0) {
    for (int i = 0; i < toc.length; ++i) {
      Serial.print(toc.message[i]);
    }
  }

  Serial.println("");

  toc_f.close();

  free(toc.message);

  delay(1000);
}

