clear all
close all

files = dir('./output/outputSenoidesEscolhidas/*.txt')#senoidesExtras/outputSenoides/

i = 1

for file = files'
  path = ['./output/outputSenoidesEscolhidas/' file.name]#senoidesExtras/outputSenoides/
  [vel, tempo, sin] = textread(path, "%f,%f,%f")


  figure(i)
  i++
  subplot(2,1,1);
  plot(tempo, vel)#/10.8
  xlim([0 3]);
  xlabel("Tempo (s)");
  ylabel("Velocidade (rad/s)")
  title('Saída');
  grid on;
  h = get(gca, 'Children');
  set(h(1), 'Color', 'b');
  set(h(1), 'LineWidth', 2);
  #figure(i)
  #i++
  subplot(2,1,2);
  plot(tempo, sin)
  xlim([0 3]);
  xlabel("Tempo(s)");
  ylabel("Tensão (V)");
  title('Entrada');
  grid on;
  h = get(gca, 'Children');
  set(h(1), 'Color', 'b');
  set(h(1), 'LineWidth', 2);
endfor
