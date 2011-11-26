/*
 * File:   Main.cpp
 * Author: guilherme
 *
 * Created on 7 de Setembro de 2011, 18:02
 */

#include "pid.h"
#include "ppm.h"

int main(int argc, char** argv) {
    int opcao = 0;
    Pid pid;
    while (opcao >= 0) {
        std::cout << "\n1-  Carregar uma imagem ppm para memoria";
        std::cout << "\n2-  Exibir uma imagem na tela";
        std::cout << "\n3-  Converter uma imagem para tons de cinza";
        std::cout << "\n4-  Executar a transformada de Fourier";
        std::cout << "\n5-  Equalizar um histograma de uma imagem";
        std::cout << "\n6-  Criar uma mascara de convolução discreta";
        std::cout << "\n7-  Criar uma mascara de convolução Gaussiana";
        std::cout << "\n8-  ChromaKey";
        std::cout << "\n9-  Remocao de fundo";
        std::cout << "\n10- Canny para deteccão de bordas finas";
        std::cout << "\n11-  Transformada de hough para a deteccao de circulos";
        std::cout << "\n12-  Classicar por distancia minima";
        std::cout << "\n13-  Classicar por Bayesiana";
        std::cout << "\nO que deseja fazer: ";
        cin >> opcao;
        char * img_name = new char [30];
        switch (opcao) {

            case 0:
                opcao = -1;
                break;
            case 1:
            {
                std::cout << "\nAtencao. O arquivo output.ppm sera criado com a imagem ppm";
                std::cout << "\nDigite o nome do arquivo que deseja utilizar a imagem: ";
                cin >> img_name;
                PPM ppm(img_name);
                ppm.saveImage("output.ppm");
            }
                break;

            case 2:
            {
                std::cout << "\nDigite o nome do arquivo que deseja utilizar a imagem: ";
                cin >> img_name;
                IplImage *img = cvLoadImage(img_name);
                pid.showImage(img);
            }
                break;
            case 3:
            {
                std::cout << "\nDigite o nome do arquivo que deseja utilizar a imagem: ";
                cin >> img_name;
                IplImage *img = cvLoadImage(img_name);
                pid.toGray(img);
                pid.showImage(img);
            }
                break;

            case 4:
            {
                std::cout << "\nAtencao. O arquivo fourier.jpg sera criado com o resultado da imagem";
                std::cout << "\nDigite o nome do arquivo que deseja utilizar a imagem: ";
                cin >> img_name;
                IplImage *img = cvLoadImage(img_name, 0);
                pid.fourier(img);
                pid.showImage(img);
            }
                break;

            case 5:
            {
                std::cout << "\nDigite o nome do arquivo que deseja utilizar a imagem: ";
                cin >> img_name;
                IplImage *img = cvLoadImage(img_name, 0);
                pid.Histograma(img);
                pid.showImage(img);
            }
                break;
            case 6:
                pid.CreatMask();
                break;
            case 7:
                pid.MaskGauss();
                break;
            case 8:
            {
                char isHsv [3];
                std::cout << "Deseja executar a imagem como [rgb] ou [hsv]";
                cin >> isHsv;
                if (strcmp(isHsv, "hsv") == 0) {
                    pid.ChromaKey("video2.mp4", "video.mp4", true);

                } else
                    pid.ChromaKey("video2.mp4", "video.mp4", false);
            }
                break;
            case 9:
            {
                IplImage *img = cvLoadImage("eisten.png");
                IplImage *img2 = cvLoadImage("eisten2.png");
                char * isHsv = new char [3];
                std::cout << "Deseja executar a imagem como [rgb] ou [hsv]";
                cin >> isHsv;
                if (strcmp(isHsv, "hsv") == 0)
                    pid.subBackground(img2, img, true);
                else
                    pid.subBackground(img2, img);
                pid.showImage(img2);
            }
                break;
            case 10:
            {
                pid.canny("imagem.jpg");
            }
                break;
            case 11:
            {
                pid.hough("forma.jpg");
            }
                break;
            case 12:
            {
                char * img_name = new char [40];
                std::cout << "Digite o nome da imagem: ";
                cin >> img_name;
                pid.MindDist(img_name);
            }
                break;
            case 13:
            {
                char * img_name = new char [40];
                std::cout << "Digite o nome da imagem: ";
                cin >> img_name;
                pid.bayesiana(img_name);
            }
                break;
        }
    }
    return 0;
}