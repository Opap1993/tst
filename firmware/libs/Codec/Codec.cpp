//
//
//

#include "Codec.h"

void CodecClass::init(Stream *serial) {
	debug = serial;
}

void CodecClass::setMeasurements(sensors_measurements sensors_m) {
	measurements = sensors_m;
}

void CodecClass::setPosition(gps_position gposition) {
	position = gposition;
}

uint8_t CodecClass::encodeAdcChannel(byte *payload, uint8_t start, int index) {
	payload[start] = (measurements.adc[index] >> 8) & 0xFF;
	payload[start + 1] = measurements.adc[index] & 0xFF;
	
	return 2;
}
uint8_t CodecClass::encodeBatteryVoltage(byte *payload, uint8_t start) {
	int voltage = round(measurements.battery * 10);
	
	payload[start] = voltage & 0xFF;
	
	return 1;
}

uint8_t CodecClass::encodeChannels(byte *payload, uint8_t start){
	
	for (int i = 0; i <= 3; i++) {
		encodeAdcChannel(payload, start, i);
		start += 2;
	}
	
	return 8;
}

uint8_t CodecClass::encodePosition(byte *payload, uint8_t start) {
	
	int lat = round((position.lat + 91) * 100000);
	int lng = round((position.lng + 181) * 100000);
	
	debug->print("Lat:");
	debug->println(lat);
	
	debug->print("Lng:");
	debug->println(lng);
	
	payload[start] = (lat >> 24) & 0xFF;
	payload[start+1] = (lat >> 16) & 0xFF;
	payload[start+2] = (lat >> 8) & 0xFF;
	payload[start+3] = (lat & 0XFF);

	payload[start+4] = (lng >> 24) & 0xFF;
	payload[start+5] = (lng >> 16) & 0xFF;
	payload[start+6] = (lng >> 8) & 0xFF;
	payload[start+7] = (lng & 0XFF);
	
	return 8;
}

uint8_t CodecClass::encodeBME(byte *payload, uint8_t start){
	
	uint8_t  temp = round(measurements.bme.temperature) + 100;
	uint8_t  hum = round(measurements.bme.humidity);
	int pressure = round(measurements.bme.pressure);
	
	payload[start] = (temp & 0XFF);

	payload[start+1] = (hum & 0XFF);

	payload[start+2] = (pressure >> 16) & 0xFF;
	payload[start+3] = (pressure >> 8) & 0xFF;
	payload[start+4] = (pressure & 0XFF);
	
	return 5;
}

uint8_t CodecClass::encodePrefix(byte *payload, encoding_type encoding) {
	payload[0] = 0;
	
	if(encoding == MEASUREMENT) {
		payload[0] |= 1 << 7;
	}
	else if (encoding == WITHGPS) {
		payload[0] |= 1 << 6;
		} else if (encoding == PING) {
		payload[0] |= 1 << 5;
	}
	
	return 1;
}

uint8_t CodecClass::encode(byte *payload, encoding_type encoding, int size){
	uint8_t payload_size = 0;
	
	payload_size += encodePrefix(payload, encoding);
	
	if(encoding == WITHGPS) {
		payload_size += encodePosition(payload, payload_size);
	}
	
	if (encoding != PING) {
		payload_size += encodeBME(payload, payload_size);
		payload_size += encodeChannels(payload, payload_size);
		payload_size += encodeBatteryVoltage(payload, payload_size);
	}
	
	return payload_size;
}

CodecClass Codec;