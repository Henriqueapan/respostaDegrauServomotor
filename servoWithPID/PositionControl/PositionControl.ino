#include <TimerOne.h>
#include <PID_v1.h>

#define chA 3 // Pino canal A do encoder
#define chB 2 // Pino canal B do encoder
#define REFERENCIA_PIN A0
#define MOTOR_PIN_1 7 // Pino do motor (IN1)
#define MOTOR_PIN_2 6 // Pino do motor (IN2)
#define MOTOR_ENABLE 5 // Pino do enable da ponte H
#define RESOLUCAO_ENCODER 200 // Resolução do encoder (quantidade de passos que representa 1 volta completa)
#define PERIODO_AMOSTRAGEM 10000 // Microssegundos
#define PERIODO_LEITURA_REFERENCIA 1000000 // Microssegundos
#define INV_MICRO .000001

#define PWM_MIN 85 // Valor mínimo de PWM para acionar o motor
#define DEAD_ZONE 10 

#define WINDOW_SIZE 25 // Tamanho da janela para a média móvel

// Defina Variáveis
double Setpoint, Input, Output;
// Parâmetros PID
double Kp = 0.039365, Ki = 0.015513, Kd = 0.0074816; // Considerando Gzoh
// double Kp = 0.047273, Ki = 0.021656, Kd = 0.0098097; // desconsiderando Gzoh
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

volatile double buffer[WINDOW_SIZE] = {0};
volatile int bufferIndex = 0;
volatile double somaMedia = 0;

volatile unsigned long tempo_atual = 0;
volatile unsigned long tempo_anterior = 0;

volatile double velocidade = 0;
double PERIODO_AMOSTRAGEM_SEC = PERIODO_AMOSTRAGEM * INV_MICRO;
double INV_RESOLUCAO = 1.0/(double)RESOLUCAO_ENCODER;
double INV_AMOSTRAGEM = 1.0/(double)PERIODO_AMOSTRAGEM;
double INV_AMOSTRAGEM_SEC = 1.0/(double)PERIODO_AMOSTRAGEM_SEC;
double INV_LEITURA = 1.0/1020.0;

volatile double erro = 0;
volatile double ref = 0;
volatile double saida_controle = 0;

double COEF_EQ_DIFERENCAS_POSICAO = (PERIODO_AMOSTRAGEM/2) * INV_MICRO;
double coef_control = (2+147.1663896*PERIODO_AMOSTRAGEM*INV_MICRO);
volatile double velocidade_anterior = 0;
volatile double erro_anterior = 0;
volatile double erro_ante_anterior = 0;

volatile double posicao = 0;
volatile double posicao_anterior = 0;
volatile double saida_controle_anterior = 0;
volatile double saida_controle_ante_anterior = 0;

volatile double in_anterior = 0;

volatile int contador_passos_motor = 0;
volatile int chA_atual;
volatile int chB_atual;
volatile int chA_antigo = 0;
volatile int chB_antigo = 0;

volatile int pwm_val = 0;

// double K = 0.839625176;
double K = 0.5;

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

    //   TCCR1B = TCCR1B & 0b11111000 | 1;
    attachInterrupt(digitalPinToInterrupt(chA), leituraEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(chB), leituraEncoder, CHANGE);
    Serial.begin(115200);
}

void loop() {
    // Serial.println(contador_passos_motor);
    // Serial.println(String(erro) + " / " + String(erro_anterior) + " / / " + String(saida_controle) + " / " + String(saida_controle_anterior));
    // Serial.println("ref: " + String(ref) + " / " + "erro: " +  String(erro) + " / " + "saida: " + String(saida_controle,7) + " PWM: " + String(pwm_val));
    // Serial.println(String(ref) + " / " + String(erro) + " / " + String(velocidade) + " / " + String(saida_controle)+ "/ " + String(pwm_val)); 
    // Serial.println(String(contador_passos_motor)  + " / " + String(saida_controle));
}

void interrupcao(){
    
    // refAleatoria();
    // Serial.println(contador_passos_motor);
    calculaVelocidade();
    // Serial.println(velocidade);
    // calculaPOS();
    // integrador(mediaMovelVelocidade(velocidade));
    integrador(velocidade);
    // Serial.println(posicao);
    // Atualiza a referência no período de sua atualização ou na primeira execução da rotina de interrupção
    tempo_atual = micros();
    if (((tempo_atual - tempo_anterior) > PERIODO_LEITURA_REFERENCIA) || tempo_anterior == 0){
        atualizaReferencia();
        tempo_anterior = tempo_atual;
    } 
    
    controladorPOS();
    // controladorPID();
    // saida_controle = gzoh(saida_controle);
    Serial.println(erro*RAD_TO_DEG);

    // atualizarPWM();
    atualizarPWM2();
    // atualizarPWM3(); 
    // Serial.println(saida_controle);
    // Serial.println(pwm_val);
    // controlaMotor(1,0,255);
}

void atualizarPWM(){
    pwm_val = constrain(saida_controle*255,-254,254);
    if (saida_controle <= 0){
        controlaMotor(1, 0, abs(pwm_val));
    }
    else{
        controlaMotor(0, 1, abs(pwm_val));
    }
}

void atualizarPWM2(){
    pwm_val = saida_controle*255;

    if (abs(pwm_val) < DEAD_ZONE) {
        // Se o valor do PWM estiver dentro da zona morta, desligue o motor
        digitalWrite(MOTOR_PIN_1, 1);
        digitalWrite(MOTOR_PIN_2, 1);
        analogWrite(MOTOR_ENABLE, 255);
    } else {
        // Garanta que o valor do PWM seja pelo menos PWM_MIN
        if (abs(pwm_val) < PWM_MIN) {
            pwm_val = pwm_val < 0 ? - PWM_MIN : PWM_MIN;
        }
        pwm_val = constrain(pwm_val,-254,254);
        if (erro < 0){
            controlaMotor(0, 1, abs(pwm_val));
        }
        else{
            controlaMotor(1, 0, abs(pwm_val));
        }
    }
}

void atualizarPWM3(){
    if (erro >= 1){
        pwm_val = constrain(map(255*saida_controle, -1, 1, 40, 254),-254,254);
        controlaMotor(0, 1, abs(pwm_val));
    }
    else if(erro <= -1){
        pwm_val = constrain(map(255*saida_controle, -1, 1, 40, 254),-254,254);
        controlaMotor(1, 0, abs(pwm_val));
    }
    else{
       controlaMotor(1, 1, 254); 
    }
}

void atualizaReferencia() {
    ref = analogRead(REFERENCIA_PIN) * INV_LEITURA * TWO_PI;
}

void controladorPID(){
    // Setpoint = analogRead(REFERENCIA_PIN) * INV_LEITURA * TWO_PI;
    Setpoint = ref;
    Input = posicao;
    myPID.Compute();
    saida_controle = Output;
}


void controladorPOS(){
    // ref = analogRead(REFERENCIA_PIN) * INV_LEITURA * TWO_PI;
    erro = ref - posicao;
    // saida_controle =  erro * K;  
    // saida_controle = (0.24237 * (1 + 0.134 * INV_AMOSTRAGEM_SEC) * posicao + 0.24237 * (1 - 0.134 * INV_AMOSTRAGEM_SEC) * posicao_anterior) - saida_controle_anterior;
    // saida_controle = K * ((1 + 0.22 * INV_AMOSTRAGEM_SEC) * erro + (1 - 0.22 * INV_AMOSTRAGEM_SEC) * erro_anterior) - saida_controle_anterior;
    // saida_controle = (K * (erro*(2 + 14.95 *PERIODO_AMOSTRAGEM_SEC) + erro_anterior * (14.95 * PERIODO_AMOSTRAGEM_SEC - 2)) - saida_controle_anterior * (147.1663896 * PERIODO_AMOSTRAGEM_SEC - 2)) * coef_control;
    
    // saida_controle = (0.8396 * erro - 0.7201 * erro_anterior) + 0.9063 * saida_controle_anterior;
    // saida_controle = K * (erro - 0.8576 * erro_anterior) + 0.9063 * saida_controle_anterior;
    // saida_controle = K * (0.0084592 * erro_anterior - 0.007285908996 * erro_ante_anterior) + 0.236 * saida_controle_ante_anterior - 0.60749289 * saida_controle_ante_anterior;
    saida_controle = K*(erro - 1.6995*erro_anterior + 0.71022674*erro_ante_anterior) + 1.0116*saida_controle - 0.25583364*saida_controle_ante_anterior; //AVANÇO 
    // saida_controle = K*(erro - 0.597569*erro_anterior + 0.0010539702*erro_ante_anterior) + 1.7338*saida_controle - 0.75151561*saida_controle_ante_anterior; //ATRASO   
    saida_controle_ante_anterior = saida_controle_anterior;
    erro_ante_anterior = erro_anterior;
    saida_controle_anterior = saida_controle;
    erro_anterior = erro;
}

void calculaVelocidade(){
    velocidade = contador_passos_motor * INV_RESOLUCAO * TWO_PI * INV_AMOSTRAGEM_SEC;
    contador_passos_motor = 0;
}

void calculaPOS(){
    posicao = contador_passos_motor * INV_RESOLUCAO * TWO_PI;
    contador_passos_motor = 0;
}

void integrador(double vel_atual) {
//   posicao = posicao_anterior + COEF_EQ_DIFERENCAS_POSICAO * vel_atual + COEF_EQ_DIFERENCAS_POSICAO * velocidade_anterior;
  posicao = posicao_anterior + 0.0005 * vel_atual + 0.0005 * velocidade_anterior;
  if (posicao > TWO_PI){
    posicao = posicao - TWO_PI;
  }
  else if(posicao < -TWO_PI){
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

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Função para calcular a média móvel da velocidade
double mediaMovel(double variavel) {
    // Subtrai o valor mais antigo da soma
    somaMedia -= buffer[bufferIndex];

    // Substitui o valor mais antigo pelo novo valor
    buffer[bufferIndex] = variavel;

    // Adiciona o novo valor à soma
    somaMedia += variavel;

    // Atualiza o índice do buffer, garantindo que ele volte ao início quando atingir o tamanho da janela
    bufferIndex = (bufferIndex + 1) % WINDOW_SIZE;

    // Calcula a média móvel
    return somaMedia / WINDOW_SIZE;
}

// void refAleatoria(){
//     if(cont_periodos >= 1000){
//         ref = random();
//     }
// }

double gzoh(double in){
    double out;
    out = 0.5*in - 0.5*in_anterior;
    in_anterior = in;
    return out; 
}