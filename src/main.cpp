#include <Arduino.h>
#include <CAN.h>
#define LED_PIN 13
unsigned long previousMillis = 0;  // will store last time a CAN Message was send
const int interval = 500;

void onReceive(int packetSize);
int serial_putc(char c, struct __file*) {
    Serial.write(c);
    return c;
}

void printf_begin(void) {
    fdevopen(&serial_putc, 0);
}

void setup() {
    Serial.begin(115200);
    printf_begin();
    Serial.println("Basic Demo - Arduino-Nano-CAN");
    pinMode(LED_PIN, OUTPUT);
    if (!CAN.begin(125E3)) {
        Serial.println("Starting CAN failed!");
        while (1)
            ;
    }
    Serial.println("Starting CAN ok!");
    // add filter to only receive the CAN bus ID's we care about
    // CAN.filter(0x2);
    // register the receive callback
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
    printf(" from 0x%08X, DLC %d, Data ", (int)(CAN.packetId()), CAN.packetDlc());
    for (int i = 0; i < (int)CAN.packetDlc() && CAN.available(); i++) {
        printf("0x%02X ", CAN.read());
    }
    printf("\n");
}

int heartbeat = 0;

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));

        heartbeat++;
        if (heartbeat > 15) {
            heartbeat = 0;
        }
        CAN.beginPacket(0x1, 8);
        CAN.write(heartbeat);
        CAN.write(0x02);
        CAN.write(0x03);
        CAN.write(0x04);
        CAN.write(0x05);
        CAN.write(0x06);
        CAN.write(0x07);
        CAN.write(0x08);
        CAN.endPacket();
    }
}