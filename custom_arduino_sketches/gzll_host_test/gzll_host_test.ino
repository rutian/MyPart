#include <RFduinoGZLL.h>

device_t role = HOST;

struct sensor_data{
  int small;
  int large;
  float humidity;
  float temperature;
};
 
void setup() {
  // start the GZLL stack  
  RFduinoGZLL.begin(role);
  Serial.begin(9600);
}

void loop() {
}

void RFduinoGZLL_onReceive(device_t device, int rssi, char *data, int len) {
    struct sensor_data packet;
    
    if (device == DEVICE0){
       memcpy(&packet, &data[0], len);
    }

    int d = (int) device;

    Serial.write((byte*) &d, 4);
    Serial.write((byte*) &packet.small, 4);
    Serial.write((byte*) &packet.large, 4);
    Serial.write((byte*) &packet.humidity, 4);
    Serial.write((byte*) &packet.temperature, 4);

}

    
