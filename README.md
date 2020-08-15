### abdulsdiodedisaster

#### Commissioning

* ESP8266 load up with esp8266_bitbash_to_mqtt jupyter micropython notebook

* Pin13 is the input D7

* Plug in Arduinio Mega 2560, set dipswitches 1-4 up, 5-7 down, 8 down

* Compile and upload allcellsscan.ino

* Pin13 is Mega output (connecto to ESP8266 Pin13/D7)

* Connect ESP8266 VV pin to Mega VIN and Grounds together

* USB on ESP8266 then check mqtt with: mosquitto_sub -h sensorcity.io -v -t "ab/#"

* use esp8266_commissioning_mqtt_as because the file is too big without being in firmware

