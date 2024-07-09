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

float INV_PERIODO_AMOSTRAGEM = 1/PERIODO_AMOSTRAGEM;
float INV_JANELA_MEDIA_MOVEL = 1/JANELA_MEDIA_MOVEL;
float INV_RESOLUCAO_ENCODER = 1/RESOLUCAO_ENCODER;
float posicao_referencia;
volatile int contador_passos_motor = 0;
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

    attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, RISING);

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
    Função que incrementa o contador de passos do eixo do motor
    e calcula a velocidade caso o período de amostragem já tenha
    passado
    */
    contador_passos_motor ++;

    tempo_atual_leitura_encoder = micros();
    delta_tempo_leitura_encoder = ((double)tempo_atual_leitura_encoder - (double)tempo_anterior_leitura_encoder);

    if(delta_tempo_leitura_encoder >= PERIODO_AMOSTRAGEM) { // Calcula a velocidade caso o período de amostragem já tenha ocorrido
        double mov_ang = ((double)contador_passos_motor*(double)INV_RESOLUCAO_ENCODER) * TWO_PI;
        velocidade_atual = (mov_ang/(double)delta_tempo_leitura_encoder) * INV_MICRO;

        tempo_anterior_leitura_encoder = tempo_atual_leitura_encoder;
    }
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