#include <Encoder.h>

// Define os pinos do motor e do encoder
#define PWM_FREQ 10// Frequência em Hertz
#define AMPLITUDE 127  // Amplitude máxima do sinal PWM (0 a 255)
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder

Encoder myEncoder(chA, chB);

volatile float valor_pwm = 0;
unsigned long tempoAtual = 0;
unsigned long tempoAnterior = 0;
double arg;
double inv_micro = 1 / 1000000.0;
long encoderValue = 0;
long previousEncoderValue = 0;

void setup() {
  TCCR1B = (TCCR1B & 0xF8) | 0x01; // Configura a frequência PWM para 31.37kHz
  // Configuração dos pinos do motor
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  
  // Configuração da frequência PWM
  Serial.begin(115200);
}

void loop() {
  tempoAtual = micros();
  if (tempoAtual <= 30 * 1000000) {
    arg = TWO_PI * PWM_FREQ * double(tempoAtual) * inv_micro;
    valor_pwm = ((127 / 2) * sin(arg)) + 1.5 * 127;

    // Escrevendo o valor PWM no pino
    controlaMotor(0, 1, valor_pwm);

    // Lê o valor atual do encoder
    encoderValue = myEncoder.read();

    // Calcula a variação da contagem do encoder
    long deltaEncoderValue = encoderValue - previousEncoderValue;

    // Imprime a variação da contagem do encoder
    Serial.println(String(deltaEncoderValue) + ", " + String((tempoAtual - tempoAnterior)) + ", " + String(valor_pwm));
    
    // Atualiza o valor anterior do encoder
    previousEncoderValue = encoderValue;
    
    // Atualiza o tempo anterior
    tempoAnterior = tempoAtual;
  } else { // Desliga o motor e para de enviar
    controlaMotor(0, 0, 0); 
    Serial.end();
  }
  //delay(10);
}

void controlaMotor(bool in1, bool in2, float pwm) {
  digitalWrite(MOTOR_PIN_1, in1);
  digitalWrite(MOTOR_PIN_2, in2);
  analogWrite(MOTOR_ENABLE, pwm);
}
