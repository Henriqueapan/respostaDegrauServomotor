#define PWM_FREQ 0.1

volatile double tempoAtual = 0.0;
volatile double tempoAnterior = 0.0;
volatile double valor_pwm;
volatile double arg;
int tuning_constant = 1;

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (tempoAtual <= 15*1000000) {
    tempoAtual = micros();

    arg = TWO_PI * PWM_FREQ /*Hz*/ * double(tempoAtual)/(1000000);//*0.0000000001;

    valor_pwm = ((127/2)*sin(arg)) + 1.5*127;

    Serial.println(String(valor_pwm) + ", " + String((tempoAtual - tempoAnterior)));
    
    tempoAnterior = tempoAtual;
  }
}
