#include "ppm.h"

PPM::PPM() {

}

PPM::PPM(char * file_name) {
    readFile(file_name);
}

void PPM::saveImage (char * file_name){
    std::ofstream file (file_name);
    file<<"P3\n";//magic number
    file<<this->rows<<" "<<this->columns<<"\n";//numero de linhas e colunas
    file<<this->MaxColors<<"\n";
    for (int x = 0; x < this->columns; x++) {
            for (int y = 0; y < this->rows; y++) {
                file<<this->imgMatrix[x][y].r<<" ";
                file<<this->imgMatrix[x][y].g<<" ";
                file<<this->imgMatrix[x][y].b<<" ";
            }
            file<<"\n";
        }
    file.close();
}

bool PPM::readFile(char * file_name) {
    std::string line;
    std::ifstream file(file_name);
    if (file.is_open()) {
        int i = 0;

        /* i é um marcado que diz quando as 3 linhas
           iniciais (p3, tamanho da imagem, e nº de pixel)
           são lidas, ele existe para ignorar comentários
         */

        while (i == 0 && file.good()) {
            getline(file, line);
            //verificar se a linha é um comentário e o magic number "P3"
            if (line.compare("P3") != 0 && line[0] != '#')
                return false; //Imagem mal formatada
            else if (line.compare("P3") == 0)
                i++;
        }
        while (i == 1 && file.good()) {
            getline(file, line);
            char * temp;
            temp = new char[line.length() + 1];
            strcpy(temp, line.c_str());
            char * pch;
            if (line[0] != '#') {
                pch = strtok(temp, " \n");
                this->rows = atoi(pch);

                pch = strtok(NULL, " \n");
                this->columns = atoi(pch);
                i++;

                this->imgMatrix = new PPMpixel *[rows];
            }

        }
        while (i == 2 && file.good()) {
            getline(file, line);
            char * temp;
            temp = new char[line.length() + 1];
            strcpy(temp, line.c_str());
            char * pch;
            if (line[0] != '#') {
                pch = strtok(temp, " \n");
                this->MaxColors = atoi(pch);
                i++;
            }
        }
        for (int x = 0; x < this->columns; x++) {
            
            char * temp;
            char * pch;
            getline(file, line);
            temp = new char[line.length() + 1];
            strcpy(temp, line.c_str());
            pch = strtok(temp, " \n");
            this->imgMatrix[x] = new PPMpixel [columns];
            
            for (int y = 0; y < this->rows; y++) {
                this->imgMatrix[x][y].r = atoi(pch);
                pch = strtok(NULL, " \n");
                this->imgMatrix[x][y].g = atoi(pch);
                pch = strtok(NULL, " \n");
                this->imgMatrix[x][y].b = atoi(pch);
                pch = strtok(NULL, " \n");
            }
        }      
    }
    file.close();
    return true;
}

