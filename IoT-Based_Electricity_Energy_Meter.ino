#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <EmonLib.h>
#include <EEPROM.h>

#define BLYNK_TEMPLATE_ID "TMPL68w_8J8A5"
#define BLYNK_TEMPLATE_NAME "IoT  Electricity Energy Meter"
#define BLYNK_AUTH_TOKEN "2_09FKBVHH5slW5K4LVYC-RaZkpArClb"


// Wi-Fi credentials
char ssid[] = "wifi name";
char pass[] = "wifi password";


// Blynk authentication token
char auth[] = "2_09FKBVHH5slW5K4LVYC-RaZkpArClb";

//LCD parameters
LiquidCrystal_I2C LCD(0x27, 20, 4);

// Energy monitoring object
EnergyMonitor emon;

// Calibration values for voltage and current
#define voltageCalibration 83.3
#define currentCalibration 0.50


// Variables for energy tracking
float kWh = 0;
float totalCost = 0;
unsigned long lastMillis = millis();

// Cost per kilowatt-hour (in Taka)
float costPerUnit = 4.14;

// Blynk virtual pin assignments
#define VRMS_VIRTUAL_PIN V0
#define IRMS_VIRTUAL_PIN V1
#define POWER_VIRTUAL_PIN V2
#define KWH_VIRTUAL_PIN V3
#define COST_VIRTUAL_PIN V4

// Blynk timer object
BlynkTimer timer;

void myTimerEvent()
{
  // Calculate voltage and current RMS
  emon.calcVI(20, 2000);

  // Calculate energy consumption (kWh)
  kWh += (emon.apparentPower * (millis() - lastMillis) / 3600000000.0);

  // Calculate total cost
  totalCost = kWh * costPerUnit;

  // Update lastMillis
  lastMillis = millis();

  // Print values to Serial Monitor
  Serial.print("Vrms: ");
  Serial.print(emon.Vrms, 2);
  Serial.print("V");

  Serial.print("\tIrms: ");
  Serial.print(emon.Irms, 4);
  Serial.print("A");

  Serial.print("\tPower: ");
  Serial.print(emon.apparentPower, 4);
  Serial.print("W");

  Serial.print("\tKWh: ");
  Serial.print(kWh, 5);
  Serial.print("kWh");

  Serial.print("\tTotal Cost: ");
  Serial.print(totalCost, 2);
  Serial.print(" Taka");

  // Store values in EEPROM
  EEPROM.put(0, emon.Vrms);
  EEPROM.put(4, emon.Irms);
  EEPROM.put(8, emon.apparentPower);
  EEPROM.put(12, kWh);

  // Update LCD display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vrms:");
  lcd.print(emon.Vrms, 2);
  lcd.print("V");
  lcd.setCursor(0, 1);
  lcd.print("Irms:");
  lcd.print(emon.Irms, 4);
  lcd.print("A");
  lcd.setCursor(0, 2);
  lcd.print("Power:");
  lcd.print(emon.apparentPower, 4);
  lcd.print("W");
  lcd.setCursor(0, 3);
  lcd.print("kWh/Unit:");
  lcd.print(kWh, 4);
  lcd.print("kWh");

  // Send values to Blynk
  
  //Blynk.virtualWrite(V0, emon.Vrms);
  //Blynk.virtualWrite(V1, emon.Irms);
  //Blynk.virtualWrite(V2, emon.apparentPower);
  //Blynk.virtualWrite(V3, kWh);
  //Blynk.virtualWrite(V4, totalCost);
  
  
  Blynk.virtualWrite(VRMS_VIRTUAL_PIN, emon.Vrms);
  Blynk.virtualWrite(IRMS_VIRTUAL_PIN, emon.Irms);
  Blynk.virtualWrite(POWER_VIRTUAL_PIN, emon.apparentPower);
  Blynk.virtualWrite(KWH_VIRTUAL_PIN, kWh);
  Blynk.virtualWrite(COST_VIRTUAL_PIN, totalCost);
}

void setup()
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize energy monitoring
  emon.voltage(35, voltageCalibration, 1.7);  // Voltage: input pin, calibration, phase_shift
  emon.current(34, currentCalibration);       // Current: input pin, calibration.

  // Set the interval for data logging and updating display
  timer.setInterval(5000L, myTimerEvent);

  // Display startup message on LCD
  lcd.setCursor(8, 0);
  lcd.print("Smart");
  lcd.setCursor(0, 1);
  lcd.print("IoT-Based");
  lcd.setCursor(0, 2);
  lcd.print("Electricity");
  lcd.setCursor(0, 3);
  lcd.print("Energy Meter");
  
  delay(3000);
  lcd.clear();
}

void loop()
{
  Blynk.run();
  timer.run();

}
