import serial, datetime, time

ser = serial.Serial(
    port="COM5",
    baudrate=115200,
    timeout=0
)

time.sleep(1.5)

# Buffer de entrada para cálculo de média móvel
M = 50
inv_M = 1/M
buffer = [0] * M
soma_buffer = sum(buffer)

# Lists tempo, velocidade e input de senóide
tempo = list()
sin_input = list()

# Tempo de execução em segundos
tempo_execucao = 12

# Variável de tempo atual
tempo_atual = 0
delta_tempo = 0

# Valor máximo a ser considerado como ruído e não sinal
MAX_NOISE_VAL = 120
TUNING_CONSTANT = 100

print("Conectando a porta: " + ser.portstr)

# Contador de execuções
i = 0

while True:
    if ser.in_waiting <= 0:
        continue
    enc_output = ser.readline().decode("ascii").rstrip("\n").rstrip("\r").rstrip().split(", ")
    if len(enc_output) != 2 and delta_tempo is not None:
        tempo_atual += delta_tempo
        continue
    elif len(enc_output) != 2: continue

    try:
        # buffer[i%M] = float(enc_output[0]) if float(enc_output[0]) > MAX_NOISE_VAL else buffer[i%M - 1]
        # buffer[i%M] = float(enc_output[0])
        sin_input.append(float(enc_output[0]) if float(enc_output[0]) > MAX_NOISE_VAL else sin_input[-1])
        delta_tempo = float(enc_output[1])/1e6
    except ValueError as e:
        continue

    tempo_atual += delta_tempo
    tempo.append(tempo_atual)

    if(tempo[i] > tempo_execucao): break
    else: print(tempo[i])

    i += 1

with open(f'./output/output{datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")}.txt', 'w') as file:
    file.writelines([f'{str(tempo)},{str(sin_input)}\n' for tempo, sin_input in zip(tempo, sin_input)])
    file.close()

ser.close()
exit(1)