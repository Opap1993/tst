// Decode decodes an array of bytes into an object.
//  - fPort contains the LoRaWAN fPort number
//  - bytes is an array of bytes, e.g. [225, 230, 255, 0]
// The function must return an object, e.g. {"temperature": 22.5}
function Decode(fPort, bytes) {
    var response = {};

    var has_gps = bytes[0] & (1 << 6);
    var is_ping = bytes[0] & (1 << 5);

    if (is_ping) {
        return response;
    }

    var index = 1;

    if (has_gps) {
        response = withGPS(bytes, index, response);
        index += 8;
    }

    response = withBme(bytes, index, response);
    index+=5;

    response = withAdc(bytes, index, response);
    response = withLeafSensor(bytes, index, response);
    index+=4;

    response = withSoilSensor(bytes, index, response);
    index+=4;

    response = withBatteryVoltage(bytes, index, response);
    
    return response;
}

function withBatteryVoltage(bytes , start, response) {
    var voltage = bytes[start] / 10;
    response['v'] = voltage;

    return response;
}

function withSoilSensor(bytes , start, response) {
    
    response['soil'] = {
        'temp': toSoilTemp(bytes[start], bytes[start + 1]),
        'hum': toSoilMoisture(bytes[start + 2], bytes[start + 3]),
    }

    return response;
}

function withLeafSensor(bytes , start, response) {
    var bits_up = (bytes[start] << 8) | (bytes[start + 1]);
    var v_up = (bits_up * 0.0008)  / 3.0 * 100;

    var bits_down = (bytes[start+2] << 8) | (bytes[start + 3]);
    var v_down = (bits_down * 0.0008)  / 3.0 * 100;

    if (v_up > 100) {
        v_up = 100;
    }

    if (v_down > 100) {
        v_down = 100;
    }

    response['leaf'] = {
        'u': Number(v_up.toFixed(2)),
        'd': Number(v_down.toFixed(2))
    }

    return response;
}

function withAdc(bytes, start, response) {
    response['adc'] = {
        'c0': (bytes[start] << 8) | (bytes[start + 1]),
        'c1': (bytes[start + 2] << 8) | (bytes[start + 3]),
        'c2': (bytes[start + 4] << 8) | (bytes[start + 5]),
        'c3': (bytes[start + 6] << 8) | (bytes[start + 7]),
    };

    return response;
}

function withGPS(bytes, start, response) {
    response['gps'] = {
        lat: lat(bytes, start),
        lng: lng(bytes, start + 4)
    }
    return response;
}

function withSoil(bytes, start , response) {
    response['soil'] = {
        temp: toSoilTemp(bytes[15], bytes[16]),
        moist: toSoilMoisture(bytes[17], bytes[18])
    };

    return response
}

function withBme(bytes, start, response) {
    response['air'] = {
        temp: bytes[start] - 100,
        hum: bytes[start + 1],
        pre: ((bytes[start + 2] << 16) | (bytes[start + 3] << 8) | (bytes[start + 4])),
    };
    
    calculateDewPoint(bytes[start] - 100, bytes[start + 1], response);
    return response;
}

function lat(bytes, start) {
    var lat = ((bytes[start] << 24) | (bytes[start + 1] << 16) | (bytes[start + 2] << 8) | (bytes[start + 3]));
    lat = (lat / 100000) - 91;
    return Number(lat.toFixed(6));
}

function lng(bytes, start) {
    var lng = ((bytes[start] << 24) | (bytes[start + 1] << 16) | (bytes[start + 2] << 8) | (bytes[start + 3]));
    lng = (lng / 100000) - 181;
    return Number(lng.toFixed(6));
}

function toSoilTemp(byte0, byte1) {
    var bits = ((byte0 << 8) | (byte1));
    var voltage = bits * 0.0008;
    var temp = (voltage - 0.5) / 0.01;
    return parseFloat(temp.toFixed(2));
}

function toSoilMoisture(byte0, byte1) {
    var bits = ((byte0 << 8) | (byte1));
    var voltage = bits * 0.0008;
    var moisture = voltage / 3 * 50;
    return parseFloat(moisture.toFixed(2));
}

function calculateDewPoint(t, rh, response) {
  var a = 17.27;
  var b = 237.7;

  var T = t; 
  var RH = rh;

  var Tdp = (b*((a*T)/(b+T) + Math.log(RH/100)))/(a-((a*T)/(b+T) + Math.log(RH/100)));

  response['tdp'] = parseFloat(Tdp.toFixed(2));

  return response;

}

