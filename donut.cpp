#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
const int screen_width = 120;
const int screen_height = 25;
const int DIMENSION = screen_width * screen_height;
const float PI = 3.1415926;
const float R1 = 2;                                        // Distance from the centre of a torus to the centre of a circle
const float R2 = 1;                                        // Radius of a circular slice of a torus
const float K1 = 10;                                       // Distance from the viewer to the donut
const float K2 = K1 * screen_height * 3 / (8 * (R1 + R2)); // Distance from the viewer to the projection

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

    float theta = 0, phix = 0, phiy = 0, phiz = 0;

    // z-buffering
    float z_buffer[DIMENSION];

    // rendered image
    char o[DIMENSION];

    printf("\x1b[30A");
    // animation loop
    while (true)
    {
        memset(z_buffer, 0, DIMENSION * sizeof(float));
        memset(o, 32, DIMENSION * sizeof(char));
        // rotational axis
        float sin_phix = sin(phix), cos_phix = cos(phix),
              sin_phiz = sin(phiz), cos_phiz = cos(phiz);

        // pixelize a torus
        for (theta = 0; theta < 2 * PI; theta += 0.07)
        { // circular slice of a torus
            float sin_theta = sin(theta), cos_theta = cos(theta);
            for (phiy = 0; phiy < 2 * PI; phiy += 0.03)
            { // one revolution of the slice about the y-axis
                float sin_phiy = sin(phiy), cos_phiy = cos(phiy);
                float f1 = R1 + R2 * cos(theta),
                      f2 = R2 * sin(theta);
                float x = f1 * (cos_phiz * cos_phiy + sin_phix * sin_phiz * sin_phiy) - f2 * sin_phiz * cos_phix,
                      y = f1 * (sin_phiz * cos_phiy - sin_phix * cos_phiz * sin_phiy) + f2 * cos_phiz * cos_phix,
                      z = K1 + (f1 * cos_phix * sin_phiy + f2 * sin_phix);
                float zi = 1 / z;

                // projection
                int xp = (int)(screen_width / 2 + x * K2 * zi),
                    yp = (int)(screen_height / 2 - y * K2 * zi);

                // normal
                float nx = cos_theta * (sin_phix * sin_phiz * sin_phiy + cos_phiz * cos_phiy) - sin_theta * cos_phix * sin_phiz,
                      ny = cos_theta * (sin_phiz * cos_phiy - sin_phix * cos_phiz * sin_phiy) + sin_theta * cos_phix * cos_phiz,
                      nz = cos_theta * cos_phix * sin_phiy + sin_theta * sin_phix;

                // light source: default = (0, 1, -1) : Behind and above the viewer
                float lx = 0, ly = 1, lz = -1, lmag = sqrt(pow(lx, 2) + pow(ly, 2) + pow(lz, 2));

                // luminance
                float l = nx * lx + ny * ly + nz * lz;

                // only when the surface faces to the viewer
                if (zi > z_buffer[yp * screen_width + xp] && 0 < xp && xp < screen_width && 0 < yp && yp < screen_height)
                {
                    z_buffer[yp * screen_width + xp] = zi;
                    o[yp * screen_width + xp] = (".,-~:;!=*#$@")[l > 0 ? (int)(l / lmag * 11) : 0];
                }
            }
        }
        printf("\x1b[H");
        for (int k = 0; k < DIMENSION; k++)
        {
            putchar(k % screen_width ? o[k] : 10);
        }
        putchar('\n');

        phix += 0.07;
        phiz += 0.05;
    }

    return 0;
}