#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "gen.h"

#include "waveforms.h"

double genHDOP(int64_t time)
{
    return square(time, 0, 3600*1, 50, 0.3, 0.5);
}
double genVDOP(int64_t time)
{
    return triangle(time, 0, 3600*1, 0.3, 0.5);
}

double genPDOP(int64_t time)
{
    return sine(time, 0, 3600*1, 0.3, 0.5);
}

int genNumberOfSatellitesUsed(int64_t time)
{
    int PRN[12];
    return genUsedGPSSatelliteList(time, PRN) + genUsedGLONASSSatelliteList(time, PRN);
}

int inView_magic(int64_t time)
{
    return (int)(square(time, 0, 60*6, 50, 4, 20) + 0.5);
}

int used_magic(int64_t time)
{
    return (int)(square(time, 180, 60 * 11, 20, 3, 10) + 0.5);
}

int genGPSSatellitesInView(int64_t time, Satellite* satellites)
{
    const int numOfSat = (inView_magic(time) / 2) + (inView_magic(time) % 2);
    const int numOfUsed = (used_magic(time) / 2) + (used_magic(time) % 2);
    int i;
    for(i = 0; i < numOfSat; i++){
        satellites[i].PRN = 10 + i;
        satellites[i].elevation = 50 + i;
        satellites[i].azimuth = 100 + 10 * i + i;
        satellites[i].used = (i < numOfUsed);
        satellites[i].SNR = 50;
    }
    return numOfSat;    
}

int genGLONASSSatellitesInView(int64_t time, Satellite* satellites)
{
    const int numOfSat = (inView_magic(time) / 2);
    const int numOfUsed = (used_magic(time) / 2);
    int i;
    for(i = 0; i < numOfSat; i++){
        satellites[i].PRN = 70 + i;
        satellites[i].elevation = 30 + i;
        satellites[i].azimuth = 200 + 10 * i + i;
        satellites[i].used = (i < numOfUsed);
        satellites[i].SNR = 70;
    }     
    return numOfSat;    
}

int genUsedGPSSatelliteList(int64_t time, int* PRN)
{
    Satellite satellites[36];
    int usedCount = 0, viewCount = genGPSSatellitesInView(time, satellites);
    int i;
    for(i = 0; i < viewCount; i++)
        if(true == satellites[i].used)
            PRN[usedCount++] = satellites[i].PRN;
    return usedCount;
}

int genUsedGLONASSSatelliteList(int64_t time, int* PRN)
{
    Satellite satellites[36];
    int usedCount = 0, viewCount = genGLONASSSatellitesInView(time, satellites);
    int i;
    for(i = 0; i < viewCount; i++)
        if(true == satellites[i].used)
            PRN[usedCount++] = satellites[i].PRN;
    return usedCount;
}

double genAltitude(int64_t time)
{
    return sawtooth(time, 0, 60 * 3, 100, 1500);
}

double genUndulation(int64_t time)
{
    return -16.57;
}

double genTrack(int64_t time)
{
    return 147.369;
}

double genSpeed(int64_t time)
{
    return 0;
}

Latitude genLatitude(int64_t time)
{
    double degree = sine(time, 0, 60 * 5, 5, 40);
    Latitude ret;
    ret.degree = (int)degree;
    double a;
    ret.minute = 60 * modf(degree, &a);
    ret.direction = North;
    return ret;
}

Longitude genLongitude(int64_t time)
{
    double degree = sine(time, 0, 60 * 5 * 2.5, 5, 55);    
    Longitude ret;
    ret.degree = (int)degree;
    double a;
    ret.minute = 60 * modf(degree, &a);
    ret.direction = East;
    return ret;
}
