clear all
close all

files = dir('./output/outputDegrau/*.txt')

i = 1

for file = files'
  path = ['./output/outputDegrau/' file.name]
  [vel, tempo] = textread(path, "%f,%f")


  figure(i)
  i++
  plot(tempo, vel)
  xlabel("Tempo (s)");
  ylabel("Velocidade (rad/s)")
endfor
