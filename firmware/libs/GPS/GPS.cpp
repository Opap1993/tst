//
//
//

#include "GPS.h"

TinyGPS gps_dev;

int gps_lat = 0, gps_lng = 0;

void GPSClass::init(Stream * serial)
{
	debug = serial;
	Serial2.begin(9600);
}

void GPSClass::sleep(){
	#if DEBUG
	debug->println("GPS Sleep");
	#endif
	Serial2.println(" ");
	delay(200);
	Serial2.println("$PMTK161,0*28");
}

void GPSClass::wakeup(){
	#if DEBUG
	debug->println("GPS Wake up ");
	#endif
	Serial2.println(" ");
	delay(200);
}

gps_position GPSClass::getPosition() {
	return position;
}

gps_position GPSClass::getGpsData(unsigned long timeout)
{
	bool newDATA = false;
	
	wakeup();

	// For {{GPS_SEARCH_TIME}} parse the GPS DATA and report some key values
	unsigned long start = millis();
	int gps_timeout = timeout;
	debug->print("============= GPS Timeout :");
	debug->println(gps_timeout);
	while (!newDATA && millis() - start < gps_timeout )
	{
		while (Serial2.available())
		{
			char c = Serial2.read();
			debug->print(c);
			if (gps_dev.encode(c)) {
				// valid sentence ?
				newDATA = true;
			}
			
		}
	}

	if (newDATA)
	{
		gps_dev.f_get_position(&lat, &lng, &age);
		gps_dev.get_datetime(&date, &time, &fix_age);
		
		debug->println("");
		
		debug->print(" LAT=");
		position.lat =  TinyGPS::GPS_INVALID_F_ANGLE == lat ? 0.0 : lat;
		debug->print(position.lat);

		debug->print(" LNG=");
		position.lng = TinyGPS::GPS_INVALID_F_ANGLE == lng ? 0.0 : lng;
		debug->println(position.lng);
		
		debug->println("");
		debug->print("Lat: ");
		debug->print(position.lat, 6);
		debug->print(" Lng: ");
		debug->println(position.lng, 6);
		
	}
	else
	{
		position.lat = 0;
		position.lng = 0;
		debug->println("No GPS Data");
	}
	sleep();
	
	return position;
}

void GPSClass::loraDecode(){

	float f_lat = 0, f_lng = 0;	
	
	f_lat = ( (float) gps_lat / 100000) - 91;
	f_lng = ( (float) gps_lng / 100000) - 181;
	
	debug->println("");
	debug->print(" Decoded Lat: ");
	debug->print(f_lat, 5);
	debug->print(" Lng: ");
	debug->println(f_lng, 5);
}

GPSClass GPS;