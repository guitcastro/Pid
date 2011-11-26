#include "pid.h"

void Pid::showImage(IplImage* img) {
    cvStartWindowThread();
    cvNamedWindow("Image", 1);
    cvShowImage("Image", img);
    cvWaitKey(0);
    cvReleaseImage(&img);
    cvDestroyWindow( "Image");
}

int Pid::findMaxNC(IplImage* img) {
    CvScalar s;
    s = cvGet2D(img, 0, 0);
    double max = s.val[0];
    for (int i = 0; i < img->height; i++) {
        for (int j = 1; j < img->width; j++) {
            s = cvGet2D(img, i, j);
            if (s.val[0] > max)
                max = s.val[0];
        }
    }
    return (int) max;
}

CvScalar Pid::toGray(CvScalar pixel) {
    double x = .299 * pixel.val[0] + .587 * pixel.val[1] + .114 * pixel.val[2];
    return cvScalar(x, x, x, x);
}

void Pid::toGray(IplImage *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            CvScalar s;
            s = cvGet2D(img, i, j);
            cvSet2D(img, i, j, toGray(s));
        }
    }
}

void Pid::subBackground(IplImage *img, IplImage *img2, bool hsv) {
    cvCvtColor(img, img, CV_BGR2HSV);
    cvCvtColor(img2, img2, CV_BGR2HSV);
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            CvScalar s = cvGet2D(img, i, j);
            CvScalar s2 = cvGet2D(img2, i, j);
            if (s.val[0] == s2.val[0] && s.val[1] == s2.val[1]
                    && s.val[2] == s2.val[2] && s.val[3] == s2.val[3]) {
                cvSet2D(img, i, j, cvScalar(0, 0, 0, 0));
            }
        }
    }
}

double Pid::auxFourier(int y, int x, int v, int u, int m, int n) {
    double pi = 3.1415926535897932384626433832795;
    double num1 = ((double) x * u) / (double) m;
    double num2 = ((double) y * v) / (double) n;
    return (num1 + num2)* -2 * pi;
}

double Pid::cosineFourier(double x) {
    double pi = 3.1415926535897932384626433832795;
    return cos(pi * x / 180);
}

double Pid::sinFourier(double x) {
    double pi = 3.1415926535897932384626433832795;
    return cos(pi * x / 180);
}

void Pid::NormalizarFourier(IplImage* img_real, IplImage* img_imaginaria) {
    for (int i = 0; i < img_real->height; i++) {
        for (int j = 0; j < img_real->width; j++) {
            CvScalar s;
            s = cvGet2D(img_real, i, j);
            s.val[0] = log10(abs(s.val[0]) + 1);
            cvSet2D(img_real, i, j, s);
            s = cvGet2D(img_imaginaria, i, j);
            s.val[0] = log10(abs(s.val[0]) + 1);
            cvSet2D(img_imaginaria, i, j, s);
        }
    }
    int maxReal = Pid::findMaxNC(img_real);
    //int maximaginario = findMaxNC(img_imaginaria);
    for (int i = 0; i < img_real->height; i++) {
        for (int j = 0; j < img_real->width; j++) {
            CvScalar s;
            s = cvGet2D(img_real, i, j);
            s.val[0] = (s.val[0] / maxReal) * 255;
            cvSet2D(img_real, i, j, s);
        }
    }

    for (int i = 0; i < img_real->height / 2; i++) {
        for (int j = 0; j < img_real->width; j++) {
            CvScalar s, s2;
            int z = img_real->height / 2 + i;
            s = cvGet2D(img_real, i, j);
            s2 = cvGet2D(img_real, img_real->height / 2 + i, j);
            cvSet2D(img_real, img_real->height / 2 + i, j, s2);
            cvSet2D(img_real, i, j, s);
        }
    }
    for (int i = 0; i < img_real->height; i++) {
        for (int j = 0; j < img_real->width / 2; j++) {

            CvScalar s, s2;
            int z = img_real->width / 2 + j;

            s = cvGet2D(img_real, i, j);
            s2 = cvGet2D(img_real, i, z);
            cvSet2D(img_real, i, z, s2);
            cvSet2D(img_real, i, j, s);
        }
    }
}

int Pid::fourier(IplImage * img) {
    int m = img->height;
    int n = img->width;
    double real;
    double imaginario;
    IplImage* img_real = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
    IplImage* img_imginaria = cvCreateImage(cvSize(img->width, img->height), img->depth, 1);
    for (int u = 0; u < m; u++) {
        for (int v = 0; v < n; v++) {
            real = 0;
            imaginario = 0;
            for (int x = 0; x < m; x++) {
                for (int y = 0; y < n; y++) {
                    double aux = auxFourier(y, x, v, u, m, n);
                    real += (cvGet2D(img, x, y)).val[0] * cosineFourier(aux);
                    imaginario += (cvGet2D(img, x, y)).val[0] * sinFourier(aux);
                }
            }
            cvSet2D(img_real, u, v, cvScalar(real, 0, 0, 0));
            cvSet2D(img_imginaria, u, v, cvScalar(imaginario, 0, 0, 0));
        }
    }
    Pid::NormalizarFourier(img_real, img_imginaria);
    cvSaveImage("fourier.jpg", img_real);
    return 0;
}

int Pid::Histograma(IplImage * img) {
    int NCMax = findMaxNC(img);
    int pixels [NCMax + 1];
    for (int i = 0; i < NCMax; i++) {
        pixels[i] = 0;
    }
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            CvScalar s;
            s = cvGet2D(img, i, j);
            pixels[(int) s.val[0]]++;
        }
    }

    for (int i = 1; i < NCMax; i++) {
        pixels[i] = pixels[i] + pixels[i - 1];
    }
    for (int i = 1; i < NCMax; i++) {
        double x = (double) pixels[i] / (img->width * img->height);
        pixels[i] = x * NCMax;
    }
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            if (j == 194 && i == 12) {
                int k = 0;
            }
            CvScalar s;
            s = cvGet2D(img, i, j); // get the (i,j) pixel value
            int pixel = pixels[(int) s.val[0]];
            s.val[0] = pixel;
            cvSet2D(img, i, j, s); // get the (i,j) pixel value
        }
    }
    return 0;
}

int ** Pid::CreatMask() {

    int mask [3][3];
    int ** maskNew;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            std::cout << "Digite o valor para a posição " << i << "-" << j << " da mascara";
            std::cin >> mask[i][j];
        }
    }

    maskNew = new int *[3];
    maskNew[0] = new int [3];
    maskNew[1] = new int [3];
    maskNew[2] = new int [3];


    maskNew[0][0] = mask[2][2];
    maskNew[0][1] = mask[2][1];
    maskNew[0][2] = mask[2][0];
    maskNew[1][0] = mask[1][2];
    maskNew[1][1] = mask[1][1];
    maskNew[1][2] = mask[1][0];
    maskNew[2][0] = mask[0][2];
    maskNew[2][1] = mask[0][1];
    maskNew[2][2] = mask[0][0];


    return maskNew;
}

int ** Pid::MaskGauss() {
    int tam, var;
    cout << "Entre com a dimensao da mascara" << endl;
    cin >> tam;
    cout << "Entre com a variancia da mascara" << endl;
    cin >> var;
    int maskGauss[tam][tam];
    for (int x = 0; x <= tam; x++) {
        for (int y = 0; y <= tam; y++) {
            cout << "Entre com o valor para [" << x << " ] [" << y << "]: " << endl;
            cin >> maskGauss[x][y];
        }
    }
    int ** newMaskGauss = new int *[tam];
    for (int x = 0; x <= tam; x++) {
        newMaskGauss[x] = new int [tam];
        for (int y = 0; y <= tam; y++) {
            double a1 = (double) (1 / 2 * 3.14 * pow(var, 2));
            double a2 = -(pow(x, 2));
            newMaskGauss[x][y] = (pow(a1, (a2) + pow(y, 2)) / 2 * pow((double) var, 2));
        }
    }
    return newMaskGauss;
}

void Pid::ChromaKey(char * video_name, char * background_name, bool hsv) {

    int i, j, k, height, width, step, channels;
    double red = 0, green = 0, blue = 0, red_2 = 0, green_2 = 0, blue_2 = 0;
    IplImage *img_video;
    IplImage *img_background;
    CvCapture *video = cvCaptureFromAVI(video_name);
    CvCapture *background = cvCaptureFromAVI(background_name);

    while (true) {
        img_video = cvQueryFrame(video);
        img_background = cvQueryFrame(background);
        if (hsv) {
            cvCvtColor(img_video, img_video, CV_BGR2HSV);
            cvCvtColor(img_background, img_background, CV_BGR2HSV);
        }
        if (!img_video) {
            break;
        }
        height = img_video->height;
        width = img_video->width;
        step = img_video->widthStep;
        channels = img_video->nChannels;

        for (i = 0; i < height; i++)
            for (j = 0; j < width; j++)
                for (k = 0; k < channels; k++) {
                    red = ((uchar *) (img_video->imageData + i * img_video->widthStep))[j * img_video->nChannels + 0];
                    green = ((uchar *) (img_video->imageData + i * img_video->widthStep))[j * img_video->nChannels + 1];
                    blue = ((uchar *) (img_video->imageData + i * img_video->widthStep))[j * img_video->nChannels + 2];

                    red_2 = ((uchar *) (img_background->imageData + i * img_background->widthStep))[j * img_background->nChannels + 0];
                    green_2 = ((uchar *) (img_background->imageData + i * img_background->widthStep))[j * img_background->nChannels + 1];
                    blue_2 = ((uchar *) (img_background->imageData + i * img_background->widthStep))[j * img_background->nChannels + 2];

                    if ((red <= 80) && (green >= 200) && (blue <= 80) && !hsv) {
                        // data[i*step+j*channels+k] = 0 ;
                        ((uchar *) (img_video->imageData + i * img_video->widthStep))[j * img_video->nChannels + 0] = red_2;
                        ((uchar *) (img_video->imageData + i * img_video->widthStep))[j * img_video->nChannels + 1] = green_2;
                        ((uchar *) (img_video->imageData + i * img_video->widthStep))[j * img_video->nChannels + 2] = blue_2;
                    }
                }
        cvShowImage("chromakey", img_video);
        cvWaitKey(10);
    }
    cvReleaseCapture(&video);
    cvDestroyWindow("chromakey");
}

void Pid::hough(char * img_name) {
    IplImage* image = cvLoadImage(img_name, CV_LOAD_IMAGE_GRAYSCALE);
    CvMemStorage* storage = cvCreateMemStorage(0);
    cvSmooth(image, image, CV_GAUSSIAN, 5, 5);
    CvSeq* results = cvHoughCircles(image, storage, CV_HOUGH_GRADIENT, 2, image->width / 10);
    for (int i = 0; i < results->total; i++) {
        float* p = (float*) cvGetSeqElem(results, i);
        CvPoint pt = cvPoint(cvRound(p[0]), cvRound(p[1]));
        cvCircle(image, pt, cvRound(p[2]), CV_RGB(0xff, 0xff, 0xff));
    }
    cvNamedWindow("cvHoughCircles", 1);
    cvShowImage("cvHoughCircles", image);
    cvWaitKey(0);
}

void Pid::canny(char * img_name) {
    //Converter a imagem colorida para tons de cinza
    IplImage *img = cvLoadImage(img_name, 0);
    cvCanny(img, img, 50, 200, 3);
    showImage(img);
}

CvScalar Pid::getMedia(IplImage *img) {
    CvScalar media;
    CvScalar s;
    media = cvScalar(0, 0, 0, 0);
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            s = cvGet2D(img, i, j);
            media = cvScalar(
                    media.val[0] + s.val[0],
                    media.val[1] + s.val[1],
                    media.val[2] + s.val[2],
                    0);
        }
    }
    media = cvScalar(
            media.val[0] / (img->width * img->height),
            media.val[1] / (img->width * img->height),
            media.val[2] / (img->width * img->height),
            0);
    return media;
}

IplImage * Pid::CriarAmostra(char * img_name, int i1, int i2, int j1, int j2, int width, int height) {
    IplImage *img = cvLoadImage(img_name);
    IplImage* classe1 = cvCreateImage(cvSize(width, height), img->depth, 3);
    CvScalar s;
    for (int i = i1, i0 = 0; i < i2 - 1; i++, i0++) {
        for (int j = j1, j0 = 0; j < j2; j++, j0++) {
            s = cvGet2D(img, i, j);
            cvSet2D(classe1, j0, i0, s);
        }
    }
    return classe1;
}

void Pid::bayesiana(char * img_name) {
    IplImage *img = cvLoadImage(img_name, 1);
    IplImage* classe1 = CriarAmostra("original.jpg", 0, 80, 0, 160, 80, 160);
    IplImage* classe2 = CriarAmostra("original.jpg", 380, 443, 640, 741, 63, 101);
    cv::Mat cj1, cj2; // cv::Mat(img, true), ;
    CvScalar s1, s2, m1, m2;
    m1 = getMedia(classe1);
    m2 = getMedia(classe2);
    cj1 = covariance(img, m1);
    cj2 = covariance(img, m2);
    double det1 = cv::determinant(cj1);
    double det2 = cv::determinant(cj2);
    for (int i = 0; i < img-> height; i++) {
        for (int j = 0; j < img->width; j++) {
            cv::Mat ak1, ak2;
            s1 = cvScalar(cvGet2D(img, i, j).val[0] - m1.val[0],
                    cvGet2D(img, i, j).val[1] - m1.val[1],
                    cvGet2D(img, i, j).val[2] - m1.val[2]
                    );
            s2 = cvScalar(cvGet2D(img, i, j).val[0] - m2.val[0],
                    cvGet2D(img, i, j).val[1] - m2.val[1],
                    cvGet2D(img, i, j).val[2] - m2.val[2]
                    );
            ak1 = cv::Mat(3, 1, cv::DataType<double>::type);
            ak2 = cv::Mat(3, 1, cv::DataType<double>::type);
            ak1.at<double>(0, 0) = s1.val[0];
            ak1.at<double>(0, 1) = s1.val[1];
            ak1.at<double>(0, 2) = s1.val[2];
            ak2.at<double>(0, 0) = s2.val[0];
            ak2.at<double>(0, 1) = s2.val[1];
            ak2.at<double>(0, 2) = s2.val[2];
            ak1 = ak1.t() * cj1.inv() * ak1;
            ak2 = ak2.t() * cj2.inv() * ak2;
            double tempOP1 = (det1 > 0) ? 0.5 * log(det1) : 0;
            double tempOP2 = (det2 > 0) ? 0.5 * log(det2) : 0;
            double resultado1 = 0.5 - tempOP1 - 0.5 * (ak1.at<double>(0, 0));
            double resultado2 = 0.5 - tempOP2 - 0.5 * (ak2.at<double>(0, 0));
            if (resultado1 > resultado2)
                cvSet2D(img, i, j, cvScalar(255, 0, 0, 0));
            if (resultado2 > resultado1)
                cvSet2D(img, i, j, cvScalar(0, 0, 255, 0));
            else
                cvSet2D(img, i, j, cvScalar(0, 255, 0, 0));
        }
    }
    showImage(img);
}

cv::Mat Pid::covariance(IplImage* classe, CvScalar media) {
    cv::Mat soma = cv::Mat::zeros(3, 3, cv::DataType<double>::type);
    cv::Mat MatrizMedia = cv::Mat(3, 1, cv::DataType<double>::type, media);
    MatrizMedia = MatrizMedia * MatrizMedia.t();
    for (int i = 0; i < classe->height; i++) {
        for (int j = 0; j < classe->width; j++) {
            CvScalar s = cvGet2D(classe, i, j);
            cv::Mat x = cv::Mat(3, 1, cv::DataType<double>::type);
            x.at<double>(0, 0) = s.val[0];
            x.at<double>(0, 1) = s.val[1];
            x.at<double>(0, 2) = s.val[2];
            x = (x * x.t());
            soma = soma + (x - MatrizMedia);
        }
    }
    return soma;
}

void Pid::MindDist(char * img_name) {
    IplImage *img = cvLoadImage(img_name, 1);
    IplImage* classe1 = CriarAmostra("original.jpg", 0, 80, 0, 160, 80, 160);
    IplImage* classe2 = CriarAmostra("original.jpg", 380, 443, 640, 741, 63, 101);
    CvScalar s, m1, m2;
    //conseguir a classes
    m1 = getMedia(classe1);
    m2 = getMedia(classe2);
    for (int i = 0; i < img-> height; i++) {
        for (int j = 0; j < img->width; j++) {
            double dx1, dx2;
            s = cvGet2D(img, i, j);
            dx1 = ((s.val[0] * m1.val[0]) + (s.val[1] * m1.val[1]) + (s.val[2] * m1.val[2]))
                    - ((m1.val[0] * m1.val[0] + m1.val[1] * m1.val[1] + m1.val[2] * m1.val[2]) / 2);
            dx2 = ((s.val[0] * m2.val[0]) + (s.val[1] * m2.val[1]) + (s.val[2] * m2.val[2]))
                    - ((m2.val[0] * m2.val[0] + m2.val[1] * m2.val[1] + m2.val[2] * m2.val[2]) / 2);

            //pertence a classe1
            if (dx1 < dx2 && dx1 > 0) {
                cvSet2D(img, i, j, cvScalar(0, 255, 0, 0));
            }
            //pertence a classe2
            if (dx2 < dx1 && dx2 > 0) {
                cvSet2D(img, i, j, cvScalar(0, 0, 255, 0));
            }
            //não pertence a nenhuma classe
            if (dx2 <= 0 && dx1 <= 0) {
                cvSet2D(img, i, j, cvScalar(255, 0, 0, 0));
            }
        }
    }
    showImage(img);
}