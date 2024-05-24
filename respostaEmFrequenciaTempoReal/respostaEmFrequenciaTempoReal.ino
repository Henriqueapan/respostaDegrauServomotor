//#define PWM_PIN 9  // Define o pino PWM a ser usado
#define PWM_FREQ 0.1 // Frequência em Hertz
#define AMPLITUDE 127  // Amplitude máxima do sinal PWM (0 a 255)
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder

volatile int chA_antigo = 0;
volatile int chB_antigo = 0;
volatile int contador = 0;
volatile float valor_pwm = 0;
volatile double tempoAtual = 0;
volatile double tempoAnterior = 0;
volatile double arg;
double radianos;
float inv_micro = 1/1000000;

void setup() {
  // Configuração do pino PWM
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder, CHANGE);
  
  // Configuração da frequência PWM
  Serial.begin(115200);
}

void loop() {
  if (tempoAtual <= 15*1000000) {
    tempoAtual = micros();

    arg = TWO_PI * PWM_FREQ /*Hz*/ * double(tempoAtual)/(1000000);//*0.0000000001;

    valor_pwm = ((127/2)*sin(arg)) + 1.5*127;

    // Escrevendo o valor PWM no pino
    controlaMotor(0,1,valor_pwm);

    Serial.println(String(valor_pwm) + ", " + String((tempoAtual - tempoAnterior)) + ", "+ String(valor_pwm));
    
    contador = 0; // Reinicia o contador
    tempoAnterior = tempoAtual;
  }
  else{ // Desliga o motor e para de enviar
    controlaMotor(0,0,0); 
    Serial.end();
  }
  delay(10);
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