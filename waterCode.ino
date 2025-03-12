// Libraries used
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>




// Define values and ports
#define ONE_WIRE_BUS 2
int relay1 = 3;
int relay2 = 4;
int relay3 = 5;
int relay4 = 6;
float desiredTemperature = 0;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define NOTE_CS5 554
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_FS5 740
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_B5 988
int tempo = 100;
int buzzer = 7;
int melody[] = {
  NOTE_E5,
  8,
  NOTE_A5,
  8,
  NOTE_A5,
  8,
  NOTE_CS5,
  8,
  NOTE_CS5,
  8,
  NOTE_CS5,
  8,
  NOTE_E5,
  8,
  NOTE_E5,
  8,
  NOTE_E5,
  -8,
  NOTE_E5,
  16,
  NOTE_B5,
  16,
  NOTE_A5,
  16,
  NOTE_GS5,
  16,
  NOTE_FS5,
  16,
  NOTE_E5,
  -8,
  NOTE_E5,
  -8,
  NOTE_A5,
  8,
  NOTE_A5,
  8,
  NOTE_CS5,
  8,
  NOTE_CS5,
  8,
  NOTE_A5,
  4,
  NOTE_E5,
  8,
  NOTE_A5,
  8,
  NOTE_GS5,
  8,
  NOTE_FS5,
  16,
  NOTE_GS5,
  16,
  NOTE_A5,
  8,
  NOTE_DS5,
  8,
  NOTE_E5,
  -4,
};
int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;
bool temperatureWithinRange = false;
float readTemperature;




// Being initializing the program
void setup(void) {
  Serial.begin(9600);
  sensors.begin();


  lcd.init();
  lcd.backlight();




  if (!sensors.getAddress(insideThermometer, 0)) {
    Serial.println("Unable to find address for Device 0");
  }




  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();




  sensors.setResolution(insideThermometer, 9);


  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC);
  Serial.println();




  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);




  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
  delay(2000);




  // Prompt user for desired temperature
  Serial.println("Enter the desired temperature (in Celsius): ");
  while (Serial.available() == 0)
    ;
  desiredTemperature = Serial.parseFloat();
}








// LCD Display printing
void printTemperature(DeviceAddress deviceAddress) {
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp is: ");
  lcd.setCursor(0, 1);
  lcd.print(tempC);
  readTemperature = tempC;
}




// Loop the motors to run
void loop(void) {
  if (temperatureWithinRange == false) {
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures();
    Serial.println("DONE");
    printTemperature(insideThermometer);
    Serial.println(readTemperature);
    delay(1000);


    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
    digitalWrite(relay3, LOW);
    digitalWrite(relay4, LOW);
    delay(3000);


    // Continuously monitor temperature until it's within 2 degree of the desired temperature
    while (!temperatureWithinRange) {
      // Request temperature again
      Serial.print("Requesting temperatures...");
      sensors.requestTemperatures();
      Serial.println("DONE");
      float currentTemperature = sensors.getTempC(insideThermometer);
      if (currentTemperature != DEVICE_DISCONNECTED_C && currentTemperature < (desiredTemperature + 2)) {
        temperatureWithinRange = true;
      }
      printTemperature(insideThermometer);
      Serial.println(readTemperature);
      delay(1000);
    }


    // At this point, the temperature is within 1 degree of the desired temperature
    // Turn off the motors and play the melody
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, HIGH);
    digitalWrite(relay4, HIGH);
    for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
      divider = melody[thisNote + 1];
      if (divider > 0) {
        noteDuration = (wholenote) / divider;
      } else if (divider < 0) {
        noteDuration = (wholenote) / abs(divider);
        noteDuration *= 1.5;
      }
      tone(buzzer, melody[thisNote], noteDuration * 0.9);
      delay(noteDuration);
      noTone(buzzer);
    }
  }
  else {
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures();
    Serial.println("DONE");
    printTemperature(insideThermometer);
    Serial.println(readTemperature);
    delay(1000);
  }
}






void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
