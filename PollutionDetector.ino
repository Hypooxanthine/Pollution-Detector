#include "DHT.h"
#include "LiquidCrystal.h"
#include "ValueManager.hpp"

/* Debug/Release mode defines */
#define __DEBUG // Remove this to use release mode : debug logging will be disabled on serial port. Serial port uses 608 bytes (~2%) from storage space and 194 bytes (~10%) from RAM.
#ifdef __DEBUG
  #define LOG_INIT(x) Serial.begin(x)
  #define LOG(x) Serial.print(x)
  #define LOGLN(x) Serial.println(x)
#else
  #define LOG_INIT(x)  
  #define LOG(x)  
  #define LOGLN(x)  
#endif

/* Pins, versions */
#define DHT_VERSION DHT11
#define PIN_DHT 6
#define POW_DHT 7
#define PIN_GAS A0
#define POW_GAS 10
#define RS 12
#define EN 11
#define D4 5
#define D5 4
#define D6 3
#define D7 2

/* Local functions headers */
void InitValues();
void GetHumidity(const bool& isSensorPowered = false, const bool& shouldUnpowerSensor = true);
void GetTemperature(const bool& isSensorPowered = false, const bool& shouldUnpowerSensor = true);
void GetGas(const bool& isSensorPowered = false, const bool& shouldUnpowerSensor = true);

/* DHT11 and LCD screen handlers */
DHT dht = DHT(PIN_DHT, DHT_VERSION);
LiquidCrystal lcd = LiquidCrystal(RS, EN, D4, D5, D6, D7);

/* Gas, humidity and temperature value handlers (ValueManager.hpp) */
auto gas = Value<uint8_t>(lcd, "Gaz:", {8, 0}, 4, "%");
auto hum = Value<uint8_t>(lcd, "Hum:", {0, 0}, 3, "%");
auto temp = Value<float>(lcd, "Temp:", {0, 1}, 11, "C", true);

/*  */
const unsigned long SENSOR_ACTIVATION_DELAY = 200;

void setup()
{
  LOG_INIT(9600);
  
  pinMode(POW_GAS, OUTPUT);
  pinMode(POW_DHT, OUTPUT);
  pinMode(PIN_GAS, INPUT);
  
  dht.begin();
  lcd.begin(16, 2);
  CreateLCDDegree(lcd);

  gas.writeLabel();
  hum.writeLabel();
  temp.writeLabel();
  
  InitValues();
  delay(1000 * 10 - SENSOR_ACTIVATION_DELAY); // We wait 10 seconds after getting all initial values, and before getting them again, again and again.
}

void loop()
{
  LOGLN("---Début void loop()");
  
  /* Reading gas value, then printing it onto serial monitor and LCD screen. */
  GetHumidity();
  delay(1000 * 10 - SENSOR_ACTIVATION_DELAY); // Waiting 10 seconds.
  
  /* Reading temperature value, then printing it onto serial monitor and LCD screen. */
  GetGas();
  delay(1000 * 10 - SENSOR_ACTIVATION_DELAY); // Waiting 10 seconds.
  
  /* Reading humidity value, then printing it onto serial monitor and LCD screen. */
  GetTemperature();
  delay(1000 * 10 - SENSOR_ACTIVATION_DELAY); // Waiting 10 seconds.
}

void InitValues()
{
  LOGLN("---Récupération des informations une première fois avant la boucle principale.");
  
  digitalWrite(POW_GAS, HIGH);
  digitalWrite(POW_DHT, HIGH);
  delay(SENSOR_ACTIVATION_DELAY);

  GetHumidity(true, false); // DHT is POWERED, we DON'T WANT to unpower it (because we'll use it just below).
  GetTemperature(true, true); // DHT is POWERED, we WANT to unpower it.
  GetGas(true, true); // Gas sensor is POWERED, we WANT to unpower it.

  LOGLN("---Fin de la récupération initiale.\n\n");
}

void GetHumidity(const bool& isSensorPowered, const bool& shouldUnpowerSensor)
{
  if(!isSensorPowered)
  {
    digitalWrite(POW_DHT, HIGH);
    delay(SENSOR_ACTIVATION_DELAY);
  }
  
  hum.value = dht.readHumidity();
  LOG("Humidité relative : "); LOG(hum.value); LOGLN("%");
  hum.writeValue();

  if(shouldUnpowerSensor)
    digitalWrite(POW_DHT, LOW);
}

void GetTemperature(const bool& isSensorPowered, const bool& shouldUnpowerSensor)
{
  if(!isSensorPowered)
  {
    digitalWrite(POW_DHT, HIGH);
    delay(SENSOR_ACTIVATION_DELAY);
  }
  
  temp.value = dht.readTemperature();
  LOG("Température : "); LOG(temp.value); LOGLN("°C");
  temp.writeValue();

  if(shouldUnpowerSensor)
    digitalWrite(POW_DHT, LOW);
}

void GetGas(const bool& isSensorPowered, const bool& shouldUnpowerSensor)
{
  if(!isSensorPowered)
  {
    digitalWrite(POW_GAS, HIGH);
    delay(SENSOR_ACTIVATION_DELAY);
  }
  
  gas.value = analogRead(PIN_GAS) / 1023.f * 100.f;
  LOG("Gaz : "); LOG(gas.value); LOGLN("%");
  gas.writeValue();

  if(shouldUnpowerSensor)
    digitalWrite(POW_GAS, LOW);
}
