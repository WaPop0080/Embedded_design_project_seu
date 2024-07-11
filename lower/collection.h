#ifndef COLLECTION_H
#define COLLECTION_H

//标准输入输出头文件
#include <stdio.h>
#include <stdlib.h>
//文件操作函数头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <termios.h>
#include <errno.h>
#include <math.h>
//字符串头文件
#include <string.h>
//摄像头文件
// #include "camera.h" 


int initADC();             //**初始化打开ADC文件
int getADC();              //**阻值读取函数

#endif
