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
//    Serial.print(device);
//    Serial.print("\n");
    struct sensor_data packet;
    if (device == DEVICE0){
       memcpy(&packet, &data[0], len);
    }

    Serial.write(device);
    Serial.write(packet.small);
    Serial.write(packet.large);
    Serial.write(packet.humidity);
    Serial.write(packet.temperature);

    
//    Serial.print(packet.small);
////    Serial.print("\n");
//    Serial.print(packet.large);
////    Serial.print("\n");
//    Serial.print(packet.humidity);
////    Serial.print("\n");
//    Serial.print(packet.temperature);
////    Serial.print("\n");
}

    
