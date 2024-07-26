#include <TimerOne.h>
#include <PID_v1.h>

#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder
#define REFERENCIA_PIN A0 // Pino de leitura de referência do potenciômetro
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define RESOLUCAO_ENCODER 200 // Resolução do encoder (quantidade de passos que representa 1 volta completa)
#define PERIODO_AMOSTRAGEM 10000 // Microssegundos
#define INV_MICRO .000001
#define PERIODO_LEITURA_REFERENCIA 1000000 // Microssegundos

// Definições das limitações do motor
#define PWM_MIN 70 // Valor mínimo de PWM para acionar o motor
#define DEAD_ZONE 13 // Valor de duty cycle abaixo do qual é definida a zona morta

// Variáveis para medição de tempo
volatile unsigned long tempo_atual = 0;
volatile unsigned long tempo_anterior = 0;

// Constantes de calculo
double PERIODO_AMOSTRAGEM_SEC = PERIODO_AMOSTRAGEM * INV_MICRO;
double INV_RESOLUCAO = 1.0/(double)RESOLUCAO_ENCODER;
double INV_AMOSTRAGEM = 1.0/(double)PERIODO_AMOSTRAGEM;
double INV_AMOSTRAGEM_SEC = 1.0/(double)PERIODO_AMOSTRAGEM_SEC;
double INV_LEITURA = 1.0/1020.0;

// Variaveis de controle
volatile double ref = 0;
volatile double saida_controle = 0;

volatile double velocidade = 0;
volatile double velocidade_anterior = 0;

volatile double erro = 0;
volatile double erro_anterior = 0;
volatile double erro_ante_anterior = 0;

volatile double posicao = 0;
volatile double posicao_anterior = 0;

volatile double saida_controle_anterior = 0;
volatile double saida_controle_ante_anterior = 0;

// Variáveis da leitura do encoder
volatile int contador_passos_motor = 0;
volatile int chA_atual;
volatile int chB_atual;
volatile int chA_antigo = 0;
volatile int chB_antigo = 0;

volatile int pwm_val = 0;

// double K = 1.6973; // Ganho do compensador por avanço de fase

// Variáveis para biblioteca PID_v1
double Setpoint, Input, Output;

// Parâmetros PD
double Kp = 0.0439, Ki = 0, Kd = 0.0024; // PD

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, P_ON_E, DIRECT);

int contPrint = 0;

void setup() {
    Timer1.initialize(PERIODO_AMOSTRAGEM);
    Timer1.attachInterrupt(interrupcao);

    pinMode(chA, INPUT);
    pinMode(chB, INPUT);
    pinMode(REFERENCIA_PIN, INPUT);
    pinMode(MOTOR_PIN_1, OUTPUT);
    pinMode(MOTOR_PIN_2, OUTPUT);
    pinMode(MOTOR_ENABLE, OUTPUT);

    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(-1, 1);
    myPID.SetSampleTime(10);

    attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder, CHANGE);
    Serial.begin(115200);
}

void loop() {
    // Serial.println(String(erro,5) + "," + String(tempo_atual) + "," + String(posicao, 5) + "," + String(ref,5));
}

void interrupcao(){

    calculaVelocidade(); //Calcula a velocidade a partir da contagem dos passos do encoder

    integrador(velocidade); // Integra a velocidade calculada e encontra a posição

    tempo_atual = micros();

    // Atualiza a referência no período de sua atualização ou na primeira execução da rotina de interrupção
    if (((tempo_atual - tempo_anterior) > PERIODO_LEITURA_REFERENCIA) || tempo_anterior == 0){
        atualizaReferencia();
        tempo_anterior = tempo_atual;
    }

    // Função que define diferentes referências a cada 2 segundos.
    // Utilizada para geração de gráficos de respostas a variações de degrau.
    // refsExperimentais();
    
    controladorPOS(); // Função que controla a posição atualizando a variavel de saída do controlador

    atualizarPWM(); // Função que atualiza a razão cíclica do motor considerando a saturação e dead zone

    erro = ref - posicao;
}

void atualizarPWM(){
    pwm_val = saida_controle*255; // Duty cycle obtido da saída do controlador

    if (abs(pwm_val) < DEAD_ZONE) { // Correção de zona morta
        // Se o valor do PWM estiver dentro da zona morta, freia o motor
        digitalWrite(MOTOR_PIN_1, 1);
        digitalWrite(MOTOR_PIN_2, 1);
        analogWrite(MOTOR_ENABLE, 255);
    } else { // Correção de saturação
        // Caso o valor do duty cycle do PWM não esteja dentro da zona morta, garante
        // que o valor do duty cycle seja pelo menos PWM_MIN, garantindo o giro do motor
        if (abs(pwm_val) < PWM_MIN) {
            // Identifica se o duty cycle é positivo ou negativo para evitar
            // uma mudança não intencional de sinal na atribuição abaixo
            pwm_val = pwm_val < 0 ? - PWM_MIN : PWM_MIN;
        }

        // Restringe o valor do duty cycle ao intervalo -254 a 254
        pwm_val = constrain(pwm_val,-254,254);

        if (erro < 0){
            // Duty cycle negativo indica que o motor deve girar no sentido anti-horário
            controlaMotor(0, 1, abs(pwm_val));
        }
        else{
            // Duty cycle negativo indica que o motor deve girar no sentido horário
            controlaMotor(1, 0, abs(pwm_val));
        }
    }
}

void atualizaReferencia() {
    ref = analogRead(REFERENCIA_PIN) * INV_LEITURA * (5.8);
}

void controladorPOSLib(){
    Setpoint = ref;
    Input = posicao;

    myPID.Compute();

    saida_controle = Output;
}


void controladorPOS(){
    erro = ref - posicao;
    
    // CONTROLADOR DE AVANÇO DE FASE DUPLO
    // saida_controle = K*(erro - 1.6995*erro_anterior + 0.71022674*erro_ante_anterior)
    //                  + 1.0116*saida_controle_anterior - 0.25583364*saida_controle_ante_anterior;
    
    // CONTROLADOR PD
    saida_controle = Kp*(erro+erro_anterior) + (2*0.125*0.44*Kp*INV_AMOSTRAGEM_SEC)*(erro+erro_anterior)-saida_controle_anterior;

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

void refsExperimentais(){
    if (tempo_atual < (5 * 1000000)) {
        ref = 0.0000000000;  // DELAY ANTES DE COMEÇAR O EXPERIMENTO
    } else if (tempo_atual < (7 * 1000000)) {
        ref = 0.5235987755982989;  // REFERENCIA EM 30º NOS 2s DE EXPERIMENTO
    } else if (tempo_atual < (9 * 1000000)) {
        ref = 1.221730476396030;  // REFERENCIA EM 70º NOS 4s DE EXPERIMENTO
    } else if (tempo_atual < (11 * 1000000)) {
        ref = 2.879385241571817;  // REFERENCIA EM 165º NOS 6s DE EXPERIMENTO
    } else if (tempo_atual < (13 * 1000000)) {
        ref = 1.745329251994330;  // REFERENCIA EM 100º NOS 8s DE EXPERIMENTO
    } else if (tempo_atual < (15 * 1000000)) {
        ref = 5.410520681182422;  // REFERENCIA EM 310º NOS 10s DE EXPERIMENTO
    } else if (tempo_atual < (17 * 1000000)) {
        ref = 4.014257279127542;  // REFERENCIA EM 230º NOS 12s DE EXPERIMENTO
    } else if (tempo_atual < (19 * 1000000)) {
        ref = 0.785398163397448;  // REFERENCIA EM 45º NOS 14s DE EXPERIMENTO
    } else if (tempo_atual < (21 * 1000000)) {
        ref = 2.617993877991494;  // REFERENCIA EM 150º NOS 16s DE EXPERIMENTO
    } else if (tempo_atual < (23 * 1000000)) {
        ref = 4.712388980384690;  // REFERENCIA EM 270º NOS 18s DE EXPERIMENTO
    } else if (tempo_atual < (25 * 1000000)) {
        ref = 0.349065850398866;  // REFERENCIA EM 20º NOS 20s DE EXPERIMENTO
    } else {
        // Serial.end();
    }
}