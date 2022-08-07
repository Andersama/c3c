// Copyright (c) 2020 Christoffer Lerno. All rights reserved.
// Use of this source code is governed by a LGPLv3.0
// a copy of which can be found in the LICENSE file.

#include "compiler_internal.h"
#include "base_n.h"

#define OUTPUT(x, ...) fprintf(file, x, ## __VA_ARGS__)
#define INDENT() indent_line(file, indent)

static void c_type_append_func_to_scratch(FunctionPrototype* prototype, const char* name);
static void c_type_append_name_to_scratch(Type* type, const char* name);

void c_emit_expr(FILE* file, void* context, int indent, struct BEValue* value, Expr* expr);
void c_emit_stmt(FILE* file, void* context, int indent, Ast* ast);
void c_emit_function_body(FILE* file, void* context, int indent, Decl* decl);

static void indent_line(FILE* file, int indent)
{
	for (int i = 0; i < indent * 3; i++)
	{
		fputc(' ', file);
	}
}
static void header_gen_decl(FILE* file, int indent, Decl* decl);

static void header_gen_method(FILE* file, SemaContext* c, Decl* decl)
{
	fprintf(file, "/* method */\n");
}

static void header_gen_function_decl(FILE* file, SemaContext* c, Decl* decl)
{
	assert(decl->decl_kind == DECL_FUNC);
	//if we want print documentation and comments
	fprintf(file, "/* function */\n");

	FunctionPrototype* prototype = decl->type->func.prototype;

	unsigned params = vec_size(prototype->params);

	if (prototype->ret_by_ref) {

	}

	if (decl->func_decl.attr_noinline)
	{
		OUTPUT("%s", "__declspec(noinline) ");
		//llvm_attribute_add(c, function, attribute_id.noinline, -1);
	}
	if (decl->func_decl.attr_noreturn)
	{
		OUTPUT("%s", "__declspec(noreturn) ");
		//llvm_attribute_add(c, function, attribute_id.noreturn, -1);
	}

	AlignSize type_alignment = type_abi_alignment(decl->type);
	if (decl->alignment != type_alignment)
	{
		//llvm_set_alignment(function, decl->alignment);
		OUTPUT("%s", "alignas(");
		OUTPUT("%i", type_alignment);
		OUTPUT("%s", ") ");
	}

	if (decl->section)
	{

	}

	switch (prototype->call_abi) {
	case CALL_C:
		//return LLVMCCallConv;
	case CALL_X86_STD:
		//return LLVMX86StdcallCallConv;
	case CALL_X86_FAST:
		//return LLVMX86FastcallCallConv;
	case CALL_X86_REG:
		//return LLVMX86RegCallCallConv;
	case CALL_X86_THIS:
		//return LLVMX86ThisCallCallConv;
	case CALL_X86_VECTOR:
		//return LLVMX86VectorCallCallConv;
	case CALL_AAPCS:
		//return LLVMARMAAPCSCallConv;
	case CALL_AAPCS_VFP:
		break;
		//return LLVMARMAAPCSVFPCallConv;
	default:
		//return LLVMCCallConv;
		break;
	}
	//__declspec(noinline)

	Visibility visibility = decl->visibility;
	if (decl->is_external_visible)
		visibility = VISIBLE_PUBLIC;

	switch (visibility) {
	case VISIBLE_EXTERN:
		if (decl->is_weak)
		{
			//LLVMSetLinkage(function, LLVMExternalWeakLinkage);
			//llvm_set_comdat(c, function);
		}
		else
		{
			//LLVMSetLinkage(function, LLVMExternalLinkage);
		}
		//LLVMSetVisibility(function, LLVMDefaultVisibility);
		if (prototype->call_abi == CALL_X86_STD && platform_target.os == OS_TYPE_WIN32)
		{
			//LLVMSetDLLStorageClass(function, LLVMDLLImportStorageClass);
		}
		break;
	case VISIBLE_PUBLIC:
	case VISIBLE_MODULE:
		//if (decl->is_weak) llvm_set_weak(c, function);
		if (decl->is_weak) {

		}
		break;
	case VISIBLE_LOCAL:
		/*
		LLVMSetLinkage(function, decl->is_weak ? LLVMLinkerPrivateWeakLinkage : LLVMInternalLinkage);
		LLVMSetVisibility(function, LLVMDefaultVisibility);
		*/
		break;;
	}
	uint32_t return_type_start = scratch_buffer.len;
	c_type_append_name_to_scratch(prototype->rtype, "");
	uint32_t return_type_end = scratch_buffer.len;
	OUTPUT("%.*s ", (return_type_end - return_type_start), &scratch_buffer.str[return_type_start]);
	scratch_buffer.len = return_type_start;

	if (decl->extname && *decl->extname) {
		OUTPUT("%s", decl->name);
	}
	else if (decl->name && *decl->name) {
		OUTPUT("%s", decl->name);
	}
	else {
		OUTPUT("%s", "_no_function_name_given_");
		//OUTPUT("%s", "/* unknown function name */ ");
	}

	uint32_t params_start = scratch_buffer.len;
	scratch_buffer_append_char('(');
	if (params > 0) {
		c_type_append_name_to_scratch(prototype->params[0], "");
	}
	for (unsigned i = 1; i < params; i++)
	{
		//scratch_buffer_append_char(',');
		scratch_buffer_append(", ");
		//type_append_name_to_scratch(prototype->params[i]);
		c_type_append_name_to_scratch(prototype->params[i], "");
	}
	if (prototype->variadic == VARIADIC_RAW && params > 0)
	{
		scratch_buffer_append_char(',');
	}
	if (prototype->variadic != VARIADIC_NONE)
	{
		scratch_buffer_append("...");
	}
	scratch_buffer_append_len(");\n", 3);
	uint32_t params_end = scratch_buffer.len;
	OUTPUT("%.*s", params_end - params_start, &scratch_buffer.str[params_start]);
	scratch_buffer.len = params_start;

}
#if 0
static void header_print_type(FILE* file, Type* type)
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

static void header_print_named_type(FILE* file, Type* type, const char* name) {
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
	for (unsigned i = 1; i < elements; i++)
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

static void c_type_append_name_to_scratch(Type* type, const char* name)
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
		scratch_buffer_append_len("struct ", 7);
		if (type->decl->extname && *type->decl->extname)
			scratch_buffer_append(type->decl->extname);
		else
			scratch_buffer_append(type->decl->name);

		scratch_buffer_append_len("__", 2);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		//OUTPUT("struct %s__", type->decl->extname);
		return;
	case TYPE_UNION:
		scratch_buffer_append_len("union ", 6);
		if (type->decl->extname && *type->decl->extname)
			scratch_buffer_append(type->decl->extname);
		else
			scratch_buffer_append(type->decl->name);
		scratch_buffer_append_len("__", 2);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		//OUTPUT("union %s__", type->decl->extname);
		return;
	case TYPE_ENUM:
		scratch_buffer_append_len("enum ", 5);
		if (type->decl->extname && *type->decl->extname)
			scratch_buffer_append(type->decl->extname);
		else
			scratch_buffer_append(type->decl->name);
		scratch_buffer_append_len("__", 2);
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
		scratch_buffer_append_char('*');

		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_FAILABLE_ANY:
		scratch_buffer_append_len("struct __any_failable", 21);
		//OUTPUT("%s", "struct __faultany");
		//scratch_buffer_append("void!");
		return;
	case TYPE_FAILABLE:
		//TODO: I've no clue how this is supposed to work (presumably this is impossible to reach)

		scratch_buffer_append("struct __failable_");

		uint32_t c3name_start = scratch_buffer.len;
		if (type->failable)
		{
			c_type_append_name_to_scratch(type->failable, "");
		}
		else
		{
			scratch_buffer_append("void");
		}
		uint32_t c3name_end = scratch_buffer.len;
		//scratch_buffer_append_char('!');
		//write out the base type (w/o a name)

		//c_type_append_name_to_scratch(type->array.base, "");
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
					  return;
	case TYPE_FLEXIBLE_ARRAY: {
		//c_type_append_name_to_scratch(type->array.base, name);
		//scratch_buffer_append("[*]");

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
		scratch_buffer_append_len("void", 4);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_BOOL:
		scratch_buffer_append_len("bool", 4);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_I8:
		scratch_buffer_append_len("int8_t", 6);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_I16:
		scratch_buffer_append_len("int16_t", 7);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_I32:
		scratch_buffer_append_len("int32_t", 7);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_I64:
		scratch_buffer_append_len("int64_t", 7);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_I128:
		scratch_buffer_append_len("__int128", 8);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_U8:
		scratch_buffer_append_len("uint8_t", 7);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_U16:
		scratch_buffer_append_len("uint16_t", 8);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_U32:
		scratch_buffer_append_len("uint32_t", 8);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_U64:
		scratch_buffer_append_len("uint64_t", 8);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_U128:
		scratch_buffer_append_len("unsigned __int128", 17);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_F16:
		scratch_buffer_append_len("__fp16", 6);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_F32:
		scratch_buffer_append_len("float", 5);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_F64:
		scratch_buffer_append_len("double", 6);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_F128:
		scratch_buffer_append_len("__float128", 10);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
		return;
	case TYPE_TYPEID:
		scratch_buffer_append_len("c3typeid_t", 10);
		if (name && *name) {
			scratch_buffer_append_char(' ');
			scratch_buffer_append(name);
		}
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

static void header_gen_members(FILE* file, int indent, Decl** members)
{
	VECEACH(members, i)
	{
		Decl* member = members[i];
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
		else {
			INDENT();
			OUTPUT("/* a member named [%s] : type[%i] */\n", member->name, member->decl_kind);
			//TODO;
		}
		//TODO
	}
}

static void header_gen_enum_members(FILE* file, int indent, Decl** members) {
	VECEACH(members, i)
	{
		Decl* member = members[i];
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
		if (member->decl_kind == DECL_ENUM_CONSTANT) {
			INDENT();
			OUTPUT("%s, //%i\n", member->name, i);
		}
		else {
			INDENT();
			OUTPUT("/* a member named [%s] : type[%i] */\n", member->name, member->decl_kind);
			//TODO;
		}
		//TODO
	}
}

static void header_gen_struct(FILE* file, int indent, Decl* decl)
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

static void header_gen_union(FILE* file, int indent, Decl* decl)
{
	OUTPUT("typedef union %s__ %s;\n", decl->extname, decl->extname);
	OUTPUT("union %s__\n{\n", decl->extname);
	header_gen_members(file, indent, decl->strukt.members);
	OUTPUT("};\n");
}

static void header_gen_enum(FILE* file, int indent, Decl* decl)
{
	//TODO
	OUTPUT("typedef enum %s__ %s;\n", decl->extname, decl->extname);
	OUTPUT("enum %s__\n{\n", decl->extname);
	//decl->enums
	//decl->strukt
	header_gen_enum_members(file, indent, decl->enums.values);
	OUTPUT("};\n");
}

static void header_gen_err(FILE* file, int indent, Decl* decl)
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


static void header_gen_decl(FILE* file, int indent, Decl* decl)
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
		//header_gen_function(file, indent, decl);
		return;
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

static void header_gen_var(FILE* file, SemaContext* c, Decl* decl)
{
	fprintf(file, "/* vars */\n");
}

void header_gen(Module* module)
{
	if (!vec_size(module->units))
		return;
	//TODO
	CompilationUnit* unit = module->units[0];
	const char* filename = str_cat(unit->file->name, ".h");

	SemaContext ctx;
	sema_context_init(&ctx, unit);

	printf("writing header: %s\n", filename);

	FILE* file = fopen(filename, "w");
	OUTPUT("#include <stdint.h>\n");
	OUTPUT("#ifndef __c3__\n");
	OUTPUT("#define __c3__\n");
	//OUTPUT("typedef ");
	//header_print_type(file, type_flatten(type_typeid));
	//OUTPUT(" c3typeid_t;\n");
	OUTPUT("#endif\n");

	//prototype each before they're defined
	// structs?
	VECEACH(unit->types, i)
	{
		header_gen_decl(file, 0, unit->types[i]);
	}

	VECEACH(unit->enums, i)
	{
		header_gen_decl(file, 0, unit->enums[i]);
		//llvm_emit_type_decls(gen_context, unit->enums[i]);
	}

	VECEACH(unit->functions, i)
	{
		header_gen_function_decl(file, &ctx, unit->functions[i]);
		//llvm_emit_function_decl(gen_context, unit->functions[i]);
	}

	VECEACH(unit->methods, i)
	{
		header_gen_function_decl(file, &ctx, unit->methods[i]);
		//llvm_emit_function_decl(gen_context, unit->methods[i]);
	}

	//write the actual function bodies
	VECEACH(unit->functions, i)
	{
		Decl* decl = unit->functions[i];
		if (decl->func_decl.body) {
			c_emit_function_body(file, NULL, 0, decl);
		}
		//llvm_emit_function_body(gen_context, decl);
	}
	if (unit->main_function)
		//llvm_emit_function_body(gen_context, unit->main_function);

		VECEACH(unit->methods, i)
	{
		Decl* decl = unit->methods[i];
		if (decl->func_decl.body) {
			c_emit_function_body(file, NULL, 0, decl);
		}
		//llvm_emit_function_body(gen_context, decl);
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
/*
void *llvm_gen(Module *module)
{
	if (!vec_size(module->units)) return NULL;
	assert(intrinsics_setup);
	GenContext *gen_context = cmalloc(sizeof(GenContext));
	gencontext_init(gen_context, module);
	gencontext_begin_module(gen_context);

	VECEACH(module->units, j)
	{
		CompilationUnit *unit = module->units[j];
		gencontext_init_file_emit(gen_context, unit);
		gen_context->debug.compile_unit = unit->llvm.debug_compile_unit;
		gen_context->debug.file = unit->llvm.debug_file;

		VECEACH(unit->methods, i)
		{
			llvm_emit_function_decl(gen_context, unit->methods[i]);
		}
		VECEACH(unit->types, i)
		{
			llvm_emit_type_decls(gen_context, unit->types[i]);
		}
		VECEACH(unit->enums, i)
		{
			llvm_emit_type_decls(gen_context, unit->enums[i]);
		}
		VECEACH(unit->functions, i)
		{
			llvm_emit_function_decl(gen_context, unit->functions[i]);
		}
		if (unit->main_function) llvm_emit_function_decl(gen_context, unit->main_function);
	}

	VECEACH(module->units, j)
	{
		CompilationUnit *unit = module->units[j];
		gen_context->debug.compile_unit = unit->llvm.debug_compile_unit;
		gen_context->debug.file = unit->llvm.debug_file;

		VECEACH(unit->vars, i)
		{
			llvm_get_ref(gen_context, unit->vars[i]);
		}
		VECEACH(unit->vars, i)
		{
			llvm_emit_global_variable_init(gen_context, unit->vars[i]);
		}
		VECEACH(unit->functions, i)
		{
			Decl *decl = unit->functions[i];
			if (decl->func_decl.body) llvm_emit_function_body(gen_context, decl);
		}
		if (unit->main_function) llvm_emit_function_body(gen_context, unit->main_function);

		VECEACH(unit->methods, i)
		{
			Decl *decl = unit->methods[i];
			if (decl->func_decl.body) llvm_emit_function_body(gen_context, decl);
		}

		gencontext_end_file_emit(gen_context, unit);
	}
	// EmitDeferred()

	if (llvm_use_debug(gen_context))
	{
		LLVMDIBuilderFinalize(gen_context->debug.builder);
		LLVMDisposeDIBuilder(gen_context->debug.builder);
	}

	// If it's in test, then we want to serialize the IR before it is optimized.
	if (active_target.test_output)
	{
		gencontext_print_llvm_ir(gen_context);
		gencontext_verify_ir(gen_context);
	}
	return gen_context;
}
*/

void c_emit_break(FILE* file, void* context, int indent, Ast* ast) {
	printf(__FUNCDNAME__":%s", "\n");
	//ast->contbreak_stmt.defers
	//label: defered_block:
	Ast* jump_target = astptr(ast->contbreak_stmt.ast);

	switch (jump_target->ast_kind)
	{
	case AST_IF_STMT:
		//jump = jump_target->if_stmt.codegen.break_block;
		INDENT();
		OUTPUT("%s", "break;\n");
		break;
	case AST_FOR_STMT:
		//jump = jump_target->for_stmt.codegen.exit_block;
		INDENT();
		OUTPUT("%s", "break;\n");
		break;
	case AST_IF_CATCH_SWITCH_STMT:
	case AST_SWITCH_STMT:
		//jump = jump_target->switch_stmt.codegen.exit_block;
		INDENT();
		OUTPUT("%s", "break;\n");
		break;
	case AST_FOREACH_STMT:
	default:
		UNREACHABLE
	}
	//c_emit_jmp(file, context, indent, jump);
}

void c_emit_continue(FILE* file, void* context, int indent, Ast* ast) {
	printf(__FUNCDNAME__":%s", "\n");
	//ast->contbreak_stmt.defers
	//label: defered_block:
	Ast* jump_target = astptr(ast->contbreak_stmt.ast);

	switch (jump_target->ast_kind)
	{
	case AST_IF_STMT:
	case AST_SWITCH_STMT:
	case AST_FOREACH_STMT:
		UNREACHABLE
			break;
	case AST_FOR_STMT:
		INDENT();
		OUTPUT("%s", "continue;\n");
		break;
	default:
		UNREACHABLE
	}
	//c_emit_jmp(file, context, indent, jump);
}

void c_emit_local_decl(FILE* file, void* context, int indent, Decl* decl) {
	printf(__FUNCDNAME__":%s", "\n");
	if (decl->var.is_static)
	{
		INDENT();
		OUTPUT("%s", "static ");
		if (decl->backend_ref) {
			return;
		}

		if (IS_FAILABLE(decl))
		{

		}
	}
	//TODO;
}

void c_emit_if(FILE* file, void* context, int indent, Ast* ast) {
	printf(__FUNCDNAME__":%s", "\n");
	INDENT();

	Expr* cond = exprptr(ast->if_stmt.cond);
	OUTPUT("%s", "if (");
	c_emit_expr(file, context, indent, NULL, cond);
	//emit the condition
	OUTPUT("%s", ")");

	Ast* then_body = astptr(ast->if_stmt.then_body);
	if (ast_is_not_empty(then_body))
	{
		INDENT();
		OUTPUT("%s", "else {");
		OUTPUT("%s", "}");
	}

	AstId else_id = ast->if_stmt.else_body;
	Ast* else_body = else_id ? astptr(else_id) : NULL;
	if (ast_is_not_empty(else_body))
	{
		INDENT();
		OUTPUT("%s", "{");
		OUTPUT("%s", "}");
	}
}

void c_emit_expr_stmt(FILE* file, void* context, int indent, Ast* ast) {
	printf(__FUNCDNAME__":%s", "\n");
	//struct BEValue value;
	INDENT();
	if (IS_FAILABLE(ast->expr_stmt)) {
		//emit_expr(c, &value, ast->expr_stmt);
	}
	c_emit_expr(file, context, indent, NULL, ast->expr_stmt);
	OUTPUT(";\n");
	//TODO;
}

void c_emit_statement_chain(FILE* file, void* context, int indent, AstId current) {
	printf(__FUNCDNAME__":%s", "\n");
	while (current)
	{
		c_emit_stmt(file, context, indent, ast_next(&current));
		//llvm_emit_stmt(c, ast_next(&current));
	}
	//TODO;
}

//GenContext *context
void c_emit_return(FILE* file, struct GenContext* context, int indent, Ast* ast) {
	printf(__FUNCDNAME__":%s", "\n");
	//INDENT();

	Expr* expr = ast->return_stmt.expr;
	//expr->type
	Type* return_ty = expr->type; //? the return type?
	/*
	if (return_ty && (return_ty)->canonical && type_is_failable(return_ty)) //context->cur_func_decl->type->func.prototype->rtype
	{
		printf(__FUNCDNAME__":%s", "failable \n");
		//struct BEValue be_value = { 0 };
		if (expr && expr->inner_expr)
			c_emit_expr(file, context, indent, NULL, expr->inner_expr);
		c_emit_statement_chain(file, context, indent, ast->return_stmt.cleanup);
		//c_emit_statement_chain(file, context, indent, ast->return_stmt.cleanup);
		//c_emit_return_abi(file, context, indent, , );
	}
	*/
	printf(__FUNCDNAME__":%s", "return \n");
	OUTPUT("%s", "return");

	bool has_return_value = ast->return_stmt.expr != NULL;
	if (has_return_value) {
		OUTPUT("%s", " ");
		c_emit_expr(file, context, indent, NULL, ast->return_stmt.expr);
	}
	OUTPUT("%s", ";\n");
	//TODO;
}

void c_emit_compound_stmt(FILE* file, void* context, int indent, Ast* ast)
{
	printf(__FUNCDNAME__":%s", "\n");
	/*
	if (llvm_use_debug(c))
	{
		llvm_debug_push_lexical_scope(c, ast->span);
	}
	*/
	assert(ast->ast_kind == AST_COMPOUND_STMT);
	c_emit_statement_chain(file, context, indent, ast->compound_stmt.first_stmt);
	//llvm_emit_statement_chain(c, ast->compound_stmt.first_stmt);
	/*
	if (llvm_use_debug(c))
	{
		llvm_debug_scope_pop(c);
	}
	*/
}

void c_emit_builtin_access(FILE* file, void* context, int indent, Expr* expr) {
	printf(__FUNCDNAME__":%s", "\n");
	Expr* inner = exprptr(expr->builtin_access_expr.inner);
	c_emit_expr(file, context, indent, NULL, inner);

	switch (expr->builtin_access_expr.kind)
	{
	case ACCESS_LEN:
	case ACCESS_PTR:
	case ACCESS_FAULTNAME:
	{}
	break;
	case ACCESS_ENUMNAME:
	{}
	break;
	case ACCESS_TYPEOFANY: {

	}break;
	}
}

static void c_emit_unary_expr(FILE* file, void* context, int indent, struct BEValue* value, Expr* expr)
{
	printf(__FUNCDNAME__":%s", "\n");
	//Type* type = type_reduced_from_expr(expr->unary_expr.expr);
	Expr* inner = expr->unary_expr.expr;
	//LLVMValueRef llvm_value;
	switch (expr->unary_expr.operator)
	{
	case UNARYOP_ERROR:
		FATAL_ERROR("Illegal unary op %s", expr->unary_expr.operator);
	case UNARYOP_NOT:
		//llvm_emit_expr(c, value, inner);
		OUTPUT("%s", "!");
		c_emit_expr(file, context, indent, value, inner);
		/*
		if (type_is_vector(type))
		{
			llvm_value_rvalue(c, value);
			Type* vec_type = type_vector_type(type);
			if (type_is_float(vec_type))
			{
				llvm_value = LLVMBuildFCmp(c->builder, LLVMRealUNE, value->value, llvm_get_zero(c, type), "not");
			}
			else
			{
				llvm_value = LLVMBuildICmp(c->builder, LLVMIntEQ, value->value, llvm_get_zero(c, type), "not");
			}
			Type* res_type = type_get_vector_bool(type);
			llvm_value = LLVMBuildSExt(c->builder, llvm_value, llvm_get_type(c, res_type), "");
			llvm_value_set(value, llvm_value, res_type);
			return;
		}
		*/
		/*
		switch (type->type_kind)
		{
		case ALL_FLOATS:
			llvm_value_rvalue(c, value);
			llvm_value = LLVMBuildFCmp(c->builder, LLVMRealUNE, value->value, llvm_get_zero(c, type), "not");
			break;
		case TYPE_BOOL:
			llvm_value_rvalue(c, value);
			llvm_value = LLVMBuildNot(c->builder, value->value, "not");
			break;
		case TYPE_SUBARRAY:
			if (value->kind != BE_VALUE)
			{
				llvm_emit_len_for_expr(c, value, value);
				llvm_value_rvalue(c, value);
				llvm_value = value->value;
			}
			else
			{
				llvm_value = llvm_emit_extract_value(c, value->value, 1);
			}
			llvm_value = LLVMBuildIsNull(c->builder, llvm_value, "not");
			break;
		case ALL_INTS:
		case TYPE_POINTER:
			llvm_value_rvalue(c, value);
			llvm_value = LLVMBuildIsNull(c->builder, value->value, "not");
			break;
		default:
			DEBUG_LOG("Unexpectedly tried to not %s", type_quoted_error_string(inner->type));
			UNREACHABLE
		}
		llvm_value_set_bool(value, llvm_value);
		*/
		return;
	case UNARYOP_BITNEG:
		OUTPUT("%s", "~");
		c_emit_expr(file, context, indent, value, inner);
		/*
		llvm_emit_expr(c, value, inner);
		llvm_value_rvalue(c, value);
		value->value = LLVMBuildNot(c->builder, value->value, "bnot");
		*/
		return;
	case UNARYOP_NEG:
		OUTPUT("%s", "-");
		c_emit_expr(file, context, indent, value, inner);
		/*
		llvm_emit_expr(c, value, inner);
		llvm_value_rvalue(c, value);
		if (type_is_float(type))
		{
			value->value = LLVMBuildFNeg(c->builder, value->value, "fneg");
			return;
		}
		assert(type->canonical != type_bool);
		llvm_emit_expr(c, value, expr->unary_expr.expr);
		llvm_value_rvalue(c, value);
		if (active_target.feature.trap_on_wrap)
		{
			LLVMValueRef zero = llvm_get_zero(c, expr->unary_expr.expr->type);
			LLVMTypeRef type_to_use = llvm_get_type(c, type->canonical);
			LLVMValueRef args[2] = { zero, value->value };
			LLVMValueRef call_res = llvm_emit_call_intrinsic(c, intrinsic_id.ssub_overflow,
				&type_to_use, 1, args, 2);
			value->value = llvm_emit_extract_value(c, call_res, 0);
			LLVMValueRef ok = llvm_emit_extract_value(c, call_res, 1);
			llvm_emit_panic_on_true(c, ok, "Signed negation overflow", expr->span);
			return;
		}
		value->value = LLVMBuildNeg(c->builder, value->value, "neg");
		*/
		return;
	case UNARYOP_TADDR:
	case UNARYOP_ADDR:
		OUTPUT("%s", "&");
		c_emit_expr(file, context, indent, value, inner);
		/*
		llvm_emit_expr(c, value, inner);
		// Create an addr
		llvm_value_addr(c, value);
		// Transform to value
		value->kind = BE_VALUE;
		value->type = type_lowering(expr->type);
		*/
		return;
	case UNARYOP_DEREF:
		OUTPUT("%s", "*");
		c_emit_expr(file, context, indent, value, inner);
		//llvm_emit_deref(c, value, inner, type_lowering(expr->type));
		return;
	case UNARYOP_INC:
		OUTPUT("%s", "++");
		c_emit_expr(file, context, indent, value, inner);
		//llvm_emit_pre_inc_dec(c, value, inner, 1, false);
		return;
	case UNARYOP_DEC:
		OUTPUT("%s", "--");
		c_emit_expr(file, context, indent, value, inner);
		//llvm_emit_pre_inc_dec(c, value, inner, -1, false);
		return;
	}
	UNREACHABLE
}

static void c_emit_assign_expr(FILE* file, void* context, int ident, struct BEValue* value, Expr* expr) {
	printf(__FUNCDNAME__":%s", "\n");
}

static void c_emit_op(FILE* file, void* context, int ident, uint32_t op) {
	printf(__FUNCDNAME__":%s", "\n");

	switch (op) {
	case BINARYOP_MULT:
		OUTPUT("%s", "*");
		break;
	case BINARYOP_SUB:
		OUTPUT("%s", "-");
		break;

	case BINARYOP_ADD:
		OUTPUT("%s", "+");
		break;

	case BINARYOP_DIV:
		OUTPUT("%s", "/");
		break;

	case BINARYOP_MOD:
		OUTPUT("%s", "%");
		break;

	case BINARYOP_SHR:
		OUTPUT("%s", ">>");
		break;

	case BINARYOP_SHL:
		OUTPUT("%s", "<<");
		break;

	case BINARYOP_BIT_OR:
		OUTPUT("%s", "|");
		break;

	case BINARYOP_BIT_XOR:
		OUTPUT("%s", "^");
		break;

	case BINARYOP_BIT_AND:
		OUTPUT("%s", "&");
		break;

	case BINARYOP_AND:
		OUTPUT("%s", "&&");
		break;

	case BINARYOP_OR:
		OUTPUT("%s", "||");
		break;

	case BINARYOP_OR_ERR:
		OUTPUT("%s", "||"); //?
		break;
	case BINARYOP_GT:
		OUTPUT("%s", ">");
		break;

	case BINARYOP_GE:
		OUTPUT("%s", ">=");
		break;

	case BINARYOP_LT:
		OUTPUT("%s", "<");
		break;

	case BINARYOP_LE:
		OUTPUT("%s", "<=");
		break;

	case BINARYOP_NE:
		OUTPUT("%s", "!=");
		break;

	case BINARYOP_EQ:
		OUTPUT("%s", "==");
		break;

	case BINARYOP_ASSIGN:
		OUTPUT("%s", "=");
		break;

	case BINARYOP_ADD_ASSIGN:
		OUTPUT("%s", "+=");
		break;

	case BINARYOP_BIT_AND_ASSIGN:
		OUTPUT("%s", "&=");
		break;

	case BINARYOP_BIT_OR_ASSIGN:
		OUTPUT("%s", "|=");
		break;

	case BINARYOP_BIT_XOR_ASSIGN:
		OUTPUT("%s", "^=");
		break;

	case BINARYOP_DIV_ASSIGN:
		OUTPUT("%s", "/=");
		break;

	case BINARYOP_MOD_ASSIGN:
		OUTPUT("%s", "%=");
		break;

	case BINARYOP_MULT_ASSIGN:
		OUTPUT("%s", "*=");
		break;

	case BINARYOP_SHR_ASSIGN:
		OUTPUT("%s", ">>=");
		break;

	case BINARYOP_SHL_ASSIGN:
		OUTPUT("%s", "<<=");
		break;

	case BINARYOP_SUB_ASSIGN:
		OUTPUT("%s", "-=");
		break;

	}
}

static void c_emit_binary_expr(FILE* file, void* context, int indent, struct BEValue* value, Expr* expr) {
	printf(__FUNCDNAME__":%s", "\n");

	BinaryOp binary_op = expr->binary_expr.operator;
	/*
	if (binary_op >= BINARYOP_ASSIGN && expr_is_vector_index(exprptr(expr->binary_expr.left)))
	{
		//llvm_emit_vector_assign_expr(c, be_value, expr);
		return;
	}
	*/
	if (binary_op > BINARYOP_ASSIGN)
	{
		BinaryOp base_op = binaryop_assign_base_op(binary_op);
		assert(base_op != BINARYOP_ERROR);
		c_emit_expr(file, context, indent, NULL, exprptr(expr->binary_expr.left));
		OUTPUT("%s", " ");
		c_emit_expr(file, context, indent, NULL, exprptr(expr->binary_expr.right));
		/*
		switch () {
		}
		*/
		//c_emit_expr(file, context, indent, NULL, exprptr(expr->binary_expr.right));
		/*
		BEValue addr;
		llvm_emit_expr(c, &addr, exprptr(expr->binary_expr.left));
		llvm_value_addr(c, &addr);
		gencontext_emit_binary(c, be_value, expr, &addr, base_op);
		llvm_store_value(c, &addr, be_value);
		*/
		return;
	}
	if (binary_op == BINARYOP_ASSIGN)
	{
		Expr* left = exprptr(expr->binary_expr.left);
		c_emit_expr(file, context, indent, NULL, left);
		/*
		if (left->expr_kind == EXPR_IDENTIFIER) {
			//decl_failable_ref(left->identifier_expr.decl);
			OUTPUT("%s", left->identifier_expr.ident);
		}
		*/
		OUTPUT("%s", "=");


		/*
		llvm_emit_expr(c, be_value, left);
		assert(llvm_value_is_addr(be_value));
		LLVMValueRef failable_ref = NULL;
		if (left->expr_kind == EXPR_IDENTIFIER)
		{
			failable_ref = decl_failable_ref(left->identifier_expr.decl);
			be_value->kind = BE_ADDRESS;
		}
		*be_value = llvm_emit_assign_expr(c, be_value, exprptr(expr->binary_expr.right), failable_ref);
		* */
		return;
	}
}

#if 0
void c_emit_len_for_expr(File* file, void* context, int indent, struct BEValue* value, struct BEValue* expr_to_len)
{
	switch (expr_to_len->type->type_kind)
	{
	case TYPE_SUBARRAY:
		//llvm_value_fold_failable(c, be_value);
		if (expr_to_len->kind == BE_VALUE)
		{
			//llvm_value_set(be_value, llvm_emit_extract_value(c, expr_to_len->value, 1), type_usize);
		}
		else
		{
			/*
			LLVMTypeRef subarray_type = llvm_get_type(c, expr_to_len->type);
			AlignSize alignment;
			LLVMValueRef len_addr = llvm_emit_struct_gep_raw(c,
				expr_to_len->value,
				subarray_type,
				1,
				expr_to_len->alignment,
				&alignment);
			llvm_value_set_address(be_value, len_addr, type_usize, alignment);
			*/
		}
		break;
	case TYPE_ARRAY:
	case TYPE_VECTOR:
		//llvm_value_set(be_value, llvm_const_int(c, type_usize, expr_to_len->type->array.len), type_usize);
		OUTPUT("%i", expr_to_len->type->array.len);
		break;
	default:
		UNREACHABLE
	}
}
#endif

void c_emit_subscript(FILE* file, void* context, int indent, struct BEValue* value, Expr* expr) {
	printf(__FUNCDNAME__":%s", "\n");

	bool is_value = expr->expr_kind == EXPR_SUBSCRIPT;
	Expr* parent_expr = exprptr(expr->subscript_expr.expr);
	Expr* index_expr = exprptr(expr->subscript_expr.index);
	/*
	Type* parent_type = type_lowering(parent_expr->type);
	TypeKind parent_type_kind = parent_type->type_kind;

	if (is_value && parent_type->type_kind == TYPE_VECTOR)
	{
		//llvm_emit_vector_subscript(c, value, expr);
		//c_emit_vector_subscript();
		TODO;
		return;
	}
	*/
	c_emit_expr(file, context, indent, value, parent_expr);
	//c_emit_len_for_expr();
	llvm_emit_len_for_expr();
	/*
	bool needs_len = false;
	if (parent_type_kind == TYPE_SUBARRAY)
	{
		needs_len = active_target.feature.safe_mode || expr->subscript_expr.from_back;
	}
	else if (parent_type_kind == TYPE_ARRAY)
	{
		// From back should always be folded.
		assert(expr->expr_kind != EXPR_CONST || !expr->subscript_expr.from_back);
		needs_len = (active_target.feature.safe_mode && expr->expr_kind != EXPR_CONST) || expr->subscript_expr.from_back;
	}

	if (needs_len) {
		//c_emit_len_for_expr();
	}
	*/
}

const char* expr_kind_names[] = {
	"EXPR_POISONED",
	"EXPR_ACCESS",
	"EXPR_BITACCESS",
	"EXPR_BITASSIGN",
	"EXPR_BINARY",
	"EXPR_BUILTIN",
	"EXPR_COMPILER_CONST",
	"EXPR_MACRO_BODY_EXPANSION",
	"EXPR_CALL",
	"EXPR_CAST",
	"EXPR_CATCH",
	"EXPR_CATCH_UNWRAP",
	"EXPR_COMPOUND_LITERAL",
	"EXPR_CONST",
	"EXPR_CT_CALL",
	"EXPR_CT_CONV",
	"EXPR_CT_IDENT",
	"EXPR_CT_EVAL",
	"EXPR_COND",
	"EXPR_DECL",
	"EXPR_DESIGNATOR",
	"EXPR_EXPR_BLOCK",
	"EXPR_EXPRESSION_LIST",
	"EXPR_FAILABLE",
	"EXPR_GROUP",
	"EXPR_RETHROW",
	"EXPR_FORCE_UNWRAP",
	"EXPR_HASH_IDENT",
	"EXPR_MACRO_BLOCK",
	"EXPR_IDENTIFIER",
	"EXPR_RETVAL",
	"EXPR_FLATPATH",
	"EXPR_INITIALIZER_LIST",
	"EXPR_DESIGNATED_INITIALIZER_LIST",
	"EXPR_POST_UNARY",
	"EXPR_SLICE",
	"EXPR_SLICE_ASSIGN",
	"EXPR_SUBSCRIPT",
	"EXPR_SUBSCRIPT_ADDR",
	"EXPR_STRINGIFY",
	"EXPR_ARGV_TO_SUBARRAY",
	"EXPR_TERNARY",
	"EXPR_TRY",
	"EXPR_TRY_UNWRAP",
	"EXPR_TRY_UNWRAP_CHAIN",
	"EXPR_TYPEID",
	"EXPR_TYPEINFO",
	"EXPR_UNARY",
	"EXPR_VARIANTSWITCH",
	"EXPR_NOP",
	"EXPR_TYPEID_INFO",
	"EXPR_VARIANT",
	"EXPR_BUILTIN_ACCESS"
};

void c_emit_expr(FILE* file, void* context, int indent, struct BEValue* value, Expr* expr) {
	printf(__FUNCDNAME__": [%i] %s\n", expr->expr_kind, expr_kind_names[expr->expr_kind]);

	switch (expr->expr_kind)
	{
	case NON_RUNTIME_EXPR:
	case EXPR_COND:
		VECEACH(expr->cond_expr, i)
		{
			//BEValue value;
			//llvm_emit_expr(context, &value, expr->cond_expr[i]);
			c_emit_expr(file, context, indent, value, expr->cond_expr[i]);
		}
		//c_emit_expr(file, context, indent, value, expr);
		return;
	case EXPR_CT_CONV:
		UNREACHABLE
	case EXPR_BUILTIN_ACCESS:
		//llvm_emit_builtin_access(c, value, expr);
		c_emit_builtin_access(file, context, indent, expr);
		return;
	case EXPR_RETVAL:
		//expr->
		//*value = c->retval;
		return;
	case EXPR_VARIANT:
		//llvm_emit_variant(c, value, expr);
		return;
	case EXPR_ARGV_TO_SUBARRAY:
		//llvm_emit_argv_to_subarray(c, value, expr);
		return;
	case EXPR_TRY_UNWRAP_CHAIN:
		//llvm_emit_try_unwrap_chain(c, value, expr);
		return;
	case EXPR_TRY_UNWRAP:
		//llvm_emit_try_unwrap(c, value, expr);
		return;
	case EXPR_CATCH_UNWRAP:
		//llvm_emit_catch_unwrap(c, value, expr);
		return;
	case EXPR_TYPEID_INFO:
		//llvm_emit_typeid_info(c, value, expr);
		return;
	case EXPR_BUILTIN:
		UNREACHABLE;
	case EXPR_DECL:
		c_emit_local_decl(file, context, indent, expr->decl_expr);
		//llvm_emit_local_decl(c, expr->decl_expr, value);
		return;
	case EXPR_SLICE_ASSIGN:
		//llvm_emit_slice_assign(c, value, expr);
		return;
	case EXPR_SLICE:
		//gencontext_emit_slice(c, value, expr);
		return;
	case EXPR_FAILABLE:
		//llvm_emit_failable(c, value, expr);
		return;
	case EXPR_TRY:
		//llvm_emit_try_expr(c, value, expr);
		return;
	case EXPR_CATCH:
		//llvm_emit_catch_expr(c, value, expr);
		return;
	case EXPR_NOP:
		//llvm_value_set(value, NULL, type_void);
		return;
	case EXPR_MACRO_BLOCK:
		//llvm_emit_macro_block(c, value, expr);
		return;
	case EXPR_COMPOUND_LITERAL:
		UNREACHABLE
	case EXPR_INITIALIZER_LIST:
	case EXPR_DESIGNATED_INITIALIZER_LIST:
		//llvm_emit_initializer_list_expr(c, value, expr);
		return;
	case EXPR_EXPR_BLOCK:
		//llvm_emit_expr_block(c, value, expr);

		return;
	case EXPR_UNARY:
		c_emit_unary_expr(file, context, indent, value, expr);
		//llvm_emit_unary_expr(c, value, expr);
		return;
	case EXPR_CONST:

		//llvm_emit_const_expr(c, value, expr);
		return;
	case EXPR_MACRO_BODY_EXPANSION:
		//llvm_emit_macro_body_expansion(c, value, expr);
		return;
	case EXPR_BITASSIGN:
		//llvm_emit_bitassign_expr(c, value, expr);
		return;
	case EXPR_BINARY:
		c_emit_binary_expr(file, context, indent, value, expr);
		//llvm_emit_binary_expr(c, value, expr);
		return;
	case EXPR_TERNARY:
		//gencontext_emit_ternary_expr(c, value, expr);
		return;
	case EXPR_POST_UNARY:
		//llvm_emit_post_unary_expr(c, value, expr);
		return;
	case EXPR_FORCE_UNWRAP:
		//llvm_emit_force_unwrap_expr(c, value, expr);
		return;
	case EXPR_RETHROW:
		//llvm_emit_rethrow_expr(c, value, expr);
		return;
	case EXPR_TYPEID:
	case EXPR_GROUP:
		// These are folded in the semantic analysis step.
		UNREACHABLE
	case EXPR_IDENTIFIER:
		OUTPUT("%s", expr->identifier_expr.ident);
		//llvm_value_set_decl(c, value, expr->identifier_expr.decl);
		return;
	case EXPR_SUBSCRIPT:
	case EXPR_SUBSCRIPT_ADDR:
		/*
		OUTPUT("%s", "[");
		c_emit_expr(file, context, indent, value, expr);
		OUTPUT("%s", "]");
		*/
		c_emit_subscript(file, context, indent, value, expr);
		//gencontext_emit_subscript(c, value, expr);
		return;
	case EXPR_ACCESS:

		//gencontext_emit_access_addr(c, value, expr);
		return;
	case EXPR_CALL:
		//llvm_emit_call_expr(c, value, expr);
		return;
	case EXPR_EXPRESSION_LIST:
		//gencontext_emit_expression_list_expr(c, value, expr);
		return;
	case EXPR_CAST:
		//gencontext_emit_cast_expr(c, value, expr);
		return;
	case EXPR_BITACCESS:
		//llvm_emit_bitaccess(c, value, expr);
		return;
	}
	UNREACHABLE
}

//void llvm_emit_for_stmt(GenContext *c, Ast *ast)
void c_emit_for(FILE* file, void* context, int indent, Ast* ast) {
	INDENT();

	ExprId init = ast->for_stmt.init;
	ExprId incr = ast->for_stmt.incr;
	ExprId cond = ast->for_stmt.cond;

	//Expr* cond = cond_id ? exprptr(cond_id) : NULL;

	Ast* body = astptr(ast->for_stmt.body);

	// Skipping first cond? This is do-while semantics
	bool skip_first = ast->for_stmt.flow.skip_first;
	//LoopType loop = loop_type_for_cond(cond, skip_first);

	/*
	TODO; // gaurd against infinite loops?
	if (!inc_block && !body_block && !cond_block)
	{
		if (loop == LOOP_INFINITE)
		{
		OUTPUT("%s", "//warning: infinite loop!");
		return;
		}
	}
	*/

	if (skip_first) {
		//struct BEValue dummy = { 0 };

		OUTPUT("%s", "do {");
		if (body)
			c_emit_stmt(file, context, indent, body);
		INDENT();
		OUTPUT("%s", "} while (");
		c_emit_expr(file, context, indent, NULL, cond ? exprptr(cond) : NULL);
		OUTPUT(");\n");
	}
	else {
		//struct BEValue dummy = { 0 };

		OUTPUT("%s" "for (");
		if (init)
			c_emit_expr(file, context, indent, NULL, init ? exprptr(init) : NULL);
		OUTPUT("%s", ";");
		if (cond)
			c_emit_expr(file, context, indent, NULL, cond ? exprptr(cond) : NULL);
		OUTPUT("%s", ";");
		if (incr)
			c_emit_expr(file, context, indent, NULL, incr ? exprptr(incr) : NULL);
		OUTPUT("%s", ")");

		c_emit_stmt(file, context, indent, body);
	}


}

void c_emit_block_exit_return(FILE* file, void* context, int indent, Ast* ast)
{

	//PUSH_ERROR();
	//LLVMBasicBlockRef error_return_block = NULL;
	//LLVMValueRef error_out = NULL;
	BlockExit* exit = *ast->return_stmt.block_exit_ref;
	//c->error_var = exit->block_error_var;
	//c->catch_block = exit->block_failable_exit;

	//LLVMBasicBlockRef err_cleanup_block = NULL;
	Expr* ret_expr = ast->return_stmt.expr;

	//BEValue return_value = { 0 };
	if (ret_expr)
	{
		if (ast->return_stmt.cleanup && IS_FAILABLE(ret_expr))
		{
			assert(c->catch_block);
			//err_cleanup_block = llvm_basic_block_new(c, "opt_block_cleanup");
			//c->catch_block = err_cleanup_block;
		}
		c_emit_expr(file, context, indent, NULL, ast->return_stmt.expr);
		//llvm_emit_expr(c, &return_value, ast->return_stmt.expr);
		//llvm_value_fold_failable(c, &return_value);
	}

	//POP_ERROR();

	//llvm_emit_statement_chain(c, ast->return_stmt.cleanup);
	c_emit_statement_chain(file, context, indent, ast->return_stmt.cleanup);
	/*
	if (exit->block_return_out && return_value.value)
	{
		llvm_store_value_aligned(c, exit->block_return_out, &return_value, type_alloca_alignment(return_value.type));
	}
	*/

	/*
	if (err_cleanup_block)
	{
		llvm_emit_br(c, exit->block_return_exit);
		llvm_emit_block(c, err_cleanup_block);
		llvm_emit_statement_chain(c, ast->return_stmt.cleanup);
		llvm_emit_jmp(c, exit->block_failable_exit);
	}
	else
	{
		llvm_emit_jmp(c, exit->block_return_exit);
	}
	*/
}

static const char* ast_kind_names[] = {
	"AST_POISONED",
	"AST_ASM_STMT",
	"AST_ASSERT_STMT",
	"AST_BREAK_STMT",
	"AST_CASE_STMT",
	"AST_COMPOUND_STMT",
	"AST_CONTINUE_STMT",
	"AST_CT_ASSERT",
	"AST_CT_IF_STMT",
	"AST_CT_ELSE_STMT",
	"AST_CT_FOR_STMT",
	"AST_CT_FOREACH_STMT",
	"AST_CT_SWITCH_STMT",
	"AST_DECLARE_STMT",
	"AST_DEFAULT_STMT",
	"AST_DEFER_STMT",
	"AST_EXPR_STMT",
	"AST_FOR_STMT",
	"AST_FOREACH_STMT",
	"AST_IF_CATCH_SWITCH_STMT",
	"AST_IF_STMT",
	"AST_NOP_STMT",
	"AST_RETURN_STMT",
	"AST_BLOCK_EXIT_STMT",
	"AST_SWITCH_STMT",
	"AST_NEXT_STMT",
	"AST_DOC_STMT"
};

void c_emit_stmt(FILE* file, void* context, int indent, Ast* ast) {
	printf("c_emit_stmt: [%d] %s\n", ast->ast_kind, ast_kind_names[ast->ast_kind]);
	switch (ast->ast_kind)
	{
	case AST_POISONED:
	case AST_IF_CATCH_SWITCH_STMT:
	case AST_FOREACH_STMT:
	case AST_DOC_STMT:
		UNREACHABLE
	case AST_EXPR_STMT:
		//gencontext_emit_expr_stmt(c, ast);
		c_emit_expr_stmt(file, context, indent, ast);
		break;
	case AST_DECLARE_STMT:
	{
		//BEValue value;
		//llvm_emit_local_decl(c, ast->declare_stmt, &value);
		c_emit_local_decl(file, context, indent, ast->declare_stmt);
		break;
	}
	case AST_BREAK_STMT:
		c_emit_break(file, context, indent, ast);
		//llvm_emit_break(c, ast);
		break;
	case AST_CONTINUE_STMT:
		c_emit_continue(file, context, indent, ast);
		//llvm_emit_continue(c, ast);
		break;
	case AST_IF_STMT:
		c_emit_if(file, context, indent, ast);
		//llvm_emit_if(c, ast);
		break;
	case AST_RETURN_STMT:
		c_emit_return(file, context, indent, ast);
		//llvm_emit_return(c, ast);
		break;
	case AST_BLOCK_EXIT_STMT:
		//llvm_emit_block_exit_return(c, ast);
		break;
	case AST_COMPOUND_STMT:
		c_emit_compound_stmt(file, context, indent, ast);
		//llvm_emit_compound_stmt(c, ast);
		break;
	case AST_FOR_STMT:
		c_emit_for(file, context, indent, ast);
		//llvm_emit_for_stmt(c, ast);
		break;
	case AST_NEXT_STMT:
		//gencontext_emit_next_stmt(c, ast);
		break;
	case AST_DEFER_STMT:
	case AST_NOP_STMT:
		break;
	case AST_ASM_STMT:
		//llvm_emit_asm_stmt(c, ast);
		break;
	case AST_ASSERT_STMT:
		//llvm_emit_assert_stmt(c, ast);
		break;
	case AST_CT_ASSERT:
	case AST_CT_IF_STMT:
	case AST_CT_ELSE_STMT:
	case AST_CT_FOR_STMT:
	case AST_CT_SWITCH_STMT:
	case AST_CASE_STMT:
	case AST_DEFAULT_STMT:
	case AST_CT_FOREACH_STMT:
		TODO;
	case AST_SWITCH_STMT:
		//gencontext_emit_switch(c, ast);
		break;
	}

}

void c_emit_function_body(FILE* file, void* context, int indent, Decl* decl) {
	assert(decl->func_decl.body);

	// output the function signature
	printf("c_emit_function_body: %s\n", decl->name);
	OUTPUT("/* function: %s */\n", decl->name);

	FunctionPrototype* prototype = decl->type->func.prototype;

	if (prototype->ret_abi_info->kind == ABI_ARG_INDIRECT)
	{
		if (prototype->is_failable)
		{
			//c->failable_out = LLVMGetParam(c->function, arg++);
		}
		else
		{
			//c->return_out = LLVMGetParam(c->function, arg++);
		}
	}

	if (prototype->ret_by_ref_abi_info)
	{
		assert(!c->return_out);
		//c->return_out = LLVMGetParam(c->function, arg++);
	}

	Type* return_type = prototype->rtype;

	uint32_t return_type_start = scratch_buffer.len;
	c_type_append_name_to_scratch(return_type, "");
	uint32_t return_type_end = scratch_buffer.len;
	OUTPUT("%.*s %s", (return_type_end-return_type_start), &scratch_buffer.str[return_type_start],
		decl->extname);
	scratch_buffer.len = return_type_start;

	//c_type_append_name_to_scratch(decl->func_decl.function_signature.returntype)


	if (!decl->func_decl.attr_naked)
	{
		// Generate the full signature (w/ names) for all parameters, so we can use them as local vars in code
		OUTPUT("%.*s", 1, "(");

		unsigned params_size = vec_size(decl->func_decl.function_signature.params);
		if (params_size) {
			Type* param_type = decl->func_decl.function_signature.params[0]->type;
			const char* param_name = decl->func_decl.function_signature.params[0]->name;

			uint32_t param_start = scratch_buffer.len;
			c_type_append_name_to_scratch(param_type, param_name);
			uint32_t param_end = scratch_buffer.len;

			OUTPUT("%.*s", (param_end - param_start), &scratch_buffer.str[param_start]);
			scratch_buffer.len = param_start;
		}
		//		VECEACH(decl->func_decl.function_signature.params, i)
		for (unsigned i = 1; i < params_size; i++)
		{
			Type* param_type = decl->func_decl.function_signature.params[i]->type;
			const char* param_name = decl->func_decl.function_signature.params[i]->name;

			uint32_t param_start = scratch_buffer.len;
			scratch_buffer_append_len(", ", 2);
			c_type_append_name_to_scratch(param_type, param_name);
			uint32_t param_end = scratch_buffer.len;

			OUTPUT("%.*s", (param_end - param_start), &scratch_buffer.str[param_start]);
			scratch_buffer.len = param_start;
			/*
			llvm_emit_parameter(c, decl->func_decl.function_signature.params[i], prototype->abi_args[i], &arg, i);
			*/
		}

		OUTPUT("%.*s", 2, ") ");
	}


	// output the function
	OUTPUT("%s", "{\n");
	AstId current = astptr(decl->func_decl.body)->compound_stmt.first_stmt;
	while (current)
	{
		//llvm_emit_stmt(c, ast_next(&current));
		c_emit_stmt(file, context, indent, ast_next(&current));
	}
	OUTPUT("%s", "}\n");

}
/*
void llvm_emit_function_body(GenContext *c, Decl *decl)
{
	DEBUG_LOG("Generating function %s.", decl->extname);
	assert(decl->backend_ref);

	bool emit_debug = llvm_use_debug(c);
	LLVMValueRef prev_function = c->function;
	LLVMBuilderRef prev_builder = c->builder;


	c->error_var = NULL;
	c->catch_block = NULL;

	c->function = decl->backend_ref;
	if (emit_debug)
	{
		c->debug.function = LLVMGetSubprogram(c->function);
		if (c->debug.enable_stacktrace)
		{
			scratch_buffer_clear();
			scratch_buffer_append(decl->unit->module->name->module);
			scratch_buffer_append("::");
			scratch_buffer_append(decl->name ? decl->name : "anon");
			c->debug.func_name = llvm_emit_zstring(c, scratch_buffer_to_string());

			File *file = source_file_by_id(decl->span.file_id);
			c->debug.file_name = llvm_emit_zstring(c, file->name);
		}
	}

	c->cur_func_decl = decl;

	LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(c->context, c->function, "entry");
	c->current_block = entry;
	c->current_block_is_target = true;
	c->in_block = 0;
	c->builder = LLVMCreateBuilderInContext(c->context);
	LLVMPositionBuilderAtEnd(c->builder, entry);

	LLVMValueRef alloca_point = LLVMBuildAlloca(c->builder, LLVMInt32TypeInContext(c->context), "alloca_point");
	c->alloca_point = alloca_point;

	FunctionPrototype *prototype = decl->type->func.prototype;
	unsigned arg = 0;

	if (emit_debug)
	{
		llvm_debug_scope_push(c, c->debug.function);
		if (c->debug.enable_stacktrace)
		{
			LLVMTypeRef slot_type = c->debug.stack_type;
			LLVMTypeRef ptr_to_slot_type = LLVMPointerType(slot_type, 0);
			if (!c->debug.last_ptr)
			{
				const char *name = ".$last_stack";
				LLVMValueRef last_stack = c->debug.last_ptr = llvm_add_global_type(c, name, ptr_to_slot_type, 0);
				LLVMSetThreadLocal(last_stack, true);
				LLVMSetInitializer(last_stack, LLVMConstNull(ptr_to_slot_type));
				llvm_set_weak(c, last_stack);
			}
			AlignSize alignment = llvm_abi_alignment(c, slot_type);
			c->debug.stack_slot = llvm_emit_alloca(c, slot_type, alignment, ".$stackslot");
			AlignSize align_to_use;
			LLVMValueRef prev_ptr = llvm_emit_struct_gep_raw(c, c->debug.stack_slot, slot_type, 0, alignment, &align_to_use);
			llvm_store(c, prev_ptr, LLVMBuildLoad2(c->builder, ptr_to_slot_type, c->debug.last_ptr, ""), align_to_use);
			LLVMValueRef func_name = llvm_emit_struct_gep_raw(c, c->debug.stack_slot, slot_type, 1, alignment, &align_to_use);
			llvm_store(c, func_name, c->debug.func_name, align_to_use);
			LLVMValueRef file_name = llvm_emit_struct_gep_raw(c, c->debug.stack_slot, slot_type, 2, alignment, &align_to_use);
			llvm_store(c, file_name, c->debug.file_name, align_to_use);
			c->debug.stack_slot_row = llvm_emit_struct_gep_raw(c, c->debug.stack_slot, slot_type, 3, alignment, &align_to_use);
			llvm_store(c, c->debug.last_ptr, c->debug.stack_slot, type_alloca_alignment(type_voidptr));
		}
	}

	c->failable_out = NULL;
	c->return_out = NULL;
	if (prototype->ret_abi_info->kind == ABI_ARG_INDIRECT)
	{
		if (prototype->is_failable)
		{
			c->failable_out = LLVMGetParam(c->function, arg++);
		}
		else
		{
			c->return_out = LLVMGetParam(c->function, arg++);
		}
	}
	if (prototype->ret_by_ref_abi_info)
	{
		assert(!c->return_out);
		c->return_out = LLVMGetParam(c->function, arg++);
	}


	if (!decl->func_decl.attr_naked)
	{
		// Generate LLVMValueRef's for all parameters, so we can use them as local vars in code
		VECEACH(decl->func_decl.function_signature.params, i)
		{
			llvm_emit_parameter(c, decl->func_decl.function_signature.params[i], prototype->abi_args[i], &arg, i);
		}
	}

	LLVMSetCurrentDebugLocation2(c->builder, NULL);

	assert(decl->func_decl.body);
	AstId current = astptr(decl->func_decl.body)->compound_stmt.first_stmt;
	while (current)
	{
		llvm_emit_stmt(c, ast_next(&current));
	}

	if (c->current_block && llvm_basic_block_is_unused(c->current_block))
	{
		LLVMBasicBlockRef prev_block = LLVMGetPreviousBasicBlock(c->current_block);
		LLVMDeleteBasicBlock(c->current_block);
		c->current_block = prev_block;
		LLVMPositionBuilderAtEnd(c->builder, c->current_block);
	}
	// Insert a return (and defer) if needed.
	if (c->current_block && !LLVMGetBasicBlockTerminator(c->current_block))
	{
		llvm_emit_return_implicit(c);
	}

	// erase alloca point
	if (LLVMGetInstructionParent(alloca_point))
	{
		c->alloca_point = NULL;
		LLVMInstructionEraseFromParent(alloca_point);
	}

	LLVMDisposeBuilder(c->builder);
	c->builder = NULL;

	if (llvm_use_debug(c))
	{
		llvm_debug_scope_pop(c);
	}

	c->builder = prev_builder;
	c->function = prev_function;
}
*/