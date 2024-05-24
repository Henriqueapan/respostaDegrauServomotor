import serial, datetime, time

ser = serial.Serial(
    port="COM7",
    baudrate=115200,
    timeout=0
)

time.sleep(1.5)

# Lists tempo, velocidade e input de senóide
tempo = list()
sin_input = list()

# Tempo de execução em segundos
tempo_execucao = 5

# Variável de tempo atual
tempo_atual = 0

print("Conectando a porta: " + ser.portstr)

# Contador de execuções
i = 0

while True:
    if ser.in_waiting <= 0:
        continue
    enc_output = ser.readline().decode("ascii").rstrip("\n").rstrip("\r").rstrip().split(", ")

    try:
        sin_input.append(float(enc_output[0]))
        delta_tempo = float(enc_output[1])/1000
    except ValueError as e:
        continue

    tempo_atual += delta_tempo
    tempo.append(tempo_atual)

    if(tempo[i] > tempo_execucao): break

    i += 1

with open(f'./output/output{datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")}.txt', 'w') as file:
    file.writelines([f'{str(velocidade)},{str(tempo)},{str(sin_input)}\n' for velocidade, tempo, sin_input in zip(velocidade, tempo, sin_input)])
    file.close()

ser.close()
exit(1)