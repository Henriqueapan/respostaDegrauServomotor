#include <TimerOne.h>

#define chA 2 // Pino canal A do encoder
#define chB 3 // Pino canal B do encoder
// #define REFERENCIA_PIN A0
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define JANELA_MEDIA_MOVEL 1
#define RESOLUCAO_ENCODER 200 // Resolução do encoder (quantidade de passos que representa 1 volta completa)
#define PERIODO_AMOSTRAGEM 300 // Microssegundos
#define PERIODO_ACAO_DE_CONTROLE 200 // Microssegundos
#define INV_MICRO .000001
#define CONSTANTE_GANHO_CONTROLADOR .04

double INV_PERIODO_AMOSTRAGEM = 1.0/PERIODO_AMOSTRAGEM;
double INV_RESOLUCAO_ENCODER = 1.0/RESOLUCAO_ENCODER;
float INV_JANELA_MEDIA_MOVEL = 1.0/JANELA_MEDIA_MOVEL;
float posicao_referencia;
volatile int contador_passos_motor = 0;
volatile int chA_atual;
volatile int chB_atual;
volatile int chA_antigo = 0;
volatile int chB_antigo = 0;
volatile unsigned long tempo_atual_leitura_encoder;
volatile unsigned long tempo_anterior_leitura_encoder = 0;
volatile double velocidade_atual;
volatile double erro_atual = 0.0;
volatile double saida_controlador = 0.0;
double delta_tempo_leitura_encoder;
double velocidade_referencia;

enum DirecaoRotacaoMotor {
    ESQUERDA,
    DIREITA
};

void setup() {
    pinMode(chA, INPUT);
    pinMode(chB, INPUT);
    // pinMode(REFERENCIA_PIN, INPUT);
    pinMode(MOTOR_PIN_1, OUTPUT);
    pinMode(MOTOR_PIN_2, OUTPUT);
    pinMode(MOTOR_ENABLE, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder, CHANGE);

    Serial.begin(115200);

    posicao_referencia = DEG_TO_RAD*90;

    Timer1.initialize(PERIODO_ACAO_DE_CONTROLE);
    Timer1.attachInterrupt(realizaAcaoDeControle);

    velocidade_referencia = 300; // rad*s^-1
}

void loop() {
    Serial.println(String(velocidade_atual, 10) + "//" + String(saida_controlador, 10) + "//" + String(delta_tempo_leitura_encoder, 20));
}

void leituraEncoder(void) {
    /*
    Função que atualiza o contador de passos do eixo do motor
    e calcula a velocidade caso o período de amostragem já tenha
    passado
    */
    atualizaContagemDePassosMotor();

    tempo_atual_leitura_encoder = micros();
    delta_tempo_leitura_encoder = ((double)tempo_atual_leitura_encoder - (double)tempo_anterior_leitura_encoder);

    if(delta_tempo_leitura_encoder >= PERIODO_AMOSTRAGEM) { // Calcula a velocidade caso o período de amostragem já tenha ocorrido
        double mov_ang = ((double)contador_passos_motor*(double)INV_RESOLUCAO_ENCODER) * TWO_PI;
        velocidade_atual = (mov_ang/(double)delta_tempo_leitura_encoder) * 1000000;

        contador_passos_motor = 0;
        tempo_anterior_leitura_encoder = tempo_atual_leitura_encoder;
    }
}

void atualizaContagemDePassosMotor(void) {
    /*
    Função que atualiza (incrementa ou decrementa) o contador de passos do eixo do motor
    com base nos valores atuais e anteriores assumidos pelos canais A e B
    */
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

void realizaAcaoDeControle(void) {
    erro_atual = velocidade_atual - velocidade_referencia;

    saida_controlador = abs(erro_atual * CONSTANTE_GANHO_CONTROLADOR);

    controlaMotor(erro_atual > 0 ? ESQUERDA : DIREITA, saida_controlador * 255);
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

    analogWrite(MOTOR_ENABLE, valor_pwm);
}