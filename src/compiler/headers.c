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

static void header_gen_function_decl(FILE *file, SemaContext *c, Decl *decl)
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
	} else if (decl->name && *decl->name) {
		OUTPUT("%s", decl->name);
	} else {
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
		} else {
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
		} else {
			INDENT();
			OUTPUT("/* a member named [%s] : type[%i] */\n", member->name, member->decl_kind);
			//TODO;
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
	//decl->enums
	//decl->strukt
	header_gen_enum_members(file, indent, decl->enums.values);
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

static void header_gen_var(FILE *file, SemaContext *c, Decl *decl)
{
	fprintf(file, "/* vars */\n");
}

void header_gen(Module *module)
{
	if (!vec_size(module->units))
		return;
	//TODO
	CompilationUnit *unit = module->units[0];
	const char *filename = str_cat(unit->file->name, ".h");

	SemaContext ctx;
	sema_context_init(&ctx, unit);

	printf("writing header: %s\n", filename);

	FILE *file = fopen(filename, "w");
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
		
		}
			//llvm_emit_function_body(gen_context, decl);
	}
	if (unit->main_function)
		//llvm_emit_function_body(gen_context, unit->main_function);

	VECEACH(unit->methods, i)
	{
		Decl* decl = unit->methods[i];
		if (decl->func_decl.body) {
		
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
	TODO;
}

void c_emit_if(FILE* file, void* context, int indent, Ast* ast) {
	
	INDENT();

	Expr* cond = exprptr(ast->if_stmt.cond);
	OUTPUT("%s", "if (");
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
	TODO;
}

void c_emit_statement_chain(FILE* file, void* context, int indent, AstId current) {

	while (current)
	{
		c_emit_stmt(file, context, indent, ast_next(&current));
		//llvm_emit_stmt(c, ast_next(&current));
	}
	//TODO;
}

//GenContext *context
void c_emit_return(FILE* file, struct GenContext* context, int indent, Ast* ast) {
	INDENT();

	Expr* expr = ast->return_stmt.expr;
	//expr->type
	Type* return_ty = expr->type; //? the return type?

	if (type_is_failable(return_ty)) //context->cur_func_decl->type->func.prototype->rtype
	{
		//struct BEValue be_value = { 0 };
		c_emit_expr(file, context, indent, NULL, expr->inner_expr);
		c_emit_statement_chain(file, context, indent, ast->return_stmt.cleanup);
		//c_emit_statement_chain(file, context, indent, ast->return_stmt.cleanup);
		//c_emit_return_abi(file, context, indent, , );
	}

	OUTPUT("%s", "return");

	bool has_return_value = ast->return_stmt.expr != NULL;
	if (has_return_value) {
		OUTPUT("%s", " ");
		c_emit_expr(file, context, indent, NULL, ast->return_stmt.expr);
	}
	OUTPUT("%s", ";\n");
	TODO;
}

void c_emit_compound_stmt(FILE* file, void* context, int indent, Ast* ast)
{	
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
void c_emit_expr(FILE* file, void* context, int indent, struct BEValue* value, Expr* expr) {
	TODO;
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
	} else {
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

void c_emit_stmt(FILE* file, void* context, int indent, Ast* ast) {
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
		UNREACHABLE
	case AST_SWITCH_STMT:
		//gencontext_emit_switch(c, ast);
		break;
	}
}

void c_emit_function_body(FILE* file, void* context, int indent, Decl* decl) {
	assert(decl->func_decl.body);
	AstId current = astptr(decl->func_decl.body)->compound_stmt.first_stmt;
	while (current)
	{
		//llvm_emit_stmt(c, ast_next(&current));
		c_emit_stmt(file, context, indent, ast_next(&current));
	}
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