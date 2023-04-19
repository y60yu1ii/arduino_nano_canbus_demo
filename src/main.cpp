#include <Arduino.h>
#include <CAN.h>
#define LED_PIN 13
#define CAN_125KBPS 250E3
#define CAN_250KBPS 500E3
#define CAN_500KBPS 1000E3
// my arduino nano have bug so all bitrate should be doubled

unsigned long previousMillis = 0;  // will store last time a CAN Message was send
const int interval = 100;

void onReceive(int packetSize);
int serial_putc(char c, struct __file *) {
    Serial.write(c);
    return c;
}

void printf_begin(void) {
    fdevopen(&serial_putc, 0);
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
    printf_begin();
    Serial.println("Basic Demo - Arduino-Nano-CAN");
    if (!CAN.begin(CAN_125KBPS)) {
        Serial.println("Starting CAN failed!");
        while (1)
            ;
    }
    Serial.println("Starting CAN ok!");
    // // add filter to only receive the CAN bus ID's we care about
    // // CAN.filter(0x2);
    // // register the receive callback
    CAN.onReceive(onReceive);
}

void onReceive(int packetSize) {
    if (CAN.packetExtended()) {
        printf("New extended frame");
    } else {
        printf("New standard frame");
    }

    if (CAN.packetRtr()) {
        // Remote transmission request, packet contains no data
        printf("RTR ");
    }

    printf(" from 0x%08lx, DLC %d, Data ", CAN.packetId(), CAN.packetDlc());  // %08lx , never use 08X for long int
    for (int i = 0; i < CAN.packetDlc() && CAN.available(); i++) {
        printf("0x%02X ", CAN.read());
    }
    printf("\n");
}

int heartbeat = 0;
unsigned long currentMillis;

void loop() {
    currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        // printf(">>> ");

        heartbeat++;
        if (heartbeat > 15) {
            heartbeat = 0;
        }
        // CAN.beginPacket(0x1, 8);
        CAN.beginExtendedPacket(0x1, 8);
        CAN.write(heartbeat);
        CAN.write(0x01);
        CAN.write(0x01);
        CAN.write(0x01);
        CAN.write(0x01);
        CAN.write(0x01);
        CAN.write(0x01);
        CAN.write(0x01);
        CAN.endPacket();

        CAN.beginExtendedPacket(0x3, 8);
        CAN.write(heartbeat);
        CAN.write(0x03);
        CAN.write(0x03);
        CAN.write(0x03);
        CAN.write(0x03);
        CAN.write(0x03);
        CAN.write(0x03);
        CAN.write(0x03);
        CAN.endPacket();

        CAN.beginExtendedPacket(0x4, 8);
        CAN.write(heartbeat);
        CAN.write(0x04);
        CAN.write(0x04);
        CAN.write(0x04);
        CAN.write(0x04);
        CAN.write(0x04);
        CAN.write(0x04);
        CAN.write(0x04);
        CAN.endPacket();

        // digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
}