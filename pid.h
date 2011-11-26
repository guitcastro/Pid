
#include <iostream>
#include <cv.h>
#include <cxcore.h>
#include <math.h>
#include <highgui.h>


class Pid {
private:
    /*
     * Converte o pixel , para tons de cinza
     */
    CvScalar toGray(CvScalar pixel);

    /*
     * Executa o seguinte calculo da forma de fourier:
     * -2pi * (xu/m + yv/n)
     */
    double auxFourier(int y, int x, int v, int u, int m, int n);

    /*
     * Calcula o coseno de x em rad
     */
    double cosineFourier(double x);

    /*
     * Calcula o seno de x em rad
     */
    double sinFourier(double x);



public:
    /*
     * Normaliza a transformada de Fourier
     */
    static void NormalizarFourier(IplImage* img_real, IplImage* img_imaginaria);

     /*
     * Encontra o pixel de maior valor na imagem
     */
    static int findMaxNC(IplImage* img);

    /*
     * Converte a imagem para tons de cinza
     */
    void toGray(IplImage *img);

    /*
     * Executa a transformada de Fourier
     */
    int fourier(IplImage* img);

    /*
     * Equaliza o histograma da imagem
     */
    int Histograma(IplImage* img);

    /*
     * Cria uma mascará Convolução discreta
     */
    int ** CreatMask ();

    /*
     *
     */
    int ** MaskGauss();

    Pid (){

    }

    void showImage (IplImage* img);

    void ChromaKey (char * , char *,bool = false);

    void subBackground(IplImage *img, IplImage *img2,bool= false);

    void canny(char * img_name);

    void hough(char * img_name);

    void MindDist(char * img_name);

    CvScalar getMedia(IplImage * img_name);

    IplImage * CriarAmostra (char * img_name, int i1, int i2 , int j1, int j2,int  width,int height);

    cv::Mat  covariance(IplImage* img, CvScalar media);

    void bayesiana(char * img_name);

};