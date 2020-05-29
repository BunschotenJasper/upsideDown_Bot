// include these libraries to connect to the shiftr service
#include <WiFiNINA.h> 
#include <MQTT.h>
#include <MQTTClient.h>

// Setup LED strip
#include <Adafruit_NeoPixel.h> // import LED strip library
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN 4 // Pin LED strip (I used pin 4)

//// Setup wifi credentials at location 1
const char WIFI_SSID1[] = "..."; // WiFI ssid 1
const char WIFI_PASS1[] = "..."; //WiFI password 1

//// Setup wifi credentials at location 2
const char WIFI_SSID2[] = "..."; // WiFI ssid 2
const char WIFI_PASS2[] = "..."; //WiFI password 2

// Broker key and secret of shiftr.io
const char mqttServer[] = "broker.shiftr.io"; // broker, with shiftr.io it's "broker.shiftr.io"
const int mqttServerPort = 1883; // broker mqtt port
const char key[] = "..."; // broker key (find this on shiftr)
const char secret[] = "..."; // broker secret (find this on shiftr)
const char device[] = "..."; // broker device (this name will be visible on shiftr)

// setting up Wifi/MQTT
int status = WL_IDLE_STATUS;
WiFiClient net;
MQTTClient client;
unsigned long lastMillis = 0;

////////Assigning pins and setting up variables//////////

// setting buttonPin and variable for ButtonState
const int ldrPin = A1; // LDR is connected to this pin on the Arduino (I used Pin A1)
int ldrValue; // new variable to store value of the LDR
int ldrState = 0; // variable that helps to turn LDR into a switch
int lastLdrState = 0; // variable that helps to turn LDR into a switch
int ldrThreshold = 100; // the sensitivity of the LDR sensor

// settings LEDstrip
int numLEDS = 5; // the amount of LEDs used on the LED strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLEDS, PIN, NEO_GRB + NEO_KHZ800); // setting up the LED strip

// variables for color value and number of ledpixel
int ledPixel;
int redValue;
int greenValue;
int blueValue;

// variable used for cycling through internet connections
int connectionCounter = 0;

/////// Connect to the internet and mqttServer //////////

void connect() {
  Serial.print("checking wifi...");
  while ( status != WL_CONNECTED) { // if not connected to wifi
    strip.setPixelColor(4, strip.Color(0, 0, 255)); // set LED 4 on Blue
    strip.show();
    if (connectionCounter == 0){
      Serial.print(".");
      status = WiFi.begin(WIFI_SSID1, WIFI_PASS1); // provide SSID and password of wifi
    }
    if (connectionCounter == 1){
      Serial.print("|");
      status = WiFi.begin(WIFI_SSID2, WIFI_PASS2); // provide SSID and password of wifi
    }
    connectionCounter = !connectionCounter; // connection counter is switching to opposite
    delay(100);
  }
  Serial.println("\nconnected to WiFi!\n");

  client.begin(mqttServer, mqttServerPort, net); // check the server

  Serial.println("connecting to broker...");
  while (!client.connect(device, key, secret)) { // check the broker of shiftr.io
    Serial.print(".");
    delay(1000);
  }

  Serial.println("Connected to MQTT");

  client.onMessage(messageReceived); // when there is a message, execute void(messageReceived)

  client.subscribe("/button1");  // Subscribe to button 1 - will be received in void(messageReceived)
  client.subscribe("/button2");  // Subscribe to button 2 - will be received in void(messageReceived)
  client.subscribe("/button3");  // Subscribe to button 3 - will be received in void(messageReceived)
  client.subscribe("/button4");  // Subscribe to button 4 - will be received in void(messageReceived)
}

///////// This setup runs one time in the beginning //////////

void setup() {
  Serial.begin(115200); // to see serial monitor

  strip.begin(); // begin LEDStrip
  strip.setBrightness(30); // set brightness LEDStrip
  strip.show(); // show LEDStrip

  pinMode(ldrPin, INPUT); // set ldrPin
  connect();  // setup void connect()

}

/////// Functions for setting LEDs to right state and color //////////


void setLedLight(int ledPixel, int redValue, int greenValue, int blueValue) {
    strip.setPixelColor(ledPixel, strip.Color(redValue, greenValue, blueValue)); // set LED
    Serial.println(ledPixel);
    Serial.println("-");
    strip.show();
    delay(50);   
}

//////////// Receive Messages ////////////

void messageReceived(String &topic, String &payload) {
  Serial.print("incoming: ");
  Serial.print(topic); // Print sender of signal
  Serial.print(" - ");
  if (topic == "/button1") { // received from button1
    ledPixel = 0; // select right LED from strip
    if (payload == "1") { // if signal is 1
      redValue = 255;
      blueValue = 0;
      greenValue = 0;
      Serial.println("ON");
    }
    else if (payload == "0") { // if signal is 0
      redValue = 0;
      blueValue = 0;
      greenValue = 0;
      Serial.println("OFF");
    }
  }
  if (topic == "/button2") { // received from button2
    ledPixel = 1; // select right LED from strip
    if (payload == "1") { // if signal is 1
      redValue = 255;
      blueValue = 0;
      greenValue = 0;
      Serial.println("ON");
    }
    else if (payload == "0") { // if signal is 0
      redValue = 0;
      blueValue = 0;
      greenValue = 0;
      Serial.println("OFF");
    }
  }
  if (topic == "/button3") { // received from button3 (in this case your own device)
    ledPixel = 2; // select right LED from strip
    if (payload == "1") { // if signal is 1
      redValue = 255;
      blueValue = 0;
      greenValue = 0;
      Serial.println("ON");
    }
    else if (payload == "0") { // if signal is 0
      redValue = 0;
      blueValue = 0;
      greenValue = 0;
      Serial.println("OFF");
    }
  }
  if (topic == "/button4") { // received from button4
    ledPixel = 3; // select right LED from strip
    if (payload == "1") { // if signal is 1
      redValue = 255;
      blueValue = 0;
      greenValue = 0;
      Serial.println("ON");
    }
    else if (payload == "0") { // if signal is 0
      redValue = 0;
      blueValue = 0;
      greenValue = 0;
      Serial.println("OFF");
    }
  }
  setLedLight(ledPixel, redValue, blueValue, greenValue); // take new values and send to right LED on the LED strip
}

/////////// Sending Messages /////////////

void loop() {
  client.loop(); // loop through WiFi/MQTT client

  if (!net.connected()) { // if not connected, then run connect function
    connect();
  }

  int ldrValue = analogRead(ldrPin); //analogRead the value of the LDR

  if (ldrValue > ldrThreshold) { // is the value of the ldr high or low? Adjust the number to make the sensor more or less sensitive
    ldrState = 1;
  } else {
    ldrState = 0;
  }

  if (millis() - lastMillis > 1000) {
    lastMillis = millis(); // equal to

    if (ldrState != lastLdrState) { // if state of LDR changed
        if (ldrState == 1) { // if ldrState is high
          client.publish("/button3", "1"); // client /button2 send value "1"
          Serial.println("1");
        } else { // if ldrState is low
          client.publish("/button3", "0"); // client /button2 send value "0"
          Serial.println("0");
        }
        delay(50); // wait for a moment
      lastLdrState = ldrState; // equal to
    }


    // Switch LED off internet LED when internet is connected
    if (status == WL_CONNECTED) {
      strip.setPixelColor(4, strip.Color(0, 0, 0)); // set LED
      strip.show();
    }

    lastMillis = millis(); // equal to
  }

}
