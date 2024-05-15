% plotRespostaPrealocando_matlab.m
clear all
close all

% Cria o objeto serial
s = serialport("COM7", 115200);

% Prealoca memória para o vetor de tempo e de velocidade
N = 2000;
tempo = zeros(1, N);
velocidade = zeros(1, N);

% Quantidade de posições no buffer de média móvel e o próprio buffer como array
M = 8;
inv_M = 1/M;
buffer = zeros(1, M);
soma_buffer = sum(buffer);

% Resolução do encoder
enc_res = 200

% Define o tempo de execução em segundos
tempoExecucao = 10; % 60 segundos

% Obtém o tempo inicial
tempoInicial = datetime('now');

% Inicia a leitura da porta serial
i = 1;
while true
    if s.NumBytesAvailable
        % Lê a quantidade de passos registrados pelo encoder à partir do Arduino
        passos = str2double(readline(s));
        
        tempoAtual = datetime('now');
        tempo(i) = seconds(tempoAtual - tempoInicial);

        mov_ang = (passos/enc_res) * pi; % Deslocamento angular em rad

        if i == 1
            buffer(rem(i,M)) = mov_ang/tempo(i); % O buffer na posição referente ao resto da divisão de i por M é a velocidade angular deste passo
        else
            buffer(rem(i,M)) = mov_ang/(tempo(i) - tempo(i-1));
        end

        % Atualiza o vetor de velocidade a partir da média
        soma_buffer = sum(buffer);
        velocidade(i) = soma_buffer*inv_N;
        
        % Adiciona ponto no gráfico de velocidade x tempo
        plot(tempo(1:i), velocidade(1:i));
        drawnow;
        
        % Condição de parada: tempo de execução excedido
        if tempo(i) > tempoExecucao
            break;
        end
        
        % Incrementa o índice
        i = i + 1;
    end
end

% Limpa o objeto serial
clear s

xlabel('Tempo (s)');
ylabel('velocidade (rad/s)');