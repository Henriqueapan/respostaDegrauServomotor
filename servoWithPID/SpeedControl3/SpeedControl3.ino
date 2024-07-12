#include <TimerOne.h>
#define chA 2 // Pino canal A do encoder
#define chB 3 // Pino canal B do encoder
#define REFERENCIA_PIN A0
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define RESOLUCAO_ENCODER 200 // Resolução do encoder (quantidade de passos que representa 1 volta completa)
#define PERIODO_AMOSTRAGEM 500 // Microssegundos
#define INV_MICRO .000001

#define PWM_MIN 60 // Valor mínimo de PWM para acionar o motor
#define DEAD_ZONE 10 

volatile double velocidade = 0;
double INV_RESOLUCAO = 1.0/(double)RESOLUCAO_ENCODER;
double INV_AMOSTRAGEM = 1.0/(double)PERIODO_AMOSTRAGEM;
volatile double erro = 0;
volatile double ref = 0;
volatile double saida_controle = 0;
double K = 0.5;

double COEF_EQ_DIFERENCAS_POSICAO = (PERIODO_AMOSTRAGEM/2) * INV_MICRO;
volatile double velocidade_anterior = 0;
volatile double erro_anterior = 0;

volatile double posicao = 0;
volatile double posicao_anterior = 0;
volatile double saida_controle_anterior = 0;

volatile int contador_passos_motor = 0;
volatile int chA_atual;
volatile int chB_atual;
volatile int chA_antigo = 0;
volatile int chB_antigo = 0;

volatile int pwm_val = 0;

const float a0 = K * 0.24237 * (0.134 / PERIODO_AMOSTRAGEM);
const float a1 = K * 0.24237;
const float b1 = (2 - PERIODO_AMOSTRAGEM) / (2 + PERIODO_AMOSTRAGEM);

void setup() {
  Timer1.initialize(PERIODO_AMOSTRAGEM);
  Timer1.attachInterrupt(interrupcao);

  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  pinMode(REFERENCIA_PIN, INPUT);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, RISING);
  Serial.begin(115200);
}

void loop() {
    // Serial.println(String(contador_passos));
    // Serial.println("ref: " + String(ref) + " / " + "erro: " +  String(erro) + " / " + "vel: " +  String(velocidade) + " / " + "PWM: " + String(saida_controle));
    Serial.println(String(ref) + " / " + String(erro) + " / " + String(velocidade) + " / " + String(pwm_val)+ "/ " + String(saida_controle)); 
    // Serial.println(String(saida_controle,7) + " / " + String(saida_controle_anterior,7) + " / " + String(erro) + " / " + String(erro_anterior));
}

void interrupcao(){
    // leituraEncoder();
    calculaVelocidade();
    controladorVelocidade();
    // atualizarPWM();
    atualizarPWM2();
    contador_passos_motor = 0;
}

void atualizarPWM(){
    if (saida_controle <= 0){
        controlaMotor(1, 0, abs(saida_controle));
    }
    else{
        controlaMotor(0, 1, abs(saida_controle));
    }
}

void atualizarPWM2(){
    pwm_val = abs(saida_controle) * 255;

    if (pwm_val < DEAD_ZONE) {
        // Se o valor do PWM estiver dentro da zona morta, desligue o motor
        digitalWrite(MOTOR_PIN_1, LOW);
        digitalWrite(MOTOR_PIN_2, LOW);
        analogWrite(MOTOR_ENABLE, 0);
    } else {
        // Garanta que o valor do PWM seja pelo menos PWM_MIN
        if (pwm_val < PWM_MIN) {
            pwm_val = PWM_MIN;
        }
        pwm_val = (int)constrain(pwm_val,0,255);
        if (saida_controle < 0){
            controlaMotor(1, 0, pwm_val);
        }
        else{
            controlaMotor(0, 1, pwm_val);
        }
    }
}


void controladorVelocidade(){
    ref = mapFloat(analogRead(REFERENCIA_PIN), 0, 1020, 0, 1);
    // ref = mapFloat(analogRead(REFERENCIA_PIN), 0, 1020, -400, 400);
    erro = ref - velocidade;
    // saida_controle = erro * K;
    // saida_controle= a0  * erro + a1 * erro_anterior + saida_controle_anterior;
    // saida_controle = a0 * erro + a1 * erro_anterior + b1 * saida_controle_anterior;
    // saida_controle = K*(erro*(1+.134*INV_AMOSTRAGEM) + erro_anterior*(1-.134*INV_AMOSTRAGEM)) - saida_controle_anterior;
    saida_controle = K*(saida_controle_anterior + COEF_EQ_DIFERENCAS_POSICAO * erro + COEF_EQ_DIFERENCAS_POSICAO * erro_anterior);
    erro_anterior = erro;
    saida_controle_anterior = saida_controle;
}

void calculaVelocidade(){
    velocidade = contador_passos_motor * INV_RESOLUCAO * TWO_PI * INV_AMOSTRAGEM * 1000000;
    velocidade = velocidade/480.58;
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

void controlaMotor(bool in1, bool in2, int valor_pwm){
    digitalWrite(MOTOR_PIN_1, in1);
    digitalWrite(MOTOR_PIN_2, in2);
    analogWrite(MOTOR_ENABLE, valor_pwm);
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}