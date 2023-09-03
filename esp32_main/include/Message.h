/*
MIT License

Copyright (c) 2023 Lucas Yukio Fukuda Matsumoto

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#if defined(ESP32) || defined(ESP8266)
    // Serial web server
    // https://github.com/ayushsharma82/WebSerial
    #include <WebSerial.h>
#endif

#define MSG_DELAY 100

class Message{// Print to Serial and WebSerial
    public:
        void multiPrintln(){
            Serial.println();
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println();
            #endif
            delay(MSG_DELAY);
        }

        void multiPrintln(const char* msg){
            Serial.println(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrintln(long unsigned int msg){
            Serial.println(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrintln(const String& msg){
            Serial.println(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrintln(const __FlashStringHelper* msg){
            Serial.println(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrintln(const float msg){
            Serial.println(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrintln(const double msg){
            Serial.println(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrintln(double msg, int places){
            Serial.println(msg, places);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg, places);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrintln(const int msg){
            Serial.println(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrintln(const uint8_t msg){
            Serial.println(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrintln(const uint16_t msg){
            Serial.println(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrint(const char* msg){
            Serial.print(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg);
            #endif
            delay(MSG_DELAY);
        }
        
        void multiPrint(const __FlashStringHelper* msg){
            Serial.print(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrint(const float msg){
            Serial.print(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrint(const double msg){
            Serial.print(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrint(const int msg){
            Serial.print(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrint(const uint8_t msg){
            Serial.print(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrint(const uint16_t msg){
            Serial.print(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrint(long unsigned int msg){
            Serial.print(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrint(const String& msg){
            Serial.print(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrint(double msg, int places){
            Serial.print(msg, places);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg, places);
            #endif
            delay(MSG_DELAY);
        }
};