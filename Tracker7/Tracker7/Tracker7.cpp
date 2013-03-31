// Tracker7.cpp

#include<opencv/cvaux.h>
#include<opencv/highgui.h>
#include<opencv/cxcore.h>

#include<stdio.h>
#include<stdlib.h>

#define INF_LOOP 1

///////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) 
{
  CvSize size640x480 = cvSize(640,480);     // Create Image of size 640x480 to store captured frame
    
  CvCapture* pCapWebCam;        // Object to store webcam handle
  IplImage* pImgOriginal;       // Pointer to the captured webcam frame
  IplImage* pImgProcessed;      // Pointer to the processed frame

  CvMemStorage* pHoughCirclesStorage; // Storage for cvHoughCircles()

  CvSeq* pSeqCircles;           // Pointer to circles array returned by cvHoughCircles

  float* pXYRadius;             // Pointer to 3 element array that has X,Y center position
                                // and the radius.

  int i;                        // General Loop Counter
  char charCheckForEscKey;      // Check key press and Exit on Esc

  pCapWebCam = cvCaptureFromCAM(0);
  if(pCapWebCam == NULL) 
  {
    printf("Error: webcam failed to open\n");
    getchar();
    return(-1);  
  }

  

  cvNamedWindow("Original Image", CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Processed Image", CV_WINDOW_AUTOSIZE);

  pImgProcessed = cvCreateImage(size640x480,    // Image Size
                                IPL_DEPTH_8U,   // 8-bits per pixel 
                                1);             // Grayscale 1-channel

  while(INF_LOOP) 
  {
    // Capture a frame from the web cam
    pImgOriginal = cvQueryFrame(pCapWebCam);
    if(pImgOriginal == NULL) {
      printf("Error: webcam failed to capture a frame\n");
      getchar();
      return(-1);
    }

    // Threshold the image to ball color expected and create binary mask
    cvInRangeS(pImgOriginal, CV_RGB(105,0,0), CV_RGB(255,50,50), pImgProcessed);
    
    // Smooth binary mask to make hough detection more robust
    cvSmooth(pImgProcessed, pImgProcessed, CV_GAUSSIAN, 9, 9);

    // Detect circles 
    pHoughCirclesStorage = cvCreateMemStorage(0); // 0 passed allocates 64KB memory
    pSeqCircles = cvHoughCircles(pImgProcessed, 
                                 pHoughCirclesStorage, 
                                 CV_HOUGH_GRADIENT,
                                 2,                   // Accumulator resolution param
                                 pImgProcessed->height / 4, // Min distance between circle centers
                                 100, 50,    // Canny edge high and low thresholds
                                 10, 400);   // Circle min and max radius

    for(i = 0; i < pSeqCircles->total; i++) 
    { // for every circle found
    
      pXYRadius = (float*)cvGetSeqElem(pSeqCircles, i);

      printf("Ball position [X,Y,R] = [%f,%f,%f]\n", pXYRadius[0], pXYRadius[1], pXYRadius[2]);

      // Draw circle center in green color
      cvCircle(pImgOriginal, 
               cvPoint(cvRound(pXYRadius[0]), cvRound(pXYRadius[1])),
               3, 
               CV_RGB(0,255,0), 
               CV_FILLED);

      // Draw circle outline in red color
      cvCircle(pImgOriginal, 
               cvPoint(cvRound(pXYRadius[0]), cvRound(pXYRadius[1])),
               cvRound(pXYRadius[2]), 
               CV_RGB(255,0,0), 
               3);
    }

    // Show the processed images with circle overlays
    cvShowImage("Original Image", pImgOriginal);
    cvShowImage("Processed Image", pImgProcessed);

    cvReleaseMemStorage(&pHoughCirclesStorage);

    charCheckForEscKey = cvWaitKey(10);				// delay (in ms), and get key press, if any
    if(charCheckForEscKey == 27) break;				// if Esc key (ASCII 27) was pressed, jump out of while loop
  }

  cvReleaseCapture(&pCapWebCam);

  cvDestroyWindow("Original Image");
  cvDestroyWindow("Processed Image");

  return(0);
}
