/*
  Ethernet Modbus TCP Server Test
  modifed from library example of base 'ArduinoModbus' library by Arduino. 
  reference: https://www.arduino.cc/reference/en/libraries/arduinomodbus/

  Charles Ihler
*/

#include <SPI.h>
#include <Ethernet.h>

#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library
#include <ArduinoModbus.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// The IP address will be dependent on your local network:
byte mac[] = {
  0x00, 0x8D, 0xFF, 0xEF, 0xFE, 0xED  //changed this
};
IPAddress ip(192, 168, 0, 5); //changed for my address
/*
//if you need this.. 
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
*/

EthernetServer ethServer(502);

ModbusTCPServer modbusTCPServer;

const int numCoils = 10;
const int numDiscreteInputs = 10;
const int numHoldingRegisters = 10;
const int numInputRegisters = 10;

const int ledPin = LED_BUILTIN;

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  Ethernet.init(5);   // MKR ETH shield   //changed this
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // Open serial communications
  Serial.begin(9600); //changed this deleted all series
  //no blocking code as you will not usually have a computer watching the serial terminal

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    //Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    //Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  ethServer.begin();
  
  // start the Modbus TCP server
  if (!modbusTCPServer.begin()) {
    //Serial.println("Failed to start Modbus TCP Server!");
    while (1);
  }

  // configure the LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(6, OUTPUT);

  // changed this.. added all supported types
  modbusTCPServer.configureCoils(0x00, numCoils);
  modbusTCPServer.configureDiscreteInputs(0x00, numDiscreteInputs);
  modbusTCPServer.configureHoldingRegisters(0x00, numHoldingRegisters);
  modbusTCPServer.configureInputRegisters(0x00, numInputRegisters);
}

void loop() {
  // listen for incoming clients
  EthernetClient client = ethServer.available();
  
  if (client) {
    // a new client connected
    //Serial.println("new client");

    // let the Modbus TCP accept the connection 
    modbusTCPServer.accept(client);

    while (client.connected()) {
      // poll for Modbus TCP requests, while client connected
      modbusTCPServer.poll();

      // update the LED
      updateLED();
      poll_regs();
    }

    //Serial.println("client disconnected");
  }
}

void poll_regs() {
  int hrValue = modbusTCPServer.holdingRegisterRead(0x00);
  int irValue = modbusTCPServer.inputRegisterRead(0x00);
  Serial.print("hr / ir: ");
  Serial.print(hrValue);
  Serial.print(" / ");
  Serial.println(irValue);
  analogWrite(6, 512);

}
void updateLED() {
  // read the current value of the coil
  int coilValue = modbusTCPServer.coilRead(0x00);

  if (coilValue) {
    // coil value set, turn LED on
    digitalWrite(ledPin, HIGH);
  } else {
    // coil value clear, turn LED off
    digitalWrite(ledPin, LOW);
  }
}

