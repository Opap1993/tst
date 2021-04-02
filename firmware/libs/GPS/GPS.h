// GPS.h

#ifndef _GPS_h
#define _GPS_h

#include "../EnergySave/EnergySave.h"
#include "../TinyGPS/TinyGPS.h"

#include "../../Parameters.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class GPSClass
{
	protected:
	Stream *debug;
	gps_position position;
	
	public:
	void init(Stream *serial);
	void sleep();
	void wakeup();
	gps_position getPosition();
	gps_position getGpsData(unsigned long timeout);
	void loraDecode();
	char c;
	unsigned long age, fix_age, time, date, speed, course;
	float lat, lng;
};

void displayInfo();
extern GPSClass GPS;
extern int gps_lat;
extern int gps_lng;

#endif

