#include "collection.h"

//**注意：不要在.h里定义全局自变量，否则其他文件在include该.h文件时可能导致其中的全局变量重复定义
int fd_adc;
char *adc = "/dev/adc";	//滑动变阻器路径
char buffer[512];       //存放adc读取的缓存
int value;              //存储电阻值



int initADC(){  //初始化变阻器，打开变阻器。
    if((fd_adc = open(adc, O_RDWR| O_NOCTTY | O_NDELAY )) < 0){    //打开滑动变阻器adc，若失败则报错
        printf("open ADC error\n");
        return -1;
    }
    return 0;
}

int getADC()    //获取阻值信息
{
	memset(buffer,0,sizeof(buffer));
    int adc_len = read(fd_adc, buffer, 10); //读取电阻值
    value = atoi(buffer);
	value = value * 10000 / 4095;
//	close(adc);
    return value;
}


