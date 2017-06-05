#ifndef _SERIALIZE_H
#define _SERIALIZE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"


typedef enum
{
	ser_none = 0,
	ser_nil ,
	ser_int8,
	ser_int16,
	ser_int32,
	ser_int64,
	ser_double,
	ser_string8,
	ser_string16,
	ser_string32,
	ser_bool_false,
	ser_bool_true ,
	ser_table_begin,
	ser_table_end,
	ser_table_ref,
	ser_user_data,
	ser_lightuser_data
}en_ser;


typedef enum
{
	err_none = 0,
	err_no_memory = -0xff,
	err_null_string,
	err_no_fit_serialize_data,
	err_no_fit_deserialize_data,
	err_ref_table_nofit_type,
	err_unpack_error,
	err_unpack_string8_no_size_buff,
	err_unpack_string8_no_data_buff,
	err_unpack_string16_no_size_buff,
	err_unpack_string16_no_data_buff,
	err_unpack_string32_no_size_buff,
	err_unpack_string32_no_data_buff,
	err_unpack_double_no_buff,
	err_unpack_int8_no_buff,
	err_unpack_int16_no_buff,
	err_unpack_int32_no_buff,
	err_unpack_int64_no_buff,
	err_unpack_table_no_key_buff,
	err_unpack_table_no_value_buff,
	err_unpack_table_ref_no_index,
	err_unpack_table_ref_no_table,
	err_unpack_error_op_type,
	err_unpack_userdata_no_buff,
	err_unpack_lightuserdata_no_buff
}en_error;

#define USER_DATA_FLAG 	1

typedef struct
{
	char*				data;
	size_t				size;
	size_t				max_size;
	const char*			init_buff;
	size_t				init_size;
	int					table_ref;
	int					error_info;
	int					error;
	unsigned int 		support_flag;
}serialize_data;

void					serialize_init(serialize_data* ser, unsigned int support_flag, const char* buff, size_t init_size);
void 					serialize_clean(serialize_data* ser);
int 					serialize_pack(serialize_data* ser, lua_State* L, int argStartIndex);

typedef struct
{
	const char*			data;
	size_t				size;
	int					error_info;
	int					error;
}deserialize_data;
int 					deserialize_unpack(deserialize_data* deser, lua_State* L, const char* data, size_t size, int resCount);


#ifdef __cplusplus
}
#endif

#endif