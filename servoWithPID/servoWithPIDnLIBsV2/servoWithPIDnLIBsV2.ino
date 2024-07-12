// "sketch": "servoWithPID\\servoWithPIDnLIBsV2\\servoWithPIDnLIBsV2.ino"

#include <TimerOne.h>

#define chA 2 // Pino canal A do encoder
#define chB 3 // Pino canal B do encoder
#define REFERENCIA_PIN A0
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define JANELA_MEDIA_MOVEL 1
#define RESOLUCAO_ENCODER 200 // Resolução do encoder (quantidade de passos que representa 1 volta completa)
#define PERIODO_AMOSTRAGEM 300 // Microssegundos
#define PERIODO_ACAO_DE_CONTROLE 1000 // Microssegundos
#define INV_MICRO .000001
#define CONSTANTE_GANHO_CONTROLADOR 0.05

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

double COEF_EQ_DIFERENCAS_POSICAO = (PERIODO_AMOSTRAGEM/2) * INV_MICRO;
volatile double velocidade_anterior = 0;
volatile double erro_anterior = 0;
volatile double erro_ante_anterior = 0;
volatile double posicao = 0;
volatile double posicao_anterior = 0;
volatile double saida_controle_anterior = 0;


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

    //posicao_referencia = DEG_TO_RAD*90;

    Timer1.initialize(PERIODO_ACAO_DE_CONTROLE);
    Timer1.attachInterrupt(realizaAcaoDeControle);

    velocidade_referencia = 500; // rad*s^-1
}

void loop() {
    Serial.println(String(erro_atual) + "//" + String(velocidade_atual) + "//" + String(saida_controlador*255,3));
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
        integrador(velocidade_atual);
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
    // erro_atual = velocidade_atual - velocidade_referencia;
    posicao_referencia = mapFloat(analogRead(REFERENCIA_PIN), 0, 1020, 0, TWO_PI);
    erro_atual = posicao - posicao_referencia;
    //saida_controlador = abs(erro_atual * CONSTANTE_GANHO_CONTROLADOR);
    // saida_controlador = 0.567* CONSTANTE_GANHO_CONTROLADOR * PERIODO_AMOSTRAGEM * erro_atual + 0.433 * CONSTANTE_GANHO_CONTROLADOR * PERIODO_AMOSTRAGEM * erro_anterior - saida_controle_anterior;
    // saida_controle_anterior = saida_controlador;
    // saida_controlador = CONSTANTE_GANHO_CONTROLADOR * ((1 + 0.22 * INV_PERIODO_AMOSTRAGEM) * erro_atual + (1 - 0.22 * INV_PERIODO_AMOSTRAGEM) * erro_anterior) - saida_controle_anterior;
    // saida_controlador = 0.273 * saida_controle_anterior + 11.4 * erro_atual - 9.14 * erro_anterior;
    // saida_controlador = CONSTANTE_GANHO_CONTROLADOR*(saida_controle_anterior + COEF_EQ_DIFERENCAS_POSICAO*erro_atual + COEF_EQ_DIFERENCAS_POSICAO*erro_anterior);
    saida_controle_anterior = saida_controlador;
    erro_anterior = erro_atual;
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

void integrador(double vel_atual) {
  posicao = posicao_anterior + COEF_EQ_DIFERENCAS_POSICAO*vel_atual + COEF_EQ_DIFERENCAS_POSICAO*velocidade_anterior;

  if (posicao > TWO_PI) posicao = posicao - TWO_PI;

  posicao_anterior = posicao;
  velocidade_anterior = vel_atual;
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}