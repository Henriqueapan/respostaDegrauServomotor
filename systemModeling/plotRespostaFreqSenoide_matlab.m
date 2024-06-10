% warning('off', 'all');
% plotRespostaPrealocando.m
clear all
close all
pkg load instrument-control

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
tempoExecucao = 20; % 60 segundos

% Obtém o tempo inicial
% tempoInicial = time();
tempoAtual = 0;

sin_input = zeros(1, N);

% Inicia a leitura da porta serial
i = 1;
while true
    if s.bytesavailable() > 0
        % Lê a quantidade de passos registrados pelo encoder à partir do Arduino
        enc_output = textscan(readline(s), "%n, %n, %n");
        passos = enc_output{1};
        delta_tempo = enc_output{2}/1000;
        if i > 1
            sin_input(i) = enc_output{3};
            % tempoAtual = time();
            tempoAtual += delta_tempo;
            % tempo(i) = tempoAtual - tempoInicial;
            tempo(i) = tempoAtual;
        end



        mov_ang = (passos/enc_res) * 2 * pi; % Deslocamento angular em rad
        disp(mov_ang);
        if i == 1
##            buffer(rem(i,M)) = mov_ang/delta_tempo; % O buffer na posição referente ao resto da divisão de i por M é a velocidade angular deste passo
##            disp(buffer(rem(i,M)));
        elseif i < M
            buffer(rem(i,M)) = mov_ang/delta_tempo;
            disp(buffer(rem(i,M)));

        else
            buffer(rem(i,M) + 1) = mov_ang/delta_tempo;
            disp(buffer(rem(i,M)+1));
        end

        % Atualiza o vetor de velocidade a partir da média
        soma_buffer = sum(buffer);
        velocidade(i) = soma_buffer*inv_M;

        % Adiciona ponto no gráfico de velocidade x tempo
        plot(tempo(1:i), velocidade(1:i));
        hold on;
        plot(tempo(1:i), sin_input(1:i));
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