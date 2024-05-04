#define imagesv2_h

struct imagen{
    int x_pos;
    int y_pos;
    int x_lon;
    int y_lon;
    int pixels[100];
};

imagen imgtest = {5,5,10,10,};