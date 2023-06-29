#include <vector>
#include <ArduinoJson.h>

volatile boolean isOnChange = false;
volatile int detectedInterrupt = -1;

struct Measurement {
  int timestamp;
  int pin;
  bool allocated = false;
};

const int BUFFER_SIZE = 5000;
volatile int measurementIndex = -1;
volatile Measurement measurements[BUFFER_SIZE];

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  initInterrupts();
  pinMode(LED_BUILTIN, OUTPUT);
}

void initInterrupts() {
  for (int pin = 0; pin < 33; pin++) {
   // pinMode(pin, INPUT_PULLUP);

    switch(pin) {
      case 1: 
        attachInterrupt(digitalPinToInterrupt(pin), onChange1, RISING);
        break;
      case 2:
        attachInterrupt(digitalPinToInterrupt(pin), onChange2, FALLING);
        break;
      case 3:
        attachInterrupt(digitalPinToInterrupt(pin), onChange3, FALLING);
        break;
      case 4:
        attachInterrupt(digitalPinToInterrupt(pin), onChange4, FALLING);
        break;
      case 5:
        attachInterrupt(digitalPinToInterrupt(pin), onChange5, FALLING);
        break;
      case 6:
        attachInterrupt(digitalPinToInterrupt(pin), onChange6, FALLING);
        break;
      case 7:
        attachInterrupt(digitalPinToInterrupt(pin), onChange7, FALLING);
        break;
      case 8:
        attachInterrupt(digitalPinToInterrupt(pin), onChange8, FALLING);
        break;
    }
  }
}

/* Assume an ISR cannot be interrupted */
void onChange(int interruptPin) {
  isOnChange = true;

  /* Loop through buffer until a free slot is found */
  int i = 0;
  int freeIndex = -1;
  do {
    if (!measurements[i].allocated) {
      freeIndex = i;
    }
    i++;
  } while (i < BUFFER_SIZE && freeIndex == -1);

  if (freeIndex != -1) {
    measurements[freeIndex].allocated = true;
    measurements[freeIndex].pin = interruptPin;
    measurements[freeIndex].timestamp = micros();    
  } else {
    Serial.println("Buffer is full, cannot log measurements");
  }
}

void onChange1() {
  onChange(1);
}
void onChange2() {
  onChange(2);
}
void onChange3() {
  onChange(3);
}
void onChange4() {
  onChange(4);
}
void onChange5() {
  onChange(5);
}
void onChange6() {
  onChange(6);
}
void onChange7() {
  onChange(7);
}
void onChange8() {
  onChange(8);
}


// the loop function runs over and over again forever
void loop() {
  if (isOnChange) {
    isOnChange = false;
    flushMeasurements();
  }
  delay(50);
}


void flushMeasurements() {
  /* Convert max output buffer to json */
  const size_t capacity = JSON_ARRAY_SIZE(BUFFER_SIZE) + BUFFER_SIZE * JSON_OBJECT_SIZE(2);
  DynamicJsonDocument doc(capacity);

  int nmbMeasurements = 0;
  for (int i=0; i < BUFFER_SIZE; i++) {
    volatile Measurement* measurement = &measurements[i];
    if (measurement->allocated) {
      /* Add measurement to http body */
      JsonObject obj = doc.createNestedObject();
      obj["timestamp"] = measurement->timestamp;
      obj["pin"] = measurement->pin;
      
      measurement->allocated = false;
      nmbMeasurements++;
    }
  }
  
  if (doc.overflowed())
    Serial.println("Ouput buffer overflowed!");
     
  /* Debug output buffer */
  char strBuf[50];
  sprintf(strBuf, "Aantal metingen: %d - ", nmbMeasurements);
  Serial.print(strBuf);
 
  serializeJson(doc, Serial);
  Serial.println("");

  
}
