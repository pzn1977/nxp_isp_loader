project(isp_loader)
cmake_minimum_required(VERSION 3.10)

add_executable(isp_loader source/assure.c 
source/chip.c 
source/fileread.c 
source/loader.c 
source/serial.c)

