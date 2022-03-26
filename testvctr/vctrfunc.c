/* [August 29, 1986] VCTRFUNC.C : Test Vector functions */
#include "testdef.h"
/* - - - - - - - - - - */
convert(base,nmbr,pattern)
/* convert using BASE the number NMBR and put the result in PATTERN */
char base;  /* can take the values B,O,D,H for Bin, Octal, Dec, and Hex */
char nmbr[],pattern[];
{ int i,j,k,value,num_type;
  char buf[MAX_NAME];

/* convert everything to decimal first */

num_type = DECIMAL;
if (nmbr[0] == '#')  /* take out the radices */
 { switch(nmbr[1])
    { case 'b' :
      case 'B' : num_type = BINARY;
                 break;
      case 'o' :
      case 'O' : num_type = OCTAL;
                 break;
      case 'd' :
      case 'D' : num_type = DECIMAL;
                 break;
      case 'h' :
      case 'H' : num_type = HEXADECIMAL;
                 break;
      default  : err(FATAL,"unkmown number base",0,0,0);
    } /* SWITCH */
   for (i=2, j=0; nmbr[i] != '\0'; ++i)
     buf[j++] = nmbr[i];
   buf[j] = '\0';
 }
else /* NMBR was decimal */
  strcpy(buf,nmbr);
switch(num_type)
 { case OCTAL       : sscanf(buf,"%o",&value); break;
   case HEXADECIMAL : sscanf(buf,"%x",&value); break;
   case DECIMAL     : sscanf(buf,"%d",&value); break;
   case BINARY
       : i = strlen(buf);
         for (j=i-1,k=1,value=0; j >= 0; --j)
          { if (j!= i-1)
              k = k*2;
            if (buf[j] == '1')
              value = value+k;
          }
         break;
   default : value = (int)atoi(nmbr);
             break;
 } /* SWITCH */
switch(base)
 { case 'b' :
   case 'B' : if (num_type == BINARY)
               { strcpy(pattern,buf);
                 return;
               }
              for (i=0 ; i<16 ; ++i)
               { if (((value>>i)&(0x1)) == 1)
                   buf[i] = '1';
                 else
                   buf[i] = '0';
               }
              buf[i] = '\0';
              j = strlen(buf)-1;  /* reverse the string */
              for (i = j,pattern[j+1] = '\0';i >= 0; --i)
                pattern[j-i] = buf[i];
              break;
   case 'd' :
   case 'D' : sprintf(pattern,"%d",value);
              break;
   case 'o' :
   case 'O' : sprintf(pattern,"%o",value);
              break;
   case 'h' :
   case 'H' : sprintf(pattern,"%x",value);
              break;
 } /* SWITCH */
} /* CONVERT */
/* - - - - - - - - - - */
#ifndef unix
abs(an_int)  /* return the absolute value of AN_INT */
int an_int;
{
if (an_int < 0)
  return(-an_int);
return(an_int);
} /* ABS */
#endif
/* - - - - - - - - - - */
mem_check(mem_typ0,mem_typ1,mem_typ2,mem_typ3)
   /* check if memory was allocated */
struct type0 *mem_typ0;
struct type1 *mem_typ1;
struct type2 *mem_typ2;
struct type3 *mem_typ3;
{
if ((mem_typ0 == NULL) && (mem_typ1 == NULL) &&
    (mem_typ2 == NULL) && (mem_typ3 == NULL))
 { fprintf(stderr,"Not enough memory");
   exit(BAD_EXIT);
 }
} /* MEM_CHECK */
/* - - - - - - - - - - */
push(list,item,a_pin,ptlist)
/* push the item (if not '?') or the pin (if not NULL_VALUE) onto the list */
struct type1 *list;
char item;
int a_pin;
struct type1 *ptlist;  /* product term list */
{
if ((a_pin == NULL_VALUE && item == '?') || list == NULL)
  inform("Fatal Error: PUSH module\n",STOP);
if (ptlist == NULL) /* if no prod terms, then push the number onto the stack */
 { typ1_entry = (struct type1 *)malloc(sizeof(struct type1));
   mem_check((struct type0 *)NULL,typ1_entry,(struct type2 *)NULL,
             (struct type3 *)NULL);
   typ1_entry->pin_num = a_pin;
   typ1_entry->operator = item;
   typ1_entry->backptr = NULL;
   typ1_entry->typ1_next = NULL;
 } /* IF PTLIST == NULL ==> note that if it is not NULL, then the
      PTLIST is already in the copied subtree rooted by TYP0_ENTRY */
for (typ1_temp = list;  /* ====> find the tail of this list */
     typ1_temp->typ1_next != NULL;
     typ1_temp = typ1_temp->typ1_next)
  ;
typ1_temp->typ1_next = typ1_entry;
typ1_entry->backptr = typ1_temp;
} /* PUSH */
/* - - - - - - - - - - */
list_initialize()
/* initialize the PIN list array and other ptrs; this routine is necessary
   before using the LIST_INSERT routine */
{
vctr_temp = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,vctr_temp,(struct type2 *)NULL,
          (struct type3 *)NULL);
vctr_temp->pin_num = NULL_VALUE;
vctr_temp->operator = '?';
vctr_temp->backptr = NULL;
vctr_temp->typ1_next = NULL;

pin_vctr = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,pin_vctr,(struct type2 *)NULL,
          (struct type3 *)NULL);
pin_vctr->pin_num = NULL_VALUE;
pin_vctr->operator = '?';
pin_vctr->backptr = NULL;
pin_vctr->typ1_next = NULL;

vctr2 = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,vctr2,(struct type2 *)NULL,
          (struct type3 *)NULL);
vctr2->pin_num = NULL_VALUE;
vctr2->operator = '?';
vctr2->backptr = NULL;
vctr2->typ1_next = NULL;

vctr_list = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,vctr_list,(struct type2 *)NULL,
          (struct type3 *)NULL);
vctr_list->pin_num = NULL_VALUE;
vctr_list->operator = '?';
vctr_list->backptr = NULL;
vctr_list->typ1_next = NULL;

pins = (struct type3 *)malloc(sizeof(struct type3));
mem_check((struct type0 *)NULL,(struct type1 *)NULL,
          (struct type2 *)NULL,pins);
pins->pin_nmbr = NULL_VALUE;
pins->pin_type = NULL_VALUE;
pins->pname_type = NULL_VALUE;
strcpy(pins->pname,"$@");
pins->true = NULL_VALUE;
pins->complement = NULL_VALUE;
pins->num_pterms = NULL_VALUE;
pins->fdbkpath = NULL_VALUE;
pins->polarity = NULL_VALUE;
pins->tristate = NULL_VALUE;
pins->tri_1st = NULL_VALUE;
pins->preset = NULL_VALUE;
pins->pre_1st = NULL_VALUE;
pins->reset = NULL_VALUE;
pins->reset_1st = NULL_VALUE;
pins->next_pin = NULL;
typ2_entry = (struct type2 *)malloc(sizeof(struct type2));
mem_check((struct type0 *)NULL,(struct type1 *)NULL,typ2_entry,
          (struct type3 *)NULL);
typ2_entry->func_fuse = NULL_VALUE;
strcpy(typ2_entry->opt_name,"$#");
typ2_entry->addr_func_fuse = NULL_VALUE;
typ2_entry->selected = NO;
typ2_entry->count_state = NULL_VALUE;
typ2_entry->spec_options = NULL;
typ2_entry->typ2_next = NULL;
pins->spec_function = typ2_entry;
} /* LIST_INITIALIZE */
/* - - - - - - - - - - */
/* add a new pin to the PINS list */
new_pin(a_new_pin)
int a_new_pin;
{ struct type3 *tmp1, *tmp2;

tmp1 = (struct type3 *)malloc(sizeof(struct type3));
mem_check((struct type0 *)NULL,(struct type1 *)NULL,
          (struct type2 *)NULL,tmp1);
tmp1->pin_nmbr = a_new_pin;
tmp1->pin_type = NULL_VALUE;
tmp1->pname_type = NULL_VALUE;
strcpy(tmp1->pname,"$@");
tmp1->true = NULL_VALUE;
tmp1->complement = NULL_VALUE;
tmp1->num_pterms = NULL_VALUE;
tmp1->fdbkpath = NULL_VALUE;
tmp1->polarity = NULL_VALUE;
tmp1->tristate = NULL_VALUE;
tmp1->tri_1st = NULL_VALUE;
tmp1->preset = NULL_VALUE;
tmp1->pre_1st = NULL_VALUE;
tmp1->reset = NULL_VALUE;
tmp1->reset_1st = NULL_VALUE;
tmp1->next_pin = NULL;
typ2_entry = (struct type2 *)malloc(sizeof(struct type2));
mem_check((struct type0 *)NULL,(struct type1 *)NULL,typ2_entry,
          (struct type3 *)NULL);
typ2_entry->func_fuse = NULL_VALUE;
strcpy(typ2_entry->opt_name,"$#");
typ2_entry->addr_func_fuse = NULL_VALUE;
typ2_entry->selected = NO;
typ2_entry->count_state = NULL_VALUE;
typ2_entry->spec_options = NULL;
typ2_entry->typ2_next = NULL;
tmp1->spec_function = typ2_entry;
if (pins == NULL)
  pins = tmp1;
for (tmp2 = pins; tmp2->next_pin != NULL; tmp2 = tmp2->next_pin)
  ;
tmp2->next_pin = tmp1;
} /* NEW_PIN */
/* - - - - - - - - - - */
clean_list(list1)  /* free the space used by this list */
struct type1 *list1;
{ struct type1 *tmp;

tmp = list1;
while (list1 != NULL)
 { tmp = list1->typ1_next;
   free(list1);
   list1 = tmp;
 }
} /* CLEAN_LIST */
/* - - - - - - - - - - */
fuse_feature_check(a_pin,feature_name)
  /* check that the FEATURE is available for A_PIN */
int a_pin;
char feature_name[];
{ struct type2 *ptr1,*ptr2,*ptr3;
  struct type3 *pkey;

pkey = (struct type3 *)find_pin(a_pin);
for (ptr1 = pkey->spec_function;
     ptr1 != NULL;
     ptr1 = ptr1->typ2_next)
 { if (ptr1->func_fuse == FUSE)
    { for (ptr2 = ptr1; ptr2 != NULL; ptr2 = ptr2->spec_options)
      { if (!strcmp(ptr2->opt_name,feature_name))
         { for (ptr3 = ptr1; ptr3 != NULL; ptr3 = ptr3->spec_options)
             ptr3->selected = NO;
           ptr2->selected = YES;
           return(YES);  /* feature exists for this pin */
         } /* IF */
      } /* FOR */
    } /* IF */
 } /* FOR */
return(NO);
} /* CHECK_FEATURE */
/*  -   -   -   -   -   -   */
output_pin_check(out_type,pin_val)
/* check if OUT_TYPE refers to one of the valid output type options */
char out_type[];
int pin_val;
{ int temp_type;
  char a_msg[80],msg_buf[MAX_NAME];
  struct type3 *pin_key;

get_new_token = NO;
strcpy(icon,out_type);
temp_type = check_pin_type();
pin_key = (struct type3 *)find_pin(pin_val);
if (temp_type == pin_key->pin_type)
  return;
if (pin_key->pin_type == IO &&
    (temp_type == INPUT || temp_type == OUTPUT))
  return;
if (pin_key->pin_type == CLK_INPUT &&
    (temp_type == CLOCK || temp_type == INPUT))
  return;
if (pin_key->pin_type == OUTPUT && (temp_type == BREG))
  return;
if ((!strcmp(out_type,"inverted") || !strcmp(out_type,"active_low")
    || !strcmp(out_type,"low")) && pin_key->polarity == LOW)
  return;
if ((!strcmp(out_type,"noninverted") || !strcmp(out_type,"active_high")
    || !strcmp(out_type,"high")) && pin_key->polarity == HIGH)
  return;
strcpy(a_msg,"invalid pin feature for pin ");
sprintf(msg_buf,"[%d]",pin_val);
strcat(a_msg,msg_buf);
err(FATAL,a_msg,0,0,0);
} /* OUTPUT_PIN_CHECK */
/*  -   -   -   -   -   -   */
/* initialize the VCTRBUF with the VCTRCHR */
vctr_init(vctrbuf,vctr_chr,amount)
char vctrbuf[],vctr_chr;
int amount;
{ int i;
for (i = 0; i < amount; ++i)   /* initialize the vector */
  vctrbuf[i] = vctr_chr;
vctrbuf[i] = '\0';
} /* VCTR_INIT */
/*  -   -   -   -   -   -   */
/* test if the symbol can be used with the V_PTYPE */
valid_test_symbol(the_symbol,pin_loc,val_tv_buf)
char the_symbol;
int pin_loc;  /* pin location */
char val_tv_buf[];
{ int v_ptype,i;
  struct type3 *ptrxx;

ptrxx = (struct type3 *)find_pin(abs(pin_loc+1));
v_ptype = ptrxx->pin_type;
switch(the_symbol)
 { case 'C':
   case 'K':
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9': switch(v_ptype)
              { case INPUT:
                case IO   :
                case CONTROL:
                case CLOCK:
                case CLK_INPUT: break;
                case OUTPUT
                    : if (val_tv_buf[0] != 'B')
                       { for (i=0;i<physical_pins;++i)
                          { if (val_tv_buf[i] == 'P')
                              break;
                          } /* FOR */
                         if (i >= physical_pins)
                           err(FATAL,
                               "can only preload with B in pin 1 or P",
                               0,0,0);
                       } /* IF */
                      break;
                default: err(WARNING,"pin is not an input",token_type,0,0);
                         break;
              } /* SWITCH */
             break;
   case 'L':
   case 'H': switch(v_ptype)
              { case OUTPUT:
                case BREG:
                case IO: break;
                default: err(WARNING,"pin is not an output",token_type,0,0);
                         break;
              } /* SWITCH */
             break;
   case 'B': if (pin_loc != 0)
               err(WARNING,"B can only be put in first vector location",
                   token_type,0,0);
             break;
   case 'Z':
   case 'F': switch(v_ptype)
              { case INPUT:
                case CLOCK:
                case CLK_INPUT:
                case CONTROL:
                case OUTPUT:
                case BREG:
                case IO: break;
                default: err(WARNING,"cannot float this pin",token_type,0,0);
                         break;
              } /* SWITCH */
             break;
   case 'N': switch(v_ptype)
              { case VCC:
                case GND:
                case OUTPUT:
                case IO:
                case BREG: break;
                default: err(WARNING,"use only with power/gnd pins",
                             token_type,0,0);
                         break;
              } /* SWITCH */
             break;
   case 'P': switch(v_ptype)
              { case CLOCK:
                case CLK_INPUT: break;
                default: err(WARNING,"use only with CLOCK pins",
                             token_type,0,0);
                         break;
              } /* SWITCH */
             break;
   case 'X': switch(v_ptype)
              { case INPUT:
                case CLOCK:
                case CLK_INPUT:
                case CONTROL:
                case OUTPUT:
                case BREG:
                case IO: break;
                default: err(WARNING,"no default value for this pin",
                             token_type,0,0);
                         break;
              } /* SWITCH */
             break;
   default : inform("Invalid test vector symbol in VALID TEST",STOP);
             break;
 } /* SWITCH */
} /* VALID_TEST_SYMBOL */
/*  -   -   -   -   -   -   */
