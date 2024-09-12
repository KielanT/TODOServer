@echo off
copy "\\wsl$\Ubuntu\home\kielandev\.vs\TODOServer\out\build\RaspberryPi-debug\TODOServer\TODOServer" "out\"
scp out\TODOServer KielanDevServer@raspberrypi:~/Documents/Server 
del "out\TODOServer"