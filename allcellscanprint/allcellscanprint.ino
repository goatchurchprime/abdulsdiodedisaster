// Select board Arduino Mega 2560
// dipswitches 1-4 up, 5-7 down, 8 down

int outputpins[10] = {49, 47, 45, 43, 41, 39, 37, 35, 33, 31};
int inputpins[10] = {48, 46, 44, 42, 40, 38, 36, 34, 32, 30};
int switchvalues[100]; 

Stream* pserialdata = NULL; //&Serial3; 
Stream& serialdebug = Serial; 
int datapin = 13; 

void setup() {
  Serial.begin(115200); 
  Serial3.begin(115200); 
  pinMode(datapin, OUTPUT); 
  digitalWrite(datapin, 1); 

  for (int j = 0; j < 10; j++) {
      pinMode(outputpins[j], OUTPUT);
      digitalWrite(outputpins[j], LOW); 
  }
  for (int i = 0; i < 10; i++) 
      pinMode(inputpins[i], INPUT);
  delay(1000);
  for (int i = 0; i < 100; i++)
    switchvalues[i] = 0; 
}


int steadyframecount = 0; 
void loop()
{
  int nchanges = 0; 
  for (int j = 0; j < 10; j++) {
    digitalWrite(outputpins[j], HIGH); 
    delay(5); 
    for (int i = 0; i < 10; i++) {
        pinMode(inputpins[i], OUTPUT);
        digitalWrite(inputpins[i], 0);
        pinMode(inputpins[i], INPUT);
        int v = !(digitalRead(inputpins[i])); 
        if (switchvalues[i*10+j] != v)
          nchanges++; 
        switchvalues[i*10+j] = v; 
    }
    digitalWrite(outputpins[j], LOW); 
  }

  int count = 0; 
  for (int i = 0; i < 100; i++) {
    if (switchvalues[i] == 1)
      count++; 
  }
  

  if (nchanges) {
    int c = 0; 
    serialdebug.print(count); 
    serialdebug.print(": "); 
    for (int i = 0; i < 100; i++) {
      if (switchvalues[i]) {
        if (c != 0)
          serialdebug.print(","); 
        serialdebug.print(i); 
        c++; 
      }
    }
    serialdebug.println(""); 
  }

  if (pserialdata != NULL) {
    if (nchanges || ((steadyframecount % 10) == 9)) {
      pserialdata->print(count); 
      pserialdata->write(": "); 
      delay(1);  // allow the esp8266 uart buffer to keep up
      for (int i = 0; i < 100; i++) {
        pserialdata->write(switchvalues[i] ? "1" : "0");  
        delay(1);  // allow the esp8266 uart buffer to keep up
      }
      pserialdata->write("\n"); 
      pserialdata->flush(); 
      delay(20); // give time to process on other side anyway
    } else {
      delay(100); 
    }
  }

  // bitbash morse code of 100bits
  if (datapin != 0) {
    if (nchanges || ((steadyframecount % 10) == 9)) {
      digitalWrite(datapin, 0); 
      delay(50); 
      digitalWrite(datapin, 1); 
      delay(3); 
      digitalWrite(datapin, 0); 
      delay(5); 
      for (int i = 0; i < 100; i++) {
        digitalWrite(datapin, 1); 
        delay(switchvalues[i] ? 3 : 1); 
        digitalWrite(datapin, 0); 
        delay(2); 
      }
      digitalWrite(datapin, 1); 
    }
  }

  if (nchanges) 
    steadyframecount = 0; 
  else
    steadyframecount++; 
}
