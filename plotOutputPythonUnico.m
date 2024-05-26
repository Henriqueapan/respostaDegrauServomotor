clear all
close all
pkg load signal

[vel, tempo] = textread('./output/outputDegrau/output2024-05-25_12-55-46.txt', "%f,%f");

janela_mediana = 100; % Tamanho da janela para o filtro de mediana
janela_media = 100; % Tamanho da janela para o filtro de média

tempo_maximo_de_plot = .5;
idx_tempo_maximo_de_plot = Inf;

for i=1:1:length(tempo)
    if tempo(i) <= tempo_maximo_de_plot + .01 && tempo(i) >= tempo_maximo_de_plot - .01
        idx_tempo_maximo_de_plot = i
        break
    endif
endfor

disp(idx_tempo_maximo_de_plot)

if idx_tempo_maximo_de_plot ~= Inf
    vel = vel(1:idx_tempo_maximo_de_plot)
    tempo = tempo(1:idx_tempo_maximo_de_plot)
endif

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

