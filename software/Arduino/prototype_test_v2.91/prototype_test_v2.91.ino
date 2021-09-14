/*
Last Updated: 2 Sept. 2021
By Heal, https://www.hak4kidz.com 
Also available on Twitter @healwhans @DSchwartzberg @Hak4Kidz for inquiries.

------------------------------------------------------------  
PROJECT:
------------------------------------------------------------  
GrrCON 0xA badge for Arduino version 1.8.13, 2020.06.16
*/
#define VERSION "2.91"

/*
------------------------------------------------------------
LICENSE:
------------------------------------------------------------
Distributed under GNU General Public License v3.0
*/

#include <SPI.h>;
#include <Wire.h>;
#include <avr/interrupt.h>;
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// Instantiate LIS3DH using I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();
#define CTRL_REG1 0x20
#define CTRL_REG4 0x23

// Setting up an array for the LED pins
byte LEDS[] = { 2, 3, 4, 5 };
const byte LED_COUNT = sizeof(LEDS);
#define DELAY 100
#define SLEEP __asm__ __volatile__("sleep")       // in line assembler to go to sleep

// Defined for anyone looking to use MOSI MISO & SCK
#define WIRE Wire
#define MOSI 11
#define MISO 12
#define SCK 13

// Adjust this number for the sensitivity of the 'click' force
// this strongly depend on the range! for 16G, try 5-10
// for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80
#define CLICKTHRESHHOLD 45
#define TIMELIMT 10
#define TIMELATENCY 20
#define TIMEWINDOW 255

// Used to control putting the badge to sleep
int sTRACKER = 0;

void setup() {
  //Wire.begin(0x19);   // For possible future use
  Serial.begin(9600);
  while (!Serial) delay(1000);    // wait for the slow serial port to open

  // Low power related setups in this section
  // Thank you Kevin Darrah at KD Circuits for your hard work
  // https://www.tindie.com/stores/kdcircuits/

  // Save Power by writing unnessecary digital IO to LOW
  for (int i = 0; i < 18; i++) {
    pinMode(i, OUTPUT);
  }
  attachInterrupt(0, digitalInterrupt, FALLING);    // interupt for waking up

  // Disable ADC
  ADCSRA &= ~(1<<ADEN);
  Serial.println("ADC disabled");

   // BOD DISABLE - this must be called right before the __asm__ sleep instruction
      MCUCR |= (1<<BODS) | (1<<BODSE);        // set both BODS and BODSE at the same time
      MCUCR &= ~(1<<BODSE);                   // then set the BODS bit and clear the BODSE bit at the same time
  Serial.println("BOD disabled");

  // Enable SLEEP mode
  SMCR |= (1<<2);                       // power down mode
  SMCR |= 1;                            // enable sleep
  Serial.println("Sleep mode enabled");

  WDTSetup();                           // who let the dog out?

  // End of low power setup

  intro();
  lisTest();
  // Run through the LED patterns on start up
  topL();
  botL();
  botR();
  topR();
  delay(DELAY*3);
  eyes();
  eyes();
  delay(DELAY*5);
  topL();
  botL();
  botR();
  topR();
  
  // 0 = turn off click detection & interrupt
  // 1 = single click only interrupt output
  // 2 = double click only interrupt output, detect single click
  // Adjust threshhold, higher numbers are less sensitive
  lis.setClick(2, CLICKTHRESHHOLD, TIMELIMT, TIMELATENCY, TIMEWINDOW);
  delay(DELAY);
}

void loop() {
  // Check with the accelerometer for motion to blink a rando pattern.
  // Blink for X amount of times, then go to sleep.
  // Sleep until double-tapped awake.
  
  uint8_t click = lis.getClick();
  
  // Check for a double double tap to enable Morse feature
  if (click & 0x20) 
  {
    morseCode();
  }

  delay(DELAY*10);            // slows down the LIS3DH event reader to about once per second
  if (sTRACKER < 101) {      // check if sleep tracker has done this 19 times
     lis.read();             // get X Y and Z data at once, but we don't care about Z.
     getEvent();
     
     } else {
      
      Serial.println("Taking a nap");
      sTRACKER = 0;                           // reset the value of the sleep tracker

      SMCR |= 1;                              // enable sleep
      for (int s = 0; s < 10; s++) {
         SLEEP;                               // Put the baby down
         if (click & 0x20) {
          ledTest();
          break;
         }
      } 
      SMCR |= 0;                              // disable sleep
      Serial.println("Woke from nap");
  }
}

// Using I2C connection for LIS3DH
void lisTest() {
  Serial.println("LIS3DH test...");

  if (! lis.begin(0x19)) {        // change this to 0x18 for alternative SPI address
    Serial.println("LIS3DH couldnt start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");  
  lisSetup();
}

// Set up the LIS3DH options
void lisSetup() {
  lis.setRange(LIS3DH_RANGE_2_G);  // options are 2G, 4G, 8G, or 16G
  Serial.print("Range = "); Serial.print(2 << lis.getRange());
  Serial.println("G");

  lis.setDataRate(LIS3DH_DATARATE_50_HZ);
  Serial.print("Data rate set to: ");
  switch (lis.getDataRate()) {
    case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
    case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
    case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
    case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
    case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
    case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
    case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;
    case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
    case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
    case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
  }
}

// While sleeping, did someone tap my head twice?
boolean wakeOnDTap() {
  uint8_t click = lis.getClick();
  if (click & 0x20); 
  return true;
}

// Check for LIS3DH single tap event
// Saving function for possible future use
boolean sTapCheck() {
  uint8_t click = lis.getClick();

  if (click == 0) return false;
  if (! (click & 0x30)) return false;
  Serial.print("Click detected (0x"); Serial.print(click, HEX); Serial.print("): ");
  if (click & 0x10) Serial.println(" single click");
  return true;
}

// Check for LIS3DH double tap event
// Saving function for possible future use
boolean dTapCheck() {
  uint8_t click = lis.getClick();

  if (click == 0) return false;
  if (! (click & 0x30)) return false;
  if (click & 0x20) Serial.print("Double Click detected (0x"); Serial.print(click, HEX); Serial.print("): ");
  return true;
}

void ledTest() {
  // Basic LED check
  for (int i = 0; i < LED_COUNT; i++) {
    for (int j = 0; j < LED_COUNT; j++) {
      if (i != j) {
        pinMode(LEDS[i], OUTPUT);
        pinMode(LEDS[j], OUTPUT);
        digitalWrite(LEDS[i], LOW);
        digitalWrite(LEDS[j], HIGH);
        delay(DELAY);
        pinMode(LEDS[i], INPUT);
        pinMode(LEDS[j], INPUT);
      }
    }
  }
}

void getEvent() {
  // Then print out the raw data
     Serial.print("\tX:  "); Serial.print(lis.x);
     Serial.print("  \tY:  "); Serial.print(lis.y);
     
     /* Or....get a new sensor event, normalized */
     sensors_event_t event;
     lis.getEvent(&event);

     /* Display the results (acceleration is measured in m/s^2) */
     Serial.print("\t\tX: "); Serial.print(event.acceleration.x);
     if (event.acceleration.x > 1) randomLED();
     Serial.print(" \tY: "); Serial.print(event.acceleration.y);
     if (event.acceleration.y > 4) randomLED();
     Serial.print("  m/s^2 "); Serial.print("\tSleep Tracker: "); Serial.println(sTRACKER);
     Serial.println();
}

/*
 *  Low power related functions in this section
 */


void WDTSetup() {
  // Setup WatchDog Timer
  Serial.println("WDT setup");
  
  // For future reference 
  //WDP3 - WDP2 - WPD1 - WDP0 - time
  // 0      0      0      0      16 ms
  // 0      0      0      1      32 ms
  // 0      0      1      0      64 ms
  // 0      0      1      1      0.125 s
  // 0      1      0      0      0.25 s
  // 0      1      0      1      0.5 s
  // 0      1      1      0      1.0 s
  // 0      1      1      1      2.0 s
  // 1      0      0      0      4.0 s
  // 1      0      0      1      8.0 s   <-- using this one!

  // Clear the WDT reset flag;
  bitClear(MCUSR, WDRF);

  // These guys need to be set at the same time or it won't work
  // WD Change Enable (WDCE) to clear WD by setting the WDCE 
  // and Enable WD by setting the WDE bit
  WDTCSR |= (1 << WDCE) | (1 << WDE);

  // Set new WDT value to 8 seconds (WDP3 and WDP0 to 1) and 
  // enable interrupts instead of reset (WDIE to 1)
  WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
  Serial.println("WDT enabled");
}

void digitalInterrupt() {
  // Needed for the digital input interrupt
  WDTCSR |= (1<<6);     // enable interrupt mode
}

ISR(WDT_vect){
  // Needed for the Watch Dog Timer.  This is called after a
  // watch dog timer timeout - this is the interrupt function called after waking up
  SMCR |= 1;            // enable sleep
}

/*
 *   End of low power functions
 */


// Section of functions for LED patterns
void eyes()
{
  // D8
  dit();
      
  // D13
  dah();  
}

void topL()
{
  // D12
  pinMode(LEDS[0], OUTPUT);
  pinMode(LEDS[2], OUTPUT);
  digitalWrite(LEDS[0], LOW);
  digitalWrite(LEDS[2], HIGH);
  delay(DELAY);
  pinMode(LEDS[0], INPUT);
  pinMode(LEDS[2], INPUT);

  // D15
  pinMode(LEDS[0], OUTPUT);
  pinMode(LEDS[3], OUTPUT);
  digitalWrite(LEDS[0], LOW);
  digitalWrite(LEDS[3], HIGH);
  delay(DELAY);
  pinMode(LEDS[0], INPUT);
  pinMode(LEDS[3], INPUT);

  // D16
  pinMode(LEDS[1], OUTPUT);
  pinMode(LEDS[3], OUTPUT);
  digitalWrite(LEDS[1], LOW);
  digitalWrite(LEDS[3], HIGH);
  delay(DELAY);
  pinMode(LEDS[1], INPUT);
  pinMode(LEDS[3], INPUT);
}

void topR()
{
  // D11
  pinMode(LEDS[0], OUTPUT);
  pinMode(LEDS[1], OUTPUT);
  digitalWrite(LEDS[0], LOW);
  digitalWrite(LEDS[1], HIGH);
  delay(DELAY);
  pinMode(LEDS[0], INPUT);
  pinMode(LEDS[1], INPUT);

  // D6
  pinMode(LEDS[1], OUTPUT);
  pinMode(LEDS[0], OUTPUT);
  digitalWrite(LEDS[1], LOW);
  digitalWrite(LEDS[0], HIGH);
  delay(DELAY);
  pinMode(LEDS[1], INPUT);
  pinMode(LEDS[0], INPUT);

  // D7
  pinMode(LEDS[2], OUTPUT);
  pinMode(LEDS[0], OUTPUT);
  digitalWrite(LEDS[2], LOW);
  digitalWrite(LEDS[0], HIGH);
  delay(DELAY);
  pinMode(LEDS[2], INPUT);
  pinMode(LEDS[0], INPUT);
}

void botL()
{
  // D17
  pinMode(LEDS[2], OUTPUT);
  pinMode(LEDS[3], OUTPUT);
  digitalWrite(LEDS[2], LOW);
  digitalWrite(LEDS[3], HIGH);
  delay(DELAY);
  pinMode(LEDS[2], INPUT);
  pinMode(LEDS[3], INPUT);

  // D14
  pinMode(LEDS[3], OUTPUT);
  pinMode(LEDS[2], OUTPUT);
  digitalWrite(LEDS[3], LOW);
  digitalWrite(LEDS[2], HIGH);
  delay(DELAY);
  pinMode(LEDS[3], INPUT);
  pinMode(LEDS[2], INPUT);
}

void botR()
{
  // D9
  pinMode(LEDS[2], OUTPUT);
  pinMode(LEDS[1], OUTPUT);
  digitalWrite(LEDS[2], LOW);
  digitalWrite(LEDS[1], HIGH);
  delay(DELAY);
  pinMode(LEDS[2], INPUT);
  pinMode(LEDS[1], INPUT);

  // D10
  pinMode(LEDS[3], OUTPUT);
  pinMode(LEDS[1], OUTPUT);
  digitalWrite(LEDS[3], LOW);
  digitalWrite(LEDS[1], HIGH);
  delay(DELAY);
  pinMode(LEDS[3], INPUT);
  pinMode(LEDS[1], INPUT);
}

void top()
{
  topL();
  topR();
}

void bot()
{
  botL();
  botR();
}

void randomLED()
{
  int index;
  int delayTime;

  // The random() function will return a semi-random number each
  // time it is called. See http://arduino.cc/en/Reference/Random
  // for tips on how to make random() even more random.

  index = random(7);              // pick a random number between 0 and 6
  
  switch(index) {
    case 0: top(); Serial.println("\nTop"); break;
    case 1: bot(); Serial.println("\nBot"); break;
    case 2: eyes(); Serial.println("\neyes"); break;
    case 3: bot(); top(); Serial.println("\nBT"); break;
    case 4: top(); bot(); Serial.println("\nTB"); break; 
    case 5: top(); eyes(); bot(); Serial.println("\nTEB"); break;
    case 6: Serial.println("\nmessage"); delay(DELAY*5); g(); delay(DELAY*10); r(); delay(DELAY*10); r(); delay(DELAY*10); c(); delay(DELAY*10); o(); delay(DELAY*10); n(); delay(DELAY*20); break;
  }
  sTRACKER++;
}

/*
 * Serial out badge intro information
 */

void intro() {
  delay(2000);
  Serial.println("\n\n\n\n\n\n\n\n\n\n");
  Serial.println("                         EggDropX and P1nknightmare");
  Serial.println("                                  presents");
  Serial.println("                                 GrrCON 0xA");
  Serial.println("\n");
  Serial.println("                         Design and created by Heal");
  Serial.println("                                 Version " VERSION);
  Serial.println("                          https://www.hak4kidz.com");
  Serial.println("                    https://github.com/Hak4Kidz/grrcon0xa");
  Serial.println("                        Follow @Hak4Kidz for badge updates.");
  Serial.println("\n\n\n\n\n\n\n\n\n\n");
}

/*
 * Morse Code Functions Section
 */

boolean morseCode() {
  
  Serial.println("\nEntering the Morse loop");

  // LED pattern to indicate Morse feature is activated
  eyes(); delay(DELAY); eyes(); delay(DELAY); eyes();
  
  unsigned long StartTime = millis();
  Serial.print("Start time: "); Serial.println(StartTime);
  unsigned long ElapsedTime = 0;
  
  do {
     uint8_t click = 0;     
     
     unsigned long CurrentTime = 0;
     delay(DELAY);
          
     // Check for a single or double tap and then display the LEDs
     click = lis.getClick();
     click = click & 0x30;

     switch (click) {
       case 0: break;
       case 0x10:
         dit();
         ElapsedTime = 0;
         // uncomment for debugging
         Serial.print("Dit detected (0x"); Serial.print(click, HEX); Serial.println(")");
         break;
       case 0x20:
         dah();
         ElapsedTime = 0;
         // uncomment for debugging
         Serial.print("Dah detected (0x"); Serial.print(click, HEX); Serial.println(")");
         break;
       default:
         break;  
     }

    /* if (click == 0) continue;
     if (! (click & 0x30)) continue;

     // Single tap (click) check 0x10 or B00010000
     // CLICK_SRC SCLICK register
     if (click & 0x10) {
      // uncomment for debugging
      Serial.print("Dit detected (0x"); Serial.print(click, HEX); Serial.println(")");
      dit();
      ElapsedTime = 0;
      continue;
     }
     // Double tap (click) check 0x20 or B00100000
     // CLICK_SRC DCLICK register
     if (click & 0x20) {
      // uncomment for debugging
      Serial.print("Dah detected (0x"); Serial.print(click, HEX); Serial.println(")");
      dah();
      ElapsedTime = 0;
      continue;
     }
     */
     
     // Check time after last sensor reading. 
     // Calc ElapsedTime, check if ElapsedTime is less than 1 minute
     CurrentTime = millis();
     ElapsedTime = CurrentTime - StartTime;
  } while(ElapsedTime < 60000);
  
  // LED pattern to indicate Morse feature is deactivated
  top(); top(); delay(DELAY); bot(); bot();
  sTRACKER = 0;
  Serial.println("Exiting from Morse loop\n");
}

// dit and dah for Morse
void dit()
{
  // D8
  pinMode(LEDS[3], OUTPUT);
  pinMode(LEDS[0], OUTPUT);
  digitalWrite(LEDS[3], LOW);
  digitalWrite(LEDS[0], HIGH);
  delay(DELAY);
  pinMode(LEDS[3], INPUT);
  pinMode(LEDS[0], INPUT);
  delay(DELAY);
}

void dah()
{
  // D13
  pinMode(LEDS[1], OUTPUT);
  pinMode(LEDS[2], OUTPUT);
  digitalWrite(LEDS[1], LOW);
  digitalWrite(LEDS[2], HIGH);
  delay(DELAY);
  pinMode(LEDS[1], INPUT);
  pinMode(LEDS[2], INPUT);
  delay(DELAY);
}

// the Morse code alphabet
void a(){
  dit();
  dah();
}

void b(){
  dah();
  dit();
  dit();
  dit();
}

void c(){
  dah();
  dit();
  dah();
  dit();
}

void d(){
  dah();
  dit();
  dit();
}

void e(){
  dit();
}

void f(){
  dit();
  dit();
  dah();
  dit();
}

void g(){
  dah();
  dah();
  dit();
}

void h(){
  dit();
  dit();
  dit();
  dit();
}

void i(){
  dit();
  dit();
}

void j(){
  dit();
  dah();
  dah();
  dah();
}

void k(){
  dah();
  dit();
  dah();
}

void l(){
  dit();
  dah();
  dit();
  dit();
}

void m(){
  dah();
  dah();
}

void n(){
  dah();
  dit();
}

void o(){
  dah();
  dah();
  dah();
}

void p(){
  dit();
  dah();
  dah();
  dit();
}

void q(){
  dah();
  dah();
  dit();
  dah();
}

void r(){
  dit();
  dah();
  dit();
}

void s(){
  dit();
  dit();
  dit();
}

void t(){
  dah();
}

void u(){
  dit();
  dit();
  dah();
}

void v(){
  dit();
  dit();
  dit();
  dah();
}

void w(){
  dit();
  dah();
  dah();
}

void x(){
  dah();
  dit();
  dit();
  dah();
}

void y(){
  dah();
  dit();
  dah();
  dah();
}

void z(){
  dah();
  dah();
  dit();
  dit();
}

void n1(){
  dit();
  dah();
  dah();
  dah();
  dah();
}

void n2(){
  dit();
  dit();
  dah();
  dah();
  dah();
}

void n3(){
  dit();
  dit();
  dit();
  dah();
  dah();
}

void n4(){
  dit();
  dit();
  dit();
  dit();
  dah();
}

void n5(){
  dit();
  dit();
  dit();
  dit();
  dit();
}

void n6(){
  dah();
  dit();
  dit();
  dit();
  dit();
}

void n7(){
  dah();
  dah();
  dit();
  dit();
  dit();
}

void n8(){
  dah();
  dah();
  dah();
  dit();
  dit();
}

void n9(){
  dah();
  dah();
  dah();
  dah();
  dit();
}

void n0(){
  dah();
  dah();
  dah();
  dah();
  dah();
}
