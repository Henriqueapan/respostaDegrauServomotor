pkg load control;
M = [3.7 4.87 6.55 7.9 11.14 12.21 15.138 18.34 21.60 23.52 25.51 23.36 25.2 25.7 24.97 26.00];
freqs = [25 17.5 10 7.5 6 5 3.5 2.5 1.5 1 0.75 0.5 0.35 0.25 0.175 0.1] * 2 * pi;
dt = [0.0210 0.030 0.05 0.054 0.062 0.0579 0.08 0.0740 0.082 0.078 0.06 0.09 0.08 0.06 0.077 0.009]/(2*pi);
freqsReais = [17.25 12.19 6.91 5.24 3.87 3.48 2.247 1.74 1.02 0.707 0.529 0.3544 0.227 0.1762 0.1137 0.07 ]*(2*pi);
G = 20*log10(M);
fase =(dt.*freqsReais)*360;
Gfreq = (11504.73)/(s^2 + 43.37*s + 470.24);
[mag, phase, w] = bode(G);

figure()
subplot(2,1,1);
semilogx(freqsReais, G);
mag = squeeze(mag);
semilogx(w,mag);
% xlabel("Frequência (rad/s)");
ylabel("Magnitude (dB)");
grid on
xlim([.8 105])
h = get(gca, 'Children');
set(h(1), 'Color', 'b');
set(h(1), 'LineWidth', 2);

subplot(2,1,2);
semilogx(freqsReais,-fase);
phase = squeeze(phase);
semilogx(w,phase);
xlabel("Frequência (rad/s)");
ylabel("Fase (graus)");
grid on
xlim([.8 105])

h = get(gca, 'Children');
set(h(1), 'Color', 'b');
set(h(1), 'LineWidth', 2);
