// "sketch": "servoWithPID\\SpeedControl\\SpeedControl.ino"

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
double INV_RESOLUCAO = 1.0/(double)RESOLUCAO_ENCODER;
double INV_AMOSTRAGEM = 1.0/(double)PERIODO_AMOSTRAGEM;
volatile double erro = 0;
volatile double ref = 0;
volatile double saida_controle = 0;
double K = 0.036899;

double COEF_EQ_DIFERENCAS_POSICAO = (PERIODO_AMOSTRAGEM/2) * INV_MICRO;
volatile double velocidade_anterior = 0;
volatile double velocidade_ante_anterior = 0;
volatile double erro_anterior = 0;
volatile double erro_ante_anterior = 0;

volatile double posicao = 0;
volatile double posicao_anterior = 0;
volatile double posicao_ante_anterior = 0;
volatile double saida_controle_anterior = 0;

enum DirecaoRotacaoMotor {
    ESQUERDA,
    DIREITA
};

enum DirecaoRotacaoMotor direcao;

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
    Serial.println(String(ref) + " / " + String(erro) + " / " + String(velocidade) + " / " + String(saida_controle));
}

void interrupcao(){
    leituraEncoder();
    integrador(velocidade);
    controladorPOS2();
    // controladorPOS();
    // controladorVEL();
    atualizarPWM();
    atualizarMemorias();
}

void leituraEncoder(){
    contador_passos = myEnc.read();
    velocidade = contador_passos * INV_RESOLUCAO * TWO_PI * INV_AMOSTRAGEM * 1000000;
}   

void controladorVEL(){
    ref = mapFloat(analogRead(REFERENCIA_PIN), 0, 1020, 0, 500);
    erro = ref - velocidade;
    saida_controle = 255 * erro * K ;
}

void controladorPOS(){
    ref = mapFloat(analogRead(REFERENCIA_PIN), 0, 1020, 0, TWO_PI);
    erro = ref - posicao;
    saida_controle =  erro * K ;
}

void controladorPOS2(){
    ref = mapFloat(analogRead(REFERENCIA_PIN), 0, 1020, 0, TWO_PI);
    erro = ref - posicao;  
    // saida_controle = 0.567* K * PERIODO_AMOSTRAGEM * erro + 0.433 * K * PERIODO_AMOSTRAGEM * erro_anterior - saida_controle_anterior;
    // saida_controle = (0.24237 * (1 + 0.134 * INV_AMOSTRAGEM) * posicao + 0.24237 * (1 - 0.134 * INV_AMOSTRAGEM) * posicao_anterior) - saida_controle_anterior;
    saida_controle = K * ((1 + 0.22 * INV_AMOSTRAGEM) * erro + (1 - 0.22 * INV_AMOSTRAGEM) * erro_anterior) - saida_controle_anterior;
}

void atualizarPWM(){
    saida_controle = saida_controle * 255;
    if (saida_controle <= 0){
        direcao = DirecaoRotacaoMotor::ESQUERDA;
        controlaMotor(direcao, abs(saida_controle));
    }
    else{
        direcao = DirecaoRotacaoMotor::DIREITA;
        controlaMotor(direcao, abs(saida_controle));
    }
}

void atualizarMemorias(){
    velocidade_anterior = velocidade;
    posicao_anterior = posicao;
    saida_controle_anterior = saida_controle;
    // velocidade_ante_anterior = velocidade_anterior;
    erro_anterior = erro;
    // erro_ante_anterior = erro_anterior;
    myEnc.write(0);
}


float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void controlaMotor(enum DirecaoRotacaoMotor direcao, int valor_pwm) {
    switch (direcao)
    {
        case DirecaoRotacaoMotor::DIREITA:
            digitalWrite(MOTOR_PIN_1, 0);
            digitalWrite(MOTOR_PIN_2, 1);
            break;
        case DirecaoRotacaoMotor::ESQUERDA:
            digitalWrite(MOTOR_PIN_1, 1);
            digitalWrite(MOTOR_PIN_2, 0);
            break;
    }
    valor_pwm = constrain(abs(valor_pwm), 0, 255);
    analogWrite(MOTOR_ENABLE, valor_pwm);
}

void integrador(double vel_atual) {
  posicao = posicao_anterior + COEF_EQ_DIFERENCAS_POSICAO * vel_atual + COEF_EQ_DIFERENCAS_POSICAO * velocidade_anterior;
  if (posicao > TWO_PI){
    posicao = posicao - TWO_PI;
  } 
}
