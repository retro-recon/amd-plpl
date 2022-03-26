/* August 29, 1986 : DBTEST.C reads the database */
#include "testdef.h"
/*  -   -   -   -   -  */
vctr_get_index(nmbr_chr)
char nmbr_chr[];
{ int i,p;

i = HIGH;
lex(nmbr_chr);
if (nmbr_chr[0] == '-')
 { lex(nmbr_chr);
   i = LOW;
 }
switch(token_type)
 { case NUMBER
     : p = (int)atoi(nmbr_chr);
       if (i == LOW)
         return(-p);
       return(p);
   default
     : err(FATAL,"need an index/number or constant value",0,0,0);
 } /* SWITCH */
return(NULL_VALUE);
} /* VCTR_GET_INDEX */
/*  -   -   -   -   -  */
check_pin_type()
/* returns the pin type */
{
lex(icon);
if (!strcmp(icon,"input"))
  return(INPUT);
if (!strcmp(icon,"output"))
  return(OUTPUT);
if (!strcmp(icon,"io"))
  return(IO);
if (!strcmp(icon,"breg"))
  return(BREG);
if (!strcmp(icon,"vcc"))
  return(VCC);
if (!strcmp(icon,"gnd"))
  return(GND);
if (!strcmp(icon,"clock"))
  return(CLOCK);
if (!strcmp(icon,"clk_input"))
  return(CLK_INPUT);
if (!strcmp(icon,"control"))
  return(CONTROL);
err(FATAL,"no such input type/feature",0,0,0);
return(NULL_VALUE);
} /* CHECK_PIN_TYPE */
/*  -   -   -   -   -  */
check_and_array(limits)
/* checks that the number obtained from the database file
   is valid and within the LIMITS of the device; the limits
   for the AND array are from 0 to LIMITS */
int limits;
{ int i;
i = vctr_get_index(icon);
if (i == -1)
  return(i);
if (i == NULL_VALUE || (i < 0 || i > (limits-1)))
  err(FATAL,"invalid variable input value for AND array",0,0,0);
return(i);
} /* CHECK_AND_ARRAY */
/*  -   -   -   -   -  */
stop_def()
/* use '@' symbol to terminate definition for this pin */
{
lex(icon);
if (icon[0] != '@')
  err(FATAL,"use @ symbol to stop definition for this pin",0,0,0);
} /* STOP_DEF */
/*  -   -   -   -   -  */
get_valid_value(chr_str)
char chr_str[];
{ int i;
i = vctr_get_index(chr_str);
if (i == NULL_VALUE)
  err(FATAL,"expected a positive decimal number",0,0,0);
return(i);
} /* GET_VALID_VALUE */
/*  -   -   -   -   -  */
get_pterms(pinpt)
/* get number of PTs for pin X */
int pinpt;
{ struct type3 *ptr_get;
ptr_get = (struct type3 *)find_pin(pinpt);
ptr_get->num_pterms = get_valid_value(icon);  /* get the number of PTs */
if (ptr_get->num_pterms != 0)           /* get starting PT addr  */
  ptr_get->PTaddr = get_valid_value(icon);
} /* GET_PTERMS */
/*  -   -   -   -   -  */
get_3_common_functions(pin_x)
/* get PTs for 3 common functions tristate/enable, preset, reset */
int pin_x;
{ struct type3 *ptr_get;

ptr_get = (struct type3 *)find_pin(pin_x);
ptr_get->tristate = get_valid_value(icon);  /* get enable PT info */
if (ptr_get->tristate != 0)           /* if 0 this means no enable PT */
  ptr_get->tri_1st = get_valid_value(icon);
ptr_get->preset = get_valid_value(icon);    /* get sync preset INFO */
if (ptr_get->preset != 0)
  ptr_get->pre_1st = get_valid_value(icon);
ptr_get->reset = get_valid_value(icon);     /* get async reset INFO */
if (ptr_get->reset != 0)
  ptr_get->reset_1st = get_valid_value(icon);
} /* GET_3_COMMON_FUNCTIONS */
/*  -   -   -   -   -  */
insert(func_or_fuse,the_pin,new_cont_opt,name_func_fs,an_addr,state_func_fs)

/* If PT_or_FUSE is a FUSE, then create a record with the NAME_FUNC_FS
   at AN_ADDR and FUNC_FS_STATE (to set this fuse for THE_PIN).
   NEW_CONT_OPT specifies if this is a new option line or a continuation
   of the previous fuse option.
   Ex: ENABLE_HIGH and _LOW => ENABLE_HIGH wil be the new option
       while ENABLE_LOW will be a continuation of the enable option list
   If FUNC_or_FUSE is a FUNCTION, then specify set the TYPE0 field FUNC_FUSE
   as a FUNCTION and insert into the list with the data */

int func_or_fuse,the_pin,new_cont_opt;
char name_func_fs[];
int an_addr,state_func_fs;
{ struct type2 *ptr1, *ptr2;
  struct type3 *pin_key;

if (the_pin < 0 || the_pin > num_pins)
  inform("Program error: invalid pin detected",STOP);
/* create a record */
typ2_entry = (struct type2 *)malloc(sizeof(struct type2));
mem_check((struct type0 *)NULL,(struct type1 *)NULL,typ2_entry,
          (struct type3 *)NULL);
strcpy(typ2_entry->opt_name,name_func_fs);
typ2_entry->selected = new_cont_opt;
  /* if NEW_CONT_OPTION is YES, then this is the default option, if NO, then
      this is one of two options in the SPEC_OPTIONS sublist */
typ2_entry->selected = new_cont_opt; /* If YES, then this is the default
          option, if no, then this is one of many suboptions */
typ2_entry->addr_func_fuse = an_addr;
typ2_entry->count_state = state_func_fs;
typ2_entry->spec_options = NULL;
typ2_entry->typ2_next = NULL;

pin_key = (struct type3 *)find_pin(the_pin);

if (func_or_fuse == FUSE)
 { typ2_entry->func_fuse = FUSE;
                   /* find end of SPEC_FUNCTION list */
   for (ptr1 = pin_key->spec_function;
        ptr1->typ2_next != NULL;
        ptr1 = ptr1->typ2_next)
     ;
   if (new_cont_opt == YES) /* a new option list */
     ptr1->typ2_next = typ2_entry;
   else
     { if (ptr1->spec_options == NULL)
         ptr1->spec_options = typ2_entry;
       else
         { for (ptr2 = ptr1->spec_options;
                ptr2->spec_options != NULL;
                ptr2 = ptr2->spec_options)
             ;
           ptr2->spec_options = typ2_entry;
         } /* ELSE */
     } /* ELSE */
   return;
 } /* IF */
typ2_entry->func_fuse = FUNCTION;
for (ptr1 = pin_key->spec_function;  /* find end of SPEC_FUNCTION list */
     ptr1->typ2_next != NULL;
     ptr1 = ptr1->typ2_next)
  ;
ptr1->typ2_next = typ2_entry;   /* a new PT list */
} /* INSERT */
/*  -   -   -   -   -  */
get_arch_fuses(pin_y)
int pin_y;
{ int i,m,new_option;
  char buf[MAX_NAME];

for (new_option = YES;
     icon[0] != '@';
     lex(icon),new_option = YES,m = NULL_VALUE)
 { switch(icon[0])
    { case '+' : new_option = NO;
      case '#'
        : lex(icon);
          strcpy(buf,icon);
          if (token_type != VARIABLE)
            err(FATAL,"use a name for architecture fuse",0,0,0);
          i = get_valid_value(icon); /* get fuse addr */
          if (i >= 0)
             m = get_valid_value(icon); /* 0/1 to set this fuse */
          insert(FUSE,pin_y,new_option,buf,i,m);
          break;
      case '&' :
      case '!'
        : get_new_token = NO;
          return;
      default
        : err(FATAL,"database creation error: must see #/+/!",0,0,0);
    } /* SWITCH */
 } /* FOR */
get_new_token = NO;
} /* GET_ARCH_FUSES */
/*  -   -   -   -   -  */
get_special_PTs(a_pin)  /* get special PTs */
int a_pin;
{ char pt_name[MAX_NAME];
  int spec_pt_addr,num_spec_pt; /* number of special PTs & starting PT addr */

lex(icon);
strcpy(pt_name,icon);
if (token_type != VARIABLE)
  err(FATAL,"invalid special PT name",0,0,0);
num_spec_pt = get_valid_value(icon);
spec_pt_addr = get_valid_value(icon);
insert(FUNCTION,a_pin,YES,pt_name,spec_pt_addr,num_spec_pt);
} /* GET_SPECIAL_PTS */
/*  -   -   -   -   -  */
get_ctrl_pin(z_pin)
/* get the pin names for the control pins that control this pin; this is used
   by the simulator only, therefore this function just reads and disregards
   the data read here */
int z_pin;
{
lex(icon);
if (token_type != VARIABLE)
  err(FATAL,"invalid control pin name",0,0,0);
z_pin = get_valid_value(icon);
} /* GET_CTRL_PIN */
/*  -   -   -   -   -  */
get_functions(pinx)  /* get architecture fuses or special functions/PTs */
int pinx;
{
for (lex(icon); icon[0] != '@'; lex(icon))
 { switch(icon[0])
    { case '#'
        : get_arch_fuses(pinx);
          break;
      case '!'   /* special PTs */
        : get_special_PTs(pinx);
          break;
      case '&'  /* special pin controls */
        : get_ctrl_pin(pinx);
          break;
      default
        : err(FATAL,"error in specifying architecture fuses/special PTs",
              0,0,0);
     } /* SWITCH */
 } /* FOR */
get_new_token = NO;
} /* GET_FUNCTIONS */
/*  -   -   -   -   -  */
check_fdbk_src(a_tmp)
char a_tmp[MAX_NAME];
{ int i;
i = get_valid_value(a_tmp);
switch (i)
 { case LOW_FDBK
     :
   case HIGH_FDBK
     :
   case CORRECT_FDBK
     : /* means true/complement signals are correct; see MAP.23 */
   case NO_FDBK
     : return (i);
   default
     : err(FATAL,
               "invalid feedback source; use 0 (HIGH), 1 (LOW), or 2 (NONE)",
           0,0,0);
 } /* SWITCH */
return(NULL_VALUE);
} /* CHECK_FDBK_SRC */
/*  -   -   -   -   -  */
check_polarity()
{ int i;

i = get_valid_value(icon);
switch (i)
 { case LOW
     :
   case HIGH
     :
   case PROGRAMMABLE
     : return (i);
   default
     : err(FATAL,
               "polarity specification error in database; use 0, 1, or 2",
               0,0,0);
 } /* SWITCH */
} /* CHECK_POLARITY */
/*  -   -   -   -   -  */
read_part()
{ FILE *tmp_ptr;
  int i,r;
  struct type3 *ptr_key;

tmp_ptr = input_file;
  /* hold the INPUT_FILE ptr temporarily since all icons/tokens
     are taken from the file called INPUT_FILE via the function LEX */
input_file = part_file;
get_line();
num_pins = vctr_get_index(icon);  /* get the virtual number of pins */
physical_pins = vctr_get_index(icon);
  /* get the physical number of pins */
input_array = vctr_get_index(icon);
if (num_pins == NULL_VALUE || input_array == NULL_VALUE)
  err(FATAL,"incorrect database file",0,0,0);
if (input_array >= INPUT_ARRAY_SIZE)
 { inform("Program Limits exceeded: change the constant",CONTINUE);
   inform("INPUT_ARRAY_SIZE if you have the source code",STOP);
 }
for (r = 1; r <= num_pins; ++r)
 { i = vctr_get_index(icon);
   if (i < 1 || i > num_pins)
     err(FATAL,"no such pin in this device",0,0,0);
   for (ptr_key = pins; ptr_key != NULL; ptr_key = ptr_key->next_pin)
    { if (ptr_key->pin_nmbr == i)
        err(FATAL,"pin redefinition",0,0,0);
    } /* FOR */
   if (i == NULL_VALUE) /* I is not a number */
     err(FATAL,"unknown symbol in databse file",0,0,0);
   new_pin(i);
   ptr_key = (struct type3 *)find_pin(i);
   ptr_key->pin_type = check_pin_type();
  /* if pin type is BREG, then discard the next entry which is the pin
     associated with it; this is used by the simulator */
   if (ptr_key->pin_type == BREG)
     vctr_get_index(icon);
   switch(ptr_key->pin_type)
    { case INPUT : ptr_key->polarity = HIGH; /* see MAP.23 */
      case OUTPUT:
      case IO   :
      case BREG :
      case CLK_INPUT
        : ptr_key->true = check_and_array(input_array);
          if (ptr_key->true == -1)
            ptr_key->true = NULL_VALUE;
          else
            ptr_key->complement = check_and_array(input_array);
          lex(icon);
          if (icon[0] == '@')
            break;
          else
            get_new_token = NO;
          ptr_key->fdbkpath = check_fdbk_src(icon);
          ptr_key->polarity = check_polarity();
          get_pterms(i);  /* get number of PTs for pin i */
          get_3_common_functions(i);  /* get PTs for 3 common functions */
          get_functions(i);
          stop_def();
          break;
      case VCC   : /* power,ground, CLOCK or control pins */
      case GND   :
      case CONTROL:
      case CLOCK : stop_def();
                   break;  /* dedicated clock pin */
      default
        : err(FATAL,"Program Error 1",0,0,0);
    } /* SWITCH */
 } /* FOR */
if (part_file != NULL)  /* close the database file */
 { fclose(part_file);
   part_file = NULL;
 }
input_file = tmp_ptr;  /* get tokens from the input source file */
} /* READ_PART */
