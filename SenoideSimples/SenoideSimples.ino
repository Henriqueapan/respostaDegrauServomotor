#define PWM_freq 100.0// Frequência em Hertz
#define AMPLITUDE 127  // Amplitude máxima do sinal PWM (0 a 255)
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder

volatile int chA_antigo = 0;
volatile int chB_antigo = 0;
volatile int contador = 0;
unsigned long tempoAtual =0;
unsigned long tempoAnterior=0;
unsigned long timeInSeconds=0;
double periodo = (1.0/PWM_freq)*1000.0*(1000.0/360.0);
int k=0;
//double periodo = 10;

void setup() {
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder, CHANGE);
  Serial.begin(115200);
  TCCR1B = (TCCR1B & 0xF8) | 0x01;
}

void loop() {

 
  for (int i = 0; i<=360; i++){
    tempoAtual = micros();
    double rad = i*DEG_TO_RAD;
    double seno = 127*sin(rad);
    Serial.println(String(seno) + ", " + String(tempoAtual - tempoAnterior));
    tempoAnterior = tempoAtual;
    delayMicroseconds(round(periodo));
    Serial.println(round(periodo));
    // if(i==360){
    //   k++;
    // }
    // if(k>7){
    //   Serial.end();
    // }
  }


 // while(timeInSeconds<10){
  timeInSeconds = micros()/1000000.0;
  //Serial.println(timeInSeconds,7);
  double arg = TWO_PI*PWM_freq*timeInSeconds;
  //Serial.println(arg,7);
  double seno = 10000000*sin(arg);
  Serial.println(seno,7);
  double valor_pwm = map(seno,-1,1,0,255);
  //Serial.println(valor_pwm,7);
  //Serial.println(String(valor_pwm) + ", " + String(timeInSeconds- tempoAnterior));
  tempoAnterior = timeInSeconds;
  delayMicroseconds(round(periodo));
  //delayMicroseconds(100);
 // }
  //Serial.end();
  
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