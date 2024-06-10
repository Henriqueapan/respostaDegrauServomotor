close all;
clear all;

M = [3.7 4.87 6.55 7.9 11.14 12.21 15.138 18.34 21.60 23.52 25.51 23.36 25.2 25.7 24.97 26.00];
freqs = [25 17.5 10 7.5 6 5 3.5 2.5 1.5 1 0.75 0.5 0.35 0.25 0.175 0.1] * 2 * pi;
dt = [0.0210 0.030 0.05 0.054 0.062 0.0579 0.08 0.0740 0.082 0.078 0.06 0.09 0.08 0.06 0.077 0.009]/(2*pi);
freqsReais = [17.25 12.19 6.91 5.24 3.87 3.48 2.247 1.74 1.02 0.707 0.529 0.3544 0.227 0.1762 0.1137 0.07 ]*(2*pi);
G = 20*log10(M);
fase =(dt.*freqsReais)*360;

#freqQuina = 21.685;
freqQuina = 12.024;
magK =27.7711526;

freq_asint_m = logspace(log10(min(freqsReais)), log10(max(freqsReais)), 100);
mag_asint_low = magK * ones(size(freq_asint_m));  % Assíntota reta
mag_asint_high = magK - 20 * (log10(freq_asint_m) - log10(freqQuina));  % Assíntota do primeiro polo (trocar descrescimento/década caso for utilizar superamortecido)

fase_asint_low = -6*ones(size(freq_asint_m));  % Assíntota de baixa frequência (-3 graus)
fase_asint_high = -130 * ones(size(freq_asint_m));  % Assíntota de alta frequência (-130 graus)
fase_asint_mid = -90 * (log10(freq_asint_m) - log10(2.1685)); % Assintota do meio

subplot(2,1,1);
semilogx(freqsReais, G,'b', 'LineWidth', 2);
hold on;
plot([freqQuina,freqQuina],[0 30], 'b--');
semilogx(freqQuina,magK, 'go', 'MarkerSize', 5, 'MarkerFaceColor', 'g');
semilogx(freq_asint_m, mag_asint_low, 'r-.', 'LineWidth', 1.5);
semilogx(freq_asint_m, mag_asint_high, 'r-.', 'LineWidth', 1.5);
hold off;
% xlabel("Frequência (rad/s)");
ylabel("Magnitude (dB)");
grid on
xlim([.8 105])
ylim([0 30]);
legend("Magnitude experimental", "Frequência de quina 12.024 rad/s", "Quina" , "Assíntotas");
legend("Location", "southwest");

##ax=
subplot(2,1,2);
semilogx(freqsReais,-fase,'b', 'LineWidth', 2);
hold on;
##semilogx(freqQuina,-90, 'go', 'MarkerSize', 5, 'MarkerFaceColor', 'g');
plot([freqQuina,freqQuina],[0 -130], 'b--');
semilogx(freq_asint_m, fase_asint_low, 'r-.', 'LineWidth', 1.5);  % Assíntota de baixa frequência
semilogx(freq_asint_m, fase_asint_high, 'r-.', 'LineWidth', 1.5);  % Assíntota de alta frequência
semilogx(freq_asint_m, fase_asint_mid, 'r-.', 'LineWidth', 1.5);
hold off;
xlabel("Frequência (rad/s)");
ylabel("Fase (graus)");
grid on;
xlim([.8 105]);
ylim([-140 0]);
##axis([.8 105 0 -150]);
legend("Fase experimental", "Frequência de quina 12.024 rad/s" , "Assíntotas");
legend("Location", "west");
##set(ax, "xlimmode", "manual");
##set(ax, "xlim", [.8 105]);
##set(ax, "ylimmode", "manual");
##set(ax, "ylim", [-150 0]);
#27,7711526 db na assintota k

#24,46569909 de ganho na assintota k

