#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <QPainter>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QLineEdit>
#include <QTimer>
#include <pthread.h>
#include <QProcess>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <fstream>
#include <QDateTime>
#include <QApplication>
#include <QLabel>
#include <QImage>
#include <QVector>
#include <cstdlib>
#include <QFile>
#include <iostream>
//#include <QImage>
#include "qt1.h"
#include "dlinklist.c"
#include "yuv2rgb.h"         //引入图像从YUV转换为RGB格式的函数
#include "camera.h"          //引入摄像头相关函数
#include <QTextCodec>
#include <QStringList>
#include <QTextStream>
extern "C"{                  //表示用C语言编译（当C与C++混合编译时使用）
    #include "communication.h"   //**引入TCP通信连接相关函数
    #include "collection.h"		 //gyf**数据采集相关函数
}

//用于本地图片查询时的页数判断
static int i=0;

//该数组用于记录历史电阻数据（最多50个）
int historyR[50];
int n = 0;    //记录实际采集到的历史数据个数（最多50）

//在main函数获得的IP参数（用于TCP连接）
extern int mainArgc;
extern char** mainArgv;

//声明全局变量，方便引用存储路径
QString filepath = "";
QString std_path = "";			 //对照图片存储路径
int FLAG = 1;					 //是否更新对照图片存储路径

//用于子线程循环读取上位机的查询指令后，标志当前是否进入于Rwarning、R、图片的发送模式
int RwarningStarted = 0;
int RStarted = 0;
int PhotoStarted = 0;
int Dataindex=1;

//DLIST* Qt1:: p=NULL;
DLIST *p;
DLIST *q;
DLIST head;
void insert_dlinklist(DLIST *d,char *s);
QImage convertToGrayscale(const QImage &image);
QImage resizeImage(const QImage &image, const QSize &newSize);




Qt1::Qt1(QWidget *parent):QDialog(parent)
{
    setupUi(this);
    isCapOpen = false;         //表示当前是否打开了摄像头
    isToSave = false;          //表示当前是否要存储
    m_image = NULL;
    opened = 0;
    started = 0;
    historyOpened = 0;
    delayTime = 1;             //数据采集周期默认为1s


    QString strAll;
    QStringList strList;    //创建字符串列表
    QFile file("datatime.txt");      //文件路径
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //把文件所有信息读出来
        QTextCodec *codec = QTextCodec::codecForName("utf8");    //支持读取中文
        QTextStream stream(&file);
        stream.setCodec(codec);     //输出流的设置
        strAll = stream.readAll();

    }
    file.close();
    strList = strAll.split("\n");       //换行符来分割.txt文本
    Dataindex = strList.at(0).toDouble();;             //数据采集周期默认为1s
    qDebug() <<"read_filedata:"<< Dataindex;

    adcValue = 0;

    //相关初始化函数
    InitBox();
    init_dlinklist(&head);
    initADC();   //collection.c

    //将各个按键控件均禁用
    pb_prev->setDisabled(true);
    pb_next->setDisabled(true);
    pb_del->setDisabled(true);
    lb_pic->setStyleSheet("QLabel{background-color:rgb(60,60,60);}");   //图像label背景色在没有显示图像时则为黑色

    //配置并创建摄像头对像
    width = 640;           //摄像头采集的图像大小，与开发板显示大小无关
    height = 480;
    myCamera = new Camera("/dev/video0", width, height, 0);  //调用Camera.cpp中的内容
    //图片缓存数组
    frameBufRGB = new unsigned char[width * height * 3];     //图像像素点个数乘每个像素点用的字节表示个数（注意YUV格式每个像素点用2个字节表示，RGB用3个字节表示）
    frameBufYUV = new unsigned char[width * height * 2];

    //历史曲线画布初始化
    history_image = QImage(281,211,QImage::Format_RGB32);  //画布的初始化大小设为281*211，使用32位颜色
    history_image.fill(qRgb(240,240,240));                 //对画布进行填充（画布初始化背景色使用灰色）

    //设置全屏
    this->setWindowFlags(Qt::Window);
    this->showFullScreen();

    //信号与槽连接
    connect(pb_prev,SIGNAL(clicked()),this,SLOT(fun_prev()));     //按下Prev键
    connect(pb_next,SIGNAL(clicked()),this,SLOT(fun_pic()));      //按下Next键
    connect(pb_del,SIGNAL(clicked()),this,SLOT(fun_delete())); 	  //按下Delete键
    connect(pb_open,SIGNAL(clicked()),this,SLOT(fun_open())); 	  //按下Open键（打开静态存储的图片）
    connect(&t1,SIGNAL(timeout()),this,SLOT(fun_time()));         //定时器1（用于每1s重新读取并显示当前时间）（此处使用t1额外进行摄像头图像刷新，实现视频画面）
    connect(&t2,SIGNAL(timeout()),this,SLOT(data_collection()));  //定时器2（用于循环数据采集）
    connect(&t3,SIGNAL(timeout()),this,SLOT(fun_heartbeat()));    //定时器3（用于通讯心跳包检测）
    connect(comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(SetValue()));   //下拉菜单选择（用于延时拍摄时间选择）
    connect(pb_start,SIGNAL(clicked()),this,SLOT(fun_start()));     //按下Start键：用于开始数据采集
    connect(pb_history,SIGNAL(clicked()),this,SLOT(fun_history())); //按下OpenHis键（用于查看历史数据曲线）
}



//一、下拉菜单:选择采集频率功能
//下拉组合菜单初始化设置
void Qt1::InitBox()
{
    comboBox->clear();
    QStringList strList;
    strList<<"1s"<<"0.2s"<<"0.5s"<<"2s"<<"5s";
    comboBox->addItems(strList);
    comboBox->setCurrentIndex(Dataindex);

    QString text = comboBox->currentText();
    QByteArray text1 = text.toLatin1(); //QString先转成QByteArray
    char* text2 = text1.data();
    text_time->setText(text2);
    delayTime=atof(text2);
}

//下拉菜单选择槽函数:用于设置数据采集频率
// void Qt1::SetValue(){
//     if(comboBox->currentText() == "0.2s")  //0.2s采集一次
//     {
//         delayTime = 0.2;
//         text_time->setText("0.2s");  //文本框更新显示新的延时时间15s
//     }
//     else if(comboBox->currentText() == "0.5s")  //0.5s采集一次
//     {
//         delayTime = 0.5;
//         text_time->setText("0.5s");
//     }
//     else if(comboBox->currentText() == "1s")  //1s采集一次（默认）
//     {
//         delayTime = 1;
//         text_time->setText("1s");
//     }
//     else if(comboBox->currentText() == "2s")  //2s采集一次
//     {
//         delayTime = 2;
//         text_time->setText("2s");
//     }
//     else{     //5s采集一次
//         delayTime = 5;
//         text_time->setText("5s");
//     }
// }
void Qt1::SetValue(){
    if(comboBox->currentText() == "0.2s")  //0.2s采集一次
    {
        delayTime = 0.2;
        text_time->setText("0.2s");  //文本框更新显示新的延时时间15s
        Dataindex=1;
    }
    else if(comboBox->currentText() == "0.5s")  //0.5s采集一次
    {
        delayTime = 0.5;
        text_time->setText("0.5s");
        Dataindex=2;
    }
    else if(comboBox->currentText() == "1s")  //1s采集一次（默认）
    {
        delayTime = 1;
        text_time->setText("1s");
        Dataindex=0;
    }
    else if(comboBox->currentText() == "2s")  //2s采集一次
    {
        delayTime = 2;
        text_time->setText("2s");
        Dataindex=3;
    }
    else{     //5s采集一次
        delayTime = 5;
        text_time->setText("5s");
        Dataindex=4;
    }

    QFile file("datatime.txt");
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();

    QTextStream io(&file);
    file.open(QIODevice::WriteOnly);
    io<<Dataindex;
    file.close();
    qDebug() <<"filedata:"<< Dataindex;
}

//二、Start按键:数据采集功能
//Start按键槽函数，开始或停止数据采集
void Qt1::fun_start()
{
    if(started == 0){   //若当前未打开数据采集
        started = 1;
        pb_start->setText(QApplication::translate("Qt1", "Stop", 0, QApplication::UnicodeUTF8));  //将Start按键更换显示为Stop
        fun_cap_open();            //打开摄像头
        t2.start(delayTime*1000);  //开始采集
    }
    else{               //若当前已经打开了数据采集
        started = 0;
        pb_start->setText(QApplication::translate("Qt1", "Start", 0, QApplication::UnicodeUTF8));  //将Stop按键更换显示为Start
        fun_cap_open();            //关闭摄像头
        t2.stop();                 //停止采集
    }
}
//数据循环采集函数
void Qt1::data_collection(){
     if (started == 1)	//当按下Start开始采集数据时，进入循环采集数据
     {
         //1 采集、显示图片
         fun_take_photo();	//保存图片
         char *iophoto = filepath.toLatin1().data();	//将QString Unicode编码转为字符
         char IOphoto[128] = "\0";
         strcat(IOphoto,iophoto);

         //2 采集、显示ADC阻值
         adcValue = getADC();
         QString s = QString::number(adcValue);
         text_R->setText(s);

         //3 判断、显示ADC阻值报警
         if(adcValue > 9000){
            text_Rwarning->setText("HIGH");
         }
         else if(adcValue < 1000){
            text_Rwarning->setText("LOW");
         }
         else{
            text_Rwarning->setText("OK");
         }

         //4 向上位机发送数据
         //判断当前是否需要向上位机发送R，需要则发送
         if(RStarted == 1){
            sendR(adcValue);
         }
         //判断当前是否需要向上位机发送Rwarning，需要则发送
         if(RwarningStarted == 1){
            sendRwarning(adcValue);
         }

         if(FLAG==0 && PhotoStarted == 1)
         {

              //修改
              QString filename("blue.jpg");
              QImage* img=new QImage;
              if(! ( img->load(filename) ) ) //加载图像
              {
                  QMessageBox::information(this,
                                           tr("Open Blue Failed"),
                                           tr("Open Blue Failed"));
                  delete img;
                  return;
              }
              lb_bj->setPixmap(QPixmap::fromImage(*img));

         }
         else if(FLAG==1 && PhotoStarted == 1) //判断是否更新对照照片
         {
             //修改
             QString filename("red.jpg");
             QImage* img=new QImage;
             if(! ( img->load(filename) ) ) //加载图像
             {
                 QMessageBox::information(this,
                                          tr("Open Red Failed!"),
                                          tr("Open Red Failed!"));
                 delete img;
                 return;
             }
             lb_bj->setPixmap(QPixmap::fromImage(*img));

              std_path = filepath;
              FLAG = 0;
         }
         else
         {
             lb_bj->setPixmap(QPixmap(""));
         }

        //  if(FLAG==0)
        //  {
        //      //修改
        //      QString filename("blue.jpg");
        //      QImage* img=new QImage;
        //      if(! ( img->load(filename) ) ) //加载图像
        //      {
        //          QMessageBox::information(this,
        //                                   tr("打开图像失败"),
        //                                   tr("打开图像失败!"));
        //          delete img;
        //          return;
        //      }
        //      lb_bj->setPixmap(QPixmap::fromImage(*img));
        //  }

        //  else			//判断是否更新对照照片
        //  {
        //      //修改
        //      QString filename("red.jpg");
        //      QImage* img=new QImage;
        //      if(! ( img->load(filename) ) ) //加载图像
        //      {
        //          QMessageBox::information(this,
        //                                   tr("打开图像失败"),
        //                                   tr("打开图像失败!"));
        //          delete img;
        //          return;
        //      }
        //  lb_bj->setPixmap(QPixmap::fromImage(*img));
        //   std_path = filepath;
        //   FLAG = 0;
        //  }
        //1.2 监测
         int i;
         if((i = detectDynamicChange(std_path, filepath,4) ) &&  PhotoStarted == 1)
         {
            // if(i == 1){
            //     FLAG = 1;
            // }
            FLAG = 1;
            sendPhoto(IOphoto);	//发送异常照片
         }

         //5 记录阻值历史数据
         if(n < 50){     //若未存满50个数据
             historyR[n] = adcValue;
             n++;        //n始终表示当前存储的历史数据总个数
         }
         else{           //若已存满50个数据
             int i;
             for(i = 0; i < n - 1 ; i++){
                historyR[i] = historyR[i+1];
             }
             historyR[n - 1] = adcValue;
         }

         //6 只要没有点Stop键，则持续循环读取数据
         t2.start(delayTime*1000);		//课设对延迟拍照没有要求，用其参数来定义发送周期(后边可修改)
     }
     else{           //若此时是停止采集状态，则不再循环
         t2.stop();
     }
}
//图像动态检测函数
int Qt1::recog(QString A,QString B)
{
    QImage std_image = QImage(A);
    QImage chk_image = QImage(B);
//    printf("std_image:h->%d, w->%d\n",std_image.height(),std_image.width());//标准图像宽高
//    printf("chk_image:h->%d, w->%d\n",chk_image.height(),chk_image.width());//待检图像宽高
    int x, y;                   // 循环变量
    int f = 0;                  // 检测到异常的标识
    int sq_sum;					//灰度值之差平方
    int x1 = -1, x2 = 0;        // 异常区域矩形横坐标范围
    int y1 = -1, y2 = 0;        // 异常区域矩形纵坐标范围
    for(y = 0; y < std_image.height(); y++)
    {
        for(x = 0; x < std_image.width(); x++)
        {
            sq_sum = (qGray(std_image.pixel(x,y))- qGray(chk_image.pixel(x,y)))
                    *(qGray(std_image.pixel(x,y))- qGray(chk_image.pixel(x,y)));
            if(sq_sum > 9000)            //参数1：灰度值差平方检测的阈值
            {
                    if (f)
                    {
                        if (x < x1) x1 = x; else if (x > x2) x2 = x;
                        if (y < y1) y1 = y; else if (y > y2) y2 = y;
                    }
                    else
                    {
                        f = 1;
                        x1 = x; y1 = y;
                        x2 = x; y2 = y;
                    }
            }
        }
    }
    if (((x2-x1) > chk_image.width()/4) &&((y2-y1) > chk_image.height()/4))//参数2，判断异常面积
        f = 1;
    else
        f = 0;
    if(f)
    {
        printf("diff\n");
        printf("x1:%d, x2:%d, y1:%d, y2:%d\n",x1,x2,y1,y2);
    }
    else
    {
//        printf("same\n");
//        printf("x1:%d, x2:%d, y1:%d, y2:%d",x1,x2,y1,y2);
    }
    return f;
}

//三、网络通信功能
//网络通讯心跳包槽函数（10s执行一次，若执行更频繁会导致程序卡顿）
void Qt1::fun_heartbeat(){
//    thread1.m_lock.lock();      //**每次在主线程里执行网络通信相关函数时，子线程均需暂停，防止其read干扰主线程内的通信收发

    //若检测到连接断开或错误则退出程序
    if(sendHeartbeat() != 0){
        QApplication* app;
        app->exit(0);
    }

    //持续检测通讯心跳
    t3.start(10000);
//    thread1.m_lock.unlock();            //**主线程通信结束后，子线程重新开始
}
//其他通讯功能均在commnication.c中实现


//四、摄像头功能
//打开摄像头函数
void Qt1::fun_cap_open()
{
    if(isCapOpen == 0){         //若当期摄像头没打开
        myCamera->OpenDevice(); //打开摄像头
        isCapOpen = 1;       //表示摄像头已打开
        pb_prev->setDisabled(true);  //摄像头打开时这些按键不能操作
        pb_next->setDisabled(true);
        pb_del->setDisabled(true);
        pb_open->setDisabled(true);
        pb_history->setDisabled(true);

        t1.start(50);          //开始刷新图像实现动态画面（**注意不能用->，只能用.，否则报错）
    }
    else{                        //若当期摄像头已打开
        myCamera->CloseDevice(); //关闭摄像头
        isCapOpen = 0;       //表示摄像头已关闭
        if(opened == 1){         //之前已经打开过本地图片后才能启用这些按键
            pb_prev->setDisabled(false);  //摄像头关闭时这些按键可以操作
            pb_next->setDisabled(false);
            pb_del->setDisabled(false);
        }
        pb_history->setDisabled(false);
        pb_open->setDisabled(false);

        t1.stop();
    }
    display_pic();
}
//保存摄像头图像函数
void Qt1::fun_take_photo()
{
    QDateTime time = QDateTime::currentDateTime();
    QString datatime = time.toString("MM-dd-hh-mm-ss"); //获取当前时间

    QString file_p;
    file_p += "/mnt/usb/";
    file_p += datatime;	                               //图片名称
    file_p += ".jpg";
    filepath = file_p;
    QByteArray a = file_p.toUtf8();
    m_image->save(file_p,"JPG",-1);                    //保存图片
    i++;
}
//显示图片函数（摄像头开启状态时使用）
void Qt1::showCapPhoto()
{
    if(m_image != NULL) delete m_image;    //清空图像缓存
    m_image = NULL;
    m_image = new QImage(frameBufRGB, width, height, QImage::Format_RGB888);
    lb_pic->setPixmap(QPixmap::fromImage(*m_image));
}
//定时器1槽函数(每1s刷新一次):此处使用t1额外进行摄像头图像刷新，实现视频画面
void Qt1::fun_time()
{
    if(isCapOpen == 1){      //只有在摄像头打开时才进行刷新
        fun_refresh_pic();   //刷新当期图像
        QDateTime d=QDateTime::currentDateTime();	               //获取当期日期时间
        lb_time->setText(d.toString("yyyy-MM-dd-ddd hh:mm:ss"));   //在标签lb_time上显示时间

        t1.start(50);      //只要摄像头打开，则持续循环刷新实现动态画面
    }
    else{
        t1.stop();
    }
}
//刷新当前显示图片的函数（摄像头开启状态时使用）
void Qt1::fun_refresh_pic()
{
   if(!myCamera->GetBuffer(frameBufYUV))   //获取摄像头采集的图像
   {
       std::cout<< "Get Camera Buf error!\n";
       return;
   }
   //TODO:process_image() function is now empty.
   myCamera->process_image(frameBufYUV, frameBufRGB);  //将采集到的YUV格式图像转为RGB格式（调用Camera.cpp中的函数）
   showCapPhoto();   //将新采集的图片显示出来
}


//五、绘制历史曲线
void Qt1::fun_history(){
    if(historyOpened == 0){
        //1 切换历史曲线显示模式
        historyOpened = 1;  //表示当前已打开历史数据曲线
        pb_history->setText(QApplication::translate("Qt1", "CloseHis", 0, QApplication::UnicodeUTF8));  //将OpenHis按键更换显示为CloseHis

        //2 暂时隐藏图片label，占用该区域来画历史曲线，关闭相关按键功能
        lb_pic->hide();
        pb_prev->setDisabled(true);
        pb_next->setDisabled(true);
        pb_del->setDisabled(true);
        pb_open->setDisabled(true);
        pb_start->setDisabled(true);

        //3 创建QPainter
        QPainter painter(&history_image);
        painter.setRenderHint(QPainter::Antialiasing, true);//设置反锯齿模式，好看一点

        //4 设置图像位置与大小
        int pointx=10+25,pointy=201-15; //确定坐标轴起点坐标，这里定义(35,191)
        int width=251-pointx,height=161;//确定坐标轴宽度跟高度 上文定义画布为600X300，宽高依此而定。

        //5 绘制坐标轴
        painter.drawRect(5,5,271,201);//外围的矩形，从(10,10)起，到(281,211)结束，周围留了5的间隙。
        painter.drawLine(pointx,pointy,width+pointx,pointy);//坐标轴x宽度为width
        painter.drawLine(pointx,pointy-height,pointx,pointy);//坐标轴y高度为height
        srand(time(NULL));

         //6 获得数据中最大值、最小值
         int max=historyR[0];//数组里的最大值
         int min=historyR[0];
         int maxpos=0,minpos=0;
         for(int i=0;i<n;i++)
         {
             if(historyR[i]>max){
                 max=historyR[i];
                 maxpos=i;
             }
             if(historyR[i]<min){
                 min=historyR[i];
                 minpos=i;
             }
         }

         //7 画数据点和折线
         double kx=(double)width/(n-1); //x轴的系数
         double ky=(double)height/max;//y方向的比例系数
         QPen pen,penPoint;
         pen.setColor(Qt::black);
         pen.setWidth(2);
         penPoint.setColor(Qt::black);
         penPoint.setWidth(5);
         for(int i=0;i<n-1;i++)
         {
             //由于y轴是倒着的，所以y轴坐标要pointy-a[i]*ky 其中ky为比例系数
             painter.setPen(pen);      //黑色笔用于连线
             painter.drawLine(pointx+kx*i,pointy-historyR[i]*ky,pointx+kx*(i+1),pointy-historyR[i+1]*ky);
             painter.setPen(penPoint); //黑色的笔，用于标记各个点
             painter.drawPoint(pointx+kx*i,pointy-historyR[i]*ky);
         }
         painter.drawPoint(pointx+kx*(n-1),pointy-historyR[n-1]*ky);//绘制最后一个点

         //8 绘制最大值、最小值（用红色点画最大值、最小值）
         QPen penMaxMin;
         penMaxMin.setColor(Qt::red);
         penMaxMin.setWidth(7);
         painter.setPen(penMaxMin);
         painter.drawPoint(pointx+kx*maxpos,pointy-historyR[maxpos]*ky);//标记最大值点
         painter.drawPoint(pointx+kx*minpos,pointy-historyR[minpos]*ky);//标记最小值点
         painter.drawText(pointx+kx*maxpos-kx,pointy-historyR[maxpos]*ky-5,"max="+QString::number(max));  //显示最大最小值文本
         painter.drawText(pointx+kx*minpos-kx,pointy-historyR[minpos]*ky+15,"min="+QString::number(min));

         //9 绘制刻度线
         QPen penDegree;
         penDegree.setColor(Qt::black);
         penDegree.setWidth(2);
         painter.setPen(penDegree);
         //画x轴刻度线
         for(int i=0;i<10;i++)//分成10份
         {
             //选取合适的坐标，绘制一段长度为4的直线，用于表示刻度
             painter.drawLine(pointx+(i+1)*width/10,pointy,pointx+(i+1)*width/10,pointy-4);
             painter.drawText(pointx+(i+0.65)*width/10,pointy+15,QString::number((int)((i+1)*((double)n/10))));
         }
         //画y轴刻度线
         double _maStep=(double)max/10;//y轴刻度间隔需根据最大值来表示
         for(int i=0;i<10;i++)
         {
             //确定一个位置，然后画一条短短的直线表示刻度。
             painter.drawLine(pointx,pointy-(i+1)*height/10,pointx+4,pointy-(i+1)*height/10);
             painter.drawText(pointx-25,pointy-(i+0.85)*height/10,QString::number((int)(_maStep*(i+1))));
         }
    }
    else{
         //1 退出历史曲线显示模式
         historyOpened = 0;  //表示当前已关闭历史数据曲线
         pb_history->setText(QApplication::translate("Qt1", "OpenHis", 0, QApplication::UnicodeUTF8));  //将CloseHis按键更换显示为OpenHis

         //2 图片label显示，相关按钮恢复功能
         lb_pic->show();
         if(opened == 1){         //之前已经打开过本地图片后才能启用这些按键
             pb_prev->setDisabled(false);
             pb_next->setDisabled(false);
             pb_del->setDisabled(false);
         }
         pb_open->setDisabled(false);
         pb_start->setDisabled(false);

         //3 对画布进行覆盖，起到清屏作用
         history_image.fill(qRgb(240,240,240));
    }
}


//六、本地图片功能
//按下Next键槽函数
void Qt1::fun_pic()
{
    if(isCapOpen == 0){   //只有摄像头关闭时才能切换图片
        i++;                   //切换到下一张图片
        if(i==(len+1))i=1;     //若已经是最后一张，则循环到第一张

        p=p->next;
        display_pic();         //刷新显示图片
    }
}
//按下Prev键槽函数
void Qt1::fun_prev()
{
    if(isCapOpen == 0){    //只有摄像头关闭时才能切换图片
        i--;                   //切换到上一张图片
        if(i==0)i=len;         //若已经是第一张，则循环到最后一张

        p=p->prev;
        display_pic();         //刷新显示图片
    }
}
//显示图片（摄像头关闭状态时使用，可显示静态存储的照片）
void Qt1::display_pic()
{
    QString buf(p->path);
    QPixmap p(buf);
    lb_pic->setPixmap(p);
    lb_pic->setScaledContents(1);
    lb_num->setText(QString::number(i));
}
//按下Delete键槽函数
void Qt1::fun_delete()
{
    int ret;
        ret=QMessageBox::warning(this, tr("m_image browser"),
                       tr("Are you sure to delete it?"),
                       QMessageBox::Yes | QMessageBox::No,
                       QMessageBox::No);
    if(ret==QMessageBox::Yes)
    {
        remove(p->path);
        DLIST *r=(&head)->next;
        int n=1;
        while(r!=p)
        {
            n++;
            r=r->next;
        }
        p=p->next;
        del_dlinklist(&head,n);
        if(i==len)i--;
        len=legnth_dlinklist(&head);
        display_pic();
    }
//	dlinkilist_tofile(&head);
}
//按下Open键槽函数（打开静态存储的图片）
void Qt1::fun_open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/mnt/usb/",tr("Images (*.png *.xpm *.jpg)"));//lujing
    printf("open folder!\n");
    if(fileName!=NULL)
    {
        init_dlinklist(&head);
        char *str;
        char filename[100]={0};
        DIR *dp;
        struct dirent *dirp;

        int m=strlen(fileName.toAscii().data());
        str=(char*)malloc(m);
        strcpy(str,fileName.toAscii().data());
        strcpy(filename,str);

        QString buf(fileName.toAscii().data());
        QPixmap first(buf);
        lb_pic->setPixmap(first);
        lb_pic->setScaledContents(1);

        strip(filename);
        dp=opendir(filename);
        while(dirp=readdir(dp))
        {
            if(judge(dirp->d_name)==1)
            {
                strcat(filename,"/");
                strcat(filename,dirp->d_name);
                printf("%s\n",filename);
                insert_dlinklist(&head,filename);
                filename[strlen(filename)-strlen(dirp->d_name)-1]='\0';
            }
        }
        q->next=(&head)->next;
        (&head)->next->prev=q;

        len=legnth_dlinklist(&head);
        int pos=1;
        DLIST *r=(&head)->next;
        while(strcmp(r->path,str)!=0)
        {
            r=r->next;
            pos++;
        }
        p=r;
        i=pos;
        lb_num->setText(QString::number(i));
        //lb_sum->setText(QString::number(len));

        pb_prev->setDisabled(false);
        pb_next->setDisabled(false);
        pb_del->setDisabled(false);

        opened = 1;   //表示已经打开了本地图片
		display_pic();
    }
//	dlinkilist_tofile(&head);
}


//七、其他功能
void Qt1::strip(char *s)
{
    char *p=s;
    int n=0;
    while(*p)
    {
        if(*p=='/')
        {
            n++;
        }
        p++;
    }
    if(n==1)
    {
        s[1]='\0';
    }
    else
    {
        p--;
        while(*p)
        {
            if(*p=='/')
            {
                *p='\0';
                break;
            }
            p--;
        }
    }
}
int Qt1::judge(char * s)
{
    int i,len=strlen(s);
    char *p=s;
    if(len<5)
    {
        return 0;
    }
    else
    {
        while(*p)
        {
            p++;
        }
        for(i=0;i<4;i++)p--;
        if((strncmp(p,".jpg",4)==0)||(strncmp(p,".png",4)==0)||(strncmp(p,".xpm",4)==0))
        {
            return 1;
        }
        else
            return 0;
    }
}
void insert_dlinklist(DLIST *d,char *s)
{
    DLIST *r=d;
//	DLIST *q;
    q=(DLIST*)malloc(sizeof(DLIST));
    memset(q,0,sizeof(q));
    strcpy(q->path,s);

    if(r->next==NULL)
    {
        r->next=q;
        q->next=NULL;
    }
    else
    {
        while(r->next!=NULL)
        {
            r=r->next;
        }
        r->next=q;
        q->prev=r;
        q->next=NULL;
    }
    printf("insert success\n");
}
Qt1::~Qt1()
{
    delete[] frameBufRGB;
    delete[] frameBufYUV;
    delete m_image;
}
void Qt1::fun_clean_pixmap()
{
    QPixmap p;
    lb_pic->setPixmap(p);
}

QImage convertToGrayscale(const QImage &image) {
    // QT4.7中没有QImage::Format_Grayscale8
    // return image.convertToFormat(QImage::Format_Grayscale8);
    // 创建一个和原图一样大小的图像
    QImage temp_img(image.size(), QImage::Format_ARGB32);
    for (int i = 0; i < image.width(); i++)
    {
        for (int j= 0; j < image.height(); j++)
        {
            QRgb color = image.pixel(i, j);
            int gray = qGray(color);
            temp_img.setPixel(i, j, qRgba(gray, gray, gray, qAlpha(color)));
        }
    }

    return temp_img;
}

QImage resizeImage(const QImage &image, const QSize &newSize) {
    //  将图像的大小调整为newSize，
    return image.scaled(newSize, Qt::KeepAspectRatio, Qt::FastTransformation);
}

int Qt1::detectDynamicChange(QString A,QString B, int stride) {
    QImage std_image = QImage(A);
    QImage chk_image = QImage(B);
    if (std_image.size() != chk_image.size()) {
        return 0;
    }

    QImage std_gray_image = convertToGrayscale(std_image);
    QImage chk_gray_image = convertToGrayscale(chk_image);

    int dynamicDetectionFlag = 0;
    int dynamicpoints = 0;


    for (int y = 0; y < std_gray_image.height(); y += stride) {
        for (int x = 0; x < std_gray_image.width(); x += stride) {
            int gray_std_value = qGray(std_gray_image.pixel(x, y));
            int gray_chk_value = qGray(chk_gray_image.pixel(x, y));
            int diff = std::abs(gray_std_value - gray_chk_value);

            if (diff > 30) {
                dynamicpoints = dynamicpoints + 1;
                // dynamicDetectionFlag = 1;
                // break;
            }
            if(dynamicpoints > 2500)
            {
                dynamicDetectionFlag = 1;
                //当检测到动态点超过2500个时，直接返回1，不再继续检测
                break;
            }

        }
        if(dynamicDetectionFlag == 1)
        {
            break;
        }

    }
    // printf("dynamicpoints: %d\n",dynamicpoints);

    return dynamicDetectionFlag;
}
/*
QImage convertToGrayscale(const QImage& image) {
    return image.convertToFormat(QImage::Format_Grayscale8);
}
*/
/*
QImage resizeImage(const QImage& image, const QSize& newSize) {
    return image.scaled(newSize, Qt::KeepAspectRatio, Qt::FastTransformation);
}
*/
QVector<int> calculateHistogram(const QImage& image, int stride) {
    QVector<int> histogram(256, 0);

    for (int y = 0; y < image.height(); y += stride) {
        for (int x = 0; x < image.width(); x += stride) {
            int gray_value = qGray(image.pixel(x, y));
            histogram[gray_value]++;
        }
    }

    return histogram;
}

double calculateHistogramIntersection(const QVector<int>& histogram1, const QVector<int>& histogram2) {
    double intersection = 0;

    for (int i = 0; i < 256; ++i) {
        intersection += std::min(histogram1[i], histogram2[i]);
    }

    return intersection;
}

int detectDynamicChange(const QImage& std_image, const QImage& chk_image) {
    if (std_image.size() != chk_image.size()) {
        return 0;
    }

    QSize newSize(320, 240);
    QImage std_resized_image = resizeImage(std_image, newSize);
    QImage chk_resized_image = resizeImage(chk_image, newSize);

    QImage std_gray_image = convertToGrayscale(std_resized_image);
    QImage chk_gray_image = convertToGrayscale(chk_resized_image);

    int stride = 2;
    QVector<int> std_histogram = calculateHistogram(std_gray_image, stride);
    QVector<int> chk_histogram = calculateHistogram(chk_gray_image, stride);

    double histogram_intersection = calculateHistogramIntersection(std_histogram, chk_histogram);
    double total_pixels = (std_resized_image.width() * std_resized_image.height()) / (stride * stride);
    double similarity_threshold = 0.95;

    return (histogram_intersection / total_pixels) < similarity_threshold ? 1 : 0;
}