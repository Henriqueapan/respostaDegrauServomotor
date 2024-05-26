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
 # xlim([0 2]);
  xlabel("Tempo (s)");
  ylabel("Velocidade (rad/s)")
  #figure(i)
  #i++
  subplot(2,1,2);
  plot(tempo, sin)
  #xlim([0 2]);
  xlabel("Tempo(s)");
  ylabel("Tensão (V)");
endfor
