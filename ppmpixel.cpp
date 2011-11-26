#include "ppmpixel.h"
#include <fstream>
#include <stdlib.h>

PPMpixel::PPMpixel()
{

}

PPMpixel::PPMpixel(int r,int g, int b){
    this->r = r;
    this->g = g;
    this->b = b;
}
