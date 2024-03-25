/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 *
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 *
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 *
 * @license Released into the public domain.
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

#define redLed_pin 4
#define greenLed_pin 2
#define echoPin 7
#define trigPin 5

const int enterCode[] = { 113, 124, 95, 8 };
long duration;
int distance; 

LiquidCrystal_I2C lcd(0x3F, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID 
byte nuidPICC[4];

Servo lacatUsa;

void setup() {

    Serial.begin(9600);
    SPI.begin(); // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522 

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    pinMode(6, OUTPUT);
    pinMode(redLed_pin, OUTPUT);
    pinMode(greenLed_pin, OUTPUT);
    digitalWrite(redLed_pin, HIGH);

    lacatUsa.attach(3);

    Serial.println("Initializare...");
    lacatUsa.write(50);
    
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
    Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed

    lcd.init();
    lcd.backlight();
    lcd.clear();------------------------------------------------------
    lcd.setCursor(4,0);
    lcd.print("Hackster");

}

void loop() {
    
    // Clears the trigPin condition
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Displays the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance <=30){
          lcd.clear();
          lcd.print("Bine ati venit!");
          lcd.setCursor(0,1);
          lcd.print("Prezentati cardul va rog!");

        if (!rfid.PICC_IsNewCardPresent()) {
            return;
        }
        
        
        if (!rfid.PICC_ReadCardSerial())
            return;
    
        
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
       
    
        // Check is the PICC of Classic MIFARE type
        if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
            piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
            piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
            Serial.println(F("Your tag is not of type MIFARE Classic."));
            return;
        }
    
        
        Serial.println(F("Card detectat"));
        int enter = 0;
        
        for (byte i = 0; i < 4; i++) {
            nuidPICC[i] = rfid.uid.uidByte[i];
            
            if (nuidPICC[i] == enterCode[i]) {
                enter++;
                Serial.println(nuidPICC[i]);
            }
        }
        if (enter == 4) {
            
            Serial.println("Acces permis");
            lcd.clear();
            lcd.print("Acces permis!");
            lcd.setCursor(0,1);
            lcd.print("Va uram o zi buna!");
            openDoor();
            delay(400);
        }
        else {
            lcd.clear();
            lcd.print("Acces respins!");
            lcd.setCursor(0,1);
            lcd.print("Hai pa!");
            Serial.println("Acces nepermis");
            delay(1000);
        }
        Serial.println();
        //Serial.println("Apropiati cardul pentru citire");
        
    
        // Halt PICC
        rfid.PICC_HaltA();
    
        // Stop encryption on PCD
        rfid.PCD_StopCrypto1();
        }
        if (distance >30)
        lcd.clear();
}


/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void printHex(byte* buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte* buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
}

void openDoor() {
    digitalWrite(redLed_pin, LOW);
    digitalWrite(greenLed_pin, HIGH);
    lacatUsa.write(100);
    tone(6, 1000);
    delay(2000);
    noTone(6);
    digitalWrite(redLed_pin, HIGH);
    digitalWrite(greenLed_pin, LOW);
    lacatUsa.write(10);
}
