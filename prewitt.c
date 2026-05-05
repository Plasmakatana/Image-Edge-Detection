#include "common.h"

int px[3][3] = {
    {-1, 0, 1},
    {-1, 0, 1},
    {-1, 0, 1}
};

int py[3][3] = {
    {-1, -1, -1},
    {0,   0,  0},
    {1,   1,  1}
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input_image output_image\n", argv[0]);
        return 1;
    }
    char *inputFile = argv[1];
    char *outputFile = argv[2];
    int w, h, c;
    unsigned char *img = stbi_load(inputFile, &w, &h, &c, 1);

    unsigned char *out = malloc(w * h * 3);

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {

            int sx = 0, sy = 0;

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int p = img[(y+i)*w + (x+j)];
                    sx += p * px[i+1][j+1];
                    sy += p * py[i+1][j+1];
                }
            }

            int mag = sqrt(sx*sx + sy*sy);
            if (mag > 255) mag = 255;

            int idx = (y*w + x)*3;
            out[idx] = out[idx+1] = out[idx+2] = mag;
        }
    }

    stbi_write_png(outputFile, w, h, 3, out, w*3);
    printf("Saved %s\n",outputFile);
}