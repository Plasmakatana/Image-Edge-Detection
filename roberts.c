#include "common.h"

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

    for (int y = 0; y < h - 1; y++) {
        for (int x = 0; x < w - 1; x++) {

            int p1 = img[y*w + x];
            int p2 = img[y*w + (x+1)];
            int p3 = img[(y+1)*w + x];
            int p4 = img[(y+1)*w + (x+1)];

            int gx = p1 - p4;
            int gy = p2 - p3;

            int mag = sqrt(gx*gx + gy*gy);
            if (mag > 255) mag = 255;

            int idx = (y*w + x)*3;
            out[idx] = out[idx+1] = out[idx+2] = mag;
        }
    }

    stbi_write_png(outputFile, w, h, 3, out, w*3);
    printf("Saved %s\n",outputFile);
}