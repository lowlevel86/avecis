#ifndef _line_HEADER_H_
#define _line_HEADER_H_

struct rendrInfo
{
   int xWin;
   int yWin;
   float xCenter;
   float yCenter;
   float camLenZ;
   float camEndZ;
   float fogBgnZ;
   float fogEndZ;
   float perspctv;
   int ortho;
   float **zIndex;
   int **scrnBuff;
};
typedef struct rendrInfo RENDRINFO;


// a = antialiasing, f = fog

void aPix(float, float, float, int, struct rendrInfo);

void line(float, float, float,
          float, float, float, int, struct rendrInfo);
void fLine(float, float, float,
           float, float, float, int, int, struct rendrInfo);

void aLine(float, float, float,
           float, float, float, int, struct rendrInfo);
void afLine(float, float, float,
            float, float, float, int, int, struct rendrInfo);

#endif // _line_HEADER_H_
