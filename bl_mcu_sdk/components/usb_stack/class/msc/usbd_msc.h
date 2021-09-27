/**
 * @file
 * @brief USB Mass Storage Class public header
 *
 * Header follows the Mass Storage Class Specification
 * (Mass_Storage_Specification_Overview_v1.4_2-19-2010.pdf) and
 * Mass Storage Class Bulk-Only Transport Specification
 * (usbmassbulk_10.pdf).
 * Header is limited to Bulk-Only Transfer protocol.
 */

#ifndef _USBD_MSC_H__
#define _USBD_MSC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* MSC Subclass Codes */
#define MSC_SUBCLASS_RBC           0x01
#define MSC_SUBCLASS_SFF8020I_MMC2 0x02
#define MSC_SUBCLASS_QIC157        0x03
#define MSC_SUBCLASS_UFI           0x04
#define MSC_SUBCLASS_SFF8070I      0x05
#define MSC_SUBCLASS_SCSI          0x06

/* MSC Protocol Codes */
#define MSC_PROTOCOL_CBI_INT   0x00
#define MSC_PROTOCOL_CBI_NOINT 0x01
#define MSC_PROTOCOL_BULK_ONLY 0x50

/* MSC Request Codes */
#define MSC_REQUEST_RESET       0xFF
#define MSC_REQUEST_GET_MAX_LUN 0xFE

/** MSC Command Block Wrapper (CBW) Signature */
#define MSC_CBW_Signature 0x43425355
/** Bulk-only Command Status Wrapper (CSW) Signature */
#define MSC_CSW_Signature 0x53425355

/** MSC Command Block Status Values */
#define CSW_STATUS_CMD_PASSED  0x00
#define CSW_STATUS_CMD_FAILED  0x01
#define CSW_STATUS_PHASE_ERROR 0x02

/** MSC Bulk-Only Command Block Wrapper (CBW) */
struct CBW {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataLength;
    uint8_t Flags;
    uint8_t LUN;
    uint8_t CBLength;
    uint8_t CB[16];
} __packed;

/** MSC Bulk-Only Command Status Wrapper (CSW) */
struct CSW {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataResidue;
    uint8_t Status;
} __packed;

/*Length of template descriptor: 23 bytes*/
#define MSC_DESCRIPTOR_LEN (9 + 7 + 7)
// clang-format off
#define MSC_DESCRIPTOR_INIT(bFirstInterface, out_ep, in_ep,str_idx) \
    /* Interface */                                                  \
    0x09,                          /* bLength */                 \
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType */         \
    bFirstInterface,               /* bInterfaceNumber */        \
    0x00,                          /* bAlternateSetting */       \
    0x02,                          /* bNumEndpoints */           \
    USB_DEVICE_CLASS_MASS_STORAGE, /* bInterfaceClass */         \
    MSC_SUBCLASS_SCSI,             /* bInterfaceSubClass */      \
    MSC_PROTOCOL_BULK_ONLY,        /* bInterfaceProtocol */      \
    str_idx, /* iInterface */      /* Endpoint Out */            \
    0x07,                          /* bLength */                 \
    USB_DESCRIPTOR_TYPE_ENDPOINT,  /* bDescriptorType */         \
    out_ep,                        /* bEndpointAddress */        \
    0x02,                          /* bmAttributes */            \
    0x40, 0x00,                    /* wMaxPacketSize */          \
    0x01, /* bInterval */          /* Endpoint In */             \
    0x07,                          /* bLength */                 \
    USB_DESCRIPTOR_TYPE_ENDPOINT,  /* bDescriptorType */         \
    in_ep,                         /* bEndpointAddress */        \
    0x02,                          /* bmAttributes */            \
    0x40, 0x00,                    /* wMaxPacketSize */          \
    0x01                           /* bInterval */

#define MSC_HS_DESCRIPTOR_INIT(bFirstInterface, out_ep, in_ep,str_idx) \
    /* Interface */                                                  \
    0x09,                          /* bLength */                 \
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType */         \
    bFirstInterface,               /* bInterfaceNumber */        \
    0x00,                          /* bAlternateSetting */       \
    0x02,                          /* bNumEndpoints */           \
    USB_DEVICE_CLASS_MASS_STORAGE, /* bInterfaceClass */         \
    MSC_SUBCLASS_SCSI,             /* bInterfaceSubClass */      \
    MSC_PROTOCOL_BULK_ONLY,        /* bInterfaceProtocol */      \
    str_idx, /* iInterface */      /* Endpoint Out */            \
    0x07,                          /* bLength */                 \
    USB_DESCRIPTOR_TYPE_ENDPOINT,  /* bDescriptorType */         \
    out_ep,                        /* bEndpointAddress */        \
    0x02,                          /* bmAttributes */            \
    0x02, 0x00,                    /* wMaxPacketSize */          \
    0x01, /* bInterval */          /* Endpoint In */             \
    0x07,                          /* bLength */                 \
    USB_DESCRIPTOR_TYPE_ENDPOINT,  /* bDescriptorType */         \
    in_ep,                         /* bEndpointAddress */        \
    0x02,                          /* bmAttributes */            \
    0x02, 0x00,                    /* wMaxPacketSize */          \
    0x01                           /* bInterval */
// clang-format on

void usbd_msc_class_init(uint8_t out_ep, uint8_t in_ep);
void usbd_msc_get_cap(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
int usbd_msc_sector_read(uint32_t sector, uint8_t *buffer, uint32_t length);
int usbd_msc_sector_write(uint32_t sector, uint8_t *buffer, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* USB_MSC_H_ */
