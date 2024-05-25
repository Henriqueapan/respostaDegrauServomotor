clear all
close all

files = dir('./output/*.txt')

i = 1

for file = files'
  path = ['./output/' file.name]
  [vel, tempo] = textread(path, "%f,%f")


  figure(i)
  i++
  plot(tempo, vel)
  xlabel("Tempo (s)");
  ylabel("Velocidade (rad/s)")
endfor
