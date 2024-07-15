#include <TimerOne.h>

#define chA 2 // Pino canal A do encoder
#define chB 3 // Pino canal B do encoder
#define REFERENCIA_PIN A0
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define RESOLUCAO_ENCODER 200 // Resolução do encoder (quantidade de passos que representa 1 volta completa)
#define PERIODO_AMOSTRAGEM 10000 // Microssegundos
#define INV_MICRO .000001

#define PWM_MIN 60 // Valor mínimo de PWM para acionar o motor
#define DEAD_ZONE 10 

#define WINDOW_SIZE 10 // Tamanho da janela para a média móvel

volatile double buffer[WINDOW_SIZE] = {0};
volatile int bufferIndex = 0;
volatile double somaMedia = 0;

volatile double velocidade = 0;
double PERIODO_AMOSTRAGEM_SEC = PERIODO_AMOSTRAGEM * INV_MICRO;
double INV_teste = 1.0/(PERIODO_AMOSTRAGEM+0.0076454);
double INV_RESOLUCAO = 1.0/(double)RESOLUCAO_ENCODER;
double INV_AMOSTRAGEM = 1.0/(double)PERIODO_AMOSTRAGEM;
double INV_AMOSTRAGEM_SEC = 1.0/(double)PERIODO_AMOSTRAGEM_SEC;
double INV_LEITURA = 1.0/1020.0;
volatile double erro = 0;
volatile double ref = 0;
volatile double saida_controle = 0;
double K = 1;

double COEF_EQ_DIFERENCAS_POSICAO = (PERIODO_AMOSTRAGEM/2) * INV_MICRO;
volatile double velocidade_anterior = 0;
volatile double erro_anterior = 0;

volatile double posicao = 0;
volatile double posicao_anterior = 0;
volatile double saida_controle_anterior = 0;

volatile int contador_passos_motor = 0;
volatile int contador_passos_motor_medio = 0;
volatile int chA_atual;
volatile int chB_atual;
volatile int chA_antigo = 0;
volatile int chB_antigo = 0;

volatile int pwm_val = 0;

// const float a0 = K * 0.24237 * (0.134 / PERIODO_AMOSTRAGEM);
// const float a1 = K * 0.24237;
// const float b1 = (2 - PERIODO_AMOSTRAGEM) / (2 + PERIODO_AMOSTRAGEM);

void setup() {
  Timer1.initialize(PERIODO_AMOSTRAGEM);
  Timer1.attachInterrupt(interrupcao);

  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  pinMode(REFERENCIA_PIN, INPUT);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  
//   attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder2, CHANGE);
//   attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder2, CHANGE);
  attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder, CHANGE);
  Serial.begin(115200);
}

void loop() {
    // Serial.println(String(contador_passos_motor));
    // Serial.println(String(contador_passos_motor_medio));
    // Serial.println("ref: " + String(ref) + " / " + "erro: " +  String(erro) + " / " + "vel: " +  String(velocidade) + " / " + "PWM: " + String(saida_controle));
    // Serial.println(String(analogRead(REFERENCIA_PIN)) + " / " + String(erro) + " / " + String(velocidade) + " / " + String(pwm_val)+ "/ " + String(saida_controle)); 
    // Serial.println(String(saida_controle,7) + " / " + String(saida_controle_anterior,7) + " / " + String(erro) + " / " + String(erro_anterior));
}

void interrupcao(){
    // contador_passos_motor_medio = mediaMovel(contador_passos_motor);
    // Serial.println(String(contador_passos_motor));
    // Serial.println(String(contador_passos_motor_medio));
    calculaVelocidade();
    // Serial.println(contador_passos_motor);
    Serial.println(String(velocidade));
    // velocidade = mediaMovelVelocidade(velocidade);
    controladorVelocidade();
    // Serial.println(saida_controle);
    // Serial.println(erro);
    atualizarPWM();
    // atualizarPWM2();
    // controlaMotor(0,1,254);
    contador_passos_motor = 0;
}

void atualizarPWM(){
    // pwm_val = saida_controle;
    // pwm_val = saida_controle*255;
    // pwm_val = mapFloat(saida_controle,0, 5, 50,254);
    pwm_val = constrain(saida_controle,0,254);
    if (saida_controle <= 0){
        controlaMotor(1, 0, abs(pwm_val));
    }
    else{
        controlaMotor(0, 1, abs(pwm_val));
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
    // ref = mapFloat((double)analogRead(REFERENCIA_PIN), 0.0, 1020.0, 0.0, 500);
    // ref = mapFloat(analogRead(REFERENCIA_PIN), 0, 1020, -400, 400);
    ref = analogRead(REFERENCIA_PIN) * INV_LEITURA * 500;
    erro = ref - velocidade;
    // saida_controle = erro * K;
    // saida_controle= a0  * erro + a1 * erro_anterior + saida_controle_anterior;
    // saida_controle = a0 * erro + a1 * erro_anterior + b1 * saida_controle_anterior;
    // saida_controle = K*(erro*(1+.134*INV_AMOSTRAGEM) + erro_anterior*(1-.134*INV_AMOSTRAGEM)) - saida_controle_anterior;
    // saida_controle = K*(saida_controle_anterior + COEF_EQ_DIFERENCAS_POSICAO * erro + COEF_EQ_DIFERENCAS_POSICAO * erro_anterior);
    // saida_controle = (erro*(0.00525337-0.039268*PERIODO_AMOSTRAGEM)+erro_anterior*(-0.00525337+0.039268*PERIODO_AMOSTRAGEM)
    // -saida_controle_anterior*(PERIODO_AMOSTRAGEM-0.0076454))*INV_teste;
    // saida_controle = K*(0.9868*erro - 0.9868*erro_anterior + 0.7698*saida_controle_anterior);
    saida_controle = 0.7482*erro -0.64*erro_anterior +0.9353*saida_controle_anterior;
    erro_anterior = erro;
    saida_controle_anterior = saida_controle;
}

void calculaVelocidade(){
    velocidade = (double)contador_passos_motor * INV_RESOLUCAO * TWO_PI * INV_AMOSTRAGEM_SEC;
    // velocidade = velocidade/480.58;
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

    int contador_passos_motor_interna = 0;

void leituraEncoder2() {

    int chA_atual = digitalRead(chA);
    int chB_atual = digitalRead(chB);
    int contador_passos_motor_interna = 0;

    if (chA_antigo == 0 && chB_antigo == 0) {
        if(chA_atual == 1 && chB_atual == 1) contador_passos_motor_interna = contador_passos_motor_interna + 2;
        else if(chA_atual == 1 && chB_atual == 0) contador_passos_motor_interna--;
        else if(chA_atual == 0 && chB_atual == 1) contador_passos_motor_interna++;
    }
    else if (chA_antigo == 0 && chB_antigo == 1) {
        if(chA_atual == 1 && chB_atual == 1) contador_passos_motor_interna ++;
        else if(chA_atual == 1 && chB_atual == 0) contador_passos_motor_interna = contador_passos_motor_interna - 2;
        else if(chA_atual == 0 && chB_atual == 1) contador_passos_motor_interna = contador_passos_motor_interna;
        else contador_passos_motor_interna--;
    }
    else if (chA_antigo == 1 && chB_antigo == 0) {
        if(chA_atual == 1 && chB_atual == 1) contador_passos_motor_interna--;
        else if(chA_atual == 1 && chB_atual == 0) contador_passos_motor_interna = contador_passos_motor_interna;
        else if(chA_atual == 0 && chB_atual == 1) contador_passos_motor_interna = contador_passos_motor_interna - 2;
        else contador_passos_motor_interna ++;
    }
    else if (chA_antigo = 1 && chB_antigo == 1) {
        if(chA_atual == 1 && chB_atual == 1) contador_passos_motor_interna = contador_passos_motor_interna;
        else if(chA_atual == 1 && chB_atual == 0) contador_passos_motor_interna++;
        else if(chA_atual == 0 && chB_atual == 1) contador_passos_motor_interna--;
        else contador_passos_motor_interna = contador_passos_motor_interna + 2;
    }

    chA_antigo = chA_atual;
    chB_antigo = chB_atual;
    contador_passos_motor = contador_passos_motor + contador_passos_motor_interna;
}


void controlaMotor(bool in1, bool in2, int valor_pwm){
    digitalWrite(MOTOR_PIN_1, in1);
    digitalWrite(MOTOR_PIN_2, in2);
    analogWrite(MOTOR_ENABLE, valor_pwm);
}

// Função para calcular a média móvel da velocidade
double mediaMovel(double variavel) {
    // Subtrai o valor mais antigo da soma
    somaMedia -= buffer[bufferIndex];

    // Substitui o valor mais antigo pelo novo valor
    buffer[bufferIndex] = variavel;

    // Adiciona o novo valor à soma
    somaMedia += variavel;

    // Atualiza o índice do buffer, garantindo que ele volte ao início quando atingir o tamanho da janela
    bufferIndex = (bufferIndex + 1) % WINDOW_SIZE;

    // Calcula a média móvel
    return somaMedia / WINDOW_SIZE;
}
