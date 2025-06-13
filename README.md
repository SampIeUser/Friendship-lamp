# Friendship lamp
📡 Lamp on ESP32.


## Table of contents
 + [1. Description](#1_parag);
 + [2. Components to build](#2_parag);
 + [3. Connection scheme](#3_parag);
 + [4. Assembly](#4_parag);
 + [5. Operation modes](#5_parag);
 + [6. Demonstration](#6_parag);



## <a name="1_parag"></a>	1. Description
Lamps are connected through internet and emitting same color at the same time. Color is changed using 3 potentiometers,
and any combination of RGB can be selected.
It is required to have MQTT server, othervise lamps won't be connected. MQTT server may be random external
free service or your own. I use Mosquitto for this purpose.  

This lamp is meant to be made in several pcs. 


## <a name="2_parag"></a>	2. Components to build
- 22 AWG wires 1.5A
- Led diodes (i used white, blue and green, due to their operational)
- Adressable 5v led strip (WS2812). (due to operation voltage)
- ESP32
- Accumulator (I used [This](https://www.aliexpress.com/item/1005006116209915.html) 18650, 5V6800 XH2.54. (21mm x 42mm x 74mm))
- Resistor 470Ω
- Resistor 100Ω x3
- Switch button
- Potentiometer 5-10kΩ x3 (i used 10kΩ)
- 3D printer (or something that you will use as case)

## <a name="3_parag"></a> 3. Connection scheme

- D4 - Blue LED + 100Ω resistor
- D5 - Green LED + 100Ω resistor
- D18 - White LED + 100Ω resistor
- Potentiometer_GREEN - ESP32 (D32)
- Potentiometer_RED - ESP32 (D34)
- Potentiometer_BLUE ESP32 (D35)
- ALL Potentiometer VCC pins (+) to ESP32 (3.3v)
- Button - between ESP32 (VIN) and Accumulator (+)
- LED Strip (+) goes to Accumulator (+) (**after the button. may go to VIN**)
- LED Strip (data) goes to ESP32 D23 (**through 470Ω resistor**)
- Ground is shared between all components and meets in ESP32's GND pin. (GND and "-" things)

## <a name="4_parag"></a> 4. Assembly
Models i used are in "3D Models" folder. I used Creality Ender 3V3 SE printer.  
"fit_test.stl" Represents the size of cylinder which is needed to be inserted into the lamp. There is no "lamp" itself, because 
i used some random food container and sanded both sides to make it diffuse light. "fit_test" will help you to print/make your own.  
"Light diffuser" is used to additionaly diffuse the light and protect strip from physical damage. It can be easily removed/put on.
Must be printed with transparent filament.
<details>
  <summary>Assembly</summary>

  
  ![Assembly](https://github.com/user-attachments/assets/460c3ea1-13f2-42ba-838b-9c918188c114)
</details>

## <a name="5_parag"></a> 5. Operation modes
1. Offline mode  
  1.1 When turned on, lamp will try to connect to wifi (blue led will be lit). If it has no credentials, after 5 seconds it will turn into server and host it's own wifi.
It will be called "ESP32 lamp" and will not have password. White led will be lit. You must connect to it, go to 192.168.4.1 and enter WiFi credentials,
where lamp shall connect to. After this, you shall restart the lamp and it will connect to wifi.  
  1.2 If you don't want lamp to go online, just start use it and white led will turn off.

2. Online mode  
    2.1 After connecting to Wifi, blue led will turn off. Lamp will try to connect to MQTT server. If there is no connection between
   MQTT server and lamp, green led will be lit. If everything ok - all diodes will be off. When you turn the potentiometer, lamp will change
    it's color (this is useful when you have a high ping). After changing it's color, new setting will be sent to MQTT server. Lamp will set color received from MQTT server and notify you about it,
    by blinking with white and blue leds. If you see no blinking - color was not received from MQTT server.

## <a name="6_parag"></a> 6. Demonstration
Demonstration. First one connected to my home wifi, second is to other persons home wifi. Demonstration through video call.
<details>
  <summary>Gif 1</summary>
  
![gif 1](https://github.com/user-attachments/assets/23c93b9c-35e6-4644-9753-f5ba4a8a2034)
</details>
<details>
  <summary>Gif 2</summary>
  
 ![gif 2](https://github.com/user-attachments/assets/0199e91d-21a4-4752-ac6f-1ba4babfa3f1)
</details>

