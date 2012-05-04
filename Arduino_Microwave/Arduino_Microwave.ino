#include <avr/interrupt.h> 
#include <avr/io.h>
#include <stdio.h>
#include "TimerOne.h"
#include <SPI.h> 
#include <Ethernet.h>

#define THRESHOLD 400
#define CLOCK_PERIOD 1000000	// 1s
#define START 4 // start at PD2
#define WAITING 0
#define COOKING 1

volatile byte state = WAITING;
volatile byte digits[4];
volatile byte old_time[4];
volatile byte time[4];
volatile byte pin = START;
volatile byte index = 0;  // start at CC1 (left-most)

EthernetClient client;

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x2D };

void setup()
{
  Serial.begin(115200);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }

  Timer1.initialize(CLOCK_PERIOD);
   Timer1.attachInterrupt(tick);
  PCMSK2 = 0x3C;	
  PCICR = 0x5;

}

void loop()
{
   

}

void copy_time() {
  old_time[0] = time[0];
  old_time[1] = time[1];
  old_time[2] = time[2];
  old_time[3] = time[3];
}

unsigned int get_time_val(volatile byte * time) {
  return ((time[0] * 10) + time[1])*60 + (time[2] * 10) + time[3];
}

boolean has_decremented() {
  unsigned int time_val = get_time_val(time);
  unsigned int old_time_val = get_time_val(old_time);
  unsigned int diff = old_time_val - time_val;
  return (diff > 0 && diff <  3* CLOCK_PERIOD / 1000000);
}


void push_time(unsigned int time) {
  Serial.println("pushing time");

  Serial.println("connecting...");
  if (client.connect("kjyoo.co",8000)) {
    char post_request[40];
    sprintf(post_request, "POST /?time=%d HTTP/1.1", time);
    Serial.println(time);
    client.println(post_request);
    client.println();
    client.stop();
    client.flush();
    Serial.println("pushed");
  }
  else {
    Serial.println("could not connect to push time");
  }
}

void push_finish() {
  Serial.println("pushing finish");

  if (client.connect("kjyoo.co",8000)) {
    client.println("POST /?stop=1 HTTP/1.1");
    client.println();
    client.stop();
    client.flush();
    Serial.println("pushed");
  }
  else {
    Serial.println("could not connect to push finish");
  }
}

void addtime() {

if (client.connect("kjyoo.co",8000)){
  client.println("GET /Added HTTP/1.1");
  client.println();
  client.stop();
  client.flush();
  Serial.println("pusdfasdfawehed");
 
}
else {
    Serial.println("could not connect to add time");
  }

}

void tick() {
  print_time();
  
  if (!(old_time[0] == 0 && old_time[1] == 0 
    && old_time[2] == 0 && old_time[3] == 0)) {   
    if(has_decremented()&&!(time[0] == 0 &&time[1] == 0 &&time[2] == 0 &&time[3] == 0)) {
      if (state == WAITING) {
        state = COOKING;
        push_time(get_time_val(time));
        
      }
      else if (state == COOKING) {
      }
    }
    else { 
      if (state == COOKING) {
        push_finish();
        state = WAITING;
      }                  
      else if (state == WAITING) {				
      }
    }
  }
  copy_time();
  
}
void print_digits() {
  Serial.print(digits[0], HEX);
  Serial.print(',');
  Serial.print(digits[1], HEX);
  Serial.print(':');
  Serial.print(digits[2], HEX);
  Serial.print(',');
  Serial.println(digits[3], HEX);
}

void print_time() {
  Serial.print(time[0], DEC);
  Serial.print(time[1], DEC);
  Serial.print(':');
  Serial.print(time[2], DEC);
  Serial.println(time[3], DEC);
}

byte convert(byte digit) {
  switch (digit) {
  case 0:
    return 0;
  case 0x1F:
    return 0;
  case 0x6:
    return 1;
  case 0x33:
    return 2;
  case 0x27:
    return 3;
  case 0x2E:
    return 4;
  case 0x2D:
    return 5;
  case 0x3D:
    return 6;
  case 0x7:
    return 7;
  case 0x3F:
    return 8;
  case 0x2F:
    return 9;
  default:
    return -1;
  }
}

void handle_digit_change() {
  digits[index] = 0;
  for (int i = 0; i <= 5; i++) {
    digits[index] |= (analogRead(i) > THRESHOLD) << i;
  }
  time[index] = convert(digits[index]);
}

ISR (PCINT2_vect) {
  if (!(PIND & pin)) {
    handle_digit_change();
    index++;
    pin <<= 1;
    if (index == 4) {
      index = 0;
      pin = START;
    }
  }
}

