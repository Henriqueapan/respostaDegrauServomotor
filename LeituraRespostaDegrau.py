import serial, datetime, time
from math import pi

ser = serial.Serial(
    port="COM7",
    baudrate=115200,
    timeout=0
)

time.sleep(1.5)

# Lists tempo, velocidade
tempo = list()
velocidade = list()

# Buffer de velocidade para cálculo de média móvel
M = 100
inv_M = 1/M
buffer = [0] * M
soma_buffer = sum(buffer)

# Resolução do encoder
enc_res = 200

# Tempo de execução em segundos
tempo_execucao = 5.4

# Variável de tempo atual
tempo_atual = 0

print("Conectando a porta: " + ser.portstr)

# Contador de execuções
i = 0

while True:
    if ser.in_waiting <= 0:
        continue
    enc_output = ser.readline().decode("ascii").rstrip("\n").rstrip("\r").rstrip().split(",")
    if len(enc_output) != 2 and delta_tempo is not None:
        tempo_atual += delta_tempo
        continue
    elif len(enc_output) != 2: continue

    try:
        passos = float(enc_output[0])
        delta_tempo = float(enc_output[1])/1000#000
        
    except ValueError as e:
        continue

    tempo_atual += delta_tempo
    tempo.append(tempo_atual)

    mov_ang = (passos/enc_res) * 2 * pi

    buffer[i%M] = 0 if delta_tempo == 0 else mov_ang/delta_tempo


    # Atualiza o vetor de velocidade a partir da média
    soma_buffer = sum(buffer)
    velocidade.append(soma_buffer*inv_M)

    if(tempo[i] > tempo_execucao): break
    print(format(tempo[i],'.2f'))
    i += 1

with open(f'./output/output{datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")}.txt', 'w') as file:
    file.writelines([f'{str(velocidade)},{str(tempo)}\n' for velocidade, tempo in zip(velocidade, tempo)])
    file.close()

ser.close()
exit(1)