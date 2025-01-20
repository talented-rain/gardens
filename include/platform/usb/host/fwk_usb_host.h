/*
 * Hardware Abstraction Layer USB HOST Interface
 *
 * File Name:   fwk_usb_host.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.12.17
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_USB_HOST_H_
#define __FWK_USB_HOST_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */
enum __ERT_USB_HOST_INDEX
{
    /*!< KHCI 0U */
    NR_USB_HostIndexKhci0 = 0U,       
    /*!< KHCI 1U, Currently, there are no platforms which have two KHCI IPs, this is reserved to be used in the future. */                        
    NR_USB_HostIndexKhci1 = 1U,                               
    /*!< EHCI 0U */
    NR_USB_HostIndexEhci0 = 2U,       
    /*!< EHCI 1U, Currently, there are no platforms which have two EHCI IPs, this is reserved to be used in the future. */                        
    NR_USB_HostIndexEhci1 = 3U,                               

    /*!< LPC USB IP3511 FS controller 0 */
    NR_USB_HostIndexLpcIp3511Fs0 = 4U,           
    /*!< LPC USB IP3511 FS controller 1, there are no platforms which have two IP3511 IPs, this is reserved to be used in the future. */             
    NR_USB_HostIndexLpcIp3511Fs1 = 5U,                        

    /*!< LPC USB IP3511 HS controller 0 */
    NR_USB_HostIndexLpcIp3511Hs0 = 6U,            
    /*!< LPC USB IP3511 HS controller 1, there are no platforms which have two IP3511 IPs, this is reserved to be used in the future. */            
    NR_USB_HostIndexLpcIp3511Hs1 = 7U,                        

    /*!< OHCI 0U */
    NR_USB_HostIndexOhci0 = 8U,        
    /*!< OHCI 1U, Currently, there are no platforms which have two OHCI IPs, this is reserved to be used in the future. */                       
    NR_USB_HostIndexOhci1 = 9U,                               

    /*!< IP3516HS 0U */
    NR_USB_HostIndexIp3516Hs0 = 10U,         
    /*!< IP3516HS 1U, Currently, there are no platforms which have two IP3516HS IPs, this is reserved to be used in the future. */                 
    NR_USB_HostIndexIp3516Hs1 = 11U,                          

};

typedef struct fwk_usb_host_if
{
    kint32_t (*create) (kuint32_t index);
    kint32_t (*destroy) (kuint32_t index);
    kint32_t (*open) (kuint32_t index);
    kint32_t (*close) (kuint32_t index);
    kint32_t (*send) (kuint32_t index);
    kint32_t (*recv) (kuint32_t index);

} srt_fwk_usb_host_if_t;

#ifdef __cplusplus
    }
#endif

#endif /*!< __FWK_USB_HOST_H_ */
