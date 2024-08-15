
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <WiFi.h>


#define API_KEY ""
#define END_POINT ""


#define IP_ADDRESS "192.168.196.97"
#define PORT_NUMBER 8090


#define SSID_WIFI "test"
#define PASSWORD_WIFI "00000000"


WiFiClient ourClient;





PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);




void setup()
{    
    Serial.begin(115200);
    Serial.println("-------Peer to Peer HCE--------");
    
    nfc.begin();
    
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (! versiondata) {
      Serial.print("Didn't find PN53x board");
      while (1); // halt
    }
    
    // Got ok data, print it out!
    Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
    Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
    Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
    
    // Set the max number of retry attempts to read from a card
    // This prevents us from waiting forever for a card, which is
    // the default behaviour of the PN532.
    //nfc.setPassiveActivationRetries(0xFF);
    
    // configure board to read RFID tags

    nfc.SAMConfig();


    WiFi.begin(SSID_WIFI, PASSWORD_WIFI);

    Serial.print("connecting to wifi ");

    while(WiFi.status() != WL_CONNECTED) {
       Serial.print(".");
       delay(500);
    }


    Serial.println(WiFi.localIP());


    if(!ourClient.connect(IP_ADDRESS, PORT_NUMBER)) {
      Serial.println("We Are Not Connected !!! Fuck...");
      for(;;);
    }else {
      Serial.println("The Mother fucker Accept Our Connection...");
      for(;;);
    
    }

    // http request logic

    
}

void loop()
{
  bool success;
  
  uint8_t responseLength = 32;
  
  Serial.println("Waiting for an ISO14443A card");
  
  // set shield to inListPassiveTarget
  success = nfc.inListPassiveTarget();

  if(success) {
   
     Serial.println("Found something!");
                  
    uint8_t selectApdu[] = { 0x00, /* CLA */
                              0xA4, /* INS */
                              0x04, /* P1  */
                              0x00, /* P2  */
                              0x07, /* Length of AID  */
                              0xF5, 0xF3, 0xF1, 0xF6, 0xFA, 0xFE, 0xFB, /* AID defined on Android App */
                              0x00  /* Le  */ };
                              
    uint8_t response[32];  
     
    success = nfc.inDataExchange(selectApdu, sizeof(selectApdu), response, &responseLength);
    
    if(success) {
      
      //Serial.print("responseLength: "); Serial.println(responseLength);
       
      //nfc.PrintHexChar(response, responseLength);
      printResponse(response, responseLength);

      String bufferString;
      String id;
      
      for(int i = 0; i < responseLength; i++) {
        bufferString += String((char)response[i]); 
      }


      bufferString.replace("CLIENT_ID:", "");


      if(bufferString == "-1") {
        Serial.println("We Don't Know This Guy");
      }
      
      if(bufferString == "100") {
        Serial.println("We Know This Guy");
      }

      
      Serial.println(bufferString);
      

      

  
      
//      do {
//        uint8_t apdu[] = {0x01, 0x05, 0x09, 0x00};
//        uint8_t back[32];
//        uint8_t length = 32; 
//
//        success = nfc.inDataExchange(apdu, sizeof(apdu), back, &length);
//        
//        if(success) {
//          Serial.print("responseLength: "); Serial.println(length);
//          nfc.PrintHexChar(back, length);
//        }
//        else {
//          Serial.println("Broken connection?"); 
//        }
//      }
//      while(success);
    }

    delay(200);
  }
  else {
   
    Serial.println("Didn't find anything!");
  }

  delay(200);
}

void printResponse(uint8_t *response, uint8_t responseLength) {
  
   String respBuffer;

    for (int i = 0; i < responseLength; i++) {
      
      if (response[i] < 0x10) 
        respBuffer = respBuffer + "0"; //Adds leading zeros if hex value is smaller than 0x10
      
      respBuffer = respBuffer + String(response[i], HEX) + " ";                        
    }

    Serial.print("response: "); Serial.println(respBuffer);
}

void setupNFC() {
 
  nfc.begin();
    
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig(); 
}
