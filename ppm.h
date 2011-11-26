#ifndef PPM_H
#define PPM_H

#include "ppmpixel.h"
#include <string>
#include <stdlib.h>
#include <string.h>

class PPM
{
    public:
        PPM(char * file_name);
        void saveImage (char * file_name);
        PPM();
    protected:
    private:
    int readNumber (char * line);
    PPMpixel ** imgMatrix;
    int rows;
    int columns;
    int MaxColors;
    bool readFile(char * file_name);

};

#endif 
