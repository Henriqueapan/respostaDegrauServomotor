import serial
from math import pi

ser = serial.Serial(
    port="COM7",
    baudrate=115200,
    timeout=0
)

# Lists tempo, velocidade e input de senóide
tempo = list()
velocidade = list()
sin_input = list()

# Buffer de velocidade para cálculo de média móvel
M = 50
inv_M = 1/M
buffer = [0] * M
soma_buffer = sum(buffer)

# Resolução do encoder
enc_res = 200

# Pico de tensão com PWM em 255
pico_tensao = 11.0

# Tempo de execução em segundos
tempo_execucao = 1.2

# Variável de tempo atual
tempo_atual = 0

print("Conectando a porta: " + ser.portstr)

# Contador de execuções
i = 0

while True:
    if ser.in_waiting <= 0:
        continue
    enc_output = ser.readline().decode().split(", ")
    print(enc_output)

    if len(enc_output) != 3 :
        continue

    passos = float(enc_output[0])
    delta_tempo = float(enc_output[1])/1000

    # if i > 0:
    # Senóide de tensão de input baseada no duty cycle do PWM (de 0 a 255)
    sin_input.append((float(enc_output[2])/255) * pico_tensao)

    tempo_atual += delta_tempo
    tempo.append(tempo_atual)

    mov_ang = (passos/enc_res) * 2 * pi

    buffer[i%M] = 0 if delta_tempo == 0 else mov_ang/delta_tempo
    # if i < M:
    #     buffer[i%M] = 0 if delta_tempo == 0 else mov_ang/delta_tempo
    # elif i > 0 and i :
    #     buffer[i%M + 1] = 0 if delta_tempo == 0 else mov_ang/delta_tempo

    # Atualiza o vetor de velocidade a partir da média
    soma_buffer = sum(buffer)
    velocidade.append(soma_buffer*inv_M)

    if(tempo[i] > tempo_execucao): break

    i += 1

with open('./output/output.txt', 'w') as file:
    # for velocidade, tempo in zip(velocidade, tempo):
    file.writelines([f'{str(velocidade)},{str(tempo)}' for velocidade, tempo in zip(velocidade, tempo)])
    file.close()

ser.close()
exit(1)