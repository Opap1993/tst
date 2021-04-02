#include "libs/EnergySave/EnergySave.h"
#include "libs/GPS/GPS.h"
#include "libs/Sensors/Sensors.h"
#include "libs/Network/Network.h"
#include "libs/LoRaWan/LoRaWan.h"

void setup()
{
	SerialUSB.println("Setup");

	EnergySave.init(&SerialUSB);
	EnergySave.lowPowerPins(false);
	EnergySave.disableI2CDevices();
	EnergySave.setMCUSleepMode(onWakeUp);
	EnergySave.setAlarmDateTime(DEFAULT_ALARM_HOUR, DEFAULT_ALARM_MINUTE, DEFAULT_ALARM_SEC);
	Sensors.init(&SerialUSB, &EnergySave);
	
	LoraWan.init(&SerialUSB);
	Network.init(&SerialUSB, &LoraWan);
	Network.setup(10);
	
	if (Network.connected()) {
		Network.ping();
	}
	
	GPS.init(&SerialUSB);
	GPS.getGpsData(DEFAULT_GPS_TIMEOUT);
}

void loop()
{
	if (Network.connected()) {
		sendMeasurement();
		} else {
		Network.setup(10);
		if (Network.connected()) {
			sendMeasurement();
		}
	}
	EnergySave.setTime(0,0,0);
	EnergySave.standByMode();
}


void sendMeasurement() {
	Sensors.readAll(ADC_SAMPLES, SENSORS_WARM_UP);
	Network.transmitMeasurement(Sensors.getMeasurements(), GPS.getPosition());
}

void onWakeUp(){
	
}