// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp32-hal-ledc.h"

// Enable LED FLASH setting
#define CONFIG_LED_ILLUMINATOR_ENABLED 1

// LED FLASH setup
#if CONFIG_LED_ILLUMINATOR_ENABLED
    #define LED_LEDC_CHANNEL 2 //Using different ledc channel/timer than camera
    #define CONFIG_LED_MAX_INTENSITY 0// Altered from 255 to 0
#endif

int led_duty = 0;

#if CONFIG_LED_ILLUMINATOR_ENABLED
    void enable_led(bool en){// Turn LED On or Off
        int duty = en ? led_duty : 0;
        if (en && (led_duty > CONFIG_LED_MAX_INTENSITY))
            duty = CONFIG_LED_MAX_INTENSITY;
        ledcWrite(LED_LEDC_CHANNEL, duty);
    }
#endif

void setupLedFlash(int pin){
    #if CONFIG_LED_ILLUMINATOR_ENABLED
        ledcSetup(LED_LEDC_CHANNEL, 5000, 8);
        ledcAttachPin(pin, LED_LEDC_CHANNEL);
    #endif
}