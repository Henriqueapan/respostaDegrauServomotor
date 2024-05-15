close all
clear all

% Cria o objeto serial
s = serial("/dev/ttyUSB0");
set(s, 'baudrate', 115200);

fopen(s);

% Prealoca memória para o vetor de tempo e velocidade
N = 2000;
tempo = zeros(1, N);
velocidade = zeros(1, N);

% Define o tempo de execução em segundos
tempoExecucao = 10; % 60 segundos

% Obtém o tempo inicial
tempoInicial = time();

% Inicia a leitura da porta serial
i = 1;
while true
    if s.bytesavailable() > 0
        % Lê a velocidade do Arduino
        vel = str2double(readline(s));
        
        % Atualiza o vetor de tempo e velocidade
        tempoAtual = time();
        tempo(i) = tempoAtual - tempoInicial;
        printf("%f", vel);
        printf("\n");
        
        velocidade(i) = vel;
        
        % Plota o gráfico de velocidade x tempo
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
fclose(s);

xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');