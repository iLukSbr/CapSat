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
        template<typename T> void multiPrintln(T msg){
            Serial.println(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg);
            #endif
            delay(MSG_DELAY);
        }

        template<typename T, typename U> void multiPrintln(T msg, U places){
            Serial.println(msg, places);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println(msg, places);
            #endif
            delay(MSG_DELAY);
        }

        template<typename T, typename U> void multiPrint(T msg, U places){
            Serial.print(msg, places);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg, places);
            #endif
            delay(MSG_DELAY);
        }

        template<typename T> void multiPrint(T msg){
            Serial.print(msg);
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.print(msg);
            #endif
            delay(MSG_DELAY);
        }

        void multiPrintln(){
            Serial.println();
            #if defined(ESP32) || defined(ESP8266)
                WebSerial.println();
            #endif
            delay(MSG_DELAY);
        }
};