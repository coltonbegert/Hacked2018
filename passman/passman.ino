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

char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
char once = 1;
aes_context ctx;

char * encrypt(char * data, uint8_t * key, size_t * len){
  ctx = aes128_cbc_enc_start(key,iv);
  *len = strlen(data);
  char pad = *len % 16;
  if (pad == 0)
    pad = 16;
  *len = *len + pad;
  char * cipherText = malloc(sizeof(char) * *len);
  memset(cipherText, 0, *len);
  memcpy(cipherText, data, *len);
  aes128_cbc_enc_continue(ctx, cipherText, *len);
  aes128_cbc_enc_finish(ctx);
  return cipherText;
}

int decrypt(char * data, uint8_t * key, size_t len){
  ctx = aes128_cbc_dec_start(key,iv);
  aes128_cbc_dec_continue(ctx, data, len);
  aes128_cbc_dec_finish(ctx);
}

int crypto_test(){
    Serial.print("Plaintext: ");
    uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    char * data2 = malloc(sizeof(char) * 33);
    strcpy(data2, "123 456 789 123 456 789 123 456"); 
    Serial.println(data2);
    int newLen;
    char * newData = encrypt(data2, key, &newLen);

    Serial.print("encrypted:");
    Serial.println(newData);
  
    decrypt(newData, key, 32);
    Serial.print("decrypted:");
    Serial.println(data2);
    Serial.println(71 % 16);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(5000);
  crypto_test();
}


void loop() {
  // put your main code here, to run repeatedly:

  delay(1000);
  once = 0;
}

