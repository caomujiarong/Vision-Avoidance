//
//  main.cpp
//  OpenCVdemo
//
//  Created by Jacky on 13-12-4.
//  Copyright (c) 2013年 Jacky. All rights reserved.
//

/*#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char * argv[])
{
    Mat img = imread("/Users/jacky/Pictures/colorTable.jpg");
    if (img.data == 0) {
        cerr<<"image not found!!"<<endl;
        return -1;
    }
    namedWindow("image",CV_WINDOW_AUTOSIZE);
    imshow("image", img);
    waitKey();
    return 0;
}

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>

using namespace cv;
using namespace std;

Mat src;
Mat hsv;
Mat hue;

int bins = 25;

void Hist_and_Backproj(int, void* );

int main(int argc,char* argv[])
{
    src = imread(argv[1], 1);
    cvtColor(src, hsv, CV_BGR2HSV);
    
    hue.create( hsv.size(), hsv.depth() );
    int ch[] = { 0,  0};
    mixChannels(&hsv, 1, &hue, 1, ch, 1);
    
    char* window_image = "Source image";
    namedWindow(window_image, CV_WINDOW_AUTOSIZE);

    createTrackbar("* Hue  bins: ", window_image, &bins, 180, Hist_and_Backproj );
    Hist_and_Backproj(0, 0);
    
    imshow(window_image, src);
    
    waitKey(0);
    return 0;
}

void Hist_and_Backproj(int, void* )
{
    MatND hist;
    int histSize = MAX( bins, 2 );
    float hue_range[] = { 0, 180 };
    const float* ranges = { hue_range };
    
    /// 计算直方图并归一化
    calcHist( &hue, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false );
    normalize( hist, hist, 0, 255, NORM_MINMAX, -1, Mat() );
    
    /// 计算反向投影
    MatND backproj;
    calcBackProject( &hue, 1, 0, hist, backproj, &ranges, 1, true );
    
    /// 显示反向投影
    imshow( "BackProj", backproj );
    
    /// 显示直方图
    int w = 400; int h = 400;
    int bin_w = cvRound( (double) w / histSize );
    Mat histImg = Mat::zeros( w, h, CV_8UC3 );
    
    for( int i = 0; i < bins; i ++ )
    { rectangle( histImg, Point( i*bin_w, h ), Point( (i+1)*bin_w, h - cvRound( hist.at<float>(i)*h/255.0 ) ), Scalar( 0, 0, 255 ), -1 ); }
    
    imshow( "Histogram", histImg );
}

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc,char* argv[])
{
    //将图像文件加载至内存 分配图像数据结构所需要的内控 返回一个指向数据结构IplImage的内存块：
    IplImage *img = cvLoadImage("/Users/jacky/Pictures/com.tencent.ScreenCapture/QQ20130815-1@2x.png");
    //需要把这个图片放在这个project下面，比如我的：&user name\Documents\Visual Studio 2010\Projects\opencvhello\opencvhello文件夹里面
    //定义两个窗口，自由大小。本函数由HighGUI库提供。第二个参数如果为0，则窗口大小不会因图像的大小而改变。
    cvNamedWindow("Image-in",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Image-out",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Image-out2",CV_WINDOW_AUTOSIZE);
    //先显示原jpg图
    cvShowImage("Image-in",img);
    //分配空间存储处理后的图像
    IplImage *out=cvCreateImage(
                                cvGetSize(img),//当前图像大小
                                IPL_DEPTH_8U,//各通道每个像素点的类型
                                3//通道总数
                                );
    //进行高斯处理，处理的是指针img指向的内存，将处理后的数据交给out指针指向的内存，对每个像素周围3x3的区域进行高斯平滑处理（其实输入输出图像可以是相同的）
    
    cvSmooth(img,out,CV_GAUSSIAN,3,3);
    //显示处理后的图像
    cvShowImage("Image-out",out);
    
    IplImage *out2 = cvCreateImage(cvGetSize(img),//当前图像大小
                                   IPL_DEPTH_8U,//各通道每个像素点的类型
                                   3//通道总数
                                   );
    
    cvSmooth(img, out2,CV_MEDIAN,3,3);
    cvShowImage("Image-out2", out2);
    
    //清除垃圾
    cvReleaseImage(&out);
    cvReleaseImage(&img);
    cvReleaseImage(&out2);
    
    //cvWaitKey的参数如果是正值，则程序会等待数值个毫秒，然后继续运行；如果是负值或者0，就会等待用户触发按键操作，然后继续程序。
    cvWaitKey();
    //销毁窗口，养成好习惯
    cvDestroyWindow("Image-in");
    cvDestroyWindow("Image-out");
    
    cvDestroyWindow("Image-out2");
    
    return 0;
    
}
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;


int main()
{
	int height,width,step,channels;
	IplImage *contour_img=cvLoadImage("/Users/jacky/Documents/workspace/OpenCVdemo/test.jpg",-1);//3通道图像
	if (!contour_img)
	{
		cout<<"File can't be opened"<<endl;
	}
    
	IplImage *band1=cvCreateImage(cvGetSize(contour_img),IPL_DEPTH_8U,1);
	IplImage *dst=  cvCreateImage(cvGetSize(contour_img),IPL_DEPTH_8U,3);
    
	cvCopy(contour_img,dst);
	cvSplit(contour_img,band1,NULL,NULL,NULL);
    
	cvSaveImage("/Users/jacky/Documents/workspace/OpenCVdemo/reverse.jpg",band1);
	
	int i,j;
	height  =dst->height;
	width   =dst->width;
	step    =dst->widthStep;
	channels=dst->nChannels;
	cout<<"height: "<<height<<" width: "<<width<<" step: "<<step<<" channels: "<<channels<<endl;
    
	CvMemStorage *storage=cvCreateMemStorage(0);
	CvSeq *contour=0;
    
	int NoContours=cvFindContours(band1,storage,&contour,sizeof(CvContour),CV_RETR_CCOMP,CV_CHAIN_APPROX_NONE);
	cout<<"No of contours: "<<NoContours<<endl;
	CvPoint *currentPoint;
	unsigned char *p_band;
	CvSeq *v_list;
    
	int total;
    
	for (;contour!=0;contour=contour->h_next)
	{
		total=contour->total;//获得点的个数
		cout<<"total: "<<total<<endl;
		for (i=0;i<total;i++)
		{
			currentPoint=(CvPoint *)cvGetSeqElem(contour,i);
			p_band=(unsigned char *)dst->imageData+currentPoint->y*step+channels*currentPoint->x;
			*p_band=0;
			*(p_band+1)=255;
			*(p_band+2)=0;
		}
		v_list=contour->v_next;
		for (;v_list!=NULL;v_list=v_list->h_next)
		{
			total=v_list->total;
			for (i=0;i<total;i++)
			{
				currentPoint=(CvPoint *)cvGetSeqElem(v_list,i);
				p_band=(unsigned char *)dst->imageData+currentPoint->y*step+channels*currentPoint->x;
				*p_band=0;
				*(p_band+1)=2;
				*(p_band+2)=255;
			}
		}
		
	}
	cvSaveImage("/Users/jacky/Documents/workspace/OpenCVdemo/reverse3.jpg",dst);
	return 0;
}*/


#include <curl/curl.h>
//摄像头输入
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/opencv.hpp>
#include <iostream>


using namespace std;

int g_slider_position = 0;
CvCapture * g_capture = NULL;
CURL * test = curl_easy_init();

void onTrackbarSlide(int pos){
    cvSetCaptureProperty(g_capture, CV_CAP_PROP_POS_FRAMES, pos);
}

size_t process_data(void *buffer, size_t size, size_t nmemb, void *user_p)
{
    FILE *fp = (FILE *)user_p;
    size_t return_size = fwrite(buffer, size, nmemb, fp);
 	cout << (char *)buffer << endl;
	return return_size;
}


int main( int argc, char** argv){
    
   
/*
 function manualRequest(url, params, onSuccess, method){
 if(method == null) method = 'POST';
 if(method == 'GET'){
 url += '?' + params;
 params = '';
 }
 
 // make sure that no threads require authentication
 var t;
 for(t = 0; t < MAX_REQUESTS; t++){
 if(xhr[t].readyState != 0){
 try {
 if(xhr[t].status == 401){
 return;
 }
 } catch(e){
 }
 }
 }
 
 // find a free thread that's DONE
 var count = 0;
 do {
 count++;
 cur_xhr++;
 if(cur_xhr >= MAX_REQUESTS) cur_xhr=0;
 if(count > MAX_REQUESTS*2){
 return;
 }
 } while(xhr[cur_xhr].readyState != 0 && xhr[cur_xhr].readyState != 4)
 
 var cur_index = cur_xhr;
 xhr[cur_index].open (method, url, true);
 
 if(onSuccess != null){
 xhrRtn[cur_index] = onSuccess;
 // call return function once request returns successfully
 xhr[cur_index].onreadystatechange = new Function("var index = " + cur_index + "; if (xhr[index].readyState == 4 && xhr[index].status == 200){ xhrRtn[index](xhr[index]); }");
 } else {
 xhrRtn[cur_index] = null;
 xhr[cur_index].onreadystatechange = function (){ }
 }
 
 try {
 xhr[cur_index].send(params);
 } catch(e) {
 }
 }
 
 */
    
    

//"http://192.168.1.200/rev.cgi?Cmd=nav&action=18&drive=13";
//    http://192.168.10.18/GetData.cgi?4325
    CURLcode return_code = curl_global_init(CURL_GLOBAL_ALL);
    
    if (CURLE_OK != return_code)
    {
        cerr << "init libcurl failed." << endl;
        return -1;
    }
    
    CURL *easy_handle = curl_easy_init();
    curl_easy_setopt(easy_handle, CURLOPT_URL, "http://192.168.10.18/GetData.cgi?4325" );
    
    curl_easy_perform(easy_handle);
 	// 释放资源

	curl_easy_cleanup(easy_handle);
	curl_global_cleanup();
    
    return 0;
}

/*
    cvNamedWindow("Example3", CV_WINDOW_AUTOSIZE);
    g_capture = cvCreateFileCapture( "/Users/jacky/Downloads/openc.avi" );
    
    int frames = (int) cvGetCaptureProperty(g_capture, CV_CAP_PROP_FRAME_COUNT);
    
    if (frames != 0) {
        cvCreateTrackbar("Position", "Example3", &g_slider_position, frames,onTrackbarSlide);
    }
    
    IplImage * frame;
    if (g_capture) {
        while (1) {
            if (!cvGrabFrame(g_capture)) {
                break;
            }
            frame = cvQueryFrame(g_capture);
            if (!frame) {
                break;
            }
            cvShowImage("Example3", frame);
            
            char c = cvWaitKey(33);
            if ( c == 27 ) {
                break;
            }
        }
    }
    cvReleaseCapture( &g_capture );
    cvDestroyWindow( "Example3" );
    
    return(0);
}


灰度视频输出
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/opencv.hpp>

int g_slider_position = 0;
CvCapture * g_capture = NULL;

void onTrackbarSlide(int pos){
    cvSetCaptureProperty(g_capture, CV_CAP_PROP_POS_FRAMES, pos);
}

int main( int argc, char** argv){
    cvNamedWindow("Example3", CV_WINDOW_AUTOSIZE);
    g_capture = cvCreateCameraCapture(0);
    //( "/Users/jacky/Downloads/openc.avi" );
    
    int frames = (int) cvGetCaptureProperty(g_capture, CV_CAP_PROP_FRAME_COUNT);
    
    if (frames != 0) {
        cvCreateTrackbar("Position", "Example3", &g_slider_position, frames,onTrackbarSlide);
    }
    
    IplImage * frame;
    if (g_capture) {
        while (1) {
            if (!cvGrabFrame(g_capture)) {
                break;
            }
            frame = cvQueryFrame(g_capture);
            if (!frame) {
                break;
            }
            cvShowImage("Example3", frame);
            
            char c = cvWaitKey(33);
            if ( c == 27 ) {
                break;
            }
        }
    }
    cvReleaseCapture( &g_capture );
    cvDestroyWindow( "Example3" );
    
    return(0);
}

*/