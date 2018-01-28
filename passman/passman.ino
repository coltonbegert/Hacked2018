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

char once = 1;
aes_context ctx;

char * encrypt_all(char * data, uint8_t * key, const void * iv, size_t * len){
  ctx = aes128_cbc_enc_start(key,iv);
  *len = strlen(data);
  Serial.println(*len);
  int pad = *len % 16;
  pad = 16 - pad;
  Serial.println(pad);
  if (pad == 0)
    pad = 16;
  *len = *len + pad;
  Serial.println(*len);
  char * cipherText = malloc(sizeof(char) * (*len));
  memset(cipherText, 0, *len);
  memcpy(cipherText, data, strlen(data));
  int i;
  for (i = 0; i < *len; i++){
    Serial.print(i);
    Serial.print(": ");
    Serial.print(cipherText[i], DEC);
    Serial.println(" ");
  }
  aes128_cbc_enc_continue(ctx, cipherText, *len);
  aes128_cbc_enc_finish(ctx);
  return cipherText;
}


// Start Partial Decrypting with CBC mode /////
int start_partial_decrypt(aes_context * ctx, char * data, uint8_t * key, const void * iv, size_t len){
  *ctx = aes128_cbc_dec_start(key,iv);
  aes128_cbc_dec_continue(*ctx, data, len);
}

int continue_partial_decrypt(aes_context *ctx, char * data, uint8_t * key, size_t len){
  aes128_cbc_dec_continue(*ctx, data, len);
}

int end_partial_decrypt(aes_context *ctx, char * data, uint8_t * key, size_t len){
  aes128_cbc_dec_continue(*ctx, data, len);
  aes128_cbc_dec_finish(*ctx);
}


// Start Partial Encrypting with CBC mode ////
int start_partial_encrypt(aes_context * ctx, char * data, uint8_t * key, const void * iv, size_t len){
  *ctx = aes128_cbc_dec_start(key,iv);
  aes128_cbc_dec_continue(*ctx, data, len);
}

int continue_partial_encrypt(aes_context *ctx, char * data, uint8_t * key, size_t len){
  aes128_cbc_dec_continue(*ctx, data, len);
}

int end_partial_encrypt(aes_context *ctx, char * data, uint8_t * key, size_t len){
  aes128_cbc_dec_continue(*ctx, data, len);
  aes128_cbc_dec_finish(*ctx);
}

int crypto_test(){
  //uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  //uint8_t key[] = {0,1,2,3,4,5};
  char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  uint8_t * key = "Apples";
  char * data2 = malloc(sizeof(char) * 200);
  strcpy(data2, "This is some test data to encypt, lets see what happens here. This is a large me, man"); 
  Serial.print("Plaintext: ");
  Serial.println(data2);
  int newLen;
  char * newData = encrypt_all(data2, key, iv, &newLen);
  Serial.println(newLen);
  Serial.print("encrypted:");
  Serial.println(newData);

  aes_context ctx;
  size_t step = 16;
  start_partial_decrypt(&ctx, newData, key, iv, step);
  newData += step;
  int i = 1;
  while ( (i*step) < (newLen-step)){
    continue_partial_decrypt(&ctx, newData, key, step);
    newData += step;
    i++;
    Serial.println(i);
    Serial.print(" ");
  }
  end_partial_decrypt(&ctx, newData, key, step);
  
  Serial.print("decrypted:");
  Serial.println(newData-80);
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

