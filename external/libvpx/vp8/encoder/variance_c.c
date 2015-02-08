


#include "variance.h"

const int vp8_six_tap[8][6] =
{
    { 0,  0,  128,    0,   0,  0 },         // note that 1/8 pel positions are just as per alpha -0.5 bicubic
    { 0, -6,  123,   12,  -1,  0 },
    { 2, -11, 108,   36,  -8,  1 },         // New 1/4 pel 6 tap filter
    { 0, -9,   93,   50,  -6,  0 },
    { 3, -16,  77,   77, -16,  3 },         // New 1/2 pel 6 tap filter
    { 0, -6,   50,   93,  -9,  0 },
    { 1, -8,   36,  108, -11,  2 },         // New 1/4 pel 6 tap filter
    { 0, -1,   12,  123,  -6,  0 }
};


const int VP8_FILTER_WEIGHT = 128;
const int VP8_FILTER_SHIFT  =   7;
const int vp8_bilinear_taps[8][2] =
{
    { 128,   0 },
    { 112,  16 },
    {  96,  32 },
    {  80,  48 },
    {  64,  64 },
    {  48,  80 },
    {  32,  96 },
    {  16, 112 }
};

unsigned int vp8_get_mb_ss_c
(
    const short *src_ptr
)
{
    unsigned int i = 0, sum = 0;

    do
    {
        sum += (src_ptr[i] * src_ptr[i]);
        i++;
    }
    while (i < 256);

    return sum;
}


void  vp8_variance(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    int  w,
    int  h,
    unsigned int *sse,
    int *sum)
{
    int i, j;
    int diff;

    *sum = 0;
    *sse = 0;

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            diff = src_ptr[j] - ref_ptr[j];
            *sum += diff;
            *sse += diff * diff;
        }

        src_ptr += source_stride;
        ref_ptr += recon_stride;
    }
}

unsigned int
vp8_get8x8var_c
(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    unsigned int *SSE,
    int *Sum
)
{

    vp8_variance(src_ptr, source_stride, ref_ptr, recon_stride, 8, 8, SSE, Sum);
    return (*SSE - (((*Sum) * (*Sum)) >> 6));
}

unsigned int
vp8_get16x16var_c
(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    unsigned int *SSE,
    int *Sum
)
{

    vp8_variance(src_ptr, source_stride, ref_ptr, recon_stride, 16, 16, SSE, Sum);
    return (*SSE - (((*Sum) * (*Sum)) >> 8));

}



unsigned int vp8_variance16x16_c(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    unsigned int *sse)
{
    unsigned int var;
    int avg;


    vp8_variance(src_ptr, source_stride, ref_ptr, recon_stride, 16, 16, &var, &avg);
    *sse = var;
    return (var - ((avg * avg) >> 8));
}

unsigned int vp8_variance8x16_c(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    unsigned int *sse)
{
    unsigned int var;
    int avg;


    vp8_variance(src_ptr, source_stride, ref_ptr, recon_stride, 8, 16, &var, &avg);
    *sse = var;
    return (var - ((avg * avg) >> 7));
}

unsigned int vp8_variance16x8_c(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    unsigned int *sse)
{
    unsigned int var;
    int avg;


    vp8_variance(src_ptr, source_stride, ref_ptr, recon_stride, 16, 8, &var, &avg);
    *sse = var;
    return (var - ((avg * avg) >> 7));
}


unsigned int vp8_variance8x8_c(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    unsigned int *sse)
{
    unsigned int var;
    int avg;


    vp8_variance(src_ptr, source_stride, ref_ptr, recon_stride, 8, 8, &var, &avg);
    *sse = var;
    return (var - ((avg * avg) >> 6));
}

unsigned int vp8_variance4x4_c(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    unsigned int *sse)
{
    unsigned int var;
    int avg;


    vp8_variance(src_ptr, source_stride, ref_ptr, recon_stride, 4, 4, &var, &avg);
    *sse = var;
    return (var - ((avg * avg) >> 4));
}


unsigned int vp8_mse16x16_c(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    unsigned int *sse)
{
    unsigned int var;
    int avg;

    vp8_variance(src_ptr, source_stride, ref_ptr, recon_stride, 16, 16, &var, &avg);
    *sse = var;
    return var;
}


void vp8e_filter_block2d_bil_first_pass
(
    const unsigned char *src_ptr,
    unsigned short *output_ptr,
    unsigned int src_pixels_per_line,
    int pixel_step,
    unsigned int output_height,
    unsigned int output_width,
    const int *vp8_filter
)
{
    unsigned int i, j;

    for (i = 0; i < output_height; i++)
    {
        for (j = 0; j < output_width; j++)
        {
            // Apply bilinear filter
            output_ptr[j] = (((int)src_ptr[0]          * vp8_filter[0]) +
                             ((int)src_ptr[pixel_step] * vp8_filter[1]) +
                             (VP8_FILTER_WEIGHT / 2)) >> VP8_FILTER_SHIFT;
            src_ptr++;
        }

        // Next row...
        src_ptr    += src_pixels_per_line - output_width;
        output_ptr += output_width;
    }
}

void vp8e_filter_block2d_bil_second_pass
(
    const unsigned short *src_ptr,
    unsigned char  *output_ptr,
    unsigned int  src_pixels_per_line,
    unsigned int  pixel_step,
    unsigned int  output_height,
    unsigned int  output_width,
    const int *vp8_filter
)
{
    unsigned int  i, j;
    int  Temp;

    for (i = 0; i < output_height; i++)
    {
        for (j = 0; j < output_width; j++)
        {
            // Apply filter
            Temp = ((int)src_ptr[0]         * vp8_filter[0]) +
                   ((int)src_ptr[pixel_step] * vp8_filter[1]) +
                   (VP8_FILTER_WEIGHT / 2);
            output_ptr[j] = (unsigned int)(Temp >> VP8_FILTER_SHIFT);
            src_ptr++;
        }

        // Next row...
        src_ptr    += src_pixels_per_line - output_width;
        output_ptr += output_width;
    }
}


void vp8e_filter_block2d_bil
(
    const unsigned char  *src_ptr,
    unsigned char *output_ptr,
    unsigned int src_pixels_per_line,
    int  *HFilter,
    int  *VFilter
)
{

    unsigned short FData[20*16];    // Temp data bufffer used in filtering

    // First filter 1-D horizontally...
    vp8e_filter_block2d_bil_first_pass(src_ptr, FData, src_pixels_per_line, 1, 9, 8, HFilter);

    // then 1-D vertically...
    vp8e_filter_block2d_bil_second_pass(FData, output_ptr, 8, 8, 8, 8, VFilter);
}



unsigned int vp8_sub_pixel_variance4x4_c
(
    const unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    const unsigned char *dst_ptr,
    int dst_pixels_per_line,
    unsigned int *sse
)
{
    unsigned char  temp2[20*16];
    const int *HFilter, *VFilter;
    unsigned short FData3[5*4]; // Temp data bufffer used in filtering

    HFilter = vp8_bilinear_taps[xoffset];
    VFilter = vp8_bilinear_taps[yoffset];

    // First filter 1d Horizontal
    vp8e_filter_block2d_bil_first_pass(src_ptr, FData3, src_pixels_per_line, 1, 5, 4, HFilter);

    // Now filter Verticaly
    vp8e_filter_block2d_bil_second_pass(FData3, temp2, 4,  4,  4,  4, VFilter);

    return vp8_variance4x4_c(temp2, 4, dst_ptr, dst_pixels_per_line, sse);
}


unsigned int vp8_sub_pixel_variance8x8_c
(
    const unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    const unsigned char *dst_ptr,
    int dst_pixels_per_line,
    unsigned int *sse
)
{
    unsigned short FData3[9*8]; // Temp data bufffer used in filtering
    unsigned char  temp2[20*16];
    const int *HFilter, *VFilter;

    HFilter = vp8_bilinear_taps[xoffset];
    VFilter = vp8_bilinear_taps[yoffset];

    vp8e_filter_block2d_bil_first_pass(src_ptr, FData3, src_pixels_per_line, 1, 9, 8, HFilter);
    vp8e_filter_block2d_bil_second_pass(FData3, temp2, 8, 8, 8, 8, VFilter);

    return vp8_variance8x8_c(temp2, 8, dst_ptr, dst_pixels_per_line, sse);
}

unsigned int vp8_sub_pixel_variance16x16_c
(
    const unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    const unsigned char *dst_ptr,
    int dst_pixels_per_line,
    unsigned int *sse
)
{
    unsigned short FData3[17*16];   // Temp data bufffer used in filtering
    unsigned char  temp2[20*16];
    const int *HFilter, *VFilter;

    HFilter = vp8_bilinear_taps[xoffset];
    VFilter = vp8_bilinear_taps[yoffset];

    vp8e_filter_block2d_bil_first_pass(src_ptr, FData3, src_pixels_per_line, 1, 17, 16, HFilter);
    vp8e_filter_block2d_bil_second_pass(FData3, temp2, 16, 16, 16, 16, VFilter);

    return vp8_variance16x16_c(temp2, 16, dst_ptr, dst_pixels_per_line, sse);
}


unsigned int vp8_variance_halfpixvar16x16_h_c(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    unsigned int *sse)
{
    return vp8_sub_pixel_variance16x16_c(src_ptr, source_stride, 4, 0,
                                         ref_ptr, recon_stride, sse);
}


unsigned int vp8_variance_halfpixvar16x16_v_c(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    unsigned int *sse)
{
    return vp8_sub_pixel_variance16x16_c(src_ptr, source_stride, 0, 4,
                                         ref_ptr, recon_stride, sse);
}


unsigned int vp8_variance_halfpixvar16x16_hv_c(
    const unsigned char *src_ptr,
    int  source_stride,
    const unsigned char *ref_ptr,
    int  recon_stride,
    unsigned int *sse)
{
    return vp8_sub_pixel_variance16x16_c(src_ptr, source_stride, 4, 4,
                                         ref_ptr, recon_stride, sse);
}


unsigned int vp8_sub_pixel_mse16x16_c
(
    const unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    const unsigned char *dst_ptr,
    int dst_pixels_per_line,
    unsigned int *sse
)
{
    vp8_sub_pixel_variance16x16_c(src_ptr, src_pixels_per_line, xoffset, yoffset, dst_ptr, dst_pixels_per_line, sse);
    return *sse;
}

unsigned int vp8_sub_pixel_variance16x8_c
(
    const unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    const unsigned char *dst_ptr,
    int dst_pixels_per_line,
    unsigned int *sse
)
{
    unsigned short FData3[16*9];    // Temp data bufffer used in filtering
    unsigned char  temp2[20*16];
    const int *HFilter, *VFilter;

    HFilter = vp8_bilinear_taps[xoffset];
    VFilter = vp8_bilinear_taps[yoffset];

    vp8e_filter_block2d_bil_first_pass(src_ptr, FData3, src_pixels_per_line, 1, 9, 16, HFilter);
    vp8e_filter_block2d_bil_second_pass(FData3, temp2, 16, 16, 8, 16, VFilter);

    return vp8_variance16x8_c(temp2, 16, dst_ptr, dst_pixels_per_line, sse);
}

unsigned int vp8_sub_pixel_variance8x16_c
(
    const unsigned char  *src_ptr,
    int  src_pixels_per_line,
    int  xoffset,
    int  yoffset,
    const unsigned char *dst_ptr,
    int dst_pixels_per_line,
    unsigned int *sse
)
{
    unsigned short FData3[9*16];    // Temp data bufffer used in filtering
    unsigned char  temp2[20*16];
    const int *HFilter, *VFilter;


    HFilter = vp8_bilinear_taps[xoffset];
    VFilter = vp8_bilinear_taps[yoffset];


    vp8e_filter_block2d_bil_first_pass(src_ptr, FData3, src_pixels_per_line, 1, 17, 8, HFilter);
    vp8e_filter_block2d_bil_second_pass(FData3, temp2, 8, 8, 16, 8, VFilter);

    return vp8_variance8x16_c(temp2, 8, dst_ptr, dst_pixels_per_line, sse);
}