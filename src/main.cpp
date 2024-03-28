#include <Arduino.h>
#include "M5AtomS3.h"
#include "M5UnitKmeterISO.h"
#include "UNIT_HBRIDGE.h"

UNIT_HBRIDGE driver;
M5UnitKmeterISO kmeter;
uint8_t error_status = 0;
long delay_time      = 0;
float temp;
int button_step = 0;
int target_temp = 20;

void setup() {
    Serial.begin(115200);
    auto cfg = M5.config();
    AtomS3.begin(cfg);
    driver.begin(&Wire, HBRIDGE_ADDR, 2, 1, 100000L); //NOTE: Update Unit H-bridge.h for the correct i2c PINs

    while (!kmeter.begin(&Wire, KMETER_DEFAULT_ADDR, 2, 1, 100000L)) {
      Serial.println("Unit KmeterISO not found");
      }

    AtomS3.Display.setTextColor(GREEN);
    //AtomS3.Display.setTextDatum(middle_center);
    AtomS3.Display.setFont(&fonts::Orbitron_Light_24);
    AtomS3.Display.setTextSize(1);
    AtomS3.Display.drawString("Click!", AtomS3.Display.width() / 2, AtomS3.Display.height() / 2);
}

void loop() {
        error_status = kmeter.getReadyStatus();
        if (error_status == 0) {
          temp = (float)(kmeter.getCelsiusTempValue())/100;
          AtomS3.Display.clear();
          AtomS3.Display.drawString(String(temp), 30, 5);
          AtomS3.Display.drawString(String(target_temp,DEC), 50, 50);
          //Serial.printf("Chip Celsius Temp: %.2fC\r\n", ((float)(kmeter.getInternalCelsiusTempValue())) / 100);
        } 
        else {
          Serial.printf("Error: %d", kmeter.getReadyStatus());
        }
        delay(500);

if (temp < (target_temp - 1)) { // hysteresis = 1 degree lower
  driver.setDriverSpeed8Bits(255); // turn ON heat
}

if (temp > (target_temp + 1)) { // hysteresis = 1 degree higher
  driver.setDriverSpeed8Bits(0); // turn OFF heat
}



        AtomS3.update();
    if (AtomS3.BtnA.wasPressed()) {
        AtomS3.Display.clear();
        AtomS3.Display.drawString("Pressed", AtomS3.Display.width() / 2,
                                  AtomS3.Display.height() / 2);
        button_step = button_step + 1;
        if (button_step == 5) 
        {
        button_step = 0; 
        target_temp = 20;                     
        }

        Serial.println("Pressed");
    }

    if (AtomS3.BtnA.wasReleased()) {
        AtomS3.Display.clear();
        //AtomS3.Display.drawString("Released", AtomS3.Display.width() / 2, AtomS3.Display.height() / 2);
        AtomS3.Display.drawString(String(target_temp,DEC), AtomS3.Display.width() / 2,
                                  AtomS3.Display.height() / 2);
    
        driver.setDriverDirection(1);
        driver.setDriverPWMFreq(100);

    if (button_step == 0) 
        {
        //driver.setDriverSpeed8Bits(0);  
        target_temp = 20;                  
        } 

    if (button_step == 1) 
        {
        //driver.setDriverSpeed8Bits(20);
        target_temp = 25;                    
        } 
    if (button_step == 2) 
        {
        //driver.setDriverSpeed8Bits(22);
        target_temp = 28;                    
        }     
    if (button_step == 3) 
        {
        //driver.setDriverSpeed8Bits(24); 
        target_temp = 30;                    
        }  
    if (button_step == 4) 
        {
        //driver.setDriverSpeed8Bits(26);
        target_temp = 32;                     
        }                                 
    
        Serial.println("Released");
    }
}