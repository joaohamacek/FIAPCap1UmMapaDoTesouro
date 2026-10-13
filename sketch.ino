#include <Arduino.h>
#include <Wire.h>
#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

// FarmTech Solutions / FIAP - Projeto 2
// Tres botoes com memoria de estado: clique alterna 0/1.
const uint8_t PIN_DHT = 15, PIN_LDR = 34;
const uint8_t PIN_N = 25, PIN_P = 26, PIN_K = 27, PIN_RELE = 23;
const uint8_t PINOS_BOTOES[3] = {PIN_N, PIN_P, PIN_K};
const float UMIDADE_LIGA = 45.0f, UMIDADE_DESLIGA = 60.0f;
const float PH_MIN = 5.5f, PH_MAX = 7.5f;
const unsigned long DEBOUNCE_MS = 45;

DHTesp dht;
LiquidCrystal_I2C lcd(0x27, 16, 2);
bool npk[3] = {false, false, false};
bool leituraAnterior[3] = {HIGH, HIGH, HIGH};
bool leituraEstavel[3] = {HIGH, HIGH, HIGH};
unsigned long ultimaMudanca[3] = {0, 0, 0};
bool irrigando = false, sensorOK = false, nutrientesOK = false, phOK = false;
float umidade = 0, temperatura = 0, ph = 0;
int leituraLDR = 0;
unsigned long ultimaLeitura = 0, ultimaTela = 0;
uint8_t telaAtual = 0;

void lerBotoes() {
  unsigned long agora = millis();
  for (int i = 0; i < 3; i++) {
    bool leitura = digitalRead(PINOS_BOTOES[i]);
    if (leitura != leituraAnterior[i]) ultimaMudanca[i] = agora;
    if (agora - ultimaMudanca[i] >= DEBOUNCE_MS && leitura != leituraEstavel[i]) {
      leituraEstavel[i] = leitura;
      if (leitura == LOW) npk[i] = !npk[i];
    }
    leituraAnterior[i] = leitura;
  }
}

void lerSensoresEControlar() {
  TempAndHumidity dados = dht.getTempAndHumidity();
  sensorOK = !isnan(dados.humidity);
  if (!sensorOK) {
    irrigando = false;
    digitalWrite(PIN_RELE, LOW);
    Serial.println("ERRO DHT22 - RELE DESLIGADO");
    return;
  }
  umidade = dados.humidity;
  temperatura = dados.temperature;
  leituraLDR = analogRead(PIN_LDR);
  ph = 14.0f * leituraLDR / 4095.0f;
  nutrientesOK = (int)npk[0] + (int)npk[1] + (int)npk[2] >= 2;
  phOK = ph >= PH_MIN && ph <= PH_MAX;
  if (!nutrientesOK || !phOK) irrigando = false;
  else if (!irrigando && umidade < UMIDADE_LIGA) irrigando = true;
  else if (irrigando && umidade >= UMIDADE_DESLIGA) irrigando = false;
  digitalWrite(PIN_RELE, irrigando ? HIGH : LOW);
  Serial.printf("U=%.1f%% T=%.1fC N=%d P=%d K=%d LDR=%d pH=%.2f NPK_OK=%d PH_OK=%d BOMBA=%s\n",
                umidade, temperatura, npk[0], npk[1], npk[2], leituraLDR,
                ph, nutrientesOK, phOK, irrigando ? "LIGADA" : "DESLIGADA");
}

void atualizarLCD() {
  lcd.clear();
  if (!sensorOK) {
    lcd.setCursor(0, 0); lcd.print("ERRO SENSOR DHT");
    lcd.setCursor(0, 1); lcd.print("BOMBA DESLIGADA");
    return;
  }
  switch (telaAtual) {
    case 0:
      lcd.setCursor(0, 0); lcd.print("UMIDADE: "); lcd.print(umidade, 1); lcd.print("%");
      lcd.setCursor(0, 1); lcd.print("PH SOLO: "); lcd.print(ph, 2);
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("N:"); lcd.print((int)npk[0]);
      lcd.print(" P:"); lcd.print((int)npk[1]);
      lcd.print(" K:"); lcd.print((int)npk[2]);
      lcd.setCursor(0, 1); lcd.print(nutrientesOK ? "NPK: ADEQUADO" : "NPK: INSUFIC.");
      break;
    case 2:
      lcd.setCursor(0, 0); lcd.print("IRRIGACAO:");
      lcd.setCursor(0, 1); lcd.print(irrigando ? "BOMBA LIGADA" : "BOMBA DESLIGADA");
      break;
    case 3:
      lcd.setCursor(0, 0); lcd.print("TEMPERATURA:");
      lcd.setCursor(0, 1); lcd.print(temperatura, 1); lcd.print(" GRAUS C");
      break;
    case 4:
      lcd.setCursor(0, 0); lcd.print("PH: "); lcd.print(ph, 2);
      lcd.setCursor(0, 1); lcd.print(phOK ? "PH ADEQUADO" : "PH INADEQUADO");
      break;
  }
  telaAtual = (telaAtual + 1) % 5;
}

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 3; i++) {
    pinMode(PINOS_BOTOES[i], INPUT_PULLUP);
    leituraAnterior[i] = digitalRead(PINOS_BOTOES[i]);
    leituraEstavel[i] = leituraAnterior[i];
  }
  pinMode(PIN_RELE, OUTPUT);
  digitalWrite(PIN_RELE, LOW);
  analogReadResolution(12);
  dht.setup(PIN_DHT, DHTesp::DHT22);
  Wire.begin(21, 22);
  lcd.init(); lcd.backlight();
  lcd.setCursor(0, 0); lcd.print("FARMTECH - FIAP");
  lcd.setCursor(0, 1); lcd.print("Iniciando...");
  delay(2000);
  lerSensoresEControlar();
  atualizarLCD();
  ultimaLeitura = millis();
  ultimaTela = millis();
}

void loop() {
  // Os botoes sao lidos a cada iteracao; nao dependem do intervalo do DHT22.
  lerBotoes();
  unsigned long agora = millis();
  if (agora - ultimaLeitura >= 2000) {
    ultimaLeitura = agora;
    lerSensoresEControlar();
  }
  if (agora - ultimaTela >= 3000) {
    ultimaTela = agora;
    atualizarLCD();
  }
}
