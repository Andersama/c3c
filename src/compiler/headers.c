// Copyright (c) 2020 Christoffer Lerno. All rights reserved.
// Use of this source code is governed by a LGPLv3.0
// a copy of which can be found in the LICENSE file.

#include "compiler_internal.h"
#include "base_n.h"

#define OUTPUT(x, ...) fprintf(file, x, ## __VA_ARGS__)
#define INDENT() indent_line(file, indent)

static void indent_line(FILE *file, int indent)
{
	for (int i = 0; i < indent * 3; i++)
	{
		fputc(' ', file);
	}
}
static void header_gen_decl(FILE *file, int indent, Decl *decl);

static void header_gen_method(FILE *file, SemaContext *c, Decl *decl)
{
	fprintf(file, "/* method */\n");
}

static void header_gen_function(FILE *file, SemaContext *c, Decl *decl)
{
	fprintf(file, "/* function */\n");
}
#if 0
static void header_print_type(FILE *file, Type *type)
{
	RETRY:
	if (type == NULL) {
		printf("Printing null type?\n");
		return;
	}
	printf("header_print_type handling: %i\n", type->type_kind);
	switch (type->type_kind)
	{
		case CT_TYPES:
			UNREACHABLE
		case TYPE_BITSTRUCT:
			TODO
		case TYPE_FAILABLE:
		case TYPE_FAILABLE_ANY:
			// If this is reachable then we are not doing the proper lowering.
			UNREACHABLE
		case TYPE_VOID:
			OUTPUT("void");
			return;
		case TYPE_BOOL:
			OUTPUT("bool");
			return;
		case TYPE_I8:
			OUTPUT("int8_t");
			return;
		case TYPE_I16:
			OUTPUT("int16_t");
			return;
		case TYPE_I32:
			OUTPUT("int32_t");
			return;
		case TYPE_I64:
			OUTPUT("int64_t");
			return;
		case TYPE_I128:
			OUTPUT("__int128");
			return;
		case TYPE_U8:
			OUTPUT("uint8_t");
			return;
		case TYPE_U16:
			OUTPUT("uint16_t");
			return;
		case TYPE_U32:
			OUTPUT("uint32_t");
			return;
		case TYPE_U64:
			OUTPUT("uint64_t");
			return;
		case TYPE_U128:
			OUTPUT("unsigned __int128");
			return;
		case TYPE_F16:
			OUTPUT("__fp16");
			return;
		case TYPE_F32:
			OUTPUT("float");
			return;
		case TYPE_F64:
			OUTPUT("double");
			return;
		case TYPE_F128:
			OUTPUT("__float128");
			return;
		case TYPE_TYPEID:
			OUTPUT("c3typeid_t");
			return;
		case TYPE_POINTER:
			header_print_type(file, type->pointer);
			OUTPUT("*");
			return;
		case TYPE_ENUM:
		case TYPE_FAULTTYPE:
			OUTPUT("enum %s__", type->decl->extname);
			return;
		case TYPE_FLEXIBLE_ARRAY:
			header_print_type(file, type->array.base);
			// we need to print []'s in later functions?
			return;
		case TYPE_FUNC:
			/*
			// cross fingers?
			// OUTPUT("%s", type->decl->name);
			// void(type1, type2, type3...etc);
			header_print_type(file, type->func.prototype->rtype);

			OUTPUT("(*)(");
			unsigned vecsize = vec_size(type->func.prototype->params);
			Type** params = type->func.prototype->params;
			if (vecsize > 0) {
				Type* param = params[0];
				header_print_type(file, param);
			}
			for (unsigned i = 1; i < vecsize; i++)
			{
				OUTPUT(", ");
				Type* param = params[i];
				header_print_type(file, param);
			}
			OUTPUT(")");
			*/
			return;
			//TODO
		case TYPE_STRUCT:
			OUTPUT("struct %s__", type->decl->extname);
			return;
		case TYPE_UNION:
			OUTPUT("union %s__", type->decl->extname);
			return;
		case TYPE_DISTINCT:
			type = type->decl->distinct_decl.base_type;
			goto RETRY;
		case TYPE_ANYERR:
			break;
		case TYPE_TYPEDEF:
			/*
			if (type == type_usize) {
				OUTPUT("size_t");
				return;
			}
			if (type == type_iptrdiff)
			{
				OUTPUT("ptrdiff_t");
				return;
			}
			*/
			OUTPUT("%s__", type->decl->extname);
			return;
			//type = type->canonical;
			//goto RETRY;
		case TYPE_ARRAY:
			break;
		case TYPE_ANY:
			TODO
		case TYPE_SUBARRAY:
			header_print_type(file, type->array.base);
			// we need to print []'s in later functions?
			return;
			//break;
		case TYPE_VECTOR:
			break;
		/*
		case TYPE_TYPEDEF:
			type = type->canonical;
			goto RETRY;
		case TYPE_DISTINCT:
			type = type->decl->distinct_decl.base_type;
			goto RETRY;
		*/
	}
	printf("header_print_type: %i\n", (int)type->type_kind);
	TODO
}

static void header_print_named_type(FILE* file, Type* type, const char *name) {
RETRY:
	if (type == NULL) {
		printf("Printing null type?\n");
		return;
	}
	printf("header_print_type handling: %i\n", type->type_kind);
	switch (type->type_kind)
	{
	case CT_TYPES:
		UNREACHABLE
	case TYPE_BITSTRUCT:
		TODO
	case TYPE_FAILABLE:
	case TYPE_FAILABLE_ANY:
		// If this is reachable then we are not doing the proper lowering.
		UNREACHABLE
	case TYPE_VOID:
		OUTPUT("void");
		OUTPUT(" %s", name);
		return;
	case TYPE_BOOL:
		OUTPUT("bool");
		OUTPUT(" %s", name);
		return;
	case TYPE_I8:
		OUTPUT("int8_t");
		OUTPUT(" %s", name);
		return;
	case TYPE_I16:
		OUTPUT("int16_t");
		OUTPUT(" %s", name);
		return;
	case TYPE_I32:
		OUTPUT("int32_t");
		OUTPUT(" %s", name);
		return;
	case TYPE_I64:
		OUTPUT("int64_t");
		OUTPUT(" %s", name);
		return;
	case TYPE_I128:
		OUTPUT("__int128");
		OUTPUT(" %s", name);
		return;
	case TYPE_U8:
		OUTPUT("uint8_t");
		OUTPUT(" %s", name);
		return;
	case TYPE_U16:
		OUTPUT("uint16_t");
		OUTPUT(" %s", name);
		return;
	case TYPE_U32:
		OUTPUT("uint32_t");
		OUTPUT(" %s", name);
		return;
	case TYPE_U64:
		OUTPUT("uint64_t");
		OUTPUT(" %s", name);
		return;
	case TYPE_U128:
		OUTPUT("unsigned __int128");
		OUTPUT(" %s", name);
		return;
	case TYPE_F16:
		OUTPUT("__fp16");
		OUTPUT(" %s", name);
		return;
	case TYPE_F32:
		OUTPUT("float");
		OUTPUT(" %s", name);
		return;
	case TYPE_F64:
		OUTPUT("double");
		OUTPUT(" %s", name);
		return;
	case TYPE_F128:
		OUTPUT("__float128");
		OUTPUT(" %s", name);
		return;
	case TYPE_TYPEID:
		OUTPUT("c3typeid_t");
		OUTPUT(" %s", name);
		return;
	case TYPE_POINTER:
		header_print_named_type(file, type->pointer, "");
		OUTPUT("* %s", name);
		return;
	case TYPE_ENUM:
	case TYPE_FAULTTYPE:
		OUTPUT("enum %s__", type->decl->extname);
		return;
	case TYPE_FLEXIBLE_ARRAY:
		header_print_named_type(file, type->array.base, name);
		// we need to print []'s in later functions?
		return;
	case TYPE_FUNC:
		// cross fingers?
		// OUTPUT("%s", type->decl->name);
		// void(type1, type2, type3...etc);
		header_print_named_type(file, type->func.prototype->rtype, "");

		OUTPUT("(*%s)(", name);
		unsigned vecsize = vec_size(type->func.prototype->params);
		Type** params = type->func.prototype->params;
		if (vecsize > 0) {
			Type* param = params[0];
			header_print_named_type(file, param, "");
		}
		for (unsigned i = 1; i < vecsize; i++)
		{
			OUTPUT(", ");
			Type* param = params[i];
			header_print_named_type(file, param, "");
		}
		OUTPUT(")");
		return;
		//TODO
	case TYPE_STRUCT:
		OUTPUT("struct %s__", type->decl->extname);
		return;
	case TYPE_UNION:
		OUTPUT("union %s__", type->decl->extname);
		return;
	case TYPE_DISTINCT:
		type = type->decl->distinct_decl.base_type;
		goto RETRY;
	case TYPE_ANYERR:
		break;
	case TYPE_TYPEDEF:
		/*
		if (type == type_usize) {
			OUTPUT("size_t");
			return;
		}
		if (type == type_iptrdiff)
		{
			OUTPUT("ptrdiff_t");
			return;
		}
		if (type == type_uptrdiff) {
			OUTPUT("unsigned ptrdiff_t");
			return;
		}
		*/
		type = type->canonical;
		goto RETRY;
		
		//OUTPUT("%s__", type->decl->extname);
		return;
	case TYPE_ARRAY:
		header_print_type(file, type->array.base);
		// we need to print []'s in later functions?
		return;
		break;
	case TYPE_ANY:
		TODO
	case TYPE_SUBARRAY:
		header_print_type(file, type->array.base);
		// we need to print []'s in later functions?
		return;
		//break;
	case TYPE_VECTOR:
		break;
		/*
		case TYPE_TYPEDEF:
			type = type->canonical;
			goto RETRY;
		case TYPE_DISTINCT:
			type = type->decl->distinct_decl.base_type;
			goto RETRY;
		*/
	}
	printf("header_print_named_type: %i\n", (int)type->type_kind);
	TODO
}
#endif

static void c_type_append_func_to_scratch(FunctionPrototype* prototype, const char* name);
static void c_type_append_name_to_scratch(Type* type, const char* name);

static void c_type_append_func_to_scratch(FunctionPrototype* prototype, const char* name)
{
	c_type_append_name_to_scratch(prototype->rtype, "");
	scratch_buffer_append("(*");
	if (name && *name) {
		scratch_buffer_append(name);
	}
	scratch_buffer_append_char(')');
	scratch_buffer_append_char('(');
	unsigned elements = vec_size(prototype->params);
	if (elements > 0) {
		c_type_append_name_to_scratch(prototype->params[0], "");
	}
	for (unsigned i = 0; i < elements; i++)
	{
		//scratch_buffer_append_char(',');
		scratch_buffer_append(", ");
		//type_append_name_to_scratch(prototype->params[i]);
		c_type_append_name_to_scratch(prototype->params[i], "");
	}
	if (prototype->variadic == VARIADIC_RAW && elements > 0)
	{
		scratch_buffer_append_char(',');
	}
	if (prototype->variadic != VARIADIC_NONE)
	{
		scratch_buffer_append("...");
	}
	scratch_buffer_append_char(')');
}

static const char base62_alphabet_extended[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_[]*&:";

static void c_type_append_name_to_scratch(Type* type, const char *name)
{
	printf("c_type_append_name_to_scratch: %s\n", name);
	type = type->canonical;
	RETRY:
	switch (type->type_kind)
	{
	case CT_TYPES:
		UNREACHABLE
	case TYPE_BITSTRUCT:
		TODO
	//case TYPE_FAILABLE:
	//case TYPE_FAILABLE_ANY:
		// If this is reachable then we are not doing the proper lowering.
	//	UNREACHABLE
	case TYPE_TYPEDEF:
		type = type->canonical;
		goto RETRY;
		//break;
	case TYPE_FAULTTYPE:
		TODO;
		break;
	case TYPE_STRUCT:
		scratch_buffer_append("struct ");
		scratch_buffer_append(type->decl->extname);
		scratch_buffer_append("__");
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		//OUTPUT("struct %s__", type->decl->extname);
		return;
	case TYPE_UNION:
		scratch_buffer_append("union ");
		scratch_buffer_append(type->decl->extname);
		scratch_buffer_append("__");
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		//OUTPUT("union %s__", type->decl->extname);
		return;
	case TYPE_ENUM:
		scratch_buffer_append("enum ");
		scratch_buffer_append(type->decl->extname);
		scratch_buffer_append("__");
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_DISTINCT:
		//scratch_buffer_append(type->decl->name);
		type = type->decl->distinct_decl.base_type;
		goto RETRY;
		//break;
	/*
	case TYPE_BITSTRUCT:
		scratch_buffer_append(type->decl->name);
		break;
		*/
	case TYPE_POINTER:
		// looking at a func pointer (should probably dig through until we hit this)
		if (type->pointer->type_kind == TYPE_FUNC) {
			type = type->pointer;
			goto RETRY;
		}
		c_type_append_name_to_scratch(type->pointer, "");
		scratch_buffer_char('*');

		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_FAILABLE_ANY:
		scratch_buffer_append("void!");
		return;
	case TYPE_FAILABLE:
		//TODO: I've no clue how this is supposed to work (presumably this is impossible to reach)
		if (type->failable)
		{
			c_type_append_name_to_scratch(type->failable, "");
		}
		else
		{
			scratch_buffer_append("void");
		}
		scratch_buffer_append_char('!');
		return;
	case TYPE_SUBARRAY: {
		//char_is_base64()
		//type->array.base->name;
		//c_type_append_name_to_scratch(type->array.base, name);
		//scratch_buffer_append("[]");
		scratch_buffer_append("struct __subarray_");

		//write out the base type (w/o a name)
		uint32_t c3name_start = scratch_buffer.len;
		c_type_append_name_to_scratch(type->array.base, "");
		uint32_t c3name_end = scratch_buffer.len;
		scratch_buffer_append_char('\0');
		uint32_t base62_start = scratch_buffer.len;
		struct encode_result r = base62_5_encode(&scratch_buffer.str[scratch_buffer.len], (0xffff - (c3name_end)), &scratch_buffer.str[c3name_start], c3name_end - c3name_start, base62_alphabet_extended);

		//move the __subarray_base64 portion to where c3name starts
		memcpy(&scratch_buffer.str[c3name_start], &scratch_buffer.str[base62_start], r.written_chars); //(array_name_end - array_name_start)
		scratch_buffer.len = c3name_start + r.written_chars;

		//write the name
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}

	}
		/*
		scratch_buffer_append_char('[');
		scratch_buffer_append_signed_int(type->array.len);
		scratch_buffer_append_char(']');
		*/
		/*
		uint32_t c3name_end = scratch_buffer.len;
		scratch_buffer_append_char('\0');

		int count = base64_encode(&scratch_buffer.str[c3name_start], c3name_end - c3name_end, &scratch_buffer.str[scratch_buffer.len]);
		scratch_buffer.len += count;
		*/


		return;
	case TYPE_FLEXIBLE_ARRAY: {
		//c_type_append_name_to_scratch(type->array.base, name);
		//scratch_buffer_append("[*]");
		
		//prepare "struct "
		/*
		scratch_buffer_append("struct ");
		//write out the type
		uint32_t c3name_start = scratch_buffer.len;
		c_type_append_name_to_scratch(type->array.base, "");
		uint32_t c3name_end = scratch_buffer.len;
		scratch_buffer_append_char('\0');

		//convert the type to it's struct's name
		uint32_t array_name_start = scratch_buffer.len;
		scratch_buffer_append("__flexarray_");
		struct encode_result r = base62_5_encode(&scratch_buffer.str[scratch_buffer.len], (0xffff - (c3name_end)), &scratch_buffer.str[c3name_start], c3name_end - c3name_start, base62_alphabet_extended);
		//int count = base64_encode(&scratch_buffer.str[c3name_start], c3name_end - c3name_start, &scratch_buffer.str[scratch_buffer.len]);
		scratch_buffer.len += r.written_chars;
		uint32_t array_name_end = scratch_buffer.len;
		
		//move the __subarray_base64 portion to where c3name starts
		memcpy(&scratch_buffer.str[c3name_start], &scratch_buffer.str[array_name_start], (array_name_end - array_name_start));
		//scratch buffer "back" to normal
		scratch_buffer.len = c3name_start + (array_name_end - array_name_start);

		scratch_buffer_append_char(' ');
		*/
		scratch_buffer_append("struct __flexarray_");
		
		//write out the base type (w/o a name)
		uint32_t c3name_start = scratch_buffer.len;
		c_type_append_name_to_scratch(type->array.base, "");
		uint32_t c3name_end = scratch_buffer.len;
		scratch_buffer_append_char('\0');
		uint32_t base62_start = scratch_buffer.len;
		struct encode_result r = base62_5_encode(&scratch_buffer.str[scratch_buffer.len], (0xffff - (c3name_end)), &scratch_buffer.str[c3name_start], c3name_end - c3name_start, base62_alphabet_extended);

		//move the __subarray_base64 portion to where c3name starts
		memcpy(&scratch_buffer.str[c3name_start], &scratch_buffer.str[base62_start], r.written_chars); //(array_name_end - array_name_start)
		scratch_buffer.len = c3name_start + r.written_chars;

		//write the name
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}

	}
		return;
	case TYPE_VOID:
		scratch_buffer_append("void");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_BOOL:
		scratch_buffer_append("bool");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_I8:
		scratch_buffer_append("int8_t");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_I16:
		scratch_buffer_append("int16_t");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_I32:
		scratch_buffer_append("int32_t");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_I64:
		scratch_buffer_append("int64_t");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_I128:
		scratch_buffer_append("__int128");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_U8:
		scratch_buffer_append("uint8_t");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_U16:
		scratch_buffer_append("uint16_t");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_U32:
		scratch_buffer_append("uint32_t");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_U64:
		scratch_buffer_append("uint64_t");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_U128:
		scratch_buffer_append("unsigned __int128");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_F16:
		scratch_buffer_append("__fp16");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_F32:
		scratch_buffer_append("float");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_F64:
		scratch_buffer_append("double");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_F128:
		scratch_buffer_append("__float128");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_TYPEID:
		scratch_buffer_append("c3typeid_t");
		scratch_buffer_append_char(' ');
		scratch_buffer_append(name);
		return;
	case TYPE_ANYERR:
	case TYPE_ANY:
	case TYPE_VECTOR:
		scratch_buffer_append(type->name);
		return;
	/*
	case TYPE_UNTYPED_LIST:
	case TYPE_INFERRED_ARRAY:
	case TYPE_TYPEINFO:
		UNREACHABLE
			break;
			*/
	case TYPE_FUNC:
		c_type_append_func_to_scratch(type->func.prototype, name);
		return;
	case TYPE_ARRAY:
		//uint32_t c3name_start = scratch_buffer.len;
		c_type_append_name_to_scratch(type->array.base, name);
		scratch_buffer_append_char('[');
		scratch_buffer_append_signed_int(type->array.len);
		scratch_buffer_append_char(']');
		/*
		uint32_t c3name_end = scratch_buffer.len;
		scratch_buffer_append_char('\0');

		int count = base64_encode(&scratch_buffer.str[c3name_start], c3name_end - c3name_end, &scratch_buffer.str[scratch_buffer.len]);
		scratch_buffer.len += count;
		*/
		return;
	}

	UNREACHABLE
}

static void header_gen_members(FILE *file, int indent, Decl **members)
{
	VECEACH(members, i)
	{
		Decl *member = members[i];
		if (member->decl_kind == DECL_VAR)
		{
			printf("adding member: %s\n", member->name);
			INDENT();
			//header_print_named_type(file, member->type, member->name);
			scratch_buffer_clear();
			c_type_append_name_to_scratch(member->type, member->name);

			scratch_buffer.str[scratch_buffer.len] = '\0';
			printf("%.*s\n", scratch_buffer.len, &scratch_buffer.str[0]);
			OUTPUT("%.*s", scratch_buffer.len, &scratch_buffer.str[0]); //scratch_buffer.len,
			OUTPUT(";\n");
			/*
			if (is_obvious_type(member->type)) {
				header_print_obvious_type(file, member->type);
				OUTPUT(" %s;\n", member->name);
			} else {
				//didn't know what to do?
				OUTPUT("//%s %s\n", member->type->name, member->name);
			}
			*/
			//header_print_type(file, member->type);
			//OUTPUT(" %s;\n", member->name);
		}
		//TODO
	}
}
static void header_gen_struct(FILE *file, int indent, Decl *decl)
{
	if (!indent)
	{
		OUTPUT("typedef struct %s__ %s;\n", decl->extname, decl->extname);
	}
	printf("generating: %s\n", decl->name);
	INDENT();
	if (decl->name)
	{
		OUTPUT("struct %s__\n{\n", decl->extname);
	}
	else
	{
		OUTPUT("struct\n{\n");
	}
	header_gen_members(file, indent + 1, decl->strukt.members);
	INDENT();
	OUTPUT("};\n");
}

static void header_gen_union(FILE *file, int indent, Decl *decl)
{
	OUTPUT("typedef union %s__ %s;\n", decl->extname, decl->extname);
	OUTPUT("union %s__\n{\n", decl->extname);
	header_gen_members(file, indent, decl->strukt.members);
	OUTPUT("};\n");
}

static void header_gen_enum(FILE *file, int indent, Decl *decl)
{
	//TODO
	OUTPUT("typedef enum %s__ %s;\n", decl->extname, decl->extname);
	OUTPUT("enum %s__\n{\n", decl->extname);
	header_gen_members(file, indent, decl->strukt.members);
	OUTPUT("};\n");
}

static void header_gen_err(FILE *file, int indent, Decl *decl)
{
	OUTPUT("typedef struct %s_error__ %s_error;\n", decl->extname, decl->extname);
	OUTPUT("struct %s_error__\n{\n", decl->extname);
	header_gen_members(file, indent, decl->strukt.members);
	OUTPUT("};\n");
}

static void header_gen_typedef(FILE* file, int indent, Decl* decl)
{
	/*
	if (decl->name)
		OUTPUT("typedef %s__ %s;\n", decl->name, decl->name);
	if (decl->extname)
		OUTPUT("typedef %s__ %s;\n", decl->extname, decl->name);
		*/
	/*
	if (decl->type->type_kind == TYPE_POINTER) {
		printf("%s","typedef pointer type\n");
		if (decl->type->pointer->type_kind == TYPE_FUNC) {
			OUTPUT("typedef ");
			Type* type = decl->type->pointer;
			// print whole types
			header_print_type(file, type->func.prototype->rtype);
			OUTPUT("typedef (*%s__)(", decl->extname);
			unsigned vecsize = vec_size(type->func.prototype->params);
			Type** params = type->func.prototype->params;
			if (vecsize > 0) {
				Type* param = params[0];
				header_print_type(file, param);
			}
			for (unsigned i = 1; i < vecsize; i++)
			{
				OUTPUT(", ");
				Type* param = params[i];
				header_print_type(file, param);
			}
			OUTPUT(")");

			OUTPUT("typedef b%s__ %s;\n", decl->extname, decl->extname);
			return;
		}
	}*/
	if (decl->extname && decl->type) {
		OUTPUT("typedef ");
		//header_print_type(file, decl->type);
		//header_print_type(file, decl->typedef_decl.type_info->type);
		//OUTPUT(" %s__;\n", decl->extname);
		scratch_buffer_clear();

		//the typedef name
		scratch_buffer_append(decl->extname);
		scratch_buffer_append("__");
		scratch_buffer.str[scratch_buffer.len] = '\0';
		scratch_buffer.len += 1;
		uint32_t typedef_len = scratch_buffer.len;

		c_type_append_name_to_scratch(decl->type, &scratch_buffer.str[0]);
		//printf("str len: %i\n", scratch_buffer.len);
		scratch_buffer.str[scratch_buffer.len] = '\0';
		printf("%.*s\n", scratch_buffer.len, &scratch_buffer.str[typedef_len]);
		OUTPUT("%.*s", scratch_buffer.len, &scratch_buffer.str[typedef_len]); //scratch_buffer.len,
		OUTPUT(";\n");


		OUTPUT("typedef %s__ %s;\n", decl->extname, decl->extname);
	}
}

static void header_gen_distinct(FILE* file, int indent, Decl* decl)
{
	//type = type->decl->distinct_decl.base_type;
	//goto RETRY;
	if (decl->name)
		OUTPUT("typedef %s__ %s;\n", decl->name, decl->name);
	
}


static void header_gen_decl(FILE *file, int indent, Decl *decl)
{
	RETRY:
	printf("header_gen_decl handling: %i\n", (int)decl->decl_kind);
	switch (decl->decl_kind)
	{
		case NON_TYPE_DECLS:
		case DECL_ENUM_CONSTANT:
		case DECL_FAULTVALUE:
		case DECL_POISONED:
		case DECL_VAR:
		case DECL_BODYPARAM:
			UNREACHABLE
		case DECL_FUNC:
		case DECL_BITSTRUCT:
			TODO
		case DECL_TYPEDEF:
			header_gen_typedef(file, indent, decl);
			return;
		case DECL_DISTINCT:
			header_gen_distinct(file, indent, decl);
			return;
		case DECL_STRUCT:
			header_gen_struct(file, indent, decl);
			return;
		case DECL_UNION:
			header_gen_union(file, indent, decl);
			return;
		case DECL_ENUM:
			header_gen_enum(file, indent, decl);
			return;
		case DECL_FAULT:
			header_gen_err(file, indent, decl);
			return;
	}
	UNREACHABLE
}

static void header_gen_var(FILE *file, SemaContext *c, Decl *decl)
{
	fprintf(file, "/* vars */\n");
}

void header_gen(Module *module)
{
	//TODO
	CompilationUnit *unit = module->units[0];
	const char *filename = str_cat(unit->file->name, ".h");

	printf("writing header: %s\n", filename);

	FILE *file = fopen(filename, "w");
	OUTPUT("#include <stdint.h>\n");
	OUTPUT("#ifndef __c3__\n");
	OUTPUT("#define __c3__\n");
	//OUTPUT("typedef ");
	//header_print_type(file, type_flatten(type_typeid));
	//OUTPUT(" c3typeid_t;\n");
	OUTPUT("#endif\n");

	VECEACH(unit->types, i)
	{
		header_gen_decl(file, 0, unit->types[i]);
	}
	
	//TODO
	/*
	VECEACH(context->vars, i)
	{
		header_gen_var(file, context, context->vars[i]);
	}
	VECEACH(context->methods, i)
	{
		header_gen_method(file, context, context->methods[i]);
	}
	VECEACH(context->functions, i)
	{
		header_gen_function(file, context, context->functions[i]);
	}*/
	fclose(file);
}
