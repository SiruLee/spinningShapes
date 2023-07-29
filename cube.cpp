#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
const int screen_width = 60;
const int screen_height = 26;
const int DIMENSION = screen_width * screen_height;
const float STEP = 0.07;
const float PI = 3.1415926535;
const float length = 2;
const float K1 = 10;                                              // Distance from the viewer to the donut
const float K2 = K1 * screen_height * 3 / (8 * length * sqrt(3)); // Distance from the viewer to the projection
float phix = 0, phiy = 0, phiz = 0;

void render(float x, float y, float z, float nx, float ny, float nz, float *z_buffer, char *o)
{
    // rotational axis
    float sin_phix = sin(phix), cos_phix = cos(phix),
          sin_phiy = sin(phiy), cos_phiy = cos(phiy),
          sin_phiz = sin(phiz), cos_phiz = cos(phiz);

    float f1 = x * cos_phiy - z * sin_phiy,
          f2 = y * cos_phix - x * sin_phix * sin_phiy - z * sin_phix * cos_phiy;

    float xc = f1 * cos_phiz - sin_phiz * f2,
          yc = f1 * sin_phiz + cos_phiz * f2,
          zc = K1 + y * sin_phix + cos_phix * (x * sin_phiy + z * cos_phiy);

    // normal
    float nnx = cos_phiz * (cos_phiy * nx - sin_phiy * nz) - sin_phiz * (cos_phix * ny - sin_phix * (sin_phiy * nx + cos_phiy * nz)),
          nny = sin_phiz * (cos_phiy * nx - sin_phiy * nz) + cos_phiz * (cos_phix * ny - sin_phix * (sin_phiy * nx + cos_phiy * nz)),
          nnz = sin_phix * ny + cos_phix * (sin_phiy * nx + cos_phiy * nz);
    // light source: default = (0, 1, -1) : Behind and above the viewer
    float lx = 0, ly = 1, lz = -1, lmag = sqrt(pow(lx, 2) + pow(ly, 2) + pow(lz, 2));

    // luminance
    float l = nnx * lx + nny * ly + nnz * lz;

    float zi = 1 / zc;
    // projection
    int xp = (int)(screen_width / 2 + xc * K2 * zi),
        yp = (int)(screen_height / 2 - yc * K2 * zi);

    // only when the surface faces to the viewer
    if (zi > z_buffer[yp * screen_width + xp] && 0 < xp && xp < screen_width && 0 < yp && yp < screen_height)
    {
        z_buffer[yp * screen_width + xp] = zi;
        o[yp * screen_width + xp] = (".,-~:;!=*#$@")[l > 0 ? (int)(l / lmag * 11) : 0];
    }
}

/* A spinning donut animation on 30x30 screen
 * Set to the default size of Windows Command Prompt: 30x120
 */
int main()
{
    HANDLE hStdOut = NULL;
    CONSOLE_CURSOR_INFO curInfo;

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(hStdOut, &curInfo);
    curInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hStdOut, &curInfo);

    // z-buffering
    float z_buffer[DIMENSION];

    // rendered image
    char o[DIMENSION];

    // animation loop
    while (true)
    {
        memset(z_buffer, 0, DIMENSION * sizeof(float));
        memset(o, 32, DIMENSION * sizeof(char));

        // pixelize a cube
        for (float a = -length; a < length; a += STEP)
        {
            for (float b = -length; b < length; b += STEP)
            {

                float x = a, y = b, z = length;

                render(x, y, z, 0, 0, 1, z_buffer, o);
                render(-x, y, -z, 0, 0, -1, z_buffer, o);
                render(z, y, -x, 1, 0, 0, z_buffer, o);
                render(-z, y, x, -1, 0, 0, z_buffer, o);
                render(x, z, -y, 0, 1, 0, z_buffer, o);
                render(x, -z, y, 0, -1, 0, z_buffer, o);
            }
        }
        printf("\x1b[H");
        for (int k = 0; k < DIMENSION; k++)
        {
            putchar(k % screen_width ? o[k] : 10);
        }
        putchar('\n');

        phix += 0.07;
        phiy += 0.04;
        phiz += 0.05;
    }

    return 0;
}
