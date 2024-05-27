clear all
close all
pkg load signal

[vel, tempo] = textread('./output/outputDegrau/output2024-05-25_12-55-46.txt', "%f,%f");

janela_mediana = 100; % Tamanho da janela para o filtro de mediana
janela_media = 100; % Tamanho da janela para o filtro de média

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
endif

% Aplicar filtro de média
b = (1/janela_media) * ones(1, janela_media);
a = 1;
vel_filt_media = filter(b, a, vel);

figure(1)
plot(tempo, vel_filt_media);
xlim([0 .5])
ylim([-20 500])
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');
grid on;

