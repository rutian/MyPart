#include <RFduinoGZLL.h>

device_t role = HOST;

struct sensor_data{
  int small;
  int large;
  float humidity;
  float temperature;
};

struct sensor_data packet;

void setup() {
  // start the GZLL stack  
  RFduinoGZLL.begin(role);
  Serial.begin(9600);

  
}

void loop() {
//  Serial.println("am i alive?");
//  delay(1000);
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
    
    
    if (device == DEVICE0){
       memcpy(&packet, &data[0], len);
    }

    int d = (int) device;

    Serial.write((byte*) d, 4);
    Serial.write((byte*) packet.small, 4);
    Serial.write((byte*) packet.large, 4);
    Serial.write((byte*) &packet.humidity, 4);
    Serial.write((byte*) &packet.temperature, 4);

//    Serial.println(packet.small);
//    Serial.println(packet.large); 
//    Serial.println(packet.humidity);
//    Serial.println(packet.temperature);
//    Serial.println("---");

    RFduinoGZLL.sendToDevice(device, "OK");

}

    
