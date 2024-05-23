clear all
close all

[vel, tempo, sin] = textread('./output/output.txt', "%f,%f,%f")


figure(1)
plot(tempo, vel)
xlabel("Tempo (s)");
ylabel("Velocidade (rad/s)")
figure(2)
plot(tempo, sin)
xlabel("Tempo(s)");
ylabel("Tensão (V)");
