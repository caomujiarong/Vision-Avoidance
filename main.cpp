//
//  main.cpp
//  OpenCVdemo
//
//  Created by Jacky on 13-12-4.
//  Copyright (c) 2013年 Jacky. All rights reserved.
//



#include <curl/curl.h>
//摄像头输入
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace std;




size_t process_data(void *buffer, size_t size, size_t nmemb, void *user_p) { 	FILE *fp = (FILE *)user_p; 	size_t return_size = fwrite(buffer, size, nmemb, fp); 	cout << (char *)buffer << endl;
	return return_size; }



void getImage(){
    
    //"http://192.168.1.200/rev.cgi?Cmd=nav&action=18&drive=13";
    //    http://192.168.10.18/GetData.cgi?4325
    
    CURLcode code;
    code = curl_global_init(CURL_GLOBAL_DEFAULT);
    
    
    
    CURL *easy_handle = curl_easy_init();
    curl_easy_setopt(easy_handle, CURLOPT_URL, "http://192.168.10.18/Jpeg/CamImg[4325].jpg");
    // 向前"http://192.168.10.18//rev.cgi?Cmd=nav&action=18&drive=1&speed=15" );
    
    
    //wb+ 读写打开或建立一个二进制文件，允许读和写。
    FILE *fp = fopen("/Users/jacky/Documents/workspace/OpenCVdemo/data.jpg", "wb+");
    
    // 设置easy handle属性
    
	curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, &process_data);
	curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
    curl_easy_perform(easy_handle);//执行handle命令
    
 	// 释放资源
    fclose(fp);
	curl_easy_cleanup(easy_handle);
	curl_global_cleanup();
    
    return;
}



void robortControl(int i)
{
    
    CURLcode code;
    code = curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *easy_handle = curl_easy_init();
    switch (i) {
        case 1://forward
                        curl_easy_setopt(easy_handle, CURLOPT_URL, "http://192.168.10.18//rev.cgi?Cmd=nav&action=18&drive=1&speed=1");
            break;
            
        case 2://back
                        curl_easy_setopt(easy_handle, CURLOPT_URL, "http://192.168.10.18//rev.cgi?Cmd=nav&action=18&drive=2&speed=1");
            break;
            
        case 3://left
                       curl_easy_setopt(easy_handle, CURLOPT_URL, "http://192.168.10.18//rev.cgi?Cmd=nav&action=18&drive=5&speed=1");
            break;
            
        case 4://right
                       curl_easy_setopt(easy_handle, CURLOPT_URL, "http://192.168.10.18//rev.cgi?Cmd=nav&action=18&drive=6&speed=1");
            break;
            
        default:
            break;
    }
    curl_easy_perform(easy_handle);//执行handle命令
    
    curl_easy_cleanup(easy_handle);
	curl_global_cleanup();
}



#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>


using namespace cv;


Mat cannyimage;     //存储边界检测获得的结果
Mat coloredArea;    //涂色可以走的地方


int fromDirect = 0; //来的方向，一直修改，1,2,3,4代表上下左右，1即代表从上来，初始点为0

//如果原图像太大可能在递归过程中会溢出，因此用金字塔缩小......不过还有过小需要放大的问题，应该不会这么小吧？
void lesson(Mat& src){
    Mat result;
    
    pyrDown(src, result);
    
    while((result.cols>200)||(result.rows>200)){
        printf("I am still lessoning!!! and cols:%d,rows:%d  \n",result.cols,result.rows);
        src = result;
        pyrDown(src, result);
    }
}



//src.at<Vec3b>(x,y)[0];   第x行第y列的第0通道！


//涂色的颜色
int r=155,g=195,b=115;


//对coloredArea进行上色，选择(55,55,55)这个颜色了
void colorElement(int x,int y){
    coloredArea.at<Vec3b>(x,y)[0]=r;
    coloredArea.at<Vec3b>(x,y)[1]=g;
    coloredArea.at<Vec3b>(x,y)[2]=b;
}


//把是否为边界抽出来，可能会变更！

/*
 version 1: 检测单个点是不是canny输出的边界
 
 version 2: 由于canny算法会产生有断的地方，所以检测该点为中心的一个小的范围内，但凡有两个点为边界或者
 该点就是边界(减少判断次数！)我们就可认为它是边界(或者猜想它是断点)
 
 同时机器人不能视为质点，所以这样也可以划出一段比较有余地的区域
 
 
 @current version: version 2
 */
bool isBorder(int x,int y){
    
    if(cannyimage.at<u_char>(x,y) == 255){
        return true;
    }else{
        int countOfBorderPoint = 0;   //该区域内有多少个边界点
        for(int i=x-2; i<x+2; i++){
            for(int j=y-2; j<y+2; j++){
                bool outOfBounds = (i<0)||(j<0)||(i>cannyimage.rows)||(j>cannyimage.cols);
                if(!outOfBounds){
                    if(cannyimage.at<u_char>(i,j) == 255){
                        ++countOfBorderPoint;
                    }
                    if(countOfBorderPoint>1){
                        return true;
                    }
                }
            }
        }
        
        return false;
    }
}


//根据cannyimage初始化coloredArea，此时只有黑白两色
void initColoredArea(){
    for (int i=0; i<cannyimage.rows; i++) {
        for(int j=0; j<cannyimage.cols; j++){
            if(cannyimage.at<u_char>(i,j) == 255){
                coloredArea.at<Vec3b>(i,j)[0]=255;
                coloredArea.at<Vec3b>(i,j)[1]=255;
                coloredArea.at<Vec3b>(i,j)[2]=255;
            }else{
                coloredArea.at<Vec3b>(i,j)[0]=0;
                coloredArea.at<Vec3b>(i,j)[1]=0;
                coloredArea.at<Vec3b>(i,j)[2]=0;
            }
        }
        
    }
}

//如果在涂色区域已经被涂成了对应的颜色说明已经到过这个点了，在判断的时候也可以作为是否能到达的标准！


bool isReached(int x,int y){
    bool isReached = (coloredArea.at<Vec3b>(x,y)[0]==r) && (coloredArea.at<Vec3b>(x,y)[1] == g)
    && (coloredArea.at<Vec3b>(x,y)[2]==b);
    if(isReached){
        return true;
    }else{
        return false;
    }
}


//递归涂色
void colorArea(int x,int y){
    bool outOfBounds = (x<0)||(y<0)||(x>cannyimage.rows)||(y>cannyimage.cols);
    if(outOfBounds){
        return;
    }
    //越界以及如果是边界或者已经扫描过的点则停止
    if(isReached(x, y) || isBorder(x, y)){
        printf("BORDER!!!!\n");
        return;
    }
    
    colorElement(x,y);
    
    if(!(isReached(x-1, y) || isBorder(x-1, y))){
        colorArea(x-1, y);
        fromDirect = 4;     //从右到其左边的像素点
    }
    if(!(isReached(x+1, y) || isBorder(x+1, y))){
        colorArea(x+1, y);
        fromDirect = 3;     //从左到其右边的像素点
    }
    if(!(isReached(x, y+1) || isBorder(x, y+1))){
        colorArea(x, y+1);
        fromDirect = 1;     //从上到其下面的像素点
    }
    if(!(isReached(x, y-1) || isBorder(x, y-1))){
        colorArea(x, y-1);
        fromDirect = 2;     //从下到其上面的像素点
    }
}




int direction = 0;   //目前的方向，0为初始方向，正数为已往左，负数为已往右
int noise = 300;                 // 能够容忍的噪声数
int cntUnreachableDot = 0;      //不可到达点的数量


void direct(int x,int y){
    
    //判断前方是否存在障碍物，用一个矩形(70*30)圈定的范围内如果不可达的点数量超过容忍的噪音数(300)，则判定前方有障碍物
    
    int i,j;
    
    for(i=1; i<=70; i++){
        for(j=1; j<=30;j++){
            if(!isReached(x-i, (y-15+j))){
                cntUnreachableDot++;
            }
        }
    }
    
    
    bool isBlocked = (cntUnreachableDot>noise);
    
    if(!isBlocked){                 //没有检测到障碍物的话就前进两步
        robortControl(1);
        robortControl(1);
        robortControl(1);
        
        printf("前方一段距离没有障碍，可以前进\n");
        return;
    }else{
        printf("前方不远处有障碍物\n");
    }
    
    
    
    //前方有障碍物则转弯
    
    
    if(direction>=0){        //当前已经往左偏移，先检测能不能往右
        
        //判断右边是否能够
        cntUnreachableDot = 0;
        
        for(i=1; i<=70; i++){
            for(j=1; j<=30;j++){
                if(!isReached(x-j, (y+i))){
                    cntUnreachableDot++;
                }
            }
        }
        
        isBlocked = (cntUnreachableDot>noise);
        
        if(!isBlocked){
            printf("右边一段距离没有障碍，可以前进\n");
            direction--;
            robortControl(4);
            robortControl(4);
            robortControl(4);
            robortControl(4);
            robortControl(1);
            robortControl(1);
            robortControl(1);
            robortControl(1);
            return ;
        }else{
            printf("右边不远处有障碍物\n");
        }
        
        
        //判断左边是否能够
        cntUnreachableDot = 0;
        
        for(i=1; i<=70; i++){
            for(j=1; j<=30;j++){
                if(!isReached(x-j, (y-i))){
                    cntUnreachableDot++;
                }
            }
        }
        
        bool isBlocked = (cntUnreachableDot>noise);
        
        if(!isBlocked){
            printf("左边一段距离没有障碍，可以前进\n");
            robortControl(3);
            robortControl(3);
            robortControl(3);
            robortControl(3);
            robortControl(1);
            robortControl(1);
            robortControl(1);
            robortControl(1);
            direction++;
            return ;
        }else{
            printf("左边不远处有障碍物\n");
        }
        
        
    }else{                          //当前未偏移或者已往右偏移，优先往左移动
        
        //判断左边是否能够
        cntUnreachableDot = 0;
        
        for(i=1; i<=70; i++){
            for(j=1; j<=30;j++){
                if(!isReached(x-j, (y-i))){
                    cntUnreachableDot++;
                }
            }
        }
        
        bool isBlocked = (cntUnreachableDot>noise);
        
        if(!isBlocked){
            printf("左边一段距离没有障碍，可以前进\n");
            robortControl(3);
            robortControl(3);
            robortControl(3);
            robortControl(3);
            robortControl(1);
            robortControl(1);
            robortControl(1);
            robortControl(1);
            direction++;
            return ;
        }else{
            printf("左边不远处有障碍物\n");
        }
        
        //判断右边是否能够
        cntUnreachableDot = 0;
        
        for(i=1; i<=70; i++){
            for(j=1; j<=30;j++){
                if(!isReached(x-j, (y+i))){
                    cntUnreachableDot++;
                }
            }
        }
        if(!isBlocked){
            printf("右边一段距离没有障碍，可以前进\n");
            direction--;
            robortControl(4);
            robortControl(4);
            robortControl(4);
            robortControl(4);
            robortControl(1);
            robortControl(1);
            robortControl(1);
            robortControl(1);
            return ;
        }else{
            printf("右边不远处有障碍物\n");
        }
        
        
    }
    
    //如果前左右都不行就只能后退了！后退一段距离然后转一个夹角向另外的方向探寻
    robortControl(2);
    robortControl(2);
    robortControl(2);
    robortControl(2);
    robortControl(2);
    robortControl(2);
    robortControl(2);
    
    if(direction>=0){        //当前已经往左偏移，就继续往左转大于90°的夹角
        robortControl(3);
        robortControl(3);
        robortControl(3);
        robortControl(3);
        robortControl(3);
        robortControl(3);
    }else{
        robortControl(4);
        robortControl(4);
        robortControl(4);
        robortControl(4);
        robortControl(4);
        robortControl(4);
    }

    
}

int main(){
    
    long start,finish;
    
    Mat src;
    Mat convert;
    namedWindow( "test", CV_WINDOW_AUTOSIZE );
    
    while(true){
        
        start = clock();
        getImage();
        src = imread("/Users/jacky/Documents/workspace/OpenCVdemo/data.jpg",1);
        lesson(src);
        
        if(src.empty()){
            return -1;   //读不到图片？
        }
        
        
        
        cvtColor(src , convert, CV_RGB2BGR);
        blur(convert, convert, Size(2,2));
        
        
        Canny(convert, cannyimage, 40, 80 ,3,false);
        coloredArea = Mat::zeros( cannyimage.size(), CV_8UC3 );
        initColoredArea();
        
        
        //从接近最底的中点[因为最底有可能恰好是边界]，-8是随手设的！
        int start_row = coloredArea.rows - 8;
        int start_col = cannyimage.cols/2;
        colorArea(start_row, start_col);
        
        direct(start_row,start_col);
        
        finish = clock();
        
        double t =(double)(finish-start)/CLOCKS_PER_SEC;
        
        printf("  %f \n",t);
        
        
        //展示一下
        
//            imshow("test", cannyimage);
        imshow( "test", coloredArea );
        cvWaitKey(1);
        
        
        
    }
}
