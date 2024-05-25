clear all
close all

files = dir('./output/outputSenoides/*.txt')

i = 1

for file = files'
  path = ['./output/outputSenoides/' file.name]
  [vel, tempo, sin] = textread(path, "%f,%f,%f")


  figure(i)
  i++
  subplot(2,1,1);
  plot(tempo, vel)#/10.8
  xlabel("Tempo (s)");
  ylabel("Velocidade (rad/s)")
  #figure(i)
  #i++
  subplot(2,1,2);
  plot(tempo, sin)
  xlabel("Tempo(s)");
  ylabel("Tensão (V)");
endfor
