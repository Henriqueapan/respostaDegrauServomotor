clear all
close all
pkg load signal
pkg load control

janela_mediana = 1; % Tamanho da janela para o filtro de mediana
janela_media = 10; % Tamanho da janela para o filtro de média

# Comparação com modelo de degrau

# Resposta ao degrau
[vel1, tempo1] = textread('./output/outputDegrau/output2024-05-25_12-55-46.txt', "%f,%f");


tempo_maximo_de_plot1 = 5;
idx_tempo_maximo_de_plot1 = Inf;

for i=1:1:length(tempo1)
    if tempo1(i) <= tempo_maximo_de_plot1 + .0001 && tempo1(i) >= tempo_maximo_de_plot1 - .0001
        idx_tempo_maximo_de_plot1 = i
        break
    endif
endfor

disp(idx_tempo_maximo_de_plot1)

if idx_tempo_maximo_de_plot1 ~= Inf
    vel1 = vel1(1:idx_tempo_maximo_de_plot1)
    tempo1 = tempo1(1:idx_tempo_maximo_de_plot1)
endif

% Aplicar filtro de média
b = (1/janela_media) * ones(1, janela_media);
a = 1;
vel_filt_media_1 = filter(b, a, vel1);

figure(1)
plot(tempo1, vel_filt_media_1);
xlim([0 .5])
ylim([-20 500])
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');
grid on;
hold on;

s = tf('s');
G = (480.52 * 227.84)/(s^2 + 30.19*s + 227.84);


step(G);
xlim([0 .5])
ylim([-20 500])
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');

h = get(gca, 'Children');
set(h(1), 'Color', 'r');
set(h(1), 'LineWidth', 2);
set(h(2), 'Color', 'b');
set(h(2), 'LineWidth', 2)

leg = legend('Resposta Experimental', 'Resposta Simulada do Modelo');
legend('Location','east');

title('Respostas ao Degrau Experimental e Simulada usando a FTMA Modelada');

# Comparação com modelo de frequência

figure(2);

plot(tempo1, vel_filt_media_1);
xlim([0 .5])
ylim([-20 500])
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');
grid on;
hold on;

Gfreq = (11504.73)/(s^2 + 43.37*s + 470.24);

step(10.8*Gfreq, .5);
xlim([0 .5])
ylim([-20 500])
xlabel('Tempo (s)');
ylabel('Velocidade (rad/s)');

h = get(gca, 'Children');
set(h(1), 'Color', 'r');
set(h(1), 'LineWidth', 2);
set(h(2), 'Color', 'b');
set(h(2), 'LineWidth', 2)

leg = legend('Resposta Experimental', 'Resposta Simulada do Modelo');
legend('Location','southeast');

title('Respostas ao Degrau Experimental e Simulada usando a FTMA Modelada');