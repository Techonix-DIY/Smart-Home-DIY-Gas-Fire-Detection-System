#include <SoftwareSerial.h>

/* ---------- SIM800 ---------- */
#define SIM_RX D5   // TX SIM800
#define SIM_TX D6   // RX SIM800

#define PHONE_NUMBER "XXXXXX"   //phone number

SoftwareSerial sim800(SIM_RX, SIM_TX);

/* ---------- Sensors ---------- */
#define GAS_SENSOR   D1   // Active LOW
#define FIRE_SENSOR  D0   // Active LOW

/* ---------- Relays ---------- */
#define RELAY_GAS    D8
#define RELAY_FIRE   D2

/* ---------- States ---------- */
bool gasAlarm  = false;
bool fireAlarm = false;

/* ---------- Functions ---------- */
void sendAT(const char *cmd) {
  sim800.println(cmd);
  delay(800);
}


void sendSMS(const char *text) {

  sim800.println("AT+CSMP=17,167,0,0");
  delay(500);

  sim800.print("AT+CMGS=\"");
  sim800.print(PHONE_NUMBER);
  sim800.println("\"");
  delay(1500);

  sim800.println(text);
  delay(300);

  sim800.write(26); // Ctrl+Z
  delay(4000);
}

/* ---------- Setup ---------- */
void setup() {
  pinMode(GAS_SENSOR, INPUT);
  pinMode(FIRE_SENSOR, INPUT);

  pinMode(RELAY_GAS, OUTPUT);
  pinMode(RELAY_FIRE, OUTPUT);

  digitalWrite(RELAY_GAS, LOW);
  digitalWrite(RELAY_FIRE, LOW);

  Serial.begin(115200);
  sim800.begin(9600);

  delay(5000);

  sendAT("AT");
  sendAT("ATE0");

  sendAT("AT+CMGF=1");
  sendAT("AT+CSCS=\"GSM\"");
  
  Serial.println("System Ready");
  
}

/* ---------- Loop ---------- */
void loop() {

  bool gasActive  = (digitalRead(GAS_SENSOR)  == LOW); // Active LOW
  bool fireActive = (digitalRead(FIRE_SENSOR) == LOW); // Active LOW

  /* ===== GAS SENSOR ===== */
  if (gasActive && !gasAlarm) {
    gasAlarm = true;
    digitalWrite(RELAY_GAS, HIGH);

    sendSMS("Warning\nGas Detected.\nHome Fan Actived.");
    Serial.println("Gas Alarm ON");
  }

  if (!gasActive && gasAlarm) {
    gasAlarm = false;
    digitalWrite(RELAY_GAS, LOW);

    sendSMS("The gas condition has returned to normal.\nThe house fan has been turned off.");
    Serial.println("Gas Alarm OFF");
  }

  /* ===== FIRE SENSOR ===== */
  if (fireActive && !fireAlarm) {
    fireAlarm = true;
    digitalWrite(RELAY_FIRE, HIGH);

    sendSMS("Alarm\nFire detected.\nThe Water Pump activated.");
    Serial.println("Fire Alarm ON");
  }

  if (!fireActive && fireAlarm) {
    fireAlarm = false;
    digitalWrite(RELAY_FIRE, LOW);

    sendSMS("The fire situation has returned to normal.\nThe Water Pump has been disconnected.");
    Serial.println("Fire Alarm OFF");
  }

  delay(300);
}
