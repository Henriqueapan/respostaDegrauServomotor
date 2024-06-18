#include <TimerOne.h>

#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define JANELA_MEDIA_MOVEL 50
#define RESOLUCAO_ENCODER 200 // Resolução do encoder (quantidaed de passos que representa 1 volta completa)
#define INV_MICRO 1/1000000
#define PERIODO_AMOSTRAGEM 100 // Microssegundos

float INV_PERIODO_AMOSTRAGEM = 1/PERIODO_AMOSTRAGEM;
float INV_JANELA_MEDIA_MOVEL = 1/JANELA_MEDIA_MOVEL;
float INV_RESOLUCAO_ENCODER = 1/RESOLUCAO_ENCODER;
volatile int contador_passos_motor = 0;
volatile int contador_passos_motor_anterior = 0;
unsigned long tempo_atual;
unsigned long tempo_anterior = 0;
int interrupcao_atual = 0;
int chA_antigo = 0;
int chB_antigo = 0;
volatile double delta_tempo;
volatile double arr_velocidade[JANELA_MEDIA_MOVEL];
volatile double velocidade = 0;
int contador_pos_vetor_velocidade = 0;
bool motor_ligado = false;

void setup() {
  Timer1.initialize(1000);
  Timer1.attachInterrupt(calculaERegistraVel);

  // Inicialiando vetor de velocidade para média móvel
  for(int j = 0; j < JANELA_MEDIA_MOVEL; j++) {
    arr_velocidade[j] = 0;
  }

  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder, CHANGE);
  Serial.begin(115200);

}

void loop() {
  // Não deve ser necessário obter o tempo atual no loop. Isso é feito no interrupt do Timer1
  tempo_atual = micros(); // Atualiza o quanto tempo se passou desde o início

  if (tempo_atual <= 3 * 1000000){  // Condicional para acionar o motor por apenas 15 segundos

    controlaMotor(0,1,255); // Fazendo o motor girar no sentido horário na velocidade 255

    // Velocidade calculada por média móvel
    for (int i = 0; i < JANELA_MEDIA_MOVEL; i++) {
      // Serial.println(String(arr_velocidade[i]));
      velocidade += abs(arr_velocidade[i]);
      Serial.println(String(abs(arr_velocidade[i])));
      // Serial.println(String(arr_velocidade[i]));
    }
    // Serial.println(String(velocidade, 7));
    // Serial.println("Fim");
    velocidade = velocidade * INV_JANELA_MEDIA_MOVEL;
    
    // Imprime na saída serial a situação atual do vetor velocidade
    Serial.println(String(velocidade, 15));

    velocidade = 0;

    // Aplica a velocidade (obtida da média móvel no vetor double velocidade) a entrada com PID
  }
  else{ // Desliga o motor e para de enviar dados
    controlaMotor(0,0,0); 
    Serial.end();
  }

  delay(1); 
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

void calculaERegistraVel(void) {
  // Serial.println("Reached");
  if (contador_pos_vetor_velocidade == 1000) contador_pos_vetor_velocidade = 0;

  // TODO: experimentar usar a frequencia definida do Timer1 ao inves do delta_tempo
  double mov_ang = contador_passos_motor * INV_RESOLUCAO_ENCODER * 2 * PI;

  // tempo_atual = micros();
  // delta_tempo = (tempo_atual - tempo_anterior) * INV_MICRO;

  int idx_velocidade = (int)contador_pos_vetor_velocidade % JANELA_MEDIA_MOVEL;
  // Serial.println(String((double)PERIODO_AMOSTRAGEM*INV_MICRO, 10));
  arr_velocidade[idx_velocidade] = mov_ang * ((double)PERIODO_AMOSTRAGEM * INV_MICRO);//(delta_tempo);

  // tempo_anterior = tempo_atual;
  contador_passos_motor = 0;
  contador_pos_vetor_velocidade++;
}

void controlaMotor(bool in1, bool in2, float pwm) {
  digitalWrite(MOTOR_PIN_1, in1);
  digitalWrite(MOTOR_PIN_2, in2);
  analogWrite(MOTOR_ENABLE, pwm);
}
