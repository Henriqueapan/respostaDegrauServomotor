#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H

volatile int contador_passos_motor = 0;
unsigned long tempo_atual;
unsigned long tempo_anterior = 0;
int interrupcao_atual = 0;
int chA_antigo = 0;
int chB_antigo = 0;
float velocidade = 0;

void setup() {
  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder, CHANGE);
  Serial.begin(115200);

}

void loop() {
  tempo_atual = millis(); // Atualiza o quanto tempo se passou desde o início

  if (tempo_atual <= 15000){  // Condicional para acionar o motor por apenas 15000 millissegundos

    controlaMotor(0,1,255); // Fazendo o motor girar no sentido horário na velocidade 255

    // Imprimindo na saída serial os dados de 
    Serial.println(String(contador_passos_motor) + "," + String(tempo_atual-tempo_anterior));
    
    contador_passos_motor = 0; // Reinicia o contador_passos_motor
    tempo_anterior = tempo_atual; // Atualiza o tempo
  }
  else{ // Desliga o motor e para de enviar
    controlaMotor(0,0,0); 
    Serial.end();
  }

  delay(1); 
}

void leituraEncoder() {
  int chA_atual = digitalRead(chA);
  int chB_atual = digitalRead(chB);

  if (chA_antigo == 0 && chB_antigo == 0) {
    if(chA_atual == 1 && chB_atual == 1) contador_passos_motor = contador_passos_motor + 2;
    else if(chA_atual == 1 && chB_atual == 0) contador_passos_motor--;
    else if(chA_atual == 0 && chB_atual == 1) contador_passos_motor++;
  }
  else if (chA_antigo == 0 && chB_antigo == 1) {
    if(chA_atual == 1 && chB_atual == 1) contador_passos_motor ++;
    else if(chA_atual == 1 && chB_atual == 0) contador_passos_motor = contador_passos_motor - 2;
    else if(chA_atual == 0 && chB_atual == 1) contador_passos_motor = contador_passos_motor;
    else contador_passos_motor--;
  }
  else if (chA_antigo == 1 && chB_antigo == 0) {
    if(chA_atual == 1 && chB_atual == 1) contador_passos_motor--;
    else if(chA_atual == 1 && chB_atual == 0) contador_passos_motor = contador_passos_motor;
    else if(chA_atual == 0 && chB_atual == 1) contador_passos_motor = contador_passos_motor - 2;
    else contador_passos_motor ++;
  }
  else if (chA_antigo = 1 && chB_antigo == 1) {
    if(chA_atual == 1 && chB_atual == 1) contador_passos_motor = contador_passos_motor;
    else if(chA_atual == 1 && chB_atual == 0) contador_passos_motor++;
    else if(chA_atual == 0 && chB_atual == 1) contador_passos_motor--;
    else contador_passos_motor = contador_passos_motor + 2;
  }

  chA_antigo = chA_atual;
  chB_antigo = chB_atual;

}

void controlaMotor(bool in1, bool in2, float pwm) {
  digitalWrite(MOTOR_PIN_1, in1);
  digitalWrite(MOTOR_PIN_2, in2);
  analogWrite(MOTOR_ENABLE, pwm);
}
