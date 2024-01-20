/*
 * CO_Object.h
 *
 *  Created on: Jun 25, 2019
 *      Author: quangnd
 */

#ifndef CO_OBJECT_H_
#define CO_OBJECT_H_

#include "stdint.h"
#include "CO_utils.h"

/**
 * Attributes (bit masks) for OD sub-object.
 */
#if 1
typedef enum {
    ODA_SDO_R = 0x01, /**< SDO server may read from the variable */
    ODA_SDO_W = 0x02, /**< SDO server may write to the variable */
    ODA_SDO_RW = 0x03, /**< SDO server may read from or write to the variable */
    ODA_TPDO = 0x04, /**< Variable is mappable into TPDO (can be read) */
    ODA_RPDO = 0x08, /**< Variable is mappable into RPDO (can be written) */
    ODA_TRPDO = 0x0C, /**< Variable is mappable into TPDO or RPDO */
    ODA_TSRDO = 0x10, /**< Variable is mappable into transmitting SRDO */
    ODA_RSRDO = 0x20, /**< Variable is mappable into receiving SRDO */
    ODA_TRSRDO = 0x30, /**< Variable is mappable into tx or rx SRDO */
    ODA_MB = 0x40, /**< Variable is multi-byte ((u)int16_t to (u)int64_t) */
    ODA_STR = 0x80 /**< Shorter value, than specified variable size, may be
    written to the variable. SDO write will fill remaining memory with zeroes.
    Attribute is used for VISIBLE_STRING and UNICODE_STRING. */
} CO_Sub_Object_Attr_t;
#else

typedef enum {
    CO_ATTR_RO	= 0x01,
    CO_ATTR_WO	= 0x02,
    CO_ATTR_RW	= 0x03,
} CO_Sub_Object_Attr_t;

#endif

typedef struct CO_Object_t CO_Object;
typedef struct CO_Sub_Object_t CO_Sub_Object;

typedef enum {
    CO_EXT_CONFIRM_processing	= 0x01,
    CO_EXT_CONFIRM_abort		= 0x02,
    CO_EXT_CONFIRM_success		= 0x03,
} CO_Sub_Object_Ext_Confirm_Func_t;

typedef struct
{
    uint8_t* 									p_shadow_data;				//<< Buffer stores data for processing if necessary
    CO_Sub_Object_Ext_Confirm_Func_t 			(*confirm_func) (void);		//<< Last confirmation function when data has been saved in
    											// the case the device is server + download mode or client + upload mode
} CO_Sub_Object_Ext_t;	/* Extension of Sub-object*/

struct CO_Object_t
{
	uint16_t		index;				//<< Object Dictionary index
	uint8_t			sub_number;			//<< Number of all sub-object, including sub-object at sub-index 0
	CO_Sub_Object*	subs;
};

struct CO_Sub_Object_t
{
	void*			p_data;
	uint8_t 		attr;				//<< see CO_Sub_Object_Attr_t
	uint16_t 		len;
	CO_Sub_Object_Ext_t* p_ext;
};

static inline void CO_SUB_OBJ_write(CO_Object *p_obj, const uint8_t sub_index, const void *data,
		const uint8_t len)
{
	uint8_t *src = (uint8_t*) p_obj->subs[sub_index].p_data;
	uint8_t *dst = (uint8_t*) data;
	CO_memcpy(dst, src, len);
}

static inline void CO_SUB_OBJ_read(CO_Object *p_obj, const uint8_t sub_index, void *data,
		const uint8_t len)
{

	uint8_t *src = (uint8_t*) p_obj->subs[sub_index].p_data;
	uint8_t *dst = (uint8_t*) data;
	CO_memcpy(dst, src, len);
}

static inline void* CO_SUB_OBJ_get_data_pointer(CO_Object *p_obj, const uint8_t sub_index)
{
	return p_obj->subs[sub_index].p_data;
}

static inline uint16_t CO_SUB_OBJ_get_data_len(CO_Object *p_obj, const uint8_t sub_index)
{
	return p_obj->subs[sub_index].len;
}

static inline uint8_t CO_SUB_OBJ_get_sub_attr(CO_Object *p_obj, const uint8_t sub_index)
{
	return p_obj->subs[sub_index].attr;
}


#endif /* CO_OBJECT_H_ */
