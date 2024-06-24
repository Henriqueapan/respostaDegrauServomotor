#include <TimerOne.h>
#include <Encoder.h>

#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define JANELA_MEDIA_MOVEL 50
#define RESOLUCAO_ENCODER 200 // Resolução do encoder (quantidaed de passos que representa 1 volta completa)
#define PERIODO_AMOSTRAGEM 1000 // Microssegundos

Encoder myEnc(chA, chB);

float INV_PERIODO_AMOSTRAGEM = 1.0/PERIODO_AMOSTRAGEM;
float INV_JANELA_MEDIA_MOVEL = 1.0/JANELA_MEDIA_MOVEL;
float INV_RESOLUCAO_ENCODER = 1.0/RESOLUCAO_ENCODER;
volatile int contador_passos_motor = 0;
volatile double delta_tempo;
volatile double arr_velocidade[JANELA_MEDIA_MOVEL] = {0};
volatile double velocidade = 0.0;
int idx_velocidade = 0;

void setup() {
  Timer1.initialize(PERIODO_AMOSTRAGEM);
  Timer1.attachInterrupt(calculaERegistraVel);

  pinMode(chA, INPUT);
  pinMode(chB, INPUT);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  
  Serial.begin(115200);
}

void loop() {
  // Não deve ser necessário obter o tempo atual no loop. Isso é feito no interrupt do Timer1
  tempo_atual = micros(); // Atualiza o quanto tempo se passou desde o início

  if (tempo_atual <= 5 * 1000000){  // Condicional para acionar o motor por apenas 5 segundos
    controlaMotor(0,1,255); // Fazendo o motor girar no sentido horário na velocidade 255
    Serial.println(velocidade);
  }
  else{ // Desliga o motor e para de enviar dados
    controlaMotor(0,0,0); 
    Serial.end();
  }
}

void calculaERegistraVel(void) {
  contador_passos_motor = myEnc.read(); //Lendo encoder
  double mov_ang = contador_passos_motor * INV_RESOLUCAO_ENCODER * 2 * PI; // Calculando mobimento angular
  arr_velocidade[idx_velocidade] = mov_ang * INV_PERIODO_AMOSTRAGEM;  //(delta_tempo); armazenando velocidade no buffer

  idx_velocidade++; //incrementando indice

  if (idx_velocidade >= JANELA_MEDIA_MOVEL) {   // Verificando se o indice é maior que a janela
    double soma_velocidade = 0.0;           
    for (int i = 0; i < JANELA_MEDIA_MOVEL; i++) {
      soma_velocidade += arr_velocidade[i];  // fazendo somatório
    }
    velocidade = soma_velocidade / JANELA_MEDIA_MOVEL; // Calculando a velocidade media

    // Reiniciar o índice circularmente
    idx_velocidade = idx_velocidade % JANELA_MEDIA_MOVEL;
  }

  myEnc.write(0);
}

void controlaMotor(bool in1, bool in2, float pwm) {
  digitalWrite(MOTOR_PIN_1, in1);
  digitalWrite(MOTOR_PIN_2, in2);
  analogWrite(MOTOR_ENABLE, pwm);
}
