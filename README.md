# genius-arduino 🧠

Recriação do clássico jogo **Genius** (Simon Says) com Arduino Uno + ESP8266, controlado por uma interface web via WiFi na mesma rede local.

## 🎮 Como funciona

* O Arduino gera uma sequência aleatória de LEDs e sons
* A interface web exibe o painel do jogo no navegador
* O jogador repete a sequência clicando nos botões coloridos
* A cada fase acertada, a sequência cresce — erre e começa do zero

## 🛠️ Hardware

* Arduino Uno
* ESP8266 ESP-01S (módulo WiFi via SoftwareSerial)
* 4 LEDs (vermelho, amarelo, verde, azul)
* 1 Buzzer passivo
* Resistores + Protoboard + Jumpers

**Pinagem:**

| Componente | Pino Arduino |
|---|---|
| LED vermelho | 10 |
| LED amarelo | 11 |
| LED verde | 12 |
| LED azul | 13 |
| Buzzer | 8 |
| ESP TX | 5 (SoftwareSerial RX) |
| ESP RX | 6 (SoftwareSerial TX) |

## 🚀 Como rodar

**1. Configure as credenciais WiFi**

Copie o arquivo de exemplo e preencha com sua rede:

```
cp secrets.example.h secrets.h
```

Edite o `secrets.h`:

```cpp
#define WIFI_SSID "sua_rede"
#define WIFI_PASS "sua_senha"
```

**2. Suba o código para o Arduino**

Abra `genius.ino` na Arduino IDE e faça o upload.

**3. Descubra o IP**

Abra o Serial Monitor (9600 baud) — o IP do Arduino aparece após a conexão WiFi.

**4. Abra a interface web**

Abra o `genius.html` no navegador, digite o IP do Arduino e clique em **Conectar**.

## 📡 Endpoints (API do Arduino)

| Rota | Descrição |
|---|---|
| `GET /status` | Retorna fase atual, se está mostrando ou esperando input |
| `GET /btn?n=0..3` | Registra o botão pressionado pelo jogador |

## 🧱 Arquitetura

```
Navegador (genius.html) → HTTP → ESP8266 → Arduino Uno → LEDs + Buzzer
```

## 📦 Stack

* Arduino Uno
* ESP8266 (AT commands via SoftwareSerial)
* HTML + CSS + JavaScript (vanilla)

## 👨‍💻 Autor

Desenvolvido por Cairo
