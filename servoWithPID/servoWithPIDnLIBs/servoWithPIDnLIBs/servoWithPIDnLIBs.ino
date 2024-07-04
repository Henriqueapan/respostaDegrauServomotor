#include <TimerOne.h>
#include <Encoder.h>

#define chA 2 // Pino canal A do encoder
#define chB 3 // Pino canal B do encoder
// #define REFERENCIA_PIN A0
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define JANELA_MEDIA_MOVEL 1
#define RESOLUCAO_ENCODER 200 // Resolução do encoder (quantidade de passos que representa 1 volta completa)
#define PERIODO_AMOSTRAGEM 200 // Microssegundos
#define INV_MICRO .000001

Encoder myEnc(chA, chB);

float INV_PERIODO_AMOSTRAGEM = 1.0/PERIODO_AMOSTRAGEM;
float INV_JANELA_MEDIA_MOVEL = 1.0/JANELA_MEDIA_MOVEL;
float INV_RESOLUCAO_ENCODER = 1.0/RESOLUCAO_ENCODER;
float COEF_EQ_DIFERENCAS_POSICAO = (PERIODO_AMOSTRAGEM/2) * INV_MICRO;
int direcao_rotacao_atual;
volatile int contador_passos_motor = 0;
volatile int contador_idx_vetor_velocidade = 0;
volatile double delta_tempo;
volatile double arr_velocidade[JANELA_MEDIA_MOVEL] = {0};
volatile double velocidade = 0.0;
volatile double velocidade_anterior = 0.0;
volatile double tempo_atual;
volatile double tempo_anterior = 0.;
volatile double posicao_atual = 0.0;
volatile double posicao_anterior = 0.0;
volatile float saida_controlador_atual = 0.0;
volatile float saida_controlador_anterior = 0.0;
volatile float saida_controlador_ante_anterior = 0.0;
bool existe_saida_anterior = false;
volatile float erro_atual = 0.0;
volatile float erro_anterior = 0.0;
volatile float erro_ante_anterior = 0.0;
bool existe_erro_anterior = false;
volatile float posicao_referencia;
int idx_velocidade;

void setup() {
  Timer1.initialize(PERIODO_AMOSTRAGEM);
  Timer1.attachInterrupt(calculaERegistraVel);

  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  // pinMode(REFERENCIA_PIN, INPUT);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  
  Serial.begin(115200);
  
  // Inicialiando vetor de velocidade para média móvel
  for(int j = 0; j < JANELA_MEDIA_MOVEL; j++) {
    arr_velocidade[j] = 0;
  }

  // Definindo valor de teste para posicao_referencia
  posicao_referencia = DEG_TO_RAD*125;
}

void loop() {
  // Não deve ser necessário obter o tempo atual no loop. Isso é feito no interrupt do Timer1.
  // Aqui isso é feito apenas para fins de testes, visando limitar o intervalo de tempo de teste do motor
  // e evitando desgaste desnecessário do rolamento.
  tempo_atual = micros(); // Atualiza o quanto tempo se passou desde o início
  calculaSaidaControladorAtual();

  direcao_rotacao_atual = saida_controlador_atual >= 0 ? 0 : 1;

  controlaMotor(direcao_rotacao_atual, abs(saida_controlador_atual));

  // if (tempo_atual <= .005 * 1000000) {
  //   controlaMotor(0, 1, 255);
  //   // Serial.println(String(RAD_TO_DEG*posicao_atual, 5));
  //   Serial.println(String(saida_controlador_atual, 7));
  // }
  // else { // Desliga o motor e para de enviar dados
  //   controlaMotor(0,0,0);
  //   Serial.println(String(saida_controlador_atual, 7));
  //   // Serial.end();
  // }
}

void calculaERegistraVel(void) {
  if(contador_idx_vetor_velocidade >= 1000*JANELA_MEDIA_MOVEL) contador_idx_vetor_velocidade = 0;

  contador_passos_motor = myEnc.read(); //Lendo encoder
  double mov_ang = contador_passos_motor * INV_RESOLUCAO_ENCODER * 2 * PI; // Calculando movimento angular

  idx_velocidade = (int)contador_idx_vetor_velocidade % JANELA_MEDIA_MOVEL;

  arr_velocidade[idx_velocidade] = mov_ang * INV_PERIODO_AMOSTRAGEM;  //(delta_tempo); armazenando velocidade no buffer

  contador_idx_vetor_velocidade++; //incrementando indice

  velocidade = 0.;

  for(int i = 0; i < JANELA_MEDIA_MOVEL; i++) {
    velocidade += abs(arr_velocidade[i]);
  }

  // velocidade = velocidade * INV_JANELA_MEDIA_MOVEL * 1000000;
  velocidade = velocidade * INV_JANELA_MEDIA_MOVEL * 1000000;

  registraPosicaoAtual(velocidade);

  velocidade_anterior = velocidade;

  myEnc.write(0);
}

void controlaMotor(int pos, float pwm) {
  digitalWrite(MOTOR_PIN_1, pos == 0 ? 1 : 0);
  digitalWrite(MOTOR_PIN_2, pos == 0 ? 0 : 1);
  analogWrite(MOTOR_ENABLE, pwm);
}

void registraPosicaoAtual(double vel_atual) {
  posicao_atual = posicao_anterior + COEF_EQ_DIFERENCAS_POSICAO*vel_atual + COEF_EQ_DIFERENCAS_POSICAO*velocidade_anterior;

  if (posicao_atual > TWO_PI) posicao_atual = posicao_atual - TWO_PI;

  posicao_anterior = posicao_atual;
}

void calculaSaidaControladorAtual() {
  erro_atual = posicao_atual - posicao_referencia;

  saida_controlador_atual =
    1.997 * saida_controlador_anterior
    - 0.997 * saida_controlador_ante_anterior
    + 0.0000328 * erro_atual
    + 0.0000656 * erro_ante_anterior
    + 0.0000328 * erro_ante_anterior;

  if (abs(saida_controlador_atual) >= 255)
    saida_controlador_atual = saida_controlador_atual > 0 ? 255 : -255;

  // if (!existe_saida_anterior) saida_controlador_anterior = saida_controlador_atual;

  saida_controlador_ante_anterior = saida_controlador_anterior;
  saida_controlador_anterior = saida_controlador_atual;

  // if (!existe_erro_anterior) erro_anterior = erro_atual;

  erro_ante_anterior = erro_anterior;
  erro_anterior = erro_atual;
}