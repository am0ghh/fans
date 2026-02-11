#include <Arduino.h>
#include <IRremote.hpp>
#include <DHT.h>

#define IR_SEND_PIN 10
#define DHT_PIN 4
#define DHT_TYPE DHT11
#define TEMP_THRESHOLD_ON 75   // Turn fans ON above this temp (°F)
#define TEMP_THRESHOLD_OFF 73  // Turn fans OFF below this temp (°F)
#define MIN_RUN_TIME 600000    // Minimum 10 minutes runtime (milliseconds)

DHT dht(DHT_PIN, DHT_TYPE);

bool fansOn = false;
unsigned long fansOnTime = 0;

void turnFansOn() {
  if (!fansOn) {
    Serial.println("Turning fans ON");
    IrSender.sendNECRaw(0xFF00DE80, 0);  // Woozoo
    delay(1000);
    IrSender.sendNECRaw(0xFE017F80, 0);  // Cosebelle
    fansOn = true;
    fansOnTime = millis();
  }
}

void turnFansOff() {
  if (fansOn) {
    unsigned long runTime = millis() - fansOnTime;
    
    // Only turn off if fans have run for at least 10 minutes
    if (runTime >= MIN_RUN_TIME) {
      Serial.println("Turning fans OFF");
      IrSender.sendNECRaw(0xFF00DE80, 0);  // Woozoo
      delay(1000);
      IrSender.sendNECRaw(0xFE017F80, 0);  // Cosebelle
      fansOn = false;
    } else {
      Serial.print("Fans still in minimum runtime (");
      Serial.print((MIN_RUN_TIME - runTime) / 60000);
      Serial.println(" min remaining)");
    }
  }
}

void setup() {
  Serial.begin(9600);
  IrSender.begin(IR_SEND_PIN);
  dht.begin();
  Serial.println("Smart Fan Controller Started");
}

void loop() {
  float temp = dht.readTemperature(true);  // true = Fahrenheit
  
  // Handle sensor errors
  if (isnan(temp)) {
    Serial.println("Error reading temperature sensor!");
    delay(2000);
    return;
  }
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("°F");
  
  // Temperature control logic with hysteresis
  if (temp >= TEMP_THRESHOLD_ON) {
    turnFansOn();
  } else if (temp <= TEMP_THRESHOLD_OFF) {
    turnFansOff();
  }
  
  delay(5000);  // Check every 5 seconds
}