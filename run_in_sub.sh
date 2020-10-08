gcc toy-lab0.c -o lab0.exe -Wall -O3 
echo '***************lab0.exe is OK**************'
# source activate python2
# echo '切换python版本完成'
echo '***************make begin******************'
make -C sniper/
echo '***************make end********************'
./sniper/run-sniper --gdb -c  ./config-lab0.cfg -- ./lab0.exe
