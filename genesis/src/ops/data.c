/*
// Full copyright information is available in the file ../doc/CREDITS
*/

#include "defs.h"

#include "cdc_pcode.h"
#include "cdc_db.h"

COLDC_FUNC(size) {
    cData * args;
    Int      nargs,
             size;

    if (!func_init_0_or_1(&args, &nargs, 0))
	return;

    if (nargs) {
        size = size_data(&args[0]);
        pop(1);
    } else {
        size = size_object(cur_frame->object);
    }

    /* Push size of current object. */
    push_int(size);
}

COLDC_FUNC(type) {
    cData *args;
    Int type;

    /* Accept one argument of any type. */
    if (!func_init_1(&args, 0))
	return;

    /* Replace argument with symbol for type name. */
    type = args[0].type;
    pop(1);
    push_symbol(data_type_id(type));
}

COLDC_FUNC(class) {
    cData *args;
    Long cclass;

    /* Accept one argument of frob type. */
    if (!func_init_1(&args, FROB))
	return;

    /* Replace argument with class. */
    cclass = args[0].u.frob->cclass;
    pop(1);
    push_objnum(cclass);
}

COLDC_FUNC(toint) {
    cData *args;
    Long val = 0;

    /* Accept a string or integer to convert into an integer. */
    if (!func_init_1(&args, 0))
	return;

    switch (args[0].type) {
        case STRING:
            val = atol(string_chars(args[0].u.str)); break;
        case FLOAT:
            val = (Long) args[0].u.fval; break;
        case OBJNUM:
            val = args[0].u.objnum; break;
        case INTEGER:
            return;
        default:
            cthrow(type_id,
                   "The first argument (%D) is not an integer or string.",
                   &args[0]);
            return;
    }

    pop(1);
    push_int(val);
}

COLDC_FUNC(tofloat) {
      cData * args;
      float val = 0;
  
      /* Accept a string, integer or integer to convert into a float. */
      if (!func_init_1(&args, 0))
          return;
  
      switch (args[0].type) {
          case STRING:
              val = atof(string_chars(args[0].u.str)); break;
          case INTEGER:
              val = (float) args[0].u.val; break;
          case FLOAT:
              return;
          case OBJNUM:
              val = (float) args[0].u.objnum; break;
          default:
              cthrow(type_id,
                "The first argument (%D) is not an integer or string.",
                &args[0]);
              return;
      }
      pop(1);
      push_float(val);
}

COLDC_FUNC(tostr) {
    cData *args;
    cStr *str;

    /* Accept one argument of any type. */
    if (!func_init_1(&args, 0))
	return;

    /* Replace the argument with its text version. */
    str = data_tostr(&args[0]);

    pop(1);
    push_string(str);
    string_discard(str);
}

COLDC_FUNC(toliteral) {
    cData *args;
    cStr *str;

    /* Accept one argument of any type. */
    if (!func_init_1(&args, 0))
	return;

    /* Replace the argument with its unparsed version. */
    str = data_to_literal(&args[0], TRUE);
    pop(1);
    push_string(str);
    string_discard(str);
}

COLDC_FUNC(toobjnum) {
    cData *args;

    /* Accept an integer to convert into a objnum. */
    if (!func_init_1(&args, INTEGER))
	return;

    if (args[0].u.val < 0)
        cthrow(type_id, "Objnums must be 0 or greater");

    args[0].u.objnum = args[0].u.val;
    args[0].type = OBJNUM;
}

COLDC_FUNC(tosym) {
    cData *args;
    Long sym;
    char * s;

    /* Accept one string argument. */
    if (!func_init_1(&args, STRING))
	return;

    /* no NULL symbols */
    if (string_length(STR1) < 1)
        THROW((symbol_id, "Symbols must be one or more characters."))

    /* sometimes strings are not NULL terminated, make sure it is */
    s = string_chars(STR1);
    s[string_length(STR1)] = (char) NULL;

    /* this is wrong, we should check this everywhere, not just here,
       but at the moment everywhere assumes 'ident_get' returns a valid
       ident irregardless */
    if (!is_valid_ident(s))
        THROW((symbol_id, "Symbol contains non-alphanumeric characters."))

    sym = ident_get(s);

    pop(1);
    push_symbol(sym);
}

COLDC_FUNC(toerr) {
    cData *args;
    Long error;

    /* Accept one string argument. */
    if (!func_init_1(&args, STRING))
	return;

    error = ident_get(string_chars(args[0].u.str));
    pop(1);
    push_error(error);
}

COLDC_FUNC(valid) {
    cData *args;
    Int is_valid;

    /* Accept one argument of any type (only objnums can be valid, though). */
    if (!func_init_1(&args, 0))
	return;

    is_valid = (args[0].type == OBJNUM && cache_check(args[0].u.objnum));
    pop(1);
    push_int(is_valid);
}

