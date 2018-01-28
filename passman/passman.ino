#include <SD.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6

#define CS  4     // chip select

#define MASTER_KEY      "MASTRKEY.txt"
#define T_OF_C          "TABLECOL.txt"

#define ECHO_ALL true

Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, PIN, NEO_GRB + NEO_KHZ800);

char *master_pass;
uint16_t master_pass_length = 0;

char *master_aes;


union byte_union{
  uint16_t length;
  uint8_t bytes[2];
};

struct Message{
  char type;
  union byte_union length;
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
  m->length.length = 0;
  m->message = malloc(33);
  while (master_key.available() && m->length.length <= 33) {
    m->message[m->length.length++] = master_key.read();
  }
  m->message[m->length.length] = '\0';

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
  m->length.length = 0;
  m->message = malloc(33);
  while (f.available() && m->length.length <= 32) {
    m->message[m->length.length++] = f.read();
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

  m->length.length = 0;

  while (f.available() && m->length.length <= 128) {
    m->message[m->length.length++] = f.read();
  }

  return m->length.length;
}

void send_message(struct Message *m) {
  Serial.print(m->type);

  Serial.write((uint8_t *) &m->length.length, 2);
  if (m->length.length > 0) {
    Serial.write((uint8_t *)m->message, m->length.length);

  }

}

void recv_message(struct Message *m) {
  m->type = (char)Serial.read();

  m->length.bytes[1] = Serial.read();
  m->length.bytes[0] = Serial.read();

  if (m->length.length > 0) {
    m->message = malloc(m->length.length * sizeof(char));
    for (int i = 0; i < m->length.length; i++) {
      m->message[i] = (char)Serial.read();
    }
  }
  
}
void handle_message(struct Message *m) {
  switch (m->type) {
    case 'K':
      master_pass_length = m->length.length;
      master_pass = malloc(master_pass_length);
      memcpy(master_pass, m->message, master_pass_length);

      break;
    default:

    break;


  }
  if (ECHO_ALL) {
    
    strip.setPixelColor(0, 255, 255, 255);
    strip.show();
    char t = m->type;
    m->type = 'D';
    send_message(m);
    m->type = t;
    strip.setPixelColor(0, 0, 0,0);
    strip.show();
  }
  return;
}


void request_master_pass() {
  struct Message *me;
  me = malloc(sizeof(struct Message));
  me->message = NULL;
  me->type = 'K';
  me->length.length = 0;
  send_message(me);
  free(me);
  return;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(8);
  while (!Serial);  // busy spinning while Serial starts

  if (!SD.begin(CS)) {
    // Serial.println("SD card says no");
    return;
  }
  
  master_pass = NULL;
  master_aes = NULL;

  request_master_pass();
  // Serial.println("Setup complete!");
}

void loop() {
  // put your main code here, to run repeatedly:

  struct Message *me;
  me = malloc(sizeof(struct Message));
  me->message = NULL;


  if (Serial.available() >= 3) {
    recv_message(me);
    handle_message(me);
    // send_message(me);
  }
  if (me->message) {
    free(me->message);
    me->message = NULL;
  }
  free(me);
  me = NULL;
  if (master_pass) {
    strip.setPixelColor(1, 0, 255, 0);
    strip.show();
  } else {
    strip.setPixelColor(1, 0, 0, 0);
    strip.show();
  }

  if (master_pass && !master_aes) {
    strip.setPixelColor(2, 255, 255, 255);
    strip.show();
    delay(100);
    strip.setPixelColor(2,0,0,0);
    strip.show();
  }
  if (master_pass && master_aes) {
    strip.setPixelColor(2, 0, 255, 0);
    strip.show();

  }

  // if (!master_pass) {
  //   request_master_pass();
  // }

  // struct Message master_key;
  // if (!read_mkey(&master_key)) {
  //   Serial.println("Can't read master key!");
  //   return;
  // }
  
  // Serial.println(master_key.message);
  // Serial.println(master_key.length);
  
  // free(master_key.message);

  // struct Message toc = {0};
  // File toc_f = SD.open(T_OF_C, FILE_READ);
  // if (!toc_f) {
  //   Serial.println("Can't read table of contents");
  //   return;
  // }
  
  // while (read_toc(&toc, toc_f) > 0) {
  //   for (int i = 0; i < toc.length; ++i) {
  //     Serial.print(toc.message[i]);
  //   }
  // }

  // Serial.println("");

  // toc_f.close();

  // free(toc.message);


  delay(50);
}

