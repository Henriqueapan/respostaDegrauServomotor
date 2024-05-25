clear all
close all
pkg load signal

[vel, tempo] = textread('./output/output2024-05-25_12-41-44.txt', "%f,%f");

janela_mediana = 100; % Tamanho da janela para o filtro de mediana
janela_media = 100; % Tamanho da janela para o filtro de média

% Aplicar filtro de mediana
vel_filt_med = medfilt1(vel, janela_mediana);

% Aplicar filtro de média
b = (1/janela_media) * ones(1, janela_media);
a = 1;
vel_filt_media = filter(b, a, vel);

figure(1)
plot(tempo, vel);
title('Dados Originais');
xlabel('Tempo');
ylabel('Velocidade');
grid on;

figure(2)
plot(tempo, vel_filt_med);
title('Filtro de Mediana');
xlabel('Tempo');
ylabel('Velocidade Filtrada');
grid on;

figure(3)
plot(tempo, vel_filt_media);
title('Filtro de Média');
xlabel('Tempo');
ylabel('Velocidade Filtrada');
grid on;

