#include <Encoder.h>
#include <PID_v1.h>
#include <TimerOne.h>

// Definição dos pinos
const int encoderPinA = 2;
const int encoderPinB = 3;
const int motorEnablePin = 5; // Pino PWM para controle de velocidade
const int motorIn1Pin = 7;   // Pino para controle de direção do motor
const int motorIn2Pin = 6;   // Pino para controle de direção do motor
const int potPin = A0;
int speed;
// Inicialização do encoder
Encoder myEncoder(encoderPinA, encoderPinB);

// Variáveis do sistema
volatile double position = 0;
volatile double previousPosition = 0;
volatile double velocity = 0;
volatile double previousVelocity = 0;
double reference = 0;
double input, output;

// Parâmetros do controlador PID
double Kp = 0.020595, Ki = 0.020837, Kd = 0.0029681;
PID myPID(&position, &output, &reference, Kp, Ki, Kd, 1);

// Constantes para integração
const double COEF_EQ_DIFERENCAS_POSICAO = 0.01; // PERIODO_AMOSTRAGEM/2

// Função de integração
void integrador(double vel_atual) {
  position = previousPosition + COEF_EQ_DIFERENCAS_POSICAO * vel_atual + COEF_EQ_DIFERENCAS_POSICAO * previousVelocity;
  if (position > TWO_PI) {
    position -= TWO_PI;
  }
  previousPosition = position;
  previousVelocity = vel_atual;
}

// Interrupção a cada 10 ms
void timerIsr() {
  long newEncoderPos = myEncoder.read();
  // Calcula a velocidade (diferencial da posição)
  velocity = newEncoderPos / 0.001; // rad/s

  // Integra a velocidade para obter a posição
  integrador(velocity);
  
  // Leitura do potenciômetro e mapeamento para referência de 0 a 2pi
  reference = map(analogRead(potPin), 0, 1023, 0, 2 * PI);
  
  // Computa o PID
  myPID.Compute();
  
  // Controle do motor usando a saída do PID
  if (output > 0) {
    digitalWrite(motorIn1Pin, HIGH);
    digitalWrite(motorIn2Pin, LOW);
  } else {
    digitalWrite(motorIn1Pin, LOW);
    digitalWrite(motorIn2Pin, HIGH);
  }
  
  // Ajusta a velocidade do motor
  // speed = map(abs(output), 0, 1, 50, 255);
  speed = constrain(output*255,0,254);
  analogWrite(motorEnablePin, speed);
  myEncoder.write(0);
}

void setup() {
  // Inicialização dos pinos do motor
  pinMode(motorEnablePin, OUTPUT);
  pinMode(motorIn1Pin, OUTPUT);
  pinMode(motorIn2Pin, OUTPUT);
  
  // Inicialização do PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetSampleTime(1); // 10 ms
  
  // Inicialização do Timer1 para interrupção
  Timer1.initialize(1000); // 10 ms
  Timer1.attachInterrupt(timerIsr);
  Serial.begin(115200);
}

void loop() {
  // Nada para fazer no loop principal
  Serial.println(speed);
}
