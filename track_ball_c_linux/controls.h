struct buttonCtrl
{
   char *text;
   int textSz;
   float size;
   float x;
   float y;
   int down;
};

struct sliderCtrl
{
   float x;
   float y;
   float w;
   float h;
   float min;
   float max;
   float value;
   int drag;
};

struct labelCtrl
{
   char *text;
   int textSz;
   float size;
   float x;
   float y;
   int border;
};

struct rendVars
{
   float viewStart;
   float viewEnd;
   float perspective;
   int orthographic;
   int fog;
   float fogStart;
   float fogEnd;
   int antialiasing;
};

void createButton(struct buttonCtrl *, char *, float, float, float);
int clickButton(struct buttonCtrl *, int, float, float);
int onButton(struct buttonCtrl *, float, float);
void drawButton(struct buttonCtrl *, struct rendVars);

void createSlider(struct sliderCtrl *, float, float,
                  float, float, float, float, float);
int moveSlider(struct sliderCtrl *, int, float, float);
float getSliderValue(struct sliderCtrl *sldr);
int onSlider(struct sliderCtrl *, float, float);
void drawSlider(struct sliderCtrl *, struct rendVars);

void createLabel(struct labelCtrl *, char *, float, float, float, int);
int onLabel(struct labelCtrl *, float, float);
void drawLabel(struct labelCtrl *, struct rendVars);
