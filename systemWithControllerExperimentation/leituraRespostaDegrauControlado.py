import serial, datetime, time

ser = serial.Serial(
    port="COM7",
    baudrate=115200,
    timeout=0
)

time.sleep(1.5)

# Lists erro, tempo, posicao e posição referência
erro = list()
tempo = list()
posicao = list()
referencia_posicao = list()

# velocidade = list()

# Buffers de erro de posição para cálculo de média móvel
M = 20
inv_M = 1/M

buffer_erro = [0] * M
soma_buffer_erro = sum(buffer_erro)

buffer_pos = [0] * M
soma_buffer_pos = sum(buffer_pos)

# Tempo de execução em segundos
tempo_execucao = 5.4

print("Conectando a porta: " + ser.portstr)

# Contador de execuções
i = 0

while True:
    idx_buffer_iteracao_atual = i%M

    if ser.in_waiting <= 0: continue

    enc_output = ser.readline().decode("ascii").rstrip("\n").rstrip("\r").rstrip().split(",")
    if len(enc_output) != 4: continue

    try:
        erro_atual = float(enc_output[0]) # rad
        tempo_atual = float(enc_output[1]) # us
        posicao_atual = float(enc_output[2]) # rad
        referencia_posicao_atual = float(enc_output[3]) # rad
    except ValueError: continue

    tempo.append(tempo_atual/1000000) # s
    referencia_posicao.append(referencia_posicao_atual)

    buffer_erro[i%M] = erro_atual
    buffer_pos[i%M] = posicao_atual

    soma_buffer_erro = sum(buffer_erro)
    soma_buffer_pos = sum(buffer_pos)

    erro.append(soma_buffer_erro * inv_M)
    posicao.append(soma_buffer_pos * inv_M)

    if(tempo[i] > tempo_execucao): break
    print(format(tempo[i],'.2f'))
    i += 1

with open(f'./output/outputDegrau/output{datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")}.txt', 'w') as file:
    file.writelines([f'{str(erro)},{str(tempo)},{str(posicao)},{str(referencia_posicao)}\n' for erro, tempo, posicao, referencia_posicao in zip(erro, tempo, posicao, referencia_posicao)])
    file.close()

ser.close()
exit(1)