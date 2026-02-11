#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ControllerVolume.h"
#include "msg.h"
#include "logo.h"
#include "pinassign.h"

// handle timeout
const unsigned long TIMEOUT = 3000;
unsigned long lastPing = 0;

// ESP now connection
uint8_t serverMac[6] = { 0xDC, 0xB4, 0xD9, 0x06, 0xCA, 0x48 };

// volume
ControllerVolume stick_x_right;
ControllerVolume stick_y_right;

ControllerVolume stick_x_left;
ControllerVolume stick_y_left;

ControllerVolume arm_pitch_right;
ControllerVolume arm_roll_right;
ControllerVolume hand_right;

ControllerVolume arm_pitch_left;
ControllerVolume arm_roll_left;
ControllerVolume hand_left;
void init_volume();

// OLED setting
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET, 400000UL, 100000UL);

void write_LOGO(){
    display.clearDisplay();
    display.drawBitmap(
        (128 - 58)/2, 0,
        epd_bitmap_LOGO,
        58, 64,
        SSD1306_WHITE
    );
    display.display();
}

// ESP-NOW callbacks
void onReceive(const uint8_t *mac_addr,
               const uint8_t *data,
               int len)
{
    char msg[16];
    int n = min(len, (int)sizeof(msg) - 1);
    memcpy(msg, data, n);
    msg[n] = '\0';

    if (millis() - lastPing > TIMEOUT){
        write_LOGO();
    }
    if (strcmp(msg, "ping") == 0) {
        lastPing = millis();
        Serial.println("ping received");
        neopixelWrite(RGB_BUILTIN, 0, 0, 255);
    }
}

// this program don't use sended msg
void onSend(const uint8_t *mac_addr, esp_now_send_status_t status){}

// ============================================================
void setup() {
    Serial.begin(115200);
    neopixelWrite(RGB_BUILTIN, 255, 0, 0);

    // OLED setup
    Wire.begin(SDA, SCL);
    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS, false, false);
    display.clearDisplay();
    display.display();
    display.setTextSize(1);              // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner
    display.cp437(true);                 // Use full 256 char 'Code Page 437' font
    display.write("Eglantyne controller initializing...");
    display.display();

    // ADC setup
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    init_volume();
    // switch setup
    pinMode(STICK_SW_RIGHT, INPUT);
    pinMode(STICK_SW_LEFT, INPUT);
    pinMode(BUTTON_A_RIGHT, INPUT);
    pinMode(BUTTON_B_RIGHT, INPUT);
    pinMode(BUTTON_C_RIGHT, INPUT);
    pinMode(BUTTON_A_LEFT, INPUT);
    pinMode(BUTTON_B_LEFT, INPUT);
    pinMode(BUTTON_C_LEFT, INPUT);

    // ESP-NOW setup
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW init failed");
        while (1);
    }

    esp_now_register_recv_cb(onReceive);
    esp_now_register_send_cb(onSend);

    esp_now_peer_info_t peer{};
    memcpy(peer.peer_addr, serverMac, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);

    Serial.println("ESP-NOW client ready");
    write_LOGO();
}

void loop() {
    // timeout handling（Wi-Fi版と同等）
    if (millis() - lastPing > TIMEOUT) {
        neopixelWrite(RGB_BUILTIN, 255, 0, 0);
        display.clearDisplay();
        display.write("Connection timeout");
        display.display();
    }

    ControlPacket pkt = {};
    pkt.button_right[0] = digitalRead(BUTTON_A_RIGHT);
    pkt.button_right[1] = digitalRead(BUTTON_B_RIGHT);
    pkt.button_right[2] = digitalRead(BUTTON_C_RIGHT);

    pkt.stick_right[0] = stick_x_right.read();
    pkt.stick_right[1] = stick_y_right.read();
    pkt.stick_right[2] = digitalRead(STICK_SW_RIGHT);

    pkt.arm_right[0] = arm_pitch_right.read();
    pkt.arm_right[1] = arm_roll_right.read();
    pkt.arm_right[2] = hand_right.read();

    pkt.button_left[0] = digitalRead(BUTTON_A_LEFT);
    pkt.button_left[1] = digitalRead(BUTTON_B_LEFT);
    pkt.button_left[2] = digitalRead(BUTTON_C_LEFT);

    pkt.stick_left[0] = stick_x_left.read();
    pkt.stick_left[1] = stick_y_left.read();
    pkt.stick_left[2] = digitalRead(STICK_SW_LEFT);

    pkt.arm_left[0] = arm_pitch_left.read();
    pkt.arm_left[1] = arm_roll_left.read();
    pkt.arm_left[2] = hand_left.read();

    Serial.println("controller data:");
    Serial.print(" right button: ");
    Serial.print(pkt.button_right[0]); Serial.print(", ");
    Serial.print(pkt.button_right[1]); Serial.print(", ");
    Serial.print(pkt.button_right[2]); Serial.print(" | ");
    Serial.print(" stick: ");
    Serial.print(pkt.stick_right[0]); Serial.print(", ");
    Serial.print(pkt.stick_right[1]); Serial.print(", ");
    Serial.print(pkt.stick_right[2]); Serial.print(" | ");
    Serial.print(" arm: ");
    Serial.print(pkt.arm_right[0]); Serial.print(", ");
    Serial.print(pkt.arm_right[1]); Serial.print(", ");
    Serial.print(pkt.arm_right[2]); Serial.println(" ");
    Serial.print(" left button: ");
    Serial.print(pkt.button_left[0]); Serial.print(", ");
    Serial.print(pkt.button_left[1]); Serial.print(", ");
    Serial.print(pkt.button_left[2]); Serial.print(" | ");
    Serial.print(" stick: ");
    Serial.print(pkt.stick_left[0]); Serial.print(", ");
    Serial.print(pkt.stick_left[1]); Serial.print(", ");
    Serial.print(pkt.stick_left[2]); Serial.print(" | ");
    Serial.print(" arm: ");
    Serial.print(pkt.arm_left[0]); Serial.print(", ");
    Serial.print(pkt.arm_left[1]); Serial.print(", ");
    Serial.print(pkt.arm_left[2]); Serial.println(" ");

    // send pkt
    esp_now_send(serverMac,
             (uint8_t *)&pkt,
             sizeof(pkt));

    delay(10);
}

void init_volume() {
    float volume_max_abs = 300 * 3.14 / 180; // about 300 deg
    
    stick_x_right.init(STICK_X_RIGHT, FORWARD, 1976, 0.0, 2.0);
    stick_y_right.init(STICK_Y_RIGHT, REVERSE, 2017, 0.0, 2.0);

    stick_x_left.init(STICK_X_LEFT, FORWARD, 1971, 0.0, 2.0);
    stick_y_left.init(STICK_Y_LEFT, REVERSE, 1959, 0.0, 2.0);

    arm_pitch_right.init(ARM_PITCH_RIGHT, REVERSE, 3055, 0.0, volume_max_abs);
    arm_roll_right.init(ARM_ROLL_RIGHT, FORWARD, 1717, 0.0, volume_max_abs);
    hand_right.init(HAND_RIGHT, FORWARD, 1898, 0.0, volume_max_abs);

    arm_pitch_left.init(ARM_PITCH_LEFT, FORWARD, 1261, 0.0, volume_max_abs);
    arm_roll_left.init(ARM_ROLL_LEFT, REVERSE, 1853, 0.0, volume_max_abs);
    hand_left.init(HAND_LEFT, REVERSE, 3663, 0.0, volume_max_abs);
}
