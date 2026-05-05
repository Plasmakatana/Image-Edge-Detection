#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define KERNEL_SIZE 5

// ---------------- Gaussian kernel ----------------
float gaussian[KERNEL_SIZE][KERNEL_SIZE];

// generate gaussian kernel dynamically
void createGaussianKernel(float sigma) {
    float sum = 0.0;

    int half = KERNEL_SIZE / 2;

    for (int y = -half; y <= half; y++) {
        for (int x = -half; x <= half; x++) {

            float value = exp(-(x*x + y*y) / (2 * sigma * sigma));
            gaussian[y + half][x + half] = value;
            sum += value;
        }
    }

    // normalize
    for (int y = 0; y < KERNEL_SIZE; y++)
        for (int x = 0; x < KERNEL_SIZE; x++)
            gaussian[y][x] /= sum;
}

// ---------------- main ----------------
int main() {
    int width, height, channels;

    unsigned char *img = stbi_load("input.png", &width, &height, &channels, 1);
    if (!img) {
        printf("Failed to load image\n");
        return 1;
    }

    createGaussianKernel(1.4); // sigma (controls blur strength)

    float *blur = malloc(width * height * sizeof(float));
    int *logImg = malloc(width * height * sizeof(int));
    unsigned char *output = malloc(width * height * 3);

    int half = KERNEL_SIZE / 2;

    // ---------------- STEP 1: Gaussian blur ----------------
    for (int y = half; y < height - half; y++) {
        for (int x = half; x < width - half; x++) {

            float sum = 0;

            for (int ky = -half; ky <= half; ky++) {
                for (int kx = -half; kx <= half; kx++) {

                    int pixel = img[(y + ky) * width + (x + kx)];
                    sum += pixel * gaussian[ky + half][kx + half];
                }
            }

            blur[y * width + x] = sum;
        }
    }

    // ---------------- STEP 2: Laplacian ----------------
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {

            int center = blur[y * width + x];

            int lap =
                blur[(y-1)*width + x] +
                blur[(y+1)*width + x] +
                blur[y*width + (x-1)] +
                blur[y*width + (x+1)] -
                4 * center;

            logImg[y * width + x] = lap;
        }
    }

    // ---------------- STEP 3: zero crossing ----------------
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {

            int hasPos = 0, hasNeg = 0;

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {

                    int val = logImg[(y + ky) * width + (x + kx)];
                    if (val > 0) hasPos = 1;
                    if (val < 0) hasNeg = 1;
                }
            }

            int idx = (y * width + x) * 3;

            if (hasPos && hasNeg) {
                output[idx] = 255;
                output[idx+1] = 255;
                output[idx+2] = 255;
            } else {
                output[idx] = 0;
                output[idx+1] = 0;
                output[idx+2] = 0;
            }
        }
    }

    // ---------------- STEP 4: write PNG ----------------
    stbi_write_png("output.png", width, height, 3, output, width * 3);

    printf("Saved output.png (Gaussian + LoG edges)\n");

    // cleanup
    stbi_image_free(img);
    free(blur);
    free(logImg);
    free(output);

    return 0;
}