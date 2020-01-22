int outputpins[10] = {49, 47, 45, 43, 41, 39, 37, 35, 33, 31};
int inputpins[10] = {48, 46, 44, 42, 40, 38, 36, 34, 32, 30};
int switchvalues[100]; 

Stream& serialdata = Serial3; 
Stream& serialdebug = Serial; 

void setup() {
  Serial.begin(115200); 
  Serial3.begin(115200); 
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
  if (nchanges || ((steadyframecount % 10) == 9)) {
    serialdata.print(count); 
    serialdata.write(": "); 
    delay(1);  // allow the esp8266 uart buffer to keep up
    for (int i = 0; i < 100; i++) {
      serialdata.write(switchvalues[i] ? "1" : "0");  
      delay(1);  // allow the esp8266 uart buffer to keep up
    }
    serialdata.write("\n"); 
    serialdata.flush(); 
    delay(20); // give time to process on other side anyway
  } else {
    delay(100); 
  }

  if (nchanges) 
    steadyframecount = 0; 
  else
    steadyframecount++; 
}
