#include <ArduinoJson.h>

#include <vector>
#include <ArduinoJson.h>
#include <SPI.h>
#include <NativeEthernet.h>

int    HTTP_PORT   = 80;
char   HOST_NAME[] = "previewsite.eu";
String PATH_NAME   = "/muondetector/api/";

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);
EthernetClient client;

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

void initEthernet() {
   // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  initInterrupts();
  initEthernet();
  Serial.println("start");

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
  delay(1000);
}


void flushMeasurements() {
  /* Convert max output buffer to json */
  const size_t capacity = JSON_ARRAY_SIZE(BUFFER_SIZE) + BUFFER_SIZE * JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);

  int nmbMeasurements = 0;
  
  for (int i=0; i < BUFFER_SIZE; i++) {
    volatile Measurement* measurement = &measurements[i];
    if (measurement->allocated) {
      /* Add measurement to http body */
      JsonObject obj = doc.createNestedObject();
      obj["micros"] = measurement->timestamp;
      obj["pin"] = measurement->pin;
      obj["sessionId"] = 1;
      
      measurement->allocated = false;
      nmbMeasurements++;
    }
  }

  if (doc.overflowed())
    Serial.println("Ouput buffer overflowed!");
     
  /* Debug output buffer */
  char strBuf[50];
  sprintf(strBuf, "Aantal metingen: %d", nmbMeasurements);
  Serial.println(strBuf);
 
  postMeasurements(doc);
}

void postMeasurements(DynamicJsonDocument &doc) {
  if (client.connect(HOST_NAME, HTTP_PORT)) {
    String json = "";
    int jsonLength = serializeJson(doc, json);
    Serial.println("Payload length: " + String(jsonLength));
    // Serial.println(json);
    // Make a HTTP request:
    client.println("POST " + PATH_NAME + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Accept: */*");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println("User-Agent: Teensy/4.1");

    client.println("Content-length: " + String(measureJson(doc)));
    client.println();
    client.println(json);
    client.flush(); 

    while(client.connected() && !client.available()) delay(1); //waits for data

    String serverData="";

    while (client.connected() || client.available()) { //connected or data available
      char c = client.read(); //gets byte from ethernet buffer
      serverData.concat(c);
      Serial.print(c); //prints byte to serial monitor
    }
    client.stop();
    Serial.println(serverData);
    if (serverData.indexOf("200 OK") > 0){
      Serial.println(F("Successful connection"));
    } else {
      Serial.println(F("Posting data failed!"));
    }
    
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}
