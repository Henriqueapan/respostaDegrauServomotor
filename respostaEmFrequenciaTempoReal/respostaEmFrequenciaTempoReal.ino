//#define PWM_PIN 9  // Define o pino PWM a ser usado
#define PWM_freq 1000 // Frequência em Hertz
#define AMPLITUDE 127  // Amplitude máxima do sinal PWM (0 a 255)
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder

volatile int chA_antigo = 0;
volatile int chB_antigo = 0;
volatile int contador = 0;
volatile float valor_pwm;
double tempoAtual = 0;
double tempoAnterior = 0;
// double PWM_PERIOD = ((1/PWM_freq)*1000)/360; // Periodo em millissegundos
double radianos;
float inv_mili = 1/1000;

void setup() {
  // Configuração do pino PWM
  // pinMode(PWM_PIN, OUTPUT);
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
  if (tempoAtual <=20000){  // if Para acionar o motor por apenas 2500 millissegundos
    tempoAtual = millis();
    valor_pwm = (sin(2*PI*PWM_freq*tempoAtual*inv_mili));

    // Escrevendo o valor PWM no pino
    controlaMotor(0,1,valor_pwm);

    Serial.println(String(contador) + ", " + String(tempoAtual-tempoAnterior) + ", "+ String(valor_pwm));

    contador = 0; // Reinicia o contador
    tempoAnterior = tempoAtual; // Atualiza o tempo
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