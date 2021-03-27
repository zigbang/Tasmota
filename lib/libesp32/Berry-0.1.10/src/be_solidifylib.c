/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
#include "be_object.h"
#include "be_module.h"
#include "be_string.h"
#include "be_vector.h"
#include "be_class.h"
#include "be_debug.h"
#include "be_map.h"
#include "be_vm.h"
#include "be_decoder.h"
#include <string.h>
#include <stdio.h>

#if BE_USE_SOLIDIFY_MODULE

#ifndef INST_BUF_SIZE
#define INST_BUF_SIZE   96
#endif

#define logbuf(...)     snprintf(__lbuf, sizeof(__lbuf), __VA_ARGS__)

#define logfmt(...)                     \
    do {                                \
        char __lbuf[INST_BUF_SIZE];     \
        logbuf(__VA_ARGS__);            \
        be_writestring(__lbuf);         \
    } while (0)

/* output only valid types for ktab, or NULL */
static const char * m_type_ktab(int type)
{
    switch (type){
        case BE_NIL:    return "BE_NIL";
        case BE_INT:    return "BE_INT";
        case BE_REAL:   return "BE_REAL";
        case BE_BOOL:   return "BE_BOOL";
        case BE_STRING: return "BE_STRING";
        default:        return NULL;
    }
}

static void m_solidify_closure(bvm *vm, bclosure *cl, int builtins)
{   
    bproto *pr = cl->proto;
    const char * func_name = str(pr->name);
    const char * func_source = str(pr->source);
    // logfmt("// == builtin_count %i\n", builtins);

    // logfmt("// type %i, ", cl->type);
    // logfmt("// marked %i, ", cl->marked);
    // logfmt("// nupvals %i\n", cl->nupvals);

    // logfmt("// PROTO:\n");
    // logfmt("// type %i, ", pr->type);
    // logfmt("// marked %i, ", pr->marked);
    // logfmt("// nstack %i, ", pr->nstack);
    // logfmt("// argcs %i, ", pr->argc);
    // // logfmt("// varg %i\n", pr->varg);

    // logfmt("// gray %p\n", (void*)pr->gray);
    // logfmt("// upvals %p\n", (void*)pr->upvals);
    // logfmt("// proto_tab %p (%i)\n", (void*)pr->ptab, pr->nproto);

    // logfmt("// name %s\n", str(pr->name));
    // logfmt("// source %s\n", str(pr->source));

    // logfmt("\n");

    // logfmt("// ktab %p (%i)\n", (void*)pr->ktab, pr->nconst);
    // for (int i = 0; i < pr->nconst; i++) {
    //     logfmt("// const[%i] type %i  (%s) %p", i, pr->ktab[i].type, be_vtype2str(&pr->ktab[i]), pr->ktab[i].v.p);
    //     if (pr->ktab[i].type == BE_STRING) {
    //         logfmt(" = '%s'", str(pr->ktab[i].v.s));
    //     }
    //     logfmt("\n");
    // }

    logfmt("\n");
    logfmt("/********************************************************************\n");
    logfmt("** Solidified function: %s\n", func_name);
    logfmt("********************************************************************/\n\n");

    /* create static strings for name and source */
    logfmt("be_define_local_const_str(%s_str_name, \"%s\", %i, 0, %u, 0);\n",
            func_name, func_name, be_strhash(pr->name), str_len(pr->name));
    logfmt("be_define_local_const_str(%s_str_source, \"%s\", %i, 0, %u, 0);\n",
            func_name, func_source, be_strhash(pr->source), str_len(pr->source));
    
    /* create static strings first */
    for (int i = 0; i < pr->nconst; i++) {
        if (pr->ktab[i].type == BE_STRING) {
            logfmt("be_define_local_const_str(%s_str_%i, \"",
                    func_name, i);
            be_writestring(str(pr->ktab[i].v.s));
            size_t len = strlen(str(pr->ktab[i].v.s));
            if (len >= 255) {
                be_raise(vm, "internal_error", "Strings greater than 255 chars not supported yet");
            }
            logfmt("\", %i, 0, %zu, 0);\n", be_strhash(pr->ktab[i].v.s), len >= 255 ? 255 : len);
        }
    }
    logfmt("\n");

    logfmt("static const bvalue %s_ktab[%i] = {\n", func_name, pr->nconst);
    for (int k = 0; k < pr->nconst; k++) {
        int type = pr->ktab[k].type;
        const char *type_name = m_type_ktab(type);
        if (type_name == NULL) {
            char error[64];
            snprintf(error, sizeof(error), "Unsupported type in function constants: %i", type);
            be_raise(vm, "internal_error", error);
        }
        if (type == BE_STRING) {
            logfmt("  { { .s=be_local_const_str(%s_str_%i) }, %s},\n", func_name, k, type_name);
        } else if (type == BE_INT) {
            logfmt("  { { .i=%" BE_INT_FMTLEN "i }, %s},\n", pr->ktab[k].v.i, type_name);
        } else if (type == BE_REAL) {
#if BE_USE_SINGLE_FLOAT
            logfmt("  { { .p=(void*)0x%08X }, %s},\n", (uint32_t) pr->ktab[k].v.p, type_name);
#else
            logfmt("  { { .p=(void*)0x%016llX }, %s},\n", (uint64_t) pr->ktab[k].v.p, type_name);
#endif
        } else if (type == BE_BOOL) {
            logfmt("  { { .b=%i }, %s},\n", pr->ktab[k].v.b, type_name);
        }
    }
    logfmt("};\n\n");

    logfmt("static const uint32_t %s_code[%i] = {\n", func_name, pr->codesize);
    for (int pc = 0; pc < pr->codesize; pc++) {
        uint32_t ins = pr->code[pc];
        logfmt("  0x%04X,  //", ins);
        be_print_inst(ins, pc);
        bopcode op = IGET_OP(ins);
        if (op == OP_GETGBL || op == OP_SETGBL) {
            // check if the global is in built-ins
            int glb = IGET_Bx(ins);
            if (glb > builtins) {
                // not supported
                logfmt("\n===== unsupported global G%d\n", glb);
                be_raise(vm, "internal_error", "Unsupported access to non-builtin global");
            }
        }
    }
    logfmt("};\n\n");

    logfmt("static const bproto %s_proto = {\n", func_name);
    // bcommon_header
    logfmt("  NULL,     // bgcobject *next\n");
    logfmt("  %i,       // type\n", pr->type);
    logfmt("  GC_CONST,        // marked\n");
    //
    logfmt("  %i,       // nstack\n", pr->nstack);
    logfmt("  %i,       // nupvals\n", pr->nupvals);
    logfmt("  %i,       // argc\n", pr->argc);
    logfmt("  %i,       // varg\n", pr->varg);
    if (pr->nproto > 0) {
        be_raise(vm, "internal_error", "unsupported non-null proto list");
    }
    logfmt("  NULL,     // bgcobject *gray\n");
    logfmt("  NULL,     // bupvaldesc *upvals\n");
    logfmt("  (bvalue*) &%s_ktab,     // ktab\n", func_name);
    logfmt("  NULL,     // bproto **ptab\n");
    logfmt("  (binstruction*) &%s_code,     // code\n", func_name);
    logfmt("  be_local_const_str(%s_str_name),       // name\n", func_name);
    logfmt("  %i,       // codesize\n", pr->codesize);
    logfmt("  %i,       // nconst\n", pr->nconst);
    logfmt("  %i,       // nproto\n", pr->nproto);
    logfmt("  be_local_const_str(%s_str_source),     // source\n", func_name);
    //
    logfmt("#if BE_DEBUG_RUNTIME_INFO /* debug information */\n");
    logfmt("  NULL,     // lineinfo\n");
    logfmt("  0,        // nlineinfo\n");
    logfmt("#endif\n");
    logfmt("#if BE_DEBUG_VAR_INFO\n");
    logfmt("  NULL,     // varinfo\n");
    logfmt("  0,        // nvarinfo\n");
    logfmt("#endif\n");
    logfmt("};\n\n");

    // closure
    logfmt("static const bclosure %s_closure = {\n", func_name);
    // bcommon_header
    logfmt("  NULL,     // bgcobject *next\n");
    logfmt("  %i,       // type\n", cl->type);
    logfmt("  GC_CONST,        // marked\n");
    //
    logfmt("  %i,       // nupvals\n", cl->nupvals);
    logfmt("  NULL,     // bgcobject *gray\n");
    logfmt("  (bproto*) &%s_proto,     // proto\n", func_name);
    logfmt("  { NULL }     // upvals\n");
    logfmt("};\n\n");

    logfmt("/*******************************************************************/\n\n");
}

#define be_builtin_count(vm) \
    be_vector_count(&(vm)->gbldesc.builtin.vlist)

static int m_dump(bvm *vm)
{
    if (be_top(vm) >= 1) {
        bvalue *v = be_indexof(vm, 1);
        if (var_isclosure(v)) {
            m_solidify_closure(vm, var_toobj(v), be_builtin_count(vm));
        }
    }
    be_return_nil(vm);
}

#if !BE_USE_PRECOMPILED_OBJECT
be_native_module_attr_table(solidify) {
    be_native_module_function("dump", m_dump),
};

be_define_native_module(solidify, NULL);
#else
/* @const_object_info_begin
module solidify (scope: global, depend: BE_USE_SOLIDIFY_MODULE) {
    dump, func(m_dump)
}
@const_object_info_end */
#include "../generate/be_fixed_solidify.h"
#endif

#endif /* BE_USE_SOLIFIDY_MODULE */
