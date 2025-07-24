
const int segmentPins[7] = {2, 3, 4, 5, 6, 7, 8}; // Segmentos a-g
const int digitPins[4]   = {9, 10, 11, 12};       // Dígitos D1-D4
const int doisPontosPin  = 13;                    // Pino dos dois pontos (entre HH e MM)


const byte numbers[10] = {
  0b1111110, // 0
  0b0110000, // 1
  0b1101101, // 2
  0b1111001, // 3
  0b0110011, // 4
  0b1011011, // 5
  0b1011111, // 6
  0b1110000, // 7
  0b1111111, // 8
  0b1111011  // 9
};


int horas = 0;
int minutos = 0;

unsigned long ultimoMinuto       = 0;
unsigned long ultimaMultiplexacao = 0;
unsigned long ultimoPonto        = 0;
bool pontosAcesos                = false;

int digitoAtual = 0;

void setup() {
  // === CONFIGURAR PINOS ===
  for (int i = 0; i < 7; i++) pinMode(segmentPins[i], OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], HIGH); // Desliga dígito
  }
  pinMode(doisPontosPin, OUTPUT);     // Pino dos dois pontos

  // === SERIAL ===
  Serial.begin(9600);
  Serial.println("=== RELÓGIO DIGITAL 24H ===");
  Serial.println("Digite o horário no formato HH:MM");
  Serial.print("Horário: ");

  // === LEITURA SERIAL ===
  while (!Serial.available());
  String entrada = Serial.readString();
  entrada.trim();
  sscanf(entrada.c_str(), "%d:%d", &horas, &minutos);

  // === CONFIRMAÇÃO ===
  Serial.println();
  Serial.print("Horário configurado: ");
  if (horas < 10) Serial.print("0");
  Serial.print(horas);
  Serial.print(":");
  if (minutos < 10) Serial.print("0");
  Serial.println(minutos);
  Serial.println("Relógio iniciado!");
  Serial.println();

  ultimoMinuto = millis();
  ultimaMultiplexacao = millis();
  ultimoPonto = millis();
}

void loop() {
  unsigned long tempoAtual = millis();

  // === CONTAGEM DE TEMPO (a cada 60s) ===
  if (tempoAtual - ultimoMinuto >= 60000) {
    minutos++;
    if (minutos >= 60) {
      minutos = 0;
      horas++;
      if (horas >= 24) horas = 0;
    }
    ultimoMinuto = tempoAtual;
  }

  // === PISCAR OS DOIS PONTOS ===
  if (tempoAtual - ultimoPonto >= 500) {
    pontosAcesos = !pontosAcesos;
    digitalWrite(doisPontosPin, pontosAcesos ? HIGH : LOW); // Catodo comum: HIGH acende
    ultimoPonto = tempoAtual;
  }

  // === MULTIPLEXAÇÃO DOS DÍGITOS ===
  if (tempoAtual - ultimaMultiplexacao >= 5) {
    int digitos[4];
    digitos[0] = (horas / 10) % 10;
    digitos[1] = horas % 10;
    digitos[2] = (minutos / 10) % 10;
    digitos[3] = minutos % 10;

    // Apagar todos os dígitos
    for (int i = 0; i < 4; i++) digitalWrite(digitPins[i], HIGH);

    // Configurar segmentos do dígito atual
    byte pattern = numbers[digitos[digitoAtual]];
    for (int i = 0; i < 7; i++) {
      digitalWrite(segmentPins[i], (pattern >> (6 - i)) & 1);
    }

    // Ativar o dígito atual
    digitalWrite(digitPins[digitoAtual], LOW);

    digitoAtual = (digitoAtual + 1) % 4;
    ultimaMultiplexacao = tempoAtual;
  }
}