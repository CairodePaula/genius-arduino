#include <SoftwareSerial.h>
#include "secrets.h"

SoftwareSerial esp(5, 6);
String buffer = "";

const int LEDS[4] = {10, 11, 12, 13};
const int BUZZER = 8;
const int NOTAS[4] = {262, 330, 392, 523};

int sequencia[50];
int fase = 0;
int inputAtual = 0;
bool mostrando = false;
bool esperando = false;

void piscaLED(int n, int tempo = 300) {
  digitalWrite(LEDS[n], HIGH);
  tone(BUZZER, NOTAS[n], tempo);
  delay(tempo);
  digitalWrite(LEDS[n], LOW);
}

void fechaConn(int id) {
  esp.print(F("AT+CIPCLOSE="));
  esp.println(id);
  delay(50);
}

void enviaDados(int id, String json) {
  String resp = "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
                "Access-Control-Allow-Headers: *\r\n"
                "Connection: close\r\n\r\n" + json;
  esp.print(F("AT+CIPSEND="));
  esp.print(id);
  esp.print(F(","));
  esp.println(resp.length());
  delay(80);
  esp.print(resp);
  delay(100);
  fechaConn(id);
}

void enviaOptions(int id) {
  String resp = "HTTP/1.1 200 OK\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: GET, OPTIONS\r\n"
                "Access-Control-Allow-Headers: *\r\n"
                "Content-Length: 0\r\n"
                "Connection: close\r\n\r\n";
  esp.print(F("AT+CIPSEND="));
  esp.print(id);
  esp.print(F(","));
  esp.println(resp.length());
  delay(80);
  esp.print(resp);
  delay(100);
  fechaConn(id);
}

void enviaStatus(int id) {
  String json = "{\"fase\":" + String(fase) +
                ",\"esperando\":" + String(esperando ? "true" : "false") +
                ",\"mostrando\":" + String(mostrando ? "true" : "false") + "}";
  enviaDados(id, json);
}

String lerESP(int ms) {
  String r = "";
  unsigned long t = millis();
  while (millis() - t < (unsigned long)ms) {
    while (esp.available()) {
      r += (char)esp.read();
      t = millis();
    }
  }
  Serial.print(F("[ESP] ")); Serial.println(r);
  return r;
}

void iniciarESP() {
  Serial.println(F("Configurando ESP8266..."));

  esp.println(F("AT+RESTORE"));
  delay(3000);
  while (esp.available()) esp.read();

  esp.println(F("AT+RST"));
  delay(5000);
  while (esp.available()) esp.read();

  esp.println(F("AT+CWMODE=1"));
  lerESP(2000);

  Serial.println(F("Conectando ao WiFi..."));
  esp.println("AT+CWJAP=\"" + String(WIFI_SSID) + "\",\"" + String(WIFI_PASS) + "\"");

  unsigned long limite = millis() + 20000UL;
  while (millis() < limite) {
    if (esp.available()) {
      String r = lerESP(500);
      if (r.indexOf("GOT IP") >= 0) {
        Serial.println(F("WiFi conectado!"));
        break;
      }
    }
  }
  delay(1500);

  bool muxOk = false;
  int tentativas = 0;
  while (!muxOk && tentativas < 10) {
    esp.println(F("AT+CIPMUX=1"));
    String r = lerESP(2000);
    if (r.indexOf("OK") >= 0 || r.indexOf("no change") >= 0) {
      muxOk = true;
    } else {
      tentativas++;
      Serial.print(F("Tentando MUX... ")); Serial.println(tentativas);
      delay(500);
    }
  }

  bool servOk = false;
  tentativas = 0;
  while (!servOk && tentativas < 10) {
    esp.println(F("AT+CIPSERVER=1,80"));
    String r = lerESP(2000);
    if (r.indexOf("OK") >= 0 || r.indexOf("no change") >= 0) {
      servOk = true;
    } else {
      tentativas++;
      Serial.print(F("Tentando Servidor... ")); Serial.println(tentativas);
      delay(500);
    }
  }

  esp.println(F("AT+CIFSR"));
  Serial.println(lerESP(2000));
  Serial.println(F("--- SISTEMA PRONTO ---"));
  Serial.println(F("Abra genius.html no laptop!"));
}

void mostrarSequencia() {
  mostrando = true;
  esperando = false;
  delay(800);
  for (int i = 0; i <= fase; i++) {
    piscaLED(sequencia[i]);
    delay(200);
  }
  mostrando = false;
  esperando = true;
  inputAtual = 0;
  Serial.print(F("Fase: "));
  Serial.println(fase + 1);
}

void gameOver() {
  esperando = false;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) digitalWrite(LEDS[j], HIGH);
    tone(BUZZER, 150, 300);
    delay(300);
    for (int j = 0; j < 4; j++) digitalWrite(LEDS[j], LOW);
    delay(200);
  }
  fase = 0;
  sequencia[fase] = random(4);
  mostrarSequencia();
}

void processarRequisicao(String req) {
  int start = req.indexOf("+IPD,");
  if (start < 0) return;

  int idStart = start + 5;
  int idEnd = req.indexOf(",", idStart);
  int id = req.substring(idStart, idEnd).toInt();

  if (req.indexOf("OPTIONS") >= 0) {
    enviaOptions(id);
    return;
  }

  if (req.indexOf("favicon") >= 0) {
    fechaConn(id);
    return;
  }

  if (req.indexOf("GET /status") >= 0) {
    enviaStatus(id);
    return;
  }

  if (req.indexOf("GET /btn") >= 0) {
    int pos = req.indexOf("n=");
    if (pos >= 0) {
      int btn = req.charAt(pos + 2) - '0';
      if (btn >= 0 && btn <= 3) {
        if (esperando && btn == sequencia[inputAtual]) {
          inputAtual++;
          if (inputAtual > fase) {
            enviaDados(id, "{\"ok\":true,\"acertou\":true,\"fase\":" + String(fase + 1) + "}");
            piscaLED(btn, 150);
            fase++;
            sequencia[fase] = random(4);
            delay(600);
            mostrarSequencia();
          } else {
            enviaDados(id, "{\"ok\":true,\"acertou\":true,\"fase\":" + String(fase + 1) + "}");
            piscaLED(btn, 150);
          }
        } else {
          enviaDados(id, "{\"ok\":false,\"acertou\":false,\"fase\":0}");
          piscaLED(btn, 150);
          gameOver();
        }
      } else {
        fechaConn(id);
      }
      return;
    }
  }

  fechaConn(id);
}

void setup() {
  Serial.begin(9600);
  esp.begin(9600);

  for (int i = 0; i < 4; i++) pinMode(LEDS[i], OUTPUT);
  pinMode(BUZZER, OUTPUT);

  randomSeed(analogRead(A3));
  iniciarESP();

  sequencia[fase] = random(4);
  mostrarSequencia();
}

void loop() {
  if (esp.available()) {
    char c = esp.read();
    buffer += c;

    if (buffer.indexOf("GET ") >= 0 || buffer.indexOf("OPTIONS ") >= 0) {
      delay(300);
      while (esp.available()) buffer += (char)esp.read();
      processarRequisicao(buffer);
      buffer = "";
    }
  }
  if (buffer.length() > 512) buffer = "";
}
