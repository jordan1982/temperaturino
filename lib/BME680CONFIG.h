#ifndef BME680CONFIG_h
#define BME680CONFIG_h

#include "Arduino.h"
#include "bsec.h"
#include <EEPROM.h>
#define STATE_SAVE_PERIOD  UINT32_C(360 * 60 * 1000) // 360 minutes - 4 times a day

typedef struct
{
    float rawTemperature;
	float pressure;
	float rawHumidity;
	float gasResistance;
	float iaq;
	float iaqAccuracy;
	float temperature;
	float humidity;
	float staticIaq;
	float co2Equivalent;
	float breathVocEquivalent;
}
bmeValues;


class BME680CONFIG
{
   private:
		// Create an object of the class Bsec
		Bsec iaqSensor;
		uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
		uint16_t stateUpdateCounter = 0;
		String output;


   public:
      void setupBME680(void);
      void checkIaqSensorStatus(void);
	  void errLeds(void);
	  void loadState(void);
	  void updateState(void);
	  bmeValues readAndSendValues(void);
};

#endif