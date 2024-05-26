clear all
close all

FILENAME = 'output2024-05-25_12-55-46';
MEAN_INF_VAL_WINDOW = 100;
MEAN_FILTER_WINDOW = 100;

path = ['./output/outputDegrau/' FILENAME '.txt'];

[vel, tempo] = textread(path, "%f,%f");
vel = medfilt1(vel, MEAN_FILTER_WINDOW);

vel_inf_sum = 0;
len_vel = length(vel);

for i=0:1:MEAN_INF_VAL_WINDOW-1
    vel_inf_sum += vel(len_vel - i);
endfor

vel_inf = vel_inf_sum / MEAN_INF_VAL_WINDOW;
disp(vel_inf)

settle_time = Inf;

for j=len_vel:-1:1
    if vel(j) <= .95 * vel_inf || vel(j) >= 1.05 * vel_inf
        settle_time = tempo(j)
        break
    endif
endfor

disp("Tempo de acomodação:")
disp(settle_time)

disp("wn:")
disp(4.8/settle_time)