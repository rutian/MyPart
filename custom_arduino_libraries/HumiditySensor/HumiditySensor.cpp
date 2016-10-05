#include "HumiditySensor.h"


// ----- Globals -----

const byte sensor_address = B01000000;
const byte temperature_register = 0;
const byte humidity_register = 1;
const byte acquisition_parameter_register = 2;

//software reset bit normal/reset 0/1; 
//reserved; heater off/on 0/1; 
//mode t&h/t|h 0/1; 
//battery >2.8/<2.8 0/1 read only; 
//temp resolution 14/11 0/1; 
//humidity resolution 14/11/8 00/01/10; 
const byte parameter_byte_1 = B00000000;
//all bits reserved must be 0
const byte parameter_byte_2 = B00000000;


//------- Functions -------


HumiditySensor::HumiditySensor(){}

HumiditySensor::~HumiditySensor(){}


void HumiditySensor::setup(){
  Wire.begin();

  //set acquisition parameters
  Wire.beginTransmission(sensor_address);
  //tell it which register by writing to pointer register
  Wire.write(acquisition_parameter_register);
  //write to that register
  Wire.write(parameter_byte_1);
  Wire.write(parameter_byte_2);
  Wire.endTransmission();

  Serial.begin(9600);
  while (!Serial);
  Serial.println("\n Humidity Sensor Initiated");
}

//returns degrees celsius (see data sheet)
float temperatureConversion(long input) {
	float temp = input / pow(2, 16);
	temp = temp * 165 - 40;
	return temp;
}

//returns percent relative humidity (see data sheet)
float humidityConversion(long input) {
	float hum = input / pow(2, 16);
	hum = hum * 100;
	return hum;
}

//combine byte outputs into an int
long combine_bytes(byte a, byte b) {
 	long raw = (long) a;
	raw = raw << 8;
	raw = raw | b;
	return raw;
}

long getReading(byte register_addr) {
	Wire.beginTransmission(sensor_address);
		Serial.print("\n Sending command to sensor at ");
		Serial.print(sensor_address, HEX);
		Serial.print("\n");

	Wire.write(register_addr);
	Wire.endTransmission();

	//wait for it to get the reading
	delay(10);

	//2 bytes per reading
	Wire.requestFrom((int) sensor_address, 2);
	
	if (Wire.available() != 2) {
		Serial.println("Error reading sensor; too few bytes returned.");
	} else {
	 	byte result_1 = Wire.read(); 
	 	byte result_2 = Wire.read(); 

	 	long raw_reading = combine_bytes(result_1, result_2);
	 	return raw_reading;
	 }
	 return -1;
}

float HumiditySensor::getTemperature(){
	long raw_temp = getReading(temperature_register);

	if (raw_temp == -1) {
		Serial.println("Error reading temperature");
		return -1;
	}

 	//convert to degrees celsius
 	float temperature = temperatureConversion(raw_temp);

	    Serial.print("Temperature: ");  
	    Serial.print(temperature);        
	    Serial.print("\n");

	return temperature;
}


float HumiditySensor::getHumidity(){
	long raw_hum = getReading(humidity_register);

	if (raw_hum == -1) {
		Serial.println("Error reading temperature");
		return -1;
	}

 	//convert to degrees celsius
 	float humidity = humidityConversion(raw_hum);

	    Serial.print("Humidity: ");  
	    Serial.print(humidity);        
	    Serial.print("\n");

	return humidity;
}




