pkg load control;

M = [3.7 4.87 6.55 7.9 11.14 12.21 15.138 18.34 21.60 23.52 25.51 23.36 25.2 25.7 24.97 26.00];
freqs = [25 17.5 10 7.5 6 5 3.5 2.5 1.5 1 0.75 0.5 0.35 0.25 0.175 0.1] * 2 * pi;
dt = [0.0210 0.030 0.05 0.054 0.062 0.0579 0.08 0.0740 0.082 0.078 0.06 0.09 0.08 0.06 0.077 0.009]/(2*pi);
freqsReais = [17.25 12.19 6.91 5.24 3.87 3.48 2.247 1.74 1.02 0.707 0.529 0.3544 0.227 0.1762 0.1137 0.07 ]*(2*pi);
G = 20*log10(M);
fase = (dt .* freqsReais) * 360;

s = tf('s');
#Gteorico = 11504.73 / (s^2 + 43.37 * s + 470.24); #Criticamente amortecido
#Gteorico = 44.69907407 / (0.06966136364* s + 1); #Primeira ordem
#Gteorico = 22 / (0.06966136364* s + 1); #Primeira ordem chutando o ganho k
#Gteorico = (24.4*(10*12.024^2)) / ((s + 12.024)*(s+10*12.024)); #Super amortecido (chutando 2do polo em  freq 10 vezes maior que a primeira freq de quina)
Gteorico = (24.4*(7*12.024^2)) / ((s + 12.024)*(s+7*12.024)); #Super amortecido (chutando 2do polo em freq 7 vezes maior que a primeira freq de quina)

[mag, phase, w] = bode(Gteorico, {0.43, 109});
mag = squeeze(mag);
phase = squeeze(phase);

figure();
subplot(2, 1, 1);
semilogx(freqsReais, G, 'b', 'LineWidth', 2);
hold on;
semilogx(w, 20*log10(mag), 'r', 'LineWidth', 2);
hold off;
grid on;
title('Diagrama de Bode - Magnitude');
xlabel('Frequência (rad/s)');
ylabel('Magnitude (dB)');
legend('Diagrama de Bode Experimental', 'Diagrama de Bode Teórico');
xlim([0.43, 109]);
subplot(2, 1, 2);
semilogx(freqsReais, -fase, 'b', 'LineWidth', 2);
hold on;
semilogx(w, phase, 'r', 'LineWidth', 2);
hold off;
grid on;
title('Diagrama de Bode - Fase');
xlabel('Frequência (rad/s)');
ylabel('Fase (graus)');
legend('Diagrama de Bode Experimental', 'Diagrama de Bode Teórico');
xlim([0.43, 109]);
