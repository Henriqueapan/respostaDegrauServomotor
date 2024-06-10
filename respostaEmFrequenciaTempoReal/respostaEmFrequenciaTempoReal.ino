//#define PWM_PIN 9  // Define o pino PWM a ser usado
#define PWM_FREQ 7.5 // Frequência em Hertz
#define AMPLITUDE 127  // Amplitude máxima do sinal PWM (0 a 255)
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder

volatile int chA_antigo = 0;
volatile int chB_antigo = 0;
volatile int contador_passos_motor = 0;
volatile float valor_pwm = 0;
volatile unsigned long tempo_atual = 0;
volatile unsigned long tempo_anterior = 0;
volatile double arg_senoide;
double radianos;
double inv_micro = 1/1000000.0;

void setup() {
  TCCR1B = (TCCR1B & 0xF8) | 0x01; // Configura a frequência PWM para 31.37kHz
  // Configuração dos pinos
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder, CHANGE);
  
  // Configuração da frequência de transmissão serial
  Serial.begin(115200);
}

void loop() {
  if (tempo_atual <= 30*1000000) {
    tempo_atual = micros();

    // Argumento da função seno que gera a senoide no instante atual
    arg_senoide = TWO_PI * PWM_FREQ /*Hz*/ * double(tempo_atual)*inv_micro;

    // Valor do duty cycle do PWM calculado utiliando senoide
    valor_pwm = ((127/2)*sin(arg_senoide)) + 1.5*127;

    // Escrevendo o valor do duty cycle do PWM no pino
    controlaMotor(0,1,valor_pwm);

    // Imprimindo na saída serial os dados de quantidades de passos do eixo do motor, intervalo de tempo referente a esses passos e o valor
    // do duty cycle do PWM no instante atual
    Serial.println(String(contador_passos_motor) + ", " + String((tempo_atual - tempo_anterior)) + ", "+ String(valor_pwm));
    
    contador_passos_motor = 0; // Reinicia o contador de passos do eixo do motor
    tempo_anterior = tempo_atual; // Define a variável de controle para cálculo de variação de tempo tempo_anterior como o tempo atual
  }
  else{ // Desliga o motor e para de enviar dados 
    controlaMotor(0,0,0); 
    Serial.end();
  }
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