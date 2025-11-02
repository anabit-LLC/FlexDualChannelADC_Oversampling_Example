/********************************************************************************************************
This example Arduino sketch is meant to work with Anabit's Flex Dual Channel ADC open source reference design

Product link: https://anabit.co/products/flex-dual-channel-adc

The Flex Dual Channel ADC design uses a single eneded 14 bit 2MSPS ADC that includes a 2 channel MUX that allows you to route signals on either input channel
to the ADC using a simple digital logic level. 

This sketch deomonstrats the concept of oversampling. It makes a single ADC measurement and then makes 16 measurements that are averaged together to show the
increased resolution benefit of oversampling. With 16 equally spaced samples being averaged together you should see a resolution increase benefit of about
2 bits. By default the Flex ADC has 14 bits of resolution and this oversampling example can push that resolution to 16 bits. See the Anabit YoutTube at
the following link to learn more about oversampling: https://youtu.be/qFzGSP7x4tA

Please report any issue with the sketch to the Anagit forum: https://anabit.co/community/forum/analog-to-digital-converters-adcs
Example code developed by Your Anabit LLC © 2025
Licensed under the Apache License, Version 2.0.
**********************************************************************************************************/

#include <SPI.h>

// Chip-select pin: default to the board's CS pin for portability.
#define CS_PIN 5
//This pin controls the ADC MUX to select input channel. This is controled by the 
//CH SEL pin on the Flex. by default the pin is pulled down selecting channel 1
//so you can leave unconnected. If you want to control the input channel connect
//a digital pin to CH SEL and define the pin number here
#define CH_SEL -1 //set to -1 if you don't want to use
#define CHAN_1 0 //digital logic low for channel 1
#define CHAN_2 1 //digital logic high for channel 2

// SPI clock for the ADC (reduce if your board/ADC wiring can't sustain this)
#define ADS_SPI_HZ 10000000UL  // 40MHz is the fastest rated speed of the ADC

// ADC/reference constants
#define VREF_VOLTAGE 4.096f   //2.5f or 3.3f, switch to correct vref on your Flex
#define ADC_BITS 16383.0f // 14-bit code range (0..16383)

// SPI mode/settings for the ADC (mode 0, MSB first)
SPISettings adsSettings(ADS_SPI_HZ, MSBFIRST, SPI_MODE0);
//set variable for what channel you want to use
const uint8_t chan = CHAN_1; 

void setup() {
  Serial.begin(115200);
  delay(2500); // time to open Serial Monitor/Plotter

  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH); //start CS pin high
  pinMode(CH_SEL, OUTPUT);
  digitalWrite(CH_SEL,chan); //select MUX channel you want to use, default is channel 1
  SPI.begin();
  Serial.println(F("Single Measurement Mode (CH0 via GPIO)"));
  readADS7945(0xC000);  // pipeline dummy read
  delayMicroseconds(5);


  while (true) {
    uint16_t raw;
    raw = readADS7945(0xC000)>>2; //read 16 bit value and shift 2 right to make 14 bit value
    float voltage = convertToVoltage(raw); //convert ADC code to voltage value
    Serial.print(F("Voltage for single reading: ")); Serial.print(voltage, 5); Serial.println(F(" V"));
    delay(20);
    float oRaw = oversampleReading(16);
    voltage = convertToFloatVoltage(oRaw); //convert ADC code to voltage value
    Serial.print(F("Voltage for 16x oversampling: ")); Serial.print(voltage, 5); Serial.println(F(" V"));
    
    Serial.println();
    delay(2000);
  }
}

void loop() {
  // not used
}

// One 16-bit framed transaction to the ADC
// will need to shift result to get correct 14 bit alignment
uint16_t readADS7945(uint16_t cmd) {
  SPI.beginTransaction(adsSettings);
  digitalWrite(CS_PIN, LOW);
  uint16_t value = SPI.transfer16(cmd);
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
  return value;
}

//This function does the oversampling measurements
//input argument is the amount of readings you want to
//average together. Default is 16.
//returns the average of the readings as a float
float oversampleReading(uint16_t oSamCnt) {
  // Prime the pipeline
  readADS7945(0xC000);
  delayMicroseconds(5);
  float sum = 0;

  noInterrupts(); //turn off interupts
  SPI.beginTransaction(adsSettings);

  for (int i = 0; i < oSamCnt; i++) {
    digitalWrite(CS_PIN, LOW);
    sum += (float)(SPI.transfer16(0xC000)>>2);
    digitalWrite(CS_PIN, HIGH);
  }
  SPI.endTransaction();
  interrupts(); //turn interupts back on
  
  return sum / (float)oSamCnt;
}

//Takes code form the ADC and converts it to a voltage value
//this function uses various constants defined earlier in the sketch
//input argument is the ADC code and returns voltage as float
float convertToVoltage(int16_t raw_code) {
    raw_code = raw_code & 0x3FFF; //apply mask to only get the 14 bits of interest
    return (((float)raw_code /ADC_BITS) * VREF_VOLTAGE);
}

//converts a float ADC code value used for averaging to a voltage
//input argument is the measured code as a float
float convertToFloatVoltage(float raw_code) {
    return ((raw_code /ADC_BITS) * VREF_VOLTAGE);
}
