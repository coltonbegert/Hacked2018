#include <SD.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, PIN, NEO_GRB + NEO_KHZ800);


struct  Message{
  char type;
  uint16_t length;
  char *message;
};


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
  // put your setup code h¨ere, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  // int incomingByte = 0;
  // if (Serial.available() > 0)
  // {
  //   // read the incoming byte:
  //   incomingByte = Serial.read();

  //   // say what you got:
  //   // Serial.print("I received: ");
  //   Serial.write(incomingByte);
  // }
  // Serial.print("test");
  struct Message m;

  m.type = 0x6d;
  m.length = 11;
  m.message = "abcdefghijk";
  // send_message(m);
  free(&m);
  struct Message *me;

  if (Serial.available() >= 3) {
    recv_message(me);

  }

  delay(1000);
}
