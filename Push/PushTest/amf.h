#pragma once
#include <stdint.h>

// AMF0数据类型;
typedef enum
{
	AMF_NUMBER = 0,			// 数字(double);
	AMF_BOOLEAN,			// 布尔;
	AMF_STRING,				// 字符串;
	AMF_OBJECT,				// 对象;
	AMF_MOVIECLIP,			// 保留,未使用;
	AMF_NULL,				// null;
	AMF_UNDEFINED,			// 未定义;
	AMF_REFERENCE,			// 引用;
	AMF_ECMA_ARRAY,			// 数组;
	AMF_OBJECT_END,			// 对象结束(0x09);
	AMF_STRICT_ARRAY,		// 严格的数组;
	AMF_DATE,				// 日期;
	AMF_LONG_STRING,		// 长字符串;
	AMF_UNSUPPORTED,		// 未支持;
	AMF_RECORDSET,			// 保留,未使用;
	AMF_XML_DOC,			// xml文档;
	AMF_TYPED_OBJECT,		// 有类型的对象;
	AMF_AVMPLUS,			// 需要扩展到AMF3;
	AMF_INVALID = 0xff		// 无效的;
}AMFDataType;

// AMF3数据类型;
typedef enum
{
	AMF3_UNDEFINED = 0,		// 未定义;
	AMF3_NULL,				// null;
	AMF3_FALSE,				// false;
	AMF3_TRUE,				// true;
	AMF3_INTEGER,			// 数字int;
	AMF3_DOUBLE,			// double;
	AMF3_STRING,			// 字符串;
	AMF3_XML_DOC,			// xml文档;
	AMF3_DATE,				// 日期;
	AMF3_ARRAY,				// 数组;
	AMF3_OBJECT,			// 对象;
	AMF3_XML,				// xml;
	AMF3_BYTE_ARRAY			// 字节数组;
} AMF3DataType;

// AMF自定义的字符串;
typedef struct AVal
{
	char *av_val;
	int av_len;
} AVal;

// AVal的快速初始化;
#define AVC(str)		{str, sizeof(str)-1}
// 比较AVal字符串;
#define AVMATCH(a1,a2)	((a1)->av_len == (a2)->av_len && !memcmp((a1)->av_val,(a2)->av_val,(a1)->av_len))

struct AMFObjectProperty;

// AMF对象, 就是由一系列的属性构成的;
typedef struct AMFObject
{
	int o_num;							// 属性数目;
	struct AMFObjectProperty *o_props;	// 属性数组;
} AMFObject;

// AMF对象的属性;
typedef struct AMFObjectProperty
{
	AVal p_name;			// 属性名称;
	AMFDataType p_type;		// 属性类型;
	union
	{
		double p_number;
		AVal p_aval;
		AMFObject p_object;
	} p_vu;					// 属性数值;
	int16_t p_UTCoffset;	// UTC偏移;
} AMFObjectProperty;