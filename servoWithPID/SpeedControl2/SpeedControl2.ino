// "sketch": "servoWithPID\\SpeedControl\\SpeedControl2\\SpeedControl2.ino"

#include <TimerOne.h>
#include <Encoder.h>

#define chA 2 // Pino canal A do encoder
#define chB 3 // Pino canal B do encoder
#define REFERENCIA_PIN A0
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define RESOLUCAO_ENCODER 200 // Resolução do encoder (quantidade de passos que representa 1 volta completa)
#define PERIODO_AMOSTRAGEM 1000 // Microssegundos
#define INV_MICRO .000001

volatile int contador_passos = 0;
volatile double velocidade = 0;
double INV_RESOLUCAO = 1.0/RESOLUCAO_ENCODER;
double INV_AMOSTRAGEM = 1.0/PERIODO_AMOSTRAGEM;
volatile double erro = 0;
volatile double erro_anterior = 0;
volatile float ref = 0;
volatile float saida_controle = 0;
volatile double saida_controle_anterior = 0;

double K = 0.8;
const double a0 = 0.13752 * PERIODO_AMOSTRAGEM;
const double a1 = 0.10494 * PERIODO_AMOSTRAGEM;

void setup() {
  Timer1.initialize(PERIODO_AMOSTRAGEM);
  Timer1.attachInterrupt(interrupcao);

  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  pinMode(REFERENCIA_PIN, INPUT);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  
  Serial.begin(115200);
}

Encoder myEnc(chA, chB);

void loop() {
    // Serial.println(String(contador_passos));
    // Serial.println("ref: " + String(ref) + " / " + "erro: " +  String(erro) + " / " + "vel: " +  String(velocidade) + " / " + "PWM: " + String(saida_controle));
    // Serial.println(String(ref) + " / " + String(erro) + " / " + String(velocidade));
    // Serial.println(String(ref) + " / " + String(velocidade) + " / " + String(saida_controle));
    Serial.println(String(saida_controle,7) + " / " + String(saida_controle_anterior,7) + " / " + String(erro) + " / " + String(erro_anterior));
}

void interrupcao(){
    // unsigned long tempo = micros();
    leituraEncoder();   
    controladorVelocidade();
    atualizarPWM();
    atualizarMemorias();
    // Serial.println(micros() - tempo);
}

void leituraEncoder(){
    contador_passos = myEnc.read();
    velocidade = contador_passos * INV_RESOLUCAO * TWO_PI * INV_AMOSTRAGEM * 1000000;
}   

void controladorVelocidade(){
    ref = mapFloat(analogRead(REFERENCIA_PIN), 0, 1020, -400, 400);
    erro = ref - velocidade;
    // saida_controle = erro * K;
    saida_controle= a0  * erro + a1 * erro_anterior + saida_controle_anterior;
}

void atualizarPWM(){
    saida_controle = saida_controle * 23.61111 ; // 255
    saida_controle = constrain(saida_controle, -254,254);
    if (saida_controle <= 0){
        controlaMotor(1, 0, abs(saida_controle));
    }
    else{
        controlaMotor(0, 1, abs(saida_controle));
    }
}

void atualizarMemorias(){
    // velocidade_anterior = velocidade;
    // velocidade_ante_anterior = velocidade_anterior;
    saida_controle_anterior = saida_controle;
    erro_anterior = erro;
    // erro_ante_anterior = erro_anterior;
    myEnc.write(0);
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void controlaMotor(bool in1, bool in2, int valor_pwm){
    digitalWrite(MOTOR_PIN_1, in1);
    digitalWrite(MOTOR_PIN_2, in2);
    analogWrite(MOTOR_ENABLE, valor_pwm);
}