clear all
close all
pkg load signal

[erro, tempo, posicao, referencia] = textread('./output/outputDegrau/output2024-05-25_12-55-46.txt', "%f,%f,%f,%f");

janela_mediana = 10; % Tamanho da janela para o filtro de mediana
janela_media = 10; % Tamanho da janela para o filtro de média

tempo_maximo_de_plot = 5;
idx_tempo_maximo_de_plot = Inf;

for i=1:1:length(tempo)
    if tempo(i) <= tempo_maximo_de_plot + .0001 && tempo(i) >= tempo_maximo_de_plot - .0001
        idx_tempo_maximo_de_plot = i
        break
    endif
endfor

disp(idx_tempo_maximo_de_plot)

if idx_tempo_maximo_de_plot ~= Inf
    vel = vel(1:idx_tempo_maximo_de_plot)
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

plot(tempo, posicao_filt_media);

plot(tempo, referencia);

plot(tempo, erro_filt_media);

xlabel('Tempo (s)');
ylabel('Posição Angular do Eixo (rad)');

h = get(gca, 'Children');
set(h(1), 'Color', 'b');
set(h(1), 'LineWidth', 2);
set(h(2), 'Color', 'r');
set(h(2), 'LineWidth', 2);
set(h(3), 'Color', 'y');
set(h(3), 'LineWidth', 2);
xlim([0 .5])
ylim([-20 500])
leg = legend('Posição Medida', 'Posição de Referência', 'Erro de posição');
legend('Location','southeast');

# Alterar conforme necessidade
title('Resposta ao Degrau da Posição Angular do Eixo do Servomotor Controlado Obtida Experimentalmente');
