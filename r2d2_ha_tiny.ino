#include "BLEDevice.h"
#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "therzogs_unifi";
const char* WIFI_PASSWORD = "f5zu-5uku-glta-7qo1";
const char* R2_MAC = "df:9a:a3:7c:3f:3f";

static BLEUUID svc1("00020001-574f-4f20-5370-6865726f2121");
static BLEUUID chr1("00020005-574f-4f20-5370-6865726f2121");
static BLEUUID chr2("00020002-574f-4f20-5370-6865726f2121");
static BLEUUID svc2("00010001-574f-4f20-5370-6865726f2121");
static BLEUUID chr3("00010002-574f-4f20-5370-6865726f2121");

BLEClient* c = nullptr;
BLERemoteCharacteristic* m = nullptr;
uint8_t s = 0;
bool connected = false;
WebServer srv(80);

uint8_t chk(uint8_t* b, size_t l) {
  uint8_t r = 0;
  for (size_t i = 0; i < l; i++) r += b[i];
  return (r ^ 0xFF);
}

void build(uint8_t* o, size_t* ol, uint8_t d, uint8_t c, uint8_t* p, size_t pl) {
  uint8_t b[256], e[512];
  size_t bl = 0, el = 0;
  
  b[bl++] = 0x0A; b[bl++] = d; b[bl++] = c; b[bl++] = s;
  for (size_t i = 0; i < pl; i++) b[bl++] = p[i];
  b[bl++] = chk(b, bl);
  
  for (size_t i = 0; i < bl; i++) {
    if (b[i] == 0xAB) { e[el++] = 0xAB; e[el++] = 0x23; }
    else if (b[i] == 0x8D) { e[el++] = 0xAB; e[el++] = 0x05; }
    else if (b[i] == 0xD8) { e[el++] = 0xAB; e[el++] = 0x50; }
    else e[el++] = b[i];
  }
  
  o[0] = 0x8D;
  memcpy(o + 1, e, el);
  o[1 + el] = 0xD8;
  *ol = 2 + el;
  s = (s + 1) % 140;
}

class CB : public BLEClientCallbacks {
  void onConnect(BLEClient* p) { connected = true; }
  void onDisconnect(BLEClient* p) { connected = false; }
};

void w(uint8_t d, uint8_t c, uint8_t* p, size_t pl) {
  if (!connected || !m) return;
  uint8_t pkt[512];
  size_t pl2;
  build(pkt, &pl2, d, c, p, pl);
  m->writeValue(pkt, pl2, false);
}

uint8_t bat() {
  if (!connected) return 0;
  BLERemoteService* bs = c->getService(BLEUUID((uint16_t)0x180F));
  if (bs) {
    BLERemoteCharacteristic* bc = bs->getCharacteristic(BLEUUID((uint16_t)0x2A19));
    if (bc && bc->canRead()) {
      String v = bc->readValue();
      if (v.length() > 0) return (uint8_t)v[0];
    }
  }
  return 0;
}

void conn() {
  if (connected) return;
  if (!c) { BLEDevice::init(""); c = BLEDevice::createClient(); c->setClientCallbacks(new CB()); }
  BLEAddress a(R2_MAC);
  if (!c->connect(a, BLE_ADDR_TYPE_RANDOM)) if (!c->connect(a, BLE_ADDR_TYPE_PUBLIC)) return;
  delay(500);
  BLERemoteService* s1 = c->getService(svc1);
  if (!s1) return;
  BLERemoteCharacteristic* h = s1->getCharacteristic(chr2);
  if (h) {
    h->registerForNotify([](BLERemoteCharacteristic* p, uint8_t* d, size_t l, bool n){});
    BLERemoteDescriptor* ds = h->getDescriptor(BLEUUID((uint16_t)0x2902));
    if (ds) { const uint8_t n[] = {0x01, 0x00}; ds->writeValue((uint8_t*)n, 2, true); }
  }
  BLERemoteCharacteristic* co = s1->getCharacteristic(chr1);
  if (!co) return;
  const char* msg = "usetheforce...band";
  co->writeValue((uint8_t*)msg, strlen(msg), true);
  delay(500);
  BLERemoteService* s2 = c->getService(svc2);
  if (!s2) return;
  m = s2->getCharacteristic(chr3);
  if (!m) return;
  m->registerForNotify([](BLERemoteCharacteristic* p, uint8_t* d, size_t l, bool n){});
  BLERemoteDescriptor* ds2 = m->getDescriptor(BLEUUID((uint16_t)0x2902));
  if (ds2) { const uint8_t n[] = {0x01, 0x00}; ds2->writeValue((uint8_t*)n, 2, true); }
  delay(300);
  uint8_t np[] = {};
  w(0x13, 0x0D, np, 0);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\nR2-D2 Bridge Starting...");
  Serial.print("Connecting to: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n\nWiFi FAILED!");
    Serial.print("Status: ");
    Serial.println(WiFi.status());
    Serial.println("\nCheck:");
    Serial.println("1. SSID is correct");
    Serial.println("2. Password is correct");
    Serial.println("3. WiFi is 2.4GHz (not 5GHz)");
    Serial.println("4. Router is powered on");
    return;
  }
  
  Serial.println("\n\nWiFi Connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm\n");
  
  Serial.println("Connecting to R2-D2...");
  conn();
  
  if (connected) {
    Serial.println("R2-D2 Connected!\n");
  } else {
    Serial.println("R2-D2 connection failed\n");
  }
  
  srv.on("/status", []() { 
    String r = "{\"c\":"; r += connected?"1":"0"; r += ",\"b\":"; r += String(bat()); r += "}";
    srv.send(200, "application/json", r);
  });
  
  srv.on("/battery", []() {
    String r = "{\"b\":"; r += String(bat()); r += "}";
    srv.send(200, "application/json", r);
  });
  
  srv.on("/wake", []() {
    if (!connected) { srv.send(503, "text/plain", "NC"); return; }
    uint8_t p[] = {};
    w(0x13, 0x0D, p, 0);
    srv.send(200, "text/plain", "OK");
  });
  
  srv.on("/sleep", []() {
    if (!connected) { srv.send(503, "text/plain", "NC"); return; }
    uint8_t p[] = {};
    w(0x13, 0x01, p, 0);
    srv.send(200, "text/plain", "OK");
  });
  
  srv.on("/anim", []() {
    if (!connected) { srv.send(503, "text/plain", "NC"); return; }
    if (!srv.hasArg("n")) { srv.send(400, "text/plain", "E"); return; }
    int n = srv.arg("n").toInt();
    if (n < 0 || n > 56) { srv.send(400, "text/plain", "E"); return; }
    uint8_t p[] = {0x00, (uint8_t)n};
    w(0x17, 0x05, p, 2);
    srv.send(200, "text/plain", "OK");
  });
  
  srv.on("/stance", []() {
    if (!connected) { srv.send(503, "text/plain", "NC"); return; }
    if (!srv.hasArg("t")) { srv.send(400, "text/plain", "E"); return; }
    int t = srv.arg("t").toInt();
    if (t < 1 || t > 2) { srv.send(400, "text/plain", "E"); return; }
    uint8_t p[] = {(uint8_t)t};
    w(0x17, 0x0D, p, 1);
    srv.send(200, "text/plain", "OK");
  });
  
  srv.begin();
  Serial.println("API Server started!");
  Serial.println("\nEndpoints:");
  Serial.println("  GET  /status");
  Serial.println("  GET  /battery");
  Serial.println("  POST /wake");
  Serial.println("  POST /sleep");
  Serial.println("  POST /anim?n=X");
  Serial.println("  POST /stance?t=X");
  Serial.println("\nReady!\n");
}

void loop() {
  srv.handleClient();
  if (!connected) { static unsigned long l = 0; if (millis() - l > 30000) { conn(); l = millis(); } }
  delay(10);
}
