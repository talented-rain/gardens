/*
 * Video Driver : LCD with framebuffer (AXIVDMA + VTC)
 *
 * File Name:   zynq7sdk_hdmi.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.11.01
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <platform/fwk_basic.h>
#include <platform/fwk_inode.h>
#include <platform/fwk_fs.h>
#include <platform/of/fwk_of.h>
#include <platform/of/fwk_of_device.h>
#include <platform/fwk_platdrv.h>
#include <platform/clk/fwk_clk.h>
#include <platform/fwk_pinctrl.h>
#include <platform/gpio/fwk_gpiodesc.h>
#include <platform/fwk_uaccess.h>
#include <platform/video/fwk_fbmem.h>

#include <zynq7/zynq7_periph.h>

/*!< The defines */
#define ZYNQ7_SDK_HDMI_XPRES_MAX                (1920)
#define ZYNQ7_SDK_HDMI_YPRES_MAX                (1080)
#define ZYNQ7_SDK_HDMI_BPP_MAX                  (4)

#define ZYNQ7_SDK_HDMI_DRAM_SIZE    \
                    (ZYNQ7_SDK_HDMI_XPRES_MAX * ZYNQ7_SDK_HDMI_YPRES_MAX * ZYNQ7_SDK_HDMI_BPP_MAX)

struct zynq7sdk_hdmi_trigger
{
	kuint32_t hsync_active;
	kuint32_t vsync_active;
	kuint32_t de_active;
	kuint32_t pixelclk_active;
};

struct zynq7sdk_hdmi_drv
{
    kuint32_t minor;

    void *base;
    struct fwk_fb_info *sprt_fb;
    struct fwk_device *sprt_dev;
    struct zynq7sdk_hdmi_trigger sgrt_trig;

    DisplayCtrl sgrt_dctrl;
    XAxiVdma sgrt_axivdma;
    XAxiVdma_Config sgrt_axicfg;
    XVtc_Config sgrt_vcfg;
};

#define ZYNQ7_SDK_HDMI_DRIVER_MINOR				0

/* The globals */

/*!< API function */
/*!
 * @brief   zynq7sdk_hdmi_init
 * @param   base
 * @retval  errno
 * @note    none
 */
static kint32_t zynq7sdk_hdmi_init(void *base, struct zynq7sdk_hdmi_drv *sprt_drv)
{
    struct fwk_fb_info *sprt_fb;
    struct fwk_fb_var_screen_info *sprt_var;
	XAxiVdma_Config *sprt_axicfg;
    XVtc_Config *sprt_vcfg;
    VideoMode sgrt_vmode;
    kuint8_t *pFrames[DISPLAY_NUM_FRAMES];
    kint32_t retval;

    if (DISPLAY_NUM_FRAMES > 1)
        return -ER_UNVALID;

    sprt_fb   = sprt_drv->sprt_fb;
    sprt_var  = &sprt_fb->sgrt_var;
    sprt_vcfg = &sprt_drv->sgrt_vcfg;
    sprt_axicfg = &sprt_drv->sgrt_axicfg;

    retval = XAxiVdma_CfgInitialize(&sprt_drv->sgrt_axivdma, sprt_axicfg, sprt_axicfg->BaseAddress);
    if (retval)
        return retval;

    sprintk(sgrt_vmode.label, "hdmi %dx%d@60Hz", sprt_var->xres, sprt_var->yres);
    sgrt_vmode.freq = ((kfloat_t)(sprt_var->pixclock / 1000)) / 1000.0;
    
    sgrt_vmode.width = sprt_var->xres;
    sgrt_vmode.height = sprt_var->yres;

    sgrt_vmode.hps = sprt_var->xres + sprt_var->right_margin;
    sgrt_vmode.hpe = sgrt_vmode.hps + sprt_var->hsync_len;
    sgrt_vmode.hmax = sgrt_vmode.hpe + sprt_var->left_margin - 1;
    sgrt_vmode.hpol = !sprt_drv->sgrt_trig.hsync_active;

    sgrt_vmode.vps = sprt_var->yres + sprt_var->lower_margin;
    sgrt_vmode.vpe = sgrt_vmode.vps + sprt_var->vsync_len;
    sgrt_vmode.vmax = sgrt_vmode.vpe + sprt_var->upper_margin - 1;
    sgrt_vmode.vpol = !sprt_drv->sgrt_trig.vsync_active;

    pFrames[0] = sprt_fb->screen_base;
    retval = DisplayInitialize( &sprt_drv->sgrt_dctrl, 
                                &sprt_drv->sgrt_axivdma, 
                                XPAR_VTC_0_DEVICE_ID, 
                                XPAR_AXI_DYNCLK_0_BASEADDR, 
                                pFrames, 
                                sprt_var->xres * (sprt_var->bits_per_pixel >> 3), 
                                &sgrt_vmode);
    if (retval)
        return retval;

    print_info("axivdma and vtc initialize successfully\n");
    print_info("screen's xres is %d, yres is: %d, pixel-width is: %d\n", 
                sprt_var->xres, sprt_var->yres, sprt_var->bits_per_pixel);

    retval = XVtc_CfgInitialize(&(sprt_drv->sgrt_dctrl.vtc), sprt_vcfg, sprt_vcfg->BaseAddress);
    if (retval)
        return retval;

    return ER_NORMAL;
}

/*!
 * @brief   driver open
 * @param   sprt_inode, user
 * @retval  errno
 * @note    none
 */
static kint32_t zynq7sdk_hdmi_open(struct fwk_fb_info *sprt_info, kint32_t user)
{
    struct zynq7sdk_hdmi_drv *sprt_drv;
    kint32_t retval;

    sprt_drv = fwk_fb_get_drvdata(sprt_info);
    retval = DisplayStart(&sprt_drv->sgrt_dctrl);
    if (retval)
        return -ER_FAILD;

    print_info("hdmi device is opened\n");

    memset_ex(sprt_info->screen_base, 0xffffffff, sprt_info->screen_size);
    print_info("clear full screen with white color\n");

    return ER_NORMAL;
}

/*!
 * @brief   driver close
 * @param   sprt_inode, user
 * @retval  errno
 * @note    none
 */
static kint32_t zynq7sdk_hdmi_close(struct fwk_fb_info *sprt_info, kint32_t user)
{
    struct zynq7sdk_hdmi_drv *sprt_drv;
    kint32_t retval;

    sprt_drv = fwk_fb_get_drvdata(sprt_info);

    memset_ex(sprt_info->screen_base, 0x00000000, sprt_info->screen_size);
    retval = DisplayStop(&sprt_drv->sgrt_dctrl);
    if (!retval)
        print_info("hdmi device is closed\n");

    return retval;
}

/*!
 * @brief   driver ioctl
 * @param   sprt_inode, user
 * @retval  errno
 * @note    none
 */
kint32_t zynq7sdk_hdmi_ioctl(struct fwk_fb_info *sprt_info, kuint32_t cmd, kuaddr_t arg)
{
    struct zynq7sdk_hdmi_drv *sprt_drv;
    struct fwk_fb_var_screen_info *sprt_var;
    kuaddr_t new_smem;

    sprt_drv = fwk_fb_get_drvdata(sprt_info);
    if (!arg)
        return -ER_NULLPTR;

    switch (cmd)
    {
        case NR_FB_IOSET_VARINFO:
            new_smem = sprt_info->sgrt_fix.smem_start;
            sprt_var = (struct fwk_fb_var_screen_info *)arg;

            if ((sprt_var->xoffset >= sprt_info->sgrt_var.xres_virtual) ||
                (sprt_var->yoffset >= sprt_info->sgrt_var.yres_virtual))
                return -ER_MORE;

            if (sprt_var->yoffset >= sprt_info->sgrt_var.yres)
            {
                new_smem += sprt_info->sgrt_fix.smem_len;
                new_smem  = mrt_align(new_smem, 8);
            }

            sprt_info->screen_base = (kuint8_t *)new_smem;
            if (DisplayChangeFrameBuffer(&sprt_drv->sgrt_dctrl, new_smem, sprt_info->sgrt_fix.smem_len))
                return -ER_FAILD;

        default:
            break;
    }

    return ER_NORMAL;
}

static const struct fwk_fb_oprts sgrt_fwk_fb_ops =
{
    .fb_open = zynq7sdk_hdmi_open,
    .fb_release = zynq7sdk_hdmi_close,
    .fb_ioctl = zynq7sdk_hdmi_ioctl,
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   probe axivdma
 * @param   sprt_pdev
 * @retval  errno
 * @note    none
 */
static kint32_t zynq7sdk_hdmi_driver_probe_axivdma(struct fwk_platdev *sprt_pdev)
{
    struct zynq7sdk_hdmi_drv *sprt_drv;
    struct fwk_device_node *sprt_node, *sprt_axivdma = mrt_nullptr;
    XAxiVdma_Config *sprt_axicfg;
    void *reg;
    kuint32_t temp;
    kint32_t retval = 0;

    struct fwk_of_device_id sgrt_axivdma_ids[] =
    {
        { .compatible = "xlnx, pl, axivdma", },
        {},
    };

    sprt_node = sprt_pdev->sgrt_dev.sprt_node;
    sprt_drv = (struct zynq7sdk_hdmi_drv *)fwk_platform_get_drvdata(sprt_pdev);
    if (!isValid(sprt_node) || !isValid(sprt_drv))
        return -ER_NODEV;

    sprt_axivdma = fwk_of_find_matching_child_and_match(sprt_node, sgrt_axivdma_ids, mrt_nullptr);
    if (!isValid(sprt_axivdma))
        return -ER_NOTFOUND;

    sprt_axicfg = &sprt_drv->sgrt_axicfg;

    retval |= fwk_of_property_read_u32(sprt_axivdma, "max-frame-store-num", &temp);
    if (!retval)
        sprt_axicfg->MaxFrameStoreNum = (kuint16_t)temp;

    retval |= fwk_of_property_read_u32(sprt_axivdma, "has-Vertical-filp", &temp);
    if (!retval)
        sprt_axicfg->HasVFlip = (kuint8_t)temp;
    
    retval |= fwk_of_property_read_u32(sprt_axivdma, "has-mm2s", (kuint32_t *)(&sprt_axicfg->HasMm2S));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "has-mm2s-dre", (kuint32_t *)(&sprt_axicfg->HasMm2SDRE));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "mm2s-word-len", (kuint32_t *)(&sprt_axicfg->Mm2SWordLen));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "has-s2mm", (kuint32_t *)(&sprt_axicfg->HasS2Mm));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "has-s2mm-dre", (kuint32_t *)(&sprt_axicfg->HasS2MmDRE));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "s2mm-word-len", (kuint32_t *)(&sprt_axicfg->S2MmWordLen));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "has-sg", (kuint32_t *)(&sprt_axicfg->HasSG));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "enable-vid-param-read", (kuint32_t *)(&sprt_axicfg->EnableVIDParamRead));
    
    retval |= fwk_of_property_read_u32(sprt_axivdma, "use-fsync", (kuint32_t *)(&sprt_axicfg->UseFsync));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "flush-on-fsync", (kuint32_t *)(&sprt_axicfg->FlushonFsync));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "mm2s-buf-depth", (kuint32_t *)(&sprt_axicfg->Mm2SBufDepth));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "s2mm-buf-depth", (kuint32_t *)(&sprt_axicfg->S2MmBufDepth));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "mm2s-gen-lock", (kuint32_t *)(&sprt_axicfg->Mm2SGenLock));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "s2mm-gen-lock", (kuint32_t *)(&sprt_axicfg->S2MmGenLock));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "internal-gen-lock", (kuint32_t *)(&sprt_axicfg->InternalGenLock));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "s2mm-sof", (kuint32_t *)(&sprt_axicfg->S2MmSOF));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "mm2s-stream-width", (kuint32_t *)(&sprt_axicfg->Mm2SStreamWidth));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "s2mm-stream-width", (kuint32_t *)(&sprt_axicfg->S2MmStreamWidth));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "mm2s-thres-reg-en", (kuint32_t *)(&sprt_axicfg->Mm2SThresRegEn));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "mm2s-frm-store-reg-en", (kuint32_t *)(&sprt_axicfg->Mm2SFrmStoreRegEn));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "mm2s-dly-cntr-en", (kuint32_t *)(&sprt_axicfg->Mm2SDlyCntrEn));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "mm2s-frm-cntr-en", (kuint32_t *)(&sprt_axicfg->Mm2SFrmCntrEn));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "s2mm-thres-reg-en", (kuint32_t *)(&sprt_axicfg->S2MmThresRegEn));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "s2mm-frm-store-reg-en", (kuint32_t *)(&sprt_axicfg->S2MmFrmStoreRegEn));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "s2mm-sly-cntr-en", (kuint32_t *)(&sprt_axicfg->S2MmDlyCntrEn));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "s2mm-frm-cntr-en", (kuint32_t *)(&sprt_axicfg->S2MmFrmCntrEn));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "enable-all-dbg-features", (kuint32_t *)(&sprt_axicfg->EnableAllDbgFeatures));
    retval |= fwk_of_property_read_u32(sprt_axivdma, "addr-width", (kuint32_t *)(&sprt_axicfg->AddrWidth));
    
    if (retval)
        return -ER_NOTFOUND;

    reg = fwk_of_iomap(sprt_axivdma, 0);
    if (!isValid(reg))
        return PTR_ERR(reg);
    
    sprt_axicfg->BaseAddress = (kuint32_t)fwk_io_remap(reg);
    if (!sprt_axicfg->BaseAddress)
        return -ER_FAILD;

    sprt_axicfg->DeviceId = XPAR_AXI_VDMA_0_DEVICE_ID;
    return ER_NORMAL;
}

/*!
 * @brief   remove axivdma
 * @param   sprt_pdev
 * @retval  errno
 * @note    none
 */
static void zynq7sdk_hdmi_driver_remove_axivdma(struct fwk_platdev *sprt_pdev)
{
    struct zynq7sdk_hdmi_drv *sprt_drv;
    XAxiVdma_Config *sprt_axicfg;

    sprt_drv = (struct zynq7sdk_hdmi_drv *)fwk_platform_get_drvdata(sprt_pdev);
    if (!isValid(sprt_drv))
        return;

    sprt_axicfg = &sprt_drv->sgrt_axicfg;
    fwk_io_unmap((void *)sprt_axicfg->BaseAddress);
}

/*!
 * @brief   probe display-timings
 * @param   sprt_pdev
 * @retval  errno
 * @note    none
 */
static kint32_t zynq7sdk_hdmi_driver_probe_timings(struct fwk_platdev *sprt_pdev)
{
    struct zynq7sdk_hdmi_drv *sprt_drv;
    struct fwk_device_node *sprt_node, *sprt_tim;
    struct fwk_fb_info *sprt_fb;
    struct fwk_fb_var_screen_info *sprt_var;
    kuint32_t phandle;
    kchar_t *format = mrt_nullptr;
    kint32_t retval = 0;

    sprt_node = sprt_pdev->sgrt_dev.sprt_node;
    sprt_drv = (struct zynq7sdk_hdmi_drv *)fwk_platform_get_drvdata(sprt_pdev);
    if (!isValid(sprt_node) || !isValid(sprt_drv))
        return -ER_NODEV;

    sprt_fb = sprt_drv->sprt_fb;
    sprt_var = &sprt_fb->sgrt_var;
    
    retval = fwk_of_property_read_u32(sprt_node, "display-timings", &phandle);
    sprt_tim = retval ? mrt_nullptr : fwk_of_find_node_by_phandle(sprt_node, phandle);
    if (!isValid(sprt_tim))
        return -ER_NOTFOUND;

    /*!< get var info */
    retval  = 0;
    retval |= fwk_of_property_read_u32(sprt_tim, "clock-freq", &sprt_var->pixclock);
    retval |= fwk_of_property_read_u32(sprt_tim, "height", &sprt_var->xres);
    retval |= fwk_of_property_read_u32(sprt_tim, "width", &sprt_var->yres);
    retval |= fwk_of_property_read_u32(sprt_tim, "h-back-porch", &sprt_var->left_margin);
    retval |= fwk_of_property_read_u32(sprt_tim, "h-sync-width", &sprt_var->hsync_len);
    retval |= fwk_of_property_read_u32(sprt_tim, "h-front-porch", &sprt_var->right_margin);
    retval |= fwk_of_property_read_u32(sprt_tim, "v-back-porch", &sprt_var->upper_margin);
    retval |= fwk_of_property_read_u32(sprt_tim, "v-sync-width", &sprt_var->vsync_len);
    retval |= fwk_of_property_read_u32(sprt_tim, "v-front-porch", &sprt_var->lower_margin);

    retval |= fwk_of_property_read_u32(sprt_tim, "h-sync-active", &sprt_drv->sgrt_trig.hsync_active);
    retval |= fwk_of_property_read_u32(sprt_tim, "v-sync-active", &sprt_drv->sgrt_trig.vsync_active);
    retval |= fwk_of_property_read_u32(sprt_tim, "de-active", &sprt_drv->sgrt_trig.de_active);
    retval |= fwk_of_property_read_u32(sprt_tim, "pixelclk-active", &sprt_drv->sgrt_trig.pixelclk_active);

    if (retval)
        return -ER_NOTFOUND;

    retval = fwk_of_property_read_string(sprt_node, "xlnx,pixel-format", &format);
    if (retval || (!format))
        return -ER_NOTFOUND;

    format = kstrcat(format, 3);
    if (!format)
        sprt_var->bits_per_pixel = 32;
    else
    {
        if (!strcmp(format, "8"))
            sprt_var->bits_per_pixel = 8;
        else if (!strcmp(format, "16"))
            sprt_var->bits_per_pixel = 16;
        else if (!strcmp(format, "24"))
            sprt_var->bits_per_pixel = 32;
        else
            sprt_var->bits_per_pixel = 32;
    }

    return ER_NORMAL;
}

/*!
 * @brief   zynq7sdk_hdmi_driver_probe
 * @param   sprt_pdev
 * @retval  errno
 * @note    none
 */
static kint32_t zynq7sdk_hdmi_driver_probe(struct fwk_platdev *sprt_pdev)
{
    struct zynq7sdk_hdmi_drv *sprt_drv;
    struct fwk_device_node *sprt_node;
    struct fwk_fb_info *sprt_fb;
    void *base, *buffer;
    kuint32_t isHdmi = 0;
    kusize_t buffer_len, mem_size;
    kint32_t retval;

    sprt_node = sprt_pdev->sgrt_dev.sprt_node;
    if (!isValid(sprt_node))
        return -ER_NODEV;

    retval = fwk_of_property_read_u32(sprt_node, "is-hdmi-device", &isHdmi);
    if (retval)
        isHdmi = 1;

    if (!isHdmi) 
    {
        print_warn("it is not a hdmi device!");
        return -ER_CHECKERR;
    }

    sprt_fb = fwk_framebuffer_alloc(sizeof(*sprt_drv), &sprt_pdev->sgrt_dev);
    if (!isValid(sprt_fb))
        return -ER_NOMEM;

    base = (void *)fwk_platform_get_address(sprt_pdev, 0);
    base = fwk_io_remap(base);
    if (!isValid(base))
        goto fail1;

    sprt_drv = (struct zynq7sdk_hdmi_drv *)fwk_fb_get_drvdata(sprt_fb);
    sprt_drv->minor = ZYNQ7_SDK_HDMI_DRIVER_MINOR;
    sprt_drv->sgrt_vcfg.BaseAddress = (kuint32_t)base;
    sprt_drv->sgrt_vcfg.DeviceId = XPAR_V_TC_0_DEVICE_ID;
    sprt_drv->sprt_fb = sprt_fb;
    sprt_drv->sprt_dev = &sprt_pdev->sgrt_dev;

    fwk_platform_set_drvdata(sprt_pdev, sprt_drv);
    retval = zynq7sdk_hdmi_driver_probe_axivdma(sprt_pdev);
    if (retval)
        goto fail2;
    
    retval = zynq7sdk_hdmi_driver_probe_timings(sprt_pdev);
    if (retval)
        goto fail3;

    sprt_fb->sprt_fbops = &sgrt_fwk_fb_ops;
    sprt_fb->node = sprt_drv->minor;
    sprt_fb->sgrt_fix.smem_len = sprt_fb->sgrt_var.xres * sprt_fb->sgrt_var.yres * (sprt_fb->sgrt_var.bits_per_pixel >> 3);
    buffer_len = sprt_fb->sgrt_fix.smem_len;

    mem_size = kmget_size(GFP_DRAM);
    if (mem_size < buffer_len)
        goto fail3;

#if (defined(CONFIG_FBUFFER_NUM) && (CONFIG_FBUFFER_NUM >= 2))
    if (mem_size >= (buffer_len << 1))
        buffer_len <<= 1;
#endif

    buffer = kmalloc(buffer_len, GFP_DRAM);
    if (!isValid(buffer))
        goto fail3;

    sprt_fb->sgrt_fix.smem_start = (kuaddr_t)buffer;
    sprt_fb->sgrt_var.xoffset = sprt_fb->sgrt_var.yoffset = 0;
    sprt_fb->sgrt_var.xres_virtual = sprt_fb->sgrt_var.xres;
    sprt_fb->sgrt_var.yres_virtual = sprt_fb->sgrt_var.yres;

    sprt_fb->screen_base = (void *)sprt_fb->sgrt_fix.smem_start;
    sprt_fb->screen_size = sprt_fb->sgrt_fix.smem_len;

    /*!< choose to enable double buffer */
    if (buffer_len >= (2 * sprt_fb->sgrt_fix.smem_len))
    {
        sprt_fb->sgrt_var.yres_virtual <<= 1;
        sprt_fb->screen_size <<= 1;
    }
    
    retval = fwk_register_framebuffer(sprt_fb);
    if (retval < 0)
        goto fail3;

    print_info("register a new framebuffer (hdmi)\n");

    retval = zynq7sdk_hdmi_init(base, sprt_drv);
    if (retval)
        goto fail4;

    return ER_NORMAL;

fail4:
    fwk_unregister_framebuffer(sprt_fb);
fail3:
    zynq7sdk_hdmi_driver_remove_axivdma(sprt_pdev);
fail2:
    fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);
    fwk_io_unmap(base);
fail1:
    kfree(sprt_fb);
    return -ER_FAILD;
}

/*!
 * @brief   zynq7sdk_hdmi_driver_remove
 * @param   sprt_dev
 * @retval  errno
 * @note    none
 */
static kint32_t zynq7sdk_hdmi_driver_remove(struct fwk_platdev *sprt_pdev)
{
    struct zynq7sdk_hdmi_drv *sprt_drv;
    struct fwk_fb_info *sprt_fb;

    sprt_drv = (struct zynq7sdk_hdmi_drv *)fwk_platform_get_drvdata(sprt_pdev);
    sprt_fb = sprt_drv->sprt_fb;

    fwk_unregister_framebuffer(sprt_fb);
    zynq7sdk_hdmi_driver_remove_axivdma(sprt_pdev);
    fwk_platform_set_drvdata(sprt_pdev, mrt_nullptr);

    fwk_io_unmap(sprt_drv->base);
    kfree((void *)sprt_fb->sgrt_fix.smem_start);
    kfree(sprt_fb);
    
    return ER_NORMAL;
}

/*!< device id for device-tree */
static const struct fwk_of_device_id sgrt_zynq7sdk_hdmi_driver_ids[] =
{
    { .compatible = "xlnx, vdma, hdmi", },
    {},
};

/*!< platform instance */
static struct fwk_platdrv sgrt_zynq7sdk_hdmi_platdriver =
{
    .probe	= zynq7sdk_hdmi_driver_probe,
    .remove	= zynq7sdk_hdmi_driver_remove,
    
    .sgrt_driver =
    {
        .name 	= "xlnx, zynq7sdk, axivtc",
        .id 	= -1,
        .sprt_of_match_table = sgrt_zynq7sdk_hdmi_driver_ids,
    },
};

/*!< --------------------------------------------------------------------- */
/*!
 * @brief   zynq7sdk_hdmi_driver_init
 * @param   none
 * @retval  errno
 * @note    none
 */
kint32_t __fwk_init zynq7sdk_hdmi_driver_init(void)
{
    return fwk_register_platdriver(&sgrt_zynq7sdk_hdmi_platdriver);
}

/*!
 * @brief   zynq7sdk_hdmi_driver_exit
 * @param   none
 * @retval  none
 * @note    none
 */
void __fwk_exit zynq7sdk_hdmi_driver_exit(void)
{
    fwk_unregister_platdriver(&sgrt_zynq7sdk_hdmi_platdriver);
}

IMPORT_DRIVER_INIT(zynq7sdk_hdmi_driver_init);
IMPORT_DRIVER_EXIT(zynq7sdk_hdmi_driver_exit);

/*!< end of file */
