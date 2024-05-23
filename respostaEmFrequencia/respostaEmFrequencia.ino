//#define PWM_PIN 9  // Define o pino PWM a ser usado
#define PWM_PERIOD 10  // Frequência PWM em Hertz
#define AMPLITUDE 127  // Amplitude máxima do sinal PWM (0 a 255)
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder

int chA_antigo = 0;
int chB_antigo = 0;
volatile int contador = 0;
double tempoAtual = 0;
double tempoAnterior = 0;

void setup() {
  // Configuração do pino PWM
  //pinMode(PWM_PIN, OUTPUT);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder, CHANGE);
  
  // Configuração da frequência PWM
  TCCR1B = (TCCR1B & 0xF8) | 0x01; // Configura a frequência PWM para 31.37kHz
  Serial.begin(115200);
}

void loop() {
  //tempoAtual =millis();
  if (tempoAtual <=10000){  // if Para acionar o motor por apenas 2500 millissegundos

    for (int i = 0; i < 360; i++) {
      tempoAtual = millis();
      // Convertendo graus para radianos
      float radianos = i * (PI / 180.0);
      // Calculando o valor do seno e mapeando para o intervalo de 0 a 255
      int valor_pwm = (sin(radianos) * AMPLITUDE/2) + 1.5*AMPLITUDE;  // Máximo de 254 ~= 255 e mínimo 190,5 (digital do duty cycle do PWM)
      // Escrevendo o valor PWM no pino
      controlaMotor(0,1,valor_pwm);
      //Serial.println(valor_pwm);
      //analogWrite(PWM_PIN, valor_pwm);

      Serial.println(String(contador) + ", " + String(tempoAtual-tempoAnterior) + ", "+ String(valor_pwm));
    
      contador = 0; // Reinicia o contador
      tempoAnterior = tempoAtual; // Atualiza o tempo
      // Pequeno atraso para a visualização
      delayMicroseconds(PWM_PERIOD);
    } // Fazendo o motor girar em alguma direção na velocidade 255

  }
  else{ // Desliga o motor e para de enviar
    controlaMotor(0,0,0); 
    Serial.end();
  }
  delay(1);
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

}