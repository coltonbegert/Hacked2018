#include <SD.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, PIN, NEO_GRB + NEO_KHZ800);

union byte_union{
  uint16_t length;
  uint8_t bytes[2];
};

struct Message{
  char type;
  union byte_union length;
  char *message;
};


void send_message(struct Message *m) {
  Serial.print(m->type);

  Serial.write((uint8_t *) &m->length.length, 2);
  Serial.write((uint8_t *)m->message, m->length.length);

}

void recv_message(struct Message *m) {
  m->type = (char)Serial.read();

  m->length.bytes[1] = Serial.read();
  m->length.bytes[0] = Serial.read();

  m->message = malloc(m->length.length * sizeof(char));
  for (int i = 0; i < m->length.length; i++) {
    m->message[i] = (char)Serial.read();
  }
  
}

void setup() {
  // put your setup code h¨ere, to run once:
  Serial.begin(9600);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(8);

}

void loop() {
  // put your main code here, to run repeatedly:

  struct Message *me;
  me = malloc(sizeof(struct Message));


  if (Serial.available() >= 3) {
    recv_message(me);
    send_message(me);

  }


  delay(50);
}
