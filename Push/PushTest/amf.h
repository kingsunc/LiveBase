#pragma once
#include <stdint.h>

// AMF0��������;
typedef enum
{
	AMF_NUMBER = 0,			// ����(double);
	AMF_BOOLEAN,			// ����;
	AMF_STRING,				// �ַ���;
	AMF_OBJECT,				// ����;
	AMF_MOVIECLIP,			// ����,δʹ��;
	AMF_NULL,				// null;
	AMF_UNDEFINED,			// δ����;
	AMF_REFERENCE,			// ����;
	AMF_ECMA_ARRAY,			// ����;
	AMF_OBJECT_END,			// �������(0x09);
	AMF_STRICT_ARRAY,		// �ϸ������;
	AMF_DATE,				// ����;
	AMF_LONG_STRING,		// ���ַ���;
	AMF_UNSUPPORTED,		// δ֧��;
	AMF_RECORDSET,			// ����,δʹ��;
	AMF_XML_DOC,			// xml�ĵ�;
	AMF_TYPED_OBJECT,		// �����͵Ķ���;
	AMF_AVMPLUS,			// ��Ҫ��չ��AMF3;
	AMF_INVALID = 0xff		// ��Ч��;
}AMFDataType;

// AMF3��������;
typedef enum
{
	AMF3_UNDEFINED = 0,		// δ����;
	AMF3_NULL,				// null;
	AMF3_FALSE,				// false;
	AMF3_TRUE,				// true;
	AMF3_INTEGER,			// ����int;
	AMF3_DOUBLE,			// double;
	AMF3_STRING,			// �ַ���;
	AMF3_XML_DOC,			// xml�ĵ�;
	AMF3_DATE,				// ����;
	AMF3_ARRAY,				// ����;
	AMF3_OBJECT,			// ����;
	AMF3_XML,				// xml;
	AMF3_BYTE_ARRAY			// �ֽ�����;
} AMF3DataType;

// AMF�Զ�����ַ���;
typedef struct AVal
{
	char *av_val;
	int av_len;
} AVal;

// AVal�Ŀ��ٳ�ʼ��;
#define AVC(str)		{str, sizeof(str)-1}
// �Ƚ�AVal�ַ���;
#define AVMATCH(a1,a2)	((a1)->av_len == (a2)->av_len && !memcmp((a1)->av_val,(a2)->av_val,(a1)->av_len))

struct AMFObjectProperty;

// AMF����, ������һϵ�е����Թ��ɵ�;
typedef struct AMFObject
{
	int o_num;							// ������Ŀ;
	struct AMFObjectProperty *o_props;	// ��������;
} AMFObject;

// AMF���������;
typedef struct AMFObjectProperty
{
	AVal p_name;			// ��������;
	AMFDataType p_type;		// ��������;
	union
	{
		double p_number;
		AVal p_aval;
		AMFObject p_object;
	} p_vu;					// ������ֵ;
	int16_t p_UTCoffset;	// UTCƫ��;
} AMFObjectProperty;