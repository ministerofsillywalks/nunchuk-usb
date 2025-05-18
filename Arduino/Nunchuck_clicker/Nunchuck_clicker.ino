#include <Nunchuk.h>
#include <Wire.h>

Nunchuk _controller;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  _controller.initialize();

}

void loop() {
  // put your main code here, to run repeatedly:
  _controller.update();
  if(!_controller.c_button()) {
    Serial.print(51);
  }
  if(!_controller.z_button()) {
    Serial.print(61);
  }
  if (_controller.z_button() && _controller.c_button()){
    Serial.print(1);
  }
  delay(150);
}
