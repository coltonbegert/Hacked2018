#include <SD.h>
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#include <aes.h>
#include <aes128_dec.h>
#include <aes128_enc.h>
#include <aes192_dec.h>
#include <aes192_enc.h>
#include <aes256_dec.h>
#include <aes256_enc.h>
#include <aes_dec.h>
#include <aes_enc.h>
#include <aes_invsbox.h>
#include <aes_keyschedule.h>
#include <aes_sbox.h>
#include <aes_types.h>
#include <AESLib.h>
#include <bcal-basic.h>
#include <bcal-cbc.h>
#include <bcal-cmac.h>
#include <bcal-ofb.h>
#include <bcal_aes128.h>
#include <bcal_aes192.h>
#include <bcal_aes256.h>
#include <blockcipher_descriptor.h>
#include <gf256mul.h>
#include <keysize_descriptor.h>
#include <memxor.h>



#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  char data[] = "0123456789012345"; //16 chars == 16 bytes
  aes128_enc_single(key, data);
  Serial.print("encrypted:");
  Serial.println(data);
  aes128_dec_single(key, data);
  Serial.print("decrypted:");
  Serial.println(data);


void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("test");
  delay(1000);
}

