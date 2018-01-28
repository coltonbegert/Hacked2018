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
  // Serial.print("recv");
  // int incomingByte;
  // if (Serial.available() > 0)
  // {
    //   // read the incoming byte:
    // incomingByte = Serial.read();

    //   // say what you got:
    //   // Serial.print("I received: ");
    // Serial.write(incomingByte);
    // Serial.print(Serial.available());
  // }
  // return;
  // struct Message m;
  // Serial.read((uint8_t *)&m->type, 1);¨
  int len = 4;
  // Serial.print("hello");
  // delay(100);
  m->type = (char)Serial.read();
  // // Serial.print(m->type);
  uint8_t byte1,byte2,byte0;
  // byte0 = Serial.read();
  byte0 = Serial.read();
  byte1 = Serial.read();

  m->length.bytes[0] = byte1;
  m->length.bytes[1] = byte0;

  // uint16_t sho = ((uint16_t)byte1 << 8) | byte2;
  // m->length = sho;
  // m->length = ((uint16_t)byte1 << 8) | byte2;
  // m->length = ((uint16_t)Serial.read() << 8) | Serial.read();
  m->message = malloc(m->length.length * sizeof(char));
  for (int i = 0; i < m->length.length; i++) {
    m->message[i] = (char)Serial.read();
    // Serial.write((uint8_t *)&m->message[i], 1);
  }
  // while (Serial.available() > 0)
  // {
  //   Serial.read();
  //   delay(100);
  // }
  // // m->length[0] = (void)Serial.read();
  // // m->length[1] = (void)Serial.read();
  // // Serial.print(m->type);
  // // Serial.print(m->length);
  // // Serial.print("world");
  // m->message = malloc(len * sizeof(char));
  // for (int i = 0; i < len; i++) {
  //   m->message[i] = Serial.read();
  // }

  // Serial.print("world");



  // Serial.print(byte0);
  // Serial.print(byte1);
  // Serial.print(byte2);

  //  m->type = (char) byte0;
  // m->type = (char) byte0;

  // Serial.print(m->type);
  // Serial.print("a");
  // Serial.print(m->length.length);
  // Serial.print("b");
  // Serial.print(m->length.byt[0]);
  // Serial.print("c");
  // Serial.print(m->length.byt[1]);
  // Serial.print("d");
  // Serial.write((uint8_t *)m->message, m->length.length);

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
  
  // send_message(m);
  // Serial.print("hello");
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
  int incomingByte = 0;
  // if (Serial.available() > 0)
  // {
  // //   // read the incoming byte:
  //   incomingByte = Serial.read();

  // //   // say what you got:
  // //   // Serial.print("I received: ");
  //   Serial.write(incomingByte);
  // }
  // Serial.print("test");

  // send_message(m);
  // free(&m);
  struct Message *me;
  me = malloc(sizeof(struct Message));


  if (Serial.available() > 0) {
    recv_message(me);
    send_message(me);

  }

  // Serial.print("f");
  delay(1000);
}
