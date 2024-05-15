#define chA 3 // Pino canal A do encoder
#define chB 4 // Pino canal B do encoder
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H

#define EncResolution 100
#define pi 3.14159

volatile int contador = 0;
unsigned long tempoAtual;
unsigned long tempoAnterior = 0;
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
  tempoAtual = millis(); // atualiza o quanto tempo se passou desde o início

  if (tempoAtual <=10000){  // if Para acionar o motor por apenas 2500 millissegundos

    controlaMotor(0,1,255); // Fazendo o motor girar em alguma direção na velocidade 255

    //velocidade = ((float)contador/EncResolution)*2*pi*(1/(tempoAtual - tempoAnterior)); // Calcula a velocidade em rad/s
    //Serial.println(velocidade); // Envia a velocidade calculada pelo serial

    Serial.println(contador);
    
    contador = 0; // Reinicia o contador
    tempoAnterior = tempoAtual; // Atualiza o tempo
  }
  else{ // Desliga o motor e para de enviar
    controlaMotor(0,0,0); 
    Serial.end();
  }

  delay(10); 
}

void leituraEncoder() {
  int chA_atual = digitalRead(chA);
  int chB_atual = digitalRead(chB);

  if (chA_antigo == 0 && chB_antigo == 0) {
    if(chA_atual == 1 && chB_atual == 1) contador = contador + 2;
    else if(chA_atual == 1 && chB_atual == 0) contador--;
    else if(chA_atual == 0 && chB_atual == 1) contador++;
  }
  else if (chA_antigo == 0 && chB_antigo == 1) {
    if(chA_atual == 1 && chB_atual == 1) contador ++;
    else if(chA_atual == 1 && chB_atual == 0) contador = contador - 2;
    else if(chA_atual == 0 && chB_atual == 1) contador = contador;
    else contador--;
  }
  else if (chA_antigo == 1 && chB_antigo == 0) {
    if(chA_atual == 1 && chB_atual == 1) contador--;
    else if(chA_atual == 1 && chB_atual == 0) contador = contador;
    else if(chA_atual == 0 && chB_atual == 1) contador = contador - 2;
    else contador ++;
  }
  else if (chA_antigo = 1 && chB_antigo == 1) {
    if(chA_atual == 1 && chB_atual == 1) contador = contador;
    else if(chA_atual == 1 && chB_atual == 0) contador++;
    else if(chA_atual == 0 && chB_atual == 1) contador--;
    else contador = contador + 2;
  }

  chA_antigo = chA_atual;
  chB_antigo = chB_atual;

  // if (digitalRead(chB) == digitalRead(chA)) {
  //   contador++;
  // } else {
  //   contador--;
  // }
}

void controlaMotor (bool in1, bool in2, float pwm){
  if (in1 == 1 && in2 == 0){
    digitalWrite(MOTOR_PIN_1, HIGH);
    digitalWrite(MOTOR_PIN_2, LOW);
  }
  else if (in1 == 0 && in2 == 1){
    digitalWrite(MOTOR_PIN_1, LOW);
    digitalWrite(MOTOR_PIN_2, HIGH);
  }
  else if (in1 == 0 && in2 == 0){
    digitalWrite(MOTOR_PIN_1, LOW);
    digitalWrite(MOTOR_PIN_2, LOW);
  }
  else if (in1 == 1 && in2 == 1){
    digitalWrite(MOTOR_PIN_1, HIGH);
    digitalWrite(MOTOR_PIN_2, LOW);
  }
  analogWrite(MOTOR_ENABLE, pwm); 
}

// void leituraEncoder() {
//   static int estadoAnterior = 0;
//   int estadoAtual;

//   estadoAtual = digitalRead(chA) << 1 | digitalRead(chB);
//   int delta = estadoAtual - estadoAnterior;
  
//   if(delta == 1 || delta == -3)
//     contador++;
//   else if(delta == -1 || delta == 3)
//     contador--;

//   estadoAnterior = estadoAtual;
// }