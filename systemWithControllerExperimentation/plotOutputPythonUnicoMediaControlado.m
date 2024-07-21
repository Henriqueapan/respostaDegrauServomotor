clear all
close all
pkg load signal

[erro, tempo, posicao, referencia] = textread('./Experimentos/output2024-07-20_20-43-47avancoCC.txt', "%f,%f,%f,%f");

janela_mediana = 10; % Tamanho da janela para o filtro de mediana
janela_media = 10; % Tamanho da janela para o filtro de média

tempo_maximo_de_plot = 24;
idx_tempo_maximo_de_plot = 3720;

for i=1:1:length(tempo)
    if tempo(i) <= tempo_maximo_de_plot + .0001 && tempo(i) >= tempo_maximo_de_plot - .0001
        idx_tempo_maximo_de_plot = i
        break
    endif
endfor

disp(idx_tempo_maximo_de_plot)

if idx_tempo_maximo_de_plot ~= Inf
    posicao = posicao(1:idx_tempo_maximo_de_plot)
##    vel = vel(1:idx_tempo_maximo_de_plot)
    tempo = tempo(1:idx_tempo_maximo_de_plot)
else
    disp("Impossível determinar o tempo máximo do plot\n")
    return
endif

% Aplicar filtro de média
b = (1/janela_media) * ones(1, janela_media);
a = 1;
erro_filt_media = filter(b, a, erro);
posicao_filt_media = filter(b, a, posicao);
ref_filt_media = filter(b, a, referencia);
##
ref_arredondada = round(referencia * 100) / 100;
##b = (1/100) * ones(1, 100);
##a = 1;
##ref_arredondada = filter(b, a, ref_arredondada);


%% FUNÇÃO PARA GERAR A REFERENCIA (POIS A LIDA DO ARDUINO TA CAGADA)
i=1;
ref = zeros(1, length(tempo));
for i = 1:length(tempo)

    if (tempo(i) < 5)
        ref(i) = 0;  % DELAY ANTES DE COMEÇAR O EXPERIMENTO
    elseif (tempo(i) < 7)
        ref(i) = 30;  % REFERENCIA EM 30º NOS 2s DE EXPERIMENTO
    elseif (tempo(i) < 9)
        ref(i) = 70;  % REFERENCIA EM 70º NOS 4s DE EXPERIMENTO
    elseif (tempo(i) < 11)
        ref(i) = 165;  % REFERENCIA EM 165º NOS 6s DE EXPERIMENTO
    elseif (tempo(i) < 13)
        ref(i) = 100;  % REFERENCIA EM 100º NOS 8s DE EXPERIMENTO
    elseif (tempo(i) < 15)
        ref(i) = 310;  % REFERENCIA EM 310º NOS 10s DE EXPERIMENTO
    elseif (tempo(i) < 17)
        ref(i) = 230;  % REFERENCIA EM 230º NOS 12s DE EXPERIMENTO
    elseif (tempo(i) < 19)
        ref(i) = 45;  % REFERENCIA EM 45º NOS 14s DE EXPERIMENTO
    elseif (tempo(i) < 21)
        ref(i) = 150;  % REFERENCIA EM 150º NOS 16s DE EXPERIMENTO
    elseif (tempo(i) < 23)
        ref(i) = 270;  % REFERENCIA EM 270º NOS 18s DE EXPERIMENTO
    elseif (tempo(i) < 25)
        ref(i) = 20;  % REFERENCIA EM 20º NOS 20s DE EXPERIMENTO
    else
        ref(i) = 0;  % Valor padrão para após o tempo especificado
    end
    i = i+1;
end
ref = ref';


% figure(1)
% plot(tempo, vel_filt_media);
% xlim([0 .5])
% ylim([-20 500])
% xlabel('Tempo (s)');
% ylabel('Velocidade (rad/s)');
% h = get(gca, 'Children');
% set(h(1), 'Color', 'b');
% set(h(1), 'LineWidth', 2);
% grid on;

figure(1)
plot(tempo, rad2deg(posicao_filt_media));
hold on;

##plot(tempo, rad2deg(referencia));
plot(tempo, ref);
##plot(tempo, rad2deg(ref_filt_media));
hold on;

##plot(tempo, rad2deg(erro_filt_media/1000));
erro_calculado = abs(ref - rad2deg(posicao_filt_media));
plot(tempo, erro_calculado);

xlabel('Tempo (s)');
ylabel('Posição Angular do Eixo (Graus)');

h = get(gca, 'Children');
set(h(1), 'Color', 'b');
set(h(1), 'LineWidth', 2);
set(h(2), 'Color', 'r');
set(h(2), 'LineWidth', 2);
set(h(3), 'Color', [1 0.647 0]);
set(h(3), 'LineWidth', 2);
xlim([5 24])
ylim([-0.25 360])
leg = legend('Posição Medida', 'Posição de Referência', 'Erro de posição');
legend('Location','northwest');
grid on;
# Alterar conforme necessidade
title('Posição Angular do Eixo do Servomotor Controlado Obtida Experimentalmente');

