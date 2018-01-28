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

int decrypt_all(char * data, uint8_t * key, const void * iv, size_t len){
  aes192_cbc_dec(key, iv, data, len);
}

int encrypt_all(char * data, uint8_t * key, const void * iv, size_t len){
  int total = ((len / 16) + 1) * 16;
  memset(data+len, total-len, total-len); 
  aes192_cbc_enc(key, iv, data, total);
  return 0;
}

//////////////// Start Partial Decrypting with CBC mode /////
int start_partial_decrypt(aes_context * ctx, char * data, uint8_t * key, const void * iv, size_t len){
  *ctx = aes192_cbc_dec_start(key,iv);
  aes192_cbc_dec_continue(*ctx, data, len);
}

int continue_partial_decrypt(aes_context *ctx, char * data, int offset, uint8_t * key, size_t len){
  aes192_cbc_dec_continue(*ctx, data+offset, len);
}

int end_partial_decrypt(aes_context *ctx, char * data, int offset, uint8_t * key, size_t len){
  aes192_cbc_dec_continue(*ctx, data+offset, len);
  aes192_cbc_dec_finish(*ctx);
}

//////////////// Start Partial Encrypting with CBC mode ////
int start_partial_encrypt(aes_context * ctx, char * data, uint8_t * key, const void * iv, size_t len){
  *ctx = aes192_cbc_enc_start(key,iv);
  aes192_cbc_enc_continue(*ctx, data, len);
}

int continue_partial_encrypt(aes_context *ctx, char * data, int offset, uint8_t * key, size_t len){
  aes192_cbc_enc_continue(*ctx, data+offset, len);
}

int end_partial_encrypt(aes_context *ctx, char * data, int offset, uint8_t * key, size_t len){
  int total = ((len / 16) + 1) * 16;
  memset(data+offset+len, total-len, total-len); 
  aes192_cbc_enc_continue(*ctx, data+offset, total);
  aes192_cbc_enc_finish(*ctx);
}

int crypto_test(){
  
  char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
  uint8_t * key = "ApplesAndBananas12";
  char * plain = malloc(150);
  char * plain2 = malloc(150);
  strcpy(plain,"Mark and Alan and Colton are at a Hackathon Today. We are best friends forever"); //78 chars
  strcpy(plain2, plain);
  Serial.print("Plaintext: ");
  Serial.println(plain);
  encrypt_all(plain, key, iv, 78);
  Serial.print("After calling encrypt_all(): ");
  Serial.println(plain);
  decrypt_all(plain, key, iv, 80);
  plain[78] = 0;
  Serial.print("After calling decrypt_all(): ");
  Serial.println(plain);

  aes_context ctx;
  Serial.print("Plaintext: ");
  Serial.println(plain2);
  start_partial_encrypt(&ctx, plain2, key, iv, 16);
  continue_partial_encrypt(&ctx, plain2, 16, key, 48);
  end_partial_encrypt(&ctx, plain2, 64, key, 16);
  Serial.print("After calling encrypt_all(): ");
  Serial.println(plain2);
  start_partial_decrypt(&ctx, plain2, key, iv, 16);
  continue_partial_decrypt(&ctx, plain2, 16, key, 32);
  end_partial_decrypt(&ctx, plain2, 48, key, 32);
  plain2[78] = 0;
  Serial.print("After calling decrypt_all(): ");
  Serial.println(plain2);

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
}

