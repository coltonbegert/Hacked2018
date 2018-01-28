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

#define PASSWORD_SALT   "BigColtonAndBigPeterAndBigAristotle"

#define PIN 6

#define CS  4     // chip select

#define MASTER_KEY      "MASTRKEY.txt"
#define T_OF_C          "TABLECOL.txt"

#define KEY_SHAPE       0x06A9F906
                        //00000110 10101001 11111001 00000110

#define NO_SHAPE        0xEAAAAAEE
                        //11101010 10101010 10101010 11101110
                       

enum led_state {NONE, BLINK_RED_KEY, ORANGE_KEY, RED_NO, PAUSE};
enum led_state led_state = NONE;

#define MAX_BRIGHT 10
#define MIN_BRIGHT -5

#define ECHO_ALL true

Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, PIN, NEO_GRB + NEO_KHZ800);

char once = 1;
aes_context ctx;
static char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};

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

void update_led()
{
  static enum led_state last_state = NONE;
  static int brightness = 1;
  static bool brighten = true;
  static int timer = 0;
  if (last_state != led_state) {
    // need to initialize the new state
    switch (led_state) {
      case BLINK_RED_KEY:
        strip.setBrightness(0);
        brightness = 1;
        brighten = true;
        break;
      case RED_NO:
        strip.setBrightness(0);
        timer = 0;
        break;
    }
  }

  last_state = led_state;

  if (led_state == PAUSE) {
    return;
  }

  if (led_state == BLINK_RED_KEY) {
    for (int i = 0; i < 32; ++i) {
      if ((KEY_SHAPE >> i) & 1) {
        strip.setPixelColor(i, strip.Color(255 / MAX_BRIGHT * max(1, brightness), 0, 0));
      }
    }
    
    if (brighten) {
      strip.setBrightness(max(brightness, 1));
      brightness++;
      brighten = (brightness != MAX_BRIGHT);
    } else {
      strip.setBrightness(max(brightness, 1));
      brightness--;
      brighten = (brightness == MIN_BRIGHT);
    }
  }

  if (led_state == ORANGE_KEY) {
    for (int i = 0; i < 32; ++i) {
      if ((KEY_SHAPE >> i) & 1) {
        strip.setPixelColor(i, strip.Color(255, 100, 0));
      }
    }
    strip.setBrightness(2);
  }

  if (led_state == RED_NO) {
    for (int i = 0; i < 32; ++i) {
      if ((NO_SHAPE >> i) & 1) {
        strip.setPixelColor(i, strip.Color(255, 0, 0));
      }
    }
    strip.setBrightness(2);
    timer += 50;
    if (timer >= 3000) {
      led_state = BLINK_RED_KEY;
    }
  }

  strip.show();

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(8);
  while (!Serial);  // busy spinning while Serial starts

  if (!SD.begin(CS)) {
    Serial.println("SD card says no");
    return;
  }
  
  master_pass = NULL;
  master_aes = NULL;

  led_state = RED_NO;
  
  //request_master_pass();
  master_pass = "password123";
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
    if (strlen(master_pass) < 24) {
      strncpy(master_pass + strlen(master_pass), PASSWORD_SALT, 24 - strlen(master_pass));
    }
    
    struct Message master_key_m;
    if (!read_mkey(&master_key_m)) {
      return;
    }

    decrypt_all(master_key_m.message, master_pass, iv, 32);

    Serial.println(master_key_m.message);

    free(master_key_m.message);
  }
  if (master_pass && master_aes) {
    strip.setPixelColor(2, 0, 255, 0);
    strip.show();
    Serial.println(master_pass);
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

  update_led();
  delay(50);
}





