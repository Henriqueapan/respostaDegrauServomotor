M = [3.7 4.87 6.55 7.9 11.14 12.21 15.138 18.34 21.60 23.52 25.51 23.36 25.2 25.7 24.97 26.00];
freqs = [25 17.5 10 7.5 6 5 3.5 2.5 1.5 1 0.75 0.5 0.35 0.25 0.175 0.1] * 2 * pi;
dt = [0.0210 0.030 0.05 0.054 0.062 0.0579 0.08 0.0740 0.082 0.078 0.06 0.09 0.08 0.06 0.077 0.009]/(2*pi);
G = 20*log(M);
fase =(dt.*freqs)*360;
subplot(2,1,1);
semilogx(freqs, G);
% xlabel("Frequência (rad/s)");
ylabel("Magnitude (dB)");
grid on
xlim([.8 105])
subplot(2,1,2);
semilogx(freqs,-fase);
xlabel("Frequência (rad/s)");
ylabel("Fase (graus)");
grid on
xlim([.8 105])
##g = tf([44.3],[0.075589 1])
##g2 = tf([868.2410319],[1 1736.482064 868.2410319])
