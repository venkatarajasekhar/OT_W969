


#include "onyx_int.h"
#include "threading.h"
#include "common.h"
#include "extend.h"


extern int vp8cx_encode_inter_macroblock(VP8_COMP *cpi, MACROBLOCK *x, TOKENEXTRA **t, int recon_yoffset, int recon_uvoffset);
extern int vp8cx_encode_intra_macro_block(VP8_COMP *cpi, MACROBLOCK *x, TOKENEXTRA **t);
extern void vp8cx_mb_init_quantizer(VP8_COMP *cpi, MACROBLOCK *x);
extern void vp8_build_block_offsets(MACROBLOCK *x);
extern void vp8_setup_block_ptrs(MACROBLOCK *x);

static
THREAD_FUNCTION thread_encoding_proc(void *p_data)
{
#if CONFIG_MULTITHREAD
    int ithread = ((ENCODETHREAD_DATA *)p_data)->ithread;
    VP8_COMP *cpi   = (VP8_COMP *)(((ENCODETHREAD_DATA *)p_data)->ptr1);
    MB_ROW_COMP *mbri = (MB_ROW_COMP *)(((ENCODETHREAD_DATA *)p_data)->ptr2);
    ENTROPY_CONTEXT_PLANES mb_row_left_context;

    //printf("Started thread %d\n", ithread);

    while (1)
    {
        if (cpi->b_multi_threaded == 0)
            break;

        //if(WaitForSingleObject(cpi->h_event_mbrencoding[ithread], INFINITE) == WAIT_OBJECT_0)
        if (sem_wait(&cpi->h_event_mbrencoding[ithread]) == 0)
        {
            if (cpi->b_multi_threaded == FALSE) // we're shutting down
                break;
            else
            {
                VP8_COMMON *cm      = &cpi->common;
                int mb_row           = mbri->mb_row;
                MACROBLOCK  *x      = &mbri->mb;
                MACROBLOCKD *xd     = &x->e_mbd;
                TOKENEXTRA **tp     = &mbri->tp;
                int *segment_counts  = mbri->segment_counts;
                int *totalrate      = &mbri->totalrate;

                {
                    int i;
                    int recon_yoffset, recon_uvoffset;
                    int mb_col;
                    int ref_fb_idx = cm->lst_fb_idx;
                    int dst_fb_idx = cm->new_fb_idx;
                    int recon_y_stride = cm->yv12_fb[ref_fb_idx].y_stride;
                    int recon_uv_stride = cm->yv12_fb[ref_fb_idx].uv_stride;
                    volatile int *last_row_current_mb_col;

                    if (ithread > 0)
                        last_row_current_mb_col = &cpi->mb_row_ei[ithread-1].current_mb_col;
                    else
                        last_row_current_mb_col = &cpi->current_mb_col_main;

                    // reset above block coeffs
                    xd->above_context = cm->above_context;
                    xd->left_context = &mb_row_left_context;

                    vp8_zero(mb_row_left_context);

                    xd->up_available = (mb_row != 0);
                    recon_yoffset = (mb_row * recon_y_stride * 16);
                    recon_uvoffset = (mb_row * recon_uv_stride * 8);


                    cpi->tplist[mb_row].start = *tp;

                    //printf("Thread mb_row = %d\n", mb_row);

                    // for each macroblock col in image
                    for (mb_col = 0; mb_col < cm->mb_cols; mb_col++)
                    {
                        int seg_map_index = (mb_row * cm->mb_cols);

                        while (mb_col > (*last_row_current_mb_col - 1) && *last_row_current_mb_col != cm->mb_cols - 1)
                        {
                            x86_pause_hint();
                            thread_sleep(0);
                        }

                        // Distance of Mb to the various image edges.
                        // These specified to 8th pel as they are always compared to values that are in 1/8th pel units
                        xd->mb_to_left_edge = -((mb_col * 16) << 3);
                        xd->mb_to_right_edge = ((cm->mb_cols - 1 - mb_col) * 16) << 3;
                        xd->mb_to_top_edge = -((mb_row * 16) << 3);
                        xd->mb_to_bottom_edge = ((cm->mb_rows - 1 - mb_row) * 16) << 3;

                        // Set up limit values for motion vectors used to prevent them extending outside the UMV borders
                        x->mv_col_min = -((mb_col * 16) + (VP8BORDERINPIXELS - 16));
                        x->mv_col_max = ((cm->mb_cols - 1 - mb_col) * 16) + (VP8BORDERINPIXELS - 16);
                        x->mv_row_min = -((mb_row * 16) + (VP8BORDERINPIXELS - 16));
                        x->mv_row_max = ((cm->mb_rows - 1 - mb_row) * 16) + (VP8BORDERINPIXELS - 16);

                        xd->dst.y_buffer = cm->yv12_fb[dst_fb_idx].y_buffer + recon_yoffset;
                        xd->dst.u_buffer = cm->yv12_fb[dst_fb_idx].u_buffer + recon_uvoffset;
                        xd->dst.v_buffer = cm->yv12_fb[dst_fb_idx].v_buffer + recon_uvoffset;
                        xd->left_available = (mb_col != 0);

                        // Is segmentation enabled
                        // MB level adjutment to quantizer
                        if (xd->segmentation_enabled)
                        {
                            // Code to set segment id in xd->mbmi.segment_id for current MB (with range checking)
                            if (cpi->segmentation_map[seg_map_index+mb_col] <= 3)
                                xd->mode_info_context->mbmi.segment_id = cpi->segmentation_map[seg_map_index+mb_col];
                            else
                                xd->mode_info_context->mbmi.segment_id = 0;

                            vp8cx_mb_init_quantizer(cpi, x);
                        }
                        else
                            xd->mode_info_context->mbmi.segment_id = 0;         // Set to Segment 0 by default


                        if (cm->frame_type == KEY_FRAME)
                        {
                            *totalrate += vp8cx_encode_intra_macro_block(cpi, x, tp);
#ifdef MODE_STATS
                            y_modes[xd->mbmi.mode] ++;
#endif
                        }
                        else
                        {
                            *totalrate += vp8cx_encode_inter_macroblock(cpi, x, tp, recon_yoffset, recon_uvoffset);

#ifdef MODE_STATS
                            inter_y_modes[xd->mbmi.mode] ++;

                            if (xd->mbmi.mode == SPLITMV)
                            {
                                int b;

                                for (b = 0; b < xd->mbmi.partition_count; b++)
                                {
                                    inter_b_modes[x->partition->bmi[b].mode] ++;
                                }
                            }

#endif

                            // Count of last ref frame 0,0 useage
                            if ((xd->mode_info_context->mbmi.mode == ZEROMV) && (xd->mode_info_context->mbmi.ref_frame == LAST_FRAME))
                                cpi->inter_zz_count ++;

                        }

                        cpi->tplist[mb_row].stop = *tp;

                        x->gf_active_ptr++;      // Increment pointer into gf useage flags structure for next mb

                        for (i = 0; i < 16; i++)
                            vpx_memcpy(&xd->mode_info_context->bmi[i], &xd->block[i].bmi, sizeof(xd->block[i].bmi));

                        // adjust to the next column of macroblocks
                        x->src.y_buffer += 16;
                        x->src.u_buffer += 8;
                        x->src.v_buffer += 8;

                        recon_yoffset += 16;
                        recon_uvoffset += 8;

                        // Keep track of segment useage
                        segment_counts[xd->mode_info_context->mbmi.segment_id] ++;

                        // skip to next mb
                        xd->mode_info_context++;
                        x->partition_info++;

                        xd->above_context++;

                        cpi->mb_row_ei[ithread].current_mb_col = mb_col;

                    }

                    //extend the recon for intra prediction
                    vp8_extend_mb_row(
                        &cm->yv12_fb[dst_fb_idx],
                        xd->dst.y_buffer + 16,
                        xd->dst.u_buffer + 8,
                        xd->dst.v_buffer + 8);

                    // this is to account for the border
                    xd->mode_info_context++;
                    x->partition_info++;

                    x->src.y_buffer += 16 * x->src.y_stride * (cpi->encoding_thread_count + 1) - 16 * cm->mb_cols;
                    x->src.u_buffer +=  8 * x->src.uv_stride * (cpi->encoding_thread_count + 1) - 8 * cm->mb_cols;
                    x->src.v_buffer +=  8 * x->src.uv_stride * (cpi->encoding_thread_count + 1) - 8 * cm->mb_cols;

                    xd->mode_info_context += xd->mode_info_stride * cpi->encoding_thread_count;
                    x->partition_info += xd->mode_info_stride * cpi->encoding_thread_count;

                    if (ithread == (cpi->encoding_thread_count - 1) || mb_row == cm->mb_rows - 1)
                    {
                        //SetEvent(cpi->h_event_main);
                        sem_post(&cpi->h_event_main);
                    }

                }

            }
        }
    }

#else
    (void) p_data;
#endif

    //printf("exit thread %d\n", ithread);
    return 0;
}

static void setup_mbby_copy(MACROBLOCK *mbdst, MACROBLOCK *mbsrc)
{

    MACROBLOCK *x = mbsrc;
    MACROBLOCK *z = mbdst;
    int i;

    z->ss               = x->ss;
    z->ss_count          = x->ss_count;
    z->searches_per_step  = x->searches_per_step;
    z->errorperbit      = x->errorperbit;

    z->sadperbit16      = x->sadperbit16;
    z->sadperbit4       = x->sadperbit4;
    z->errthresh        = x->errthresh;
    z->rddiv            = x->rddiv;
    z->rdmult           = x->rdmult;

    /*
    z->mv_col_min    = x->mv_col_min;
    z->mv_col_max    = x->mv_col_max;
    z->mv_row_min    = x->mv_row_min;
    z->mv_row_max    = x->mv_row_max;
    z->vector_range = x->vector_range ;
    */

    z->vp8_short_fdct4x4     = x->vp8_short_fdct4x4;
    z->vp8_short_fdct8x4     = x->vp8_short_fdct8x4;
    z->short_walsh4x4    = x->short_walsh4x4;
    z->quantize_b        = x->quantize_b;

    /*
    z->mvc              = x->mvc;
    z->src.y_buffer      = x->src.y_buffer;
    z->src.u_buffer      = x->src.u_buffer;
    z->src.v_buffer      = x->src.v_buffer;
    */


    vpx_memcpy(z->mvcosts,          x->mvcosts,         sizeof(x->mvcosts));
    z->mvcost[0] = &z->mvcosts[0][mv_max+1];
    z->mvcost[1] = &z->mvcosts[1][mv_max+1];
    z->mvsadcost[0] = &z->mvsadcosts[0][mv_max+1];
    z->mvsadcost[1] = &z->mvsadcosts[1][mv_max+1];


    vpx_memcpy(z->token_costs,       x->token_costs,      sizeof(x->token_costs));
    vpx_memcpy(z->inter_bmode_costs,  x->inter_bmode_costs, sizeof(x->inter_bmode_costs));
    //memcpy(z->mvcosts,            x->mvcosts,         sizeof(x->mvcosts));
    //memcpy(z->mvcost,         x->mvcost,          sizeof(x->mvcost));
    vpx_memcpy(z->mbmode_cost,       x->mbmode_cost,      sizeof(x->mbmode_cost));
    vpx_memcpy(z->intra_uv_mode_cost,  x->intra_uv_mode_cost, sizeof(x->intra_uv_mode_cost));
    vpx_memcpy(z->bmode_costs,       x->bmode_costs,      sizeof(x->bmode_costs));

    for (i = 0; i < 25; i++)
    {
        z->block[i].quant           = x->block[i].quant;
        z->block[i].quant_shift     = x->block[i].quant_shift;
        z->block[i].zbin            = x->block[i].zbin;
        z->block[i].zrun_zbin_boost   = x->block[i].zrun_zbin_boost;
        z->block[i].round           = x->block[i].round;
        /*
        z->block[i].src             = x->block[i].src;
        */
        z->block[i].src_stride       = x->block[i].src_stride;
        z->block[i].force_empty      = x->block[i].force_empty;

    }

    {
        MACROBLOCKD *xd = &x->e_mbd;
        MACROBLOCKD *zd = &z->e_mbd;

        /*
        zd->mode_info_context = xd->mode_info_context;
        zd->mode_info        = xd->mode_info;

        zd->mode_info_stride  = xd->mode_info_stride;
        zd->frame_type       = xd->frame_type;
        zd->up_available     = xd->up_available   ;
        zd->left_available   = xd->left_available;
        zd->left_context     = xd->left_context;
        zd->last_frame_dc     = xd->last_frame_dc;
        zd->last_frame_dccons = xd->last_frame_dccons;
        zd->gold_frame_dc     = xd->gold_frame_dc;
        zd->gold_frame_dccons = xd->gold_frame_dccons;
        zd->mb_to_left_edge    = xd->mb_to_left_edge;
        zd->mb_to_right_edge   = xd->mb_to_right_edge;
        zd->mb_to_top_edge     = xd->mb_to_top_edge   ;
        zd->mb_to_bottom_edge  = xd->mb_to_bottom_edge;
        zd->gf_active_ptr     = xd->gf_active_ptr;
        zd->frames_since_golden       = xd->frames_since_golden;
        zd->frames_till_alt_ref_frame   = xd->frames_till_alt_ref_frame;
        */
        zd->subpixel_predict         = xd->subpixel_predict;
        zd->subpixel_predict8x4      = xd->subpixel_predict8x4;
        zd->subpixel_predict8x8      = xd->subpixel_predict8x8;
        zd->subpixel_predict16x16    = xd->subpixel_predict16x16;
        zd->segmentation_enabled     = xd->segmentation_enabled;
        zd->mb_segement_abs_delta      = xd->mb_segement_abs_delta;
        vpx_memcpy(zd->segment_feature_data, xd->segment_feature_data, sizeof(xd->segment_feature_data));

        for (i = 0; i < 25; i++)
        {
            zd->block[i].dequant = xd->block[i].dequant;
        }
    }
}


void vp8cx_init_mbrthread_data(VP8_COMP *cpi,
                               MACROBLOCK *x,
                               MB_ROW_COMP *mbr_ei,
                               int mb_row,
                               int count
                              )
{

    VP8_COMMON *const cm = & cpi->common;
    MACROBLOCKD *const xd = & x->e_mbd;
    int i;
    (void) mb_row;

    for (i = 0; i < count; i++)
    {
        MACROBLOCK *mb = & mbr_ei[i].mb;
        MACROBLOCKD *mbd = &mb->e_mbd;

        mbd->subpixel_predict        = xd->subpixel_predict;
        mbd->subpixel_predict8x4     = xd->subpixel_predict8x4;
        mbd->subpixel_predict8x8     = xd->subpixel_predict8x8;
        mbd->subpixel_predict16x16   = xd->subpixel_predict16x16;
#if CONFIG_RUNTIME_CPU_DETECT
        mbd->rtcd                   = xd->rtcd;
#endif
        mb->gf_active_ptr            = x->gf_active_ptr;

        mb->vector_range             = 32;

        vpx_memset(mbr_ei[i].segment_counts, 0, sizeof(mbr_ei[i].segment_counts));
        mbr_ei[i].totalrate = 0;

        mb->partition_info = x->pi + x->e_mbd.mode_info_stride * (i + 1);

        mbd->mode_info_context = cm->mi   + x->e_mbd.mode_info_stride * (i + 1);
        mbd->mode_info_stride  = cm->mode_info_stride;

        mbd->frame_type = cm->frame_type;

        mbd->frames_since_golden = cm->frames_since_golden;
        mbd->frames_till_alt_ref_frame = cm->frames_till_alt_ref_frame;

        mb->src = * cpi->Source;
        mbd->pre = cm->yv12_fb[cm->lst_fb_idx];
        mbd->dst = cm->yv12_fb[cm->new_fb_idx];

        mb->src.y_buffer += 16 * x->src.y_stride * (i + 1);
        mb->src.u_buffer +=  8 * x->src.uv_stride * (i + 1);
        mb->src.v_buffer +=  8 * x->src.uv_stride * (i + 1);


        vp8_build_block_offsets(mb);

        vp8_setup_block_dptrs(mbd);

        vp8_setup_block_ptrs(mb);

        mb->rddiv = cpi->RDDIV;
        mb->rdmult = cpi->RDMULT;

        mbd->left_context = &cm->left_context;
        mb->mvc = cm->fc.mvc;

        setup_mbby_copy(&mbr_ei[i].mb, x);

    }
}


void vp8cx_create_encoder_threads(VP8_COMP *cpi)
{
    cpi->b_multi_threaded = 0;

    cpi->processor_core_count = 32; //vp8_get_proc_core_count();

    CHECK_MEM_ERROR(cpi->tplist, vpx_malloc(sizeof(TOKENLIST) * cpi->common.mb_rows));

#if CONFIG_MULTITHREAD

    if (cpi->processor_core_count > 1 && cpi->oxcf.multi_threaded > 1)
    {
        int ithread;

        if (cpi->oxcf.multi_threaded > cpi->processor_core_count)
            cpi->encoding_thread_count = cpi->processor_core_count - 1;
        else
            cpi->encoding_thread_count = cpi->oxcf.multi_threaded - 1;


        CHECK_MEM_ERROR(cpi->h_encoding_thread, vpx_malloc(sizeof(pthread_t) * cpi->encoding_thread_count));
        CHECK_MEM_ERROR(cpi->h_event_mbrencoding, vpx_malloc(sizeof(sem_t) * cpi->encoding_thread_count));
        CHECK_MEM_ERROR(cpi->mb_row_ei, vpx_memalign(32, sizeof(MB_ROW_COMP) * cpi->encoding_thread_count));
        vpx_memset(cpi->mb_row_ei, 0, sizeof(MB_ROW_COMP) * cpi->encoding_thread_count);
        CHECK_MEM_ERROR(cpi->en_thread_data, vpx_malloc(sizeof(ENCODETHREAD_DATA) * cpi->encoding_thread_count));
        //cpi->h_event_main = CreateEvent(NULL, FALSE, FALSE, NULL);
        sem_init(&cpi->h_event_main, 0, 0);

        cpi->b_multi_threaded = 1;

        //printf("[VP8:] multi_threaded encoding is enabled with %d threads\n\n", (cpi->encoding_thread_count +1));

        for (ithread = 0; ithread < cpi->encoding_thread_count; ithread++)
        {
            //cpi->h_event_mbrencoding[ithread] = CreateEvent(NULL, FALSE, FALSE, NULL);
            sem_init(&cpi->h_event_mbrencoding[ithread], 0, 0);
            cpi->en_thread_data[ithread].ithread = ithread;
            cpi->en_thread_data[ithread].ptr1 = (void *)cpi;
            cpi->en_thread_data[ithread].ptr2 = (void *)&cpi->mb_row_ei[ithread];

            //printf(" call begin thread %d \n", ithread);

            //cpi->h_encoding_thread[ithread] =   (HANDLE)_beginthreadex(
            //  NULL,           // security
            //  0,              // stksize
            //  thread_encoding_proc,
            //  (&cpi->en_thread_data[ithread]),          // Thread data
            //  0,
            //  NULL);

            pthread_create(&cpi->h_encoding_thread[ithread], 0, thread_encoding_proc, (&cpi->en_thread_data[ithread]));

        }

    }

#endif
}

void vp8cx_remove_encoder_threads(VP8_COMP *cpi)
{
#if CONFIG_MULTITHREAD

    if (cpi->b_multi_threaded)
    {
        //shutdown other threads
        cpi->b_multi_threaded = 0;
        {
            int i;

            for (i = 0; i < cpi->encoding_thread_count; i++)
            {
                //SetEvent(cpi->h_event_mbrencoding[i]);
                sem_post(&cpi->h_event_mbrencoding[i]);
                pthread_join(cpi->h_encoding_thread[i], 0);
            }

            for (i = 0; i < cpi->encoding_thread_count; i++)
                sem_destroy(&cpi->h_event_mbrencoding[i]);
        }
        //free thread related resources
        vpx_free(cpi->h_event_mbrencoding);
        vpx_free(cpi->h_encoding_thread);
        vpx_free(cpi->mb_row_ei);
        vpx_free(cpi->en_thread_data);
    }

#endif
    vpx_free(cpi->tplist);
}
