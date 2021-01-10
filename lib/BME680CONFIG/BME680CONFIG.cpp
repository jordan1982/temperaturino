/*
  LEDLIB.cpp - Libreria di esempio per gestire
  l'accensione, lo spegnimento, e il lampeggio
  di un LED.
*/

#include "BME680CONFIG.h"  // dichiarazione della classe

const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
};

/* funzione di inizializzazione */
void BME680CONFIG::setupBME680(void){
  // Sensor setup
  EEPROM.begin(BSEC_MAX_STATE_BLOB_SIZE + 1); // 1st address for the length
  Wire.begin();
  iaqSensor.begin(BME680_I2C_ADDR_SECONDARY, Wire); 
  checkIaqSensorStatus();
  iaqSensor.setConfig(bsec_config_iaq);  
  loadState();
   
  bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };
  


  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();

}

bmeValues BME680CONFIG::getValues(void){
	 bmeValues bmeValues;
	  	
	if (iaqSensor.run()) {   

	  bmeValues.iaqAccuracy = iaqSensor.iaqAccuracy;
      bmeValues.temperature= iaqSensor.temperature;

	  bmeValues.humidity = iaqSensor.humidity;
      bmeValues.gasResistance= iaqSensor.gasResistance;

	  bmeValues.pressure = iaqSensor.pressure;
      bmeValues.iaq= iaqSensor.iaq;

      bmeValues.staticIaq = iaqSensor.staticIaq;
      bmeValues.co2Equivalent= iaqSensor.co2Equivalent;
      bmeValues.breathVocEquivalent = iaqSensor.breathVocEquivalent;
      updateState();
	  unsigned long now = millis();
	  output = String(now);
      output += ", " + String(bmeValues.rawTemperature);
      output += ", " + String(bmeValues.pressure);
      output += ", " + String(bmeValues.rawHumidity);
      output += ", " + String(bmeValues.gasResistance);
      output += ", " + String(bmeValues.iaq);
      output += ", " + String(bmeValues.iaqAccuracy);
      output += ", " + String(bmeValues.temperature);
      output += ", " + String(bmeValues.humidity);
      output += ", " + String(bmeValues.staticIaq);
      output += ", " + String(bmeValues.co2Equivalent);
      output += ", " + String(bmeValues.breathVocEquivalent);
	  Serial.println(output);
	  
    }else{
      checkIaqSensorStatus();
	  
    }
	return bmeValues;
}

/* funzione di accensione del led */
void BME680CONFIG::checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);      
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

/* funzione di spegnimento del led */
void BME680CONFIG::errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

void BME680CONFIG::loadState(void)
{
  if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE) {
    // Existing state in EEPROM
    Serial.println("Reading state from EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++) {
      bsecState[i] = EEPROM.read(i + 1);
      Serial.println(bsecState[i], HEX);
    }

    iaqSensor.setState(bsecState);
    checkIaqSensorStatus();
  } else {
    // Erase the EEPROM with zeroes
    Serial.println("Erasing EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
      EEPROM.write(i, 0);

    EEPROM.commit();
  }
}

void BME680CONFIG::updateState(void)
{
  bool update = false;
  /* Set a trigger to save the state. Here, the state is saved every STATE_SAVE_PERIOD with the first state being saved once the algorithm achieves full calibration, i.e. iaqAccuracy = 3 */
  if (stateUpdateCounter == 0) {
    if (iaqSensor.iaqAccuracy >= 3) {
      update = true;
      stateUpdateCounter++;
    }
  } else {
    /* Update every STATE_SAVE_PERIOD milliseconds */
    if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis()) {
      update = true;
      stateUpdateCounter++;
    }
  }

  if (update) {
    iaqSensor.getState(bsecState);
    checkIaqSensorStatus();

    Serial.println("Writing state to EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE ; i++) {
      EEPROM.write(i + 1, bsecState[i]);
      Serial.println(bsecState[i], HEX);
    }

    EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
    EEPROM.commit();
  }
}
