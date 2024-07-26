#include <TimerOne.h>

#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder
#define REFERENCIA_PIN A0
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define RESOLUCAO_ENCODER 200 // Resolução do encoder (quantidade de passos que representa 1 volta completa)
#define PERIODO_AMOSTRAGEM 10000 // Microssegundos
// #define PERIODO_LEITURA_REFERENCIA 1000000 // Microssegundos
#define PERIODO_LEITURA_REFERENCIA 0 // Microssegundos
#define INV_MICRO .000001

#define PWM_MIN 70 // Valor mínimo de PWM para acionar o motor
#define DEAD_ZONE 13 

#define WINDOW_SIZE 25 // Tamanho da janela para a média móvel

volatile double buffer[WINDOW_SIZE] = {0};
volatile int bufferIndex = 0;
volatile double somaMedia = 0;

volatile unsigned long tempo_atual = 0;
volatile unsigned long tempo_anterior = 0;

volatile double velocidade = 0;
double PERIODO_AMOSTRAGEM_SEC = PERIODO_AMOSTRAGEM * INV_MICRO;
double INV_RESOLUCAO = 1.0/(double)RESOLUCAO_ENCODER;
double INV_AMOSTRAGEM = 1.0/(double)PERIODO_AMOSTRAGEM;
double INV_AMOSTRAGEM_SEC = 1.0/(double)PERIODO_AMOSTRAGEM_SEC;
double INV_LEITURA = 1.0/1020.0;

volatile double erro = 0;
volatile double ref = 0;
volatile double saida_controle = 0;

volatile double velocidade_anterior = 0;
volatile double erro_anterior = 0;
volatile double erro_ante_anterior = 0;

volatile double posicao = 0;
volatile double posicao_anterior = 0;
volatile double saida_controle_anterior = 0;
volatile double saida_controle_ante_anterior = 0;

volatile double in_anterior = 0;

volatile int contador_passos_motor = 0;
volatile int chA_atual;
volatile int chB_atual;
volatile int chA_antigo = 0;
volatile int chB_antigo = 0;

volatile int pwm_val = 0;

// double K = 0.02;
double K = 1.35;
double Kp = 0, Kd =0;

void setup() {
    Timer1.initialize(PERIODO_AMOSTRAGEM);
    Timer1.attachInterrupt(interrupcao);

    pinMode(chA, INPUT);
    pinMode(chB, INPUT);
    pinMode(REFERENCIA_PIN, INPUT);
    pinMode(MOTOR_PIN_1, OUTPUT);
    pinMode(MOTOR_PIN_2, OUTPUT);
    pinMode(MOTOR_ENABLE, OUTPUT);

    //   TCCR1B = TCCR1B & 0b11111000 | 1;
    attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder, CHANGE);
    Serial.begin(115200);
}

void loop() {

}

void interrupcao(){
    
    calculaVelocidade();

    integrador(velocidade);
 
    tempo_atual = micros();

    if (((tempo_atual - tempo_anterior) > PERIODO_LEITURA_REFERENCIA) || tempo_anterior == 0){
        ref = 75*DEG_TO_RAD;
        tempo_anterior = tempo_atual;
    }


    controladorPOS();
    // atualizarPWM();
    atualizarPWM2();
    erro = ref - posicao;
    Serial.println(String(0) + "," + String(tempo_atual) + "," + String(posicao, 5) + "," + String(0));
}

void atualizarPWM(){
    pwm_val = constrain(saida_controle*255,-254,254);
    if (saida_controle <= 0){
        controlaMotor(0, 1, abs(pwm_val));
    }
    else{
        controlaMotor(1, 0, abs(pwm_val));
    }
}

void atualizarPWM2(){
    pwm_val = saida_controle*255;

    if (abs(pwm_val) < DEAD_ZONE) {
        // Se o valor do PWM estiver dentro da zona morta, desligue o motor
        digitalWrite(MOTOR_PIN_1, 1);
        digitalWrite(MOTOR_PIN_2, 1);
        analogWrite(MOTOR_ENABLE, 255);
    } else {
        // Garanta que o valor do PWM seja pelo menos PWM_MIN
        if (abs(pwm_val) < PWM_MIN) {
            pwm_val = pwm_val < 0 ? - PWM_MIN : PWM_MIN;
        }
        pwm_val = constrain(pwm_val,-254,254);
        if (erro < 0){
            controlaMotor(0, 1, abs(pwm_val));
        }
        else{
            controlaMotor(1, 0, abs(pwm_val));
        }
    }
}

void controladorPOS(){
    erro = ref - posicao;
    
    // saida_controle = Kp*(erro + erro_anterior) + (2*Kd*INV_AMOSTRAGEM_SEC)*(erro - erro_anterior) - saida_controle_anterior; 

    saida_controle_ante_anterior = saida_controle_anterior;
    erro_ante_anterior = erro_anterior;
    saida_controle_anterior = saida_controle;
    erro_anterior = erro;
}

void calculaVelocidade(){
    velocidade = contador_passos_motor * INV_RESOLUCAO * TWO_PI * INV_AMOSTRAGEM_SEC;
    contador_passos_motor = 0;
}

void integrador(double vel_atual) {
//   posicao = posicao_anterior + COEF_EQ_DIFERENCAS_POSICAO * vel_atual + COEF_EQ_DIFERENCAS_POSICAO * velocidade_anterior;
  posicao = posicao_anterior + 0.0005 * vel_atual + 0.0005 * velocidade_anterior;
  if (posicao >= TWO_PI){
    posicao = posicao - TWO_PI;
  }
  else if(posicao <= -TWO_PI){
    posicao = posicao + TWO_PI;
  } 
    velocidade_anterior = velocidade;
    posicao_anterior = posicao;
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


