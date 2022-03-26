/* September 28, 1987 ENHANCE.C contains the enhancement routines
   which will hopefully make the program more user-friendly. These
   include routines to:
    - write the calculated vectors to a file
    - generate a waveform output pattern
*/
#include "simdef.h"
/* -   -   -   -   -   */
/* remove the Preload symbols P and B from the vector */
strip_PRELOAD(vctrstring)
char vctrstring[];
{ int i;
for (i = 0; vctrstring[i] != '\0'; ++i) {
  switch(vctrstring[i]) {
    case 'P':
    case 'B':
      vctrstring[i] = '0';
      break;
  } /* SWITCH */
} /* FOR */
} /* STRIP_PRELOAD */
/* -   -   -   -   -   */
/* Program calculated vectors are written to the file pointer DST.
   USER_VCTR and PRG_VCTR (user and program calculated vectors) are
   sent so that the clocks that were taken out when APPLY() was called
   can be added again; VCOUNT = vector count.
*/
prg_calculated_vectors(dst,user_vctr,prg_vctr,vcount)
FILE *dst;
char user_vctr[],prg_vctr[];
int vcount;
{ int i;

if (dst == NULL)
  inform("NULL vctr file",STOP);
if (vcount == 1) {
  fprintf(dst,"%cN Vectors calculated by Functional Simulator for [%s]*",
              STX,jmapfile);
  fprintf(dst,"\nX%d*\n",dflt_test);  /* specify default value */
}
for (i = 0; i < num_pins; ++i) {
  switch(user_vctr[i]) { /* restore the clock signals */
    case 'C':
    case 'K': prg_vctr[i] = user_vctr[i];
    default : break;
  } /* SWITCH */
} /* FOR */
fprintf(dst,"V%04d ",vcount);
/* for devices with internal registers, put test
   vector elements after the physical pins */
if (num_pins > physical_pins) {
  for (i = 0; i < physical_pins; ++i)
    fprintf(dst,"%c",prg_vctr[i]);
  fprintf(dst,"* N $ ");
  for (i = physical_pins; i < num_pins; ++i)
    fprintf(dst,"%c",prg_vctr[i]);
} /* IF */
else
  fprintf(dst,"%s",prg_vctr);
fprintf(dst,"*\n");
} /* PRG_CALCULATED_VECTORS */
/*  -   -   -   -   -   */
/* draw the waveforms for the current vector CRNT_WAVE; use the
   LAST_WAVE pattern to determine which characters to send to the
   file FPTR; use CLK_VCTR to put back the clock signals
*/
draw_waves(fptr,last_wave,crnt_wave,clk_vctr,vcount)
FILE *fptr;
char last_wave[],crnt_wave[],clk_vctr[];
int vcount;  /* vector count */
{ int i;
  char prnt_this[5];

if (fptr == NULL)
  inform("NULL WAVE file",STOP);
if (last_wave[0] == '\0')
  strcpy(last_wave,crnt_wave);
for (i = 0; i < num_pins; ++i) {
  switch(clk_vctr[i]) { /* ---- restore the clock signals ---- */
    case 'C':
    case 'K': crnt_wave[i] = clk_vctr[i];
    default : break;
  } /* SWITCH */
} /* FOR */
fprintf(fptr,"\n%3d%c",vcount,VECTOR_MARK);
for (i = 0; i < num_pins; ++i) {
  prnt_this[0] = '\0';
  switch(crnt_wave[i]) {
    case 'P':
    case 'B':
      sprintf(prnt_this,"%c%c",crnt_wave[i],C_CLOCK);
      break;
    case 'C':
      sprintf(prnt_this,"%c%c",RIGHT_CROSS,C_CLOCK);
      break;
    case 'K':
      sprintf(prnt_this,"%c%c",RIGHT_CROSS,C_CLOCK);
      break;
    case '0':
      switch(last_wave[i]) {
        case 'N':
        case 'F':
        case 'Z':
        case 'X':
        case 'P':
        case 'B':
        case 'L':
        case 'C':
        case '0':
          sprintf(prnt_this,"%c%c",VERT_BAR,SPACE);
          break;
        case 'K':
        case '1':
          sprintf(prnt_this,"%c%c",TOP_LEFT,LOW_RIGHT);
          break;
        case 'H':
          sprintf(prnt_this,"%c%c",TOP_LEFT,RIGHT_LOW_OUT);
          break;
        default :
          sprintf(prnt_this,"Z ");
      } /* SWITCH */
      break;
    case '1':
      switch(last_wave[i]) {
        case 'N':
        case 'F':
        case 'Z':
        case 'X':
        case 'K':
        case 'H':
        case '1':
          sprintf(prnt_this,"%c%c",SPACE,VERT_BAR);
          break;
        case 'P':
        case 'B':
        case 'C':
        case '0':
          sprintf(prnt_this,"%c%c",LOW_LEFT,TOP_RIGHT);
          break;
        case 'L':
          sprintf(prnt_this,"%c%c",LEFT_LOW_OUT,TOP_RIGHT);
          break;
        default :
          sprintf(prnt_this,"Z ");
      } /* SWITCH */
      break;
    case 'L':
      switch(last_wave[i]) {
        case 'N':
        case 'F':
        case 'Z':
        case 'X':
        case 'C':
        case 'P':
        case 'B':
        case '0':
        case 'L':
          sprintf(prnt_this,"%c%c",DOUBLE_VERT,SPACE);
          break;
        case 'K':
        case 'H':
          sprintf(prnt_this,"%c%c",LEFT_TOP_OUT,RIGHT_LOW_OUT);
          break;
        case '1':
          sprintf(prnt_this,"%c%c",LEFT_TOP_OUT,LOW_RIGHT);
          break;
        default :
          sprintf(prnt_this,"Z ");
      } /* SWITCH */
      break;
    case 'H':
      switch(last_wave[i]) {
        case 'N':
        case 'F':
        case 'Z':
        case 'X':
        case 'K':
        case '1':
        case 'H':
          sprintf(prnt_this,"%c%c",SPACE,DOUBLE_VERT);
          break;
        case 'P':
        case 'B':
        case 'C':
        case 'L':
          sprintf(prnt_this,"%c%c",LEFT_LOW_OUT,RIGHT_TOP_OUT);
          break;
        case '0':
          sprintf(prnt_this,"%c%c",LOW_LEFT,RIGHT_TOP_OUT);
          break;
        default :
          sprintf(prnt_this,"Z ");
      } /* SWITCH */
      break;
    case 'F':
      sprintf(prnt_this,"%c ",IN_FLOAT);
      break;
    case 'N':
      sprintf(prnt_this," %c",NO_TEST);
      break;
    case 'Z':
      sprintf(prnt_this,"%c ",OUT_HI_Z);
      break;
    case 'X':
      sprintf(prnt_this,"%c ",DONT_CARE);
      break;
    default :
      sprintf(prnt_this,"+ ");
  } /* SWITCH */
  fprintf(fptr,"%s",prnt_this);
  if (vcount%4 == 0) {
    if ((i+1)%4 == 0)
      fprintf(fptr,"%c",CROSS);
  }
  else {
    if ((i+1)%4 == 0)
      fprintf(fptr,"%c",RIGHT_CROSS);
  }
} /* FOR */
} /* DRAW_WAVES */
/*  -   -   -   -   -   */
/* Traverse the equation tree and expand any defined PTs (@) */
eq_traverse(destfp,an_eq)
FILE *destfp;
struct node2class *an_eq;
{ int i;
  char trvbuf[80];
  struct PT_element *run;

if (an_eq == NULL)
  return;
if (an_eq->symbol == NULL || destfp == NULL)
  inform("EQ_TRV Err",STOP);
switch(an_eq->symbol[0]) {
  case '^':
    fprintf(destfp,"%d",an_eq->pin_no);
    break;
  case '/':
    fprintf(destfp,"/(");
    eq_traverse(destfp,an_eq->RIGHT_1);
    fprintf(destfp,")");
    break;
  case Q_OUTPUT:
  case EQNMBR:
    pairings(an_eq->pin_no,icon);
    fprintf(destfp,"%s",icon);
    break;
  case '*':
  case '+':
  case '|':
  case '%':
    if (null_eq(an_eq->LEFT_0) == NO)
      eq_traverse(destfp,an_eq->LEFT_0);
    i = null_eq(an_eq->RIGHT_1);
    if (i == NO) {
      if (an_eq->symbol[0] != '*')  /* put PTerms on separate lines */
        fprintf(destfp,"\n\t%c ",an_eq->symbol[0]);
      else
        fprintf(destfp,"%c",an_eq->symbol[0]);
      eq_traverse(destfp,an_eq->RIGHT_1);
    }
    break;
  case '@':
    for (i = 1; an_eq->symbol[i] != '\0'; ++i)
      trvbuf[i-1] = an_eq->symbol[i];
    trvbuf[i-1] = '\0';
    for (run = product_terms; run != NULL; run = run->next_PT) {
      if (!strcmp(trvbuf,run->term_name)) {
         eq_traverse(destfp,run->pin_eq);
         break;
      }
    } /* FOR */
    if (run == NULL) {
      sprintf(main_msg,"EQ_TRV [%s] not found",an_eq->symbol);
      inform(main_msg,STOP);
    } /* IF */
    break;
  default :
    for (run = product_terms; run != NULL; run = run->next_PT) {
      if (!strcmp(an_eq->symbol,run->term_name)) {
        eq_traverse(destfp,run->pin_eq);
        break;
      }
    } /* FOR */
    if (run == NULL) {
      sprintf(main_msg,"EQ_TRV [%s] not found",an_eq->symbol);
      inform(main_msg,STOP);
    } /* IF */
    break;
} /* SWITCH */
} /* EQ_TRAVERSE */
/*  -   -   -   -   -   */
/* if the COMPARAND is NULL, then print the FEATURE_LIST to the
   DEST file ptr; if COMPARAND is not NULL, then look for this item
   in the feature list and return a YES or NO
*/
list_arch(feature_list,dest,comparand)
struct dfn_class *feature_list;
FILE *dest;
char comparand[];
{ struct dfn_class *zptr;

for (zptr = feature_list; zptr != NULL; zptr = zptr->nxt_dfn) {
  if (zptr->class1_name[0] != '\0') {
    if (zptr->class1_name[0] == '$') {
      zptr->class1_name[0] = ' ';
      if (dest == NULL) {
        if (!strcmp(comparand,zptr->class1_name)) {
          zptr->class1_name[0] = '$';  /* return to original state */
          return(YES);
        }
      } /* IF */
      else
        fprintf(dest,"%s",zptr->class1_name);
      zptr->class1_name[0] = '$';  /* return to original state */
    } /* IF */
  } /* IF */
} /* FOR */
return(NO);
} /* LIST_ARCH */
/*  -   -   -   -   -   */
/* Go to the basic unit of the logic equation and check if
   the equation is NULL:
    - if it is a PT (@PTname), check if it is NULL;
    - if it is a logic equation, then this is non-null;
    - if a pin, then also non-null
*/
null_eq(eq_x)
struct node2class *eq_x;
{ struct PT_element *run;
  int left_result,right_result,x;
  char xtrabuf[80];

if (eq_x == NULL)
  return(YES);
if (eq_x->symbol == NULL)
  inform("NULL_EQ Err",STOP);
switch(eq_x->symbol[0]) {
  case '^':   /* constant */
    return(NO);
  case EQNMBR:
  case Q_OUTPUT:
    return(NO);
  case '@':
    for (x = 1; eq_x->symbol[x] != '\0'; ++x)
      xtrabuf[x-1] = eq_x->symbol[x];
    xtrabuf[x-1] = '\0';
    for (run = product_terms; run != NULL; run = run->next_PT) {
     if (!strcmp(xtrabuf,run->term_name))
       break;
    } /* FOR */
    if (run == NULL) {
      sprintf(main_msg,"Cannot find PT [%s]",xtrabuf);
      pr_msgs(main_msg,NO);
    }
    return(null_eq(run->pin_eq));
  case '/':
    return(null_eq(eq_x->RIGHT_1));
  case '*':
  case '+':
  case '%':
  case '|':
    left_result  = null_eq(eq_x->LEFT_0 );
    right_result = null_eq(eq_x->RIGHT_1);
    if (left_result == YES && right_result == YES)
      return(YES);
    return(NO);
  default:
    for (run = product_terms; run != NULL; run = run->next_PT) {
      if (!strcmp(run->term_name,eq_x->symbol))
        return(null_eq(run->pin_eq));
    } /* FOR */
    sprintf(main_msg,"NULL_EQ [%s] not found",eq_x->symbol);
    inform(main_msg,STOP);
} /* SWITCH on EQ_X->pin_eq->symbol[0] */
return(YES);
} /* NULL_EQ */
/*  -   -   -   -   -   */
/* write the pins equations: logic, enable, special functions, etc. */
eq_write(a_pin_item,dest)
struct pin_element *a_pin_item;
FILE *dest;
{ unsigned int cell_class;

if (a_pin_item->reg_latch.PTreset != NULL) {
  if (null_eq(a_pin_item->reg_latch.PTreset->pin_eq) == NO) {
    pairings(a_pin_item->vctr_element,icon);
    fprintf(dest,"\nRESET(%s) = ",icon);
    eq_traverse(dest,a_pin_item->reg_latch.PTreset->pin_eq);
    fprintf(dest,";");
  }
}
if (a_pin_item->reg_latch.PTset != NULL) {
  if (null_eq(a_pin_item->reg_latch.PTset->pin_eq) == NO) {
    pairings(a_pin_item->vctr_element,icon);
    fprintf(dest,"\nSET(%s) = ",icon);
    eq_traverse(dest,a_pin_item->reg_latch.PTset->pin_eq);
    fprintf(dest,";");
  }
}
if (a_pin_item->reg_latch.PTobserve != NULL) {
  if (null_eq(a_pin_item->reg_latch.PTobserve->pin_eq) == NO) {
    pairings(a_pin_item->vctr_element,icon);
    fprintf(dest,"\nOBSERVE(%s) = ",icon);
    eq_traverse(dest,a_pin_item->reg_latch.PTobserve->pin_eq);
    fprintf(dest,";");
  }
}
if (a_pin_item->reg_latch.PTenable != NULL) {
  if (null_eq(a_pin_item->reg_latch.PTenable->pin_eq) == NO) {
    pairings(a_pin_item->vctr_element,icon);
    fprintf(dest,"\nENABLE(%s) = ",icon);
    eq_traverse(dest,a_pin_item->reg_latch.PTenable->pin_eq);
    fprintf(dest,";");
  }
}
cell_class = a_pin_item->reg_latch.store_type &0x0F;
if (a_pin_item->reg_latch.PTmem0_input != NULL) {
  if (null_eq(a_pin_item->reg_latch.PTmem0_input->pin_eq) == NO) {
    switch(cell_class) {
      case D_FF:
      case T_FF:
        break;
      case JK_FF:
      case SR_FF:
        fprintf(dest,"\"J or S input to JK/SR cell\"\n");
        break;
      default:
        inform("REG_CLASS ERR",STOP);
    } /* SWITCH */
    pairings(a_pin_item->vctr_element,icon);
    if ((low_pin(a_pin_item->vctr_element) == YES) &&
        (icon[0] == '/'))
      icon[0] = ' ';  /* remove the inverter / */
    fprintf(dest,"\n%s = ",icon);
    eq_traverse(dest,a_pin_item->reg_latch.PTmem0_input->pin_eq);
    fprintf(dest,";");
  }
}
if (a_pin_item->reg_latch.PTmem1_input != NULL) {
  if (null_eq(a_pin_item->reg_latch.PTmem1_input->pin_eq) == NO) {
    switch(cell_class) {
      case D_FF:
      case T_FF:
        fprintf(dest,"\"Possible REG CLASS Error !\"");
        break;
      case JK_FF:
        fprintf(dest,"K");
        break;
      case SR_FF:
        fprintf(dest,"R");
        break;
      default:
        inform("REG_CELL ERR",STOP);
    } /* SWITCH */
    pairings(a_pin_item->vctr_element,icon);
    if ((low_pin(a_pin_item->vctr_element) == YES) &&
        (icon[0] == '/'))
      icon[0] = ' ';
    fprintf(dest,"\n(%s) = ",icon);
    eq_traverse(dest,a_pin_item->reg_latch.PTmem1_input->pin_eq);
    fprintf(dest,";");
  }
}
} /* EQ_WRITE */
/*  -   -   -   -   -   */
/* write the vectors to the file DEST */
vctr_write(dest)
FILE *dest;
{ struct pin_element *listrunner;
  int flag;

for (listrunner = test_pin_elements, flag = VCC;
     listrunner != NULL;
     listrunner = listrunner->nxt_tst_pin) {
  if (listrunner->vctr_element != -1) {
         /* -------> check if internal reg */
    if (listrunner->vctr_element > physical_pins) {
      if (flag != BREG) {
        if (flag != VCC)
          fprintf(dest,";");
        fprintf(dest,"\nBREG ");
        flag = BREG;
      }
    } /* IF */
    else {
      if (listrunner->out_element == NULL) {
        if (flag != INPUT) {
          if (flag != VCC)
            fprintf(dest,";");
          fprintf(dest,"\nIn  ");
          flag = INPUT;
        }
      }
      else {
        if (flag != IO) {
          if (flag != VCC)
            fprintf(dest,";");
          fprintf(dest,"\nI_O ");
          flag = IO;
        }
      } /* ELSE */
    } /* ELSE */
    pairings(listrunner->vctr_element,icon);
    if ((low_pin(listrunner->vctr_element) == YES) &&
        (icon[0] != '/'))
      fprintf(dest,"/");
    fprintf(dest,"%s ",icon);
  } /* IF */
} /* FOR */
fprintf(dest,";\nBEGIN\n");
rd_apply_vctrs(dest,NO);
fprintf(dest,"END.\n");
} /* VCTR_WRITE */
/*  -   -   -   -   -   */
pairings(a_pin,found_name)
int a_pin;
char found_name[];
{ struct pair_name_no *searcher;

if (a_pin < 1 && a_pin > num_pins)
  inform("Pairings Err",STOP);
if (dflt_names == YES) {
  sprintf(found_name,"p%d_",a_pin);
  return;
}
for (searcher = name_pair_list;
     searcher != NULL;
     searcher = searcher->pair_next) {
  if (a_pin == searcher->dev_no) {
    strcpy(found_name,searcher->devpin_name);
    return;
  }
} /* FOR */
sprintf(found_name,"p%d_",a_pin);
} /* PAIRINGS */
/*  -   -   -   -   -   */
/* store the name-no pair in the NAME_PAIR_LIST */
store_name(aname,a_no)
char aname[];
int a_no;
{ struct pair_name_no *s_tmp,*run1;

s_tmp = (struct pair_name_no *)malloc(sizeof(struct pair_name_no));
mem_size_chk((struct nmbr_list *)NULL,s_tmp);
strcpy(s_tmp->devpin_name,aname);
s_tmp->dev_no = a_no;
s_tmp->pair_next = NULL;
if (name_pair_list == NULL) {
  name_pair_list = s_tmp;
  return;
}
for (run1 = name_pair_list;
     run1->pair_next != NULL;
     run1 = run1->pair_next)
  ;
run1->pair_next = s_tmp;
} /* STORE_NAME */
/*  -   -   -   -   -   */
/* Look for pin names in the NAMES_FILE file. If this file is not
   specified (*), then use the default names Pn_, where n = pin number.
   The pin name file is composed of a design name, a part name, and then
   the name-pin number pairs which are terminated by a @. This file is
   terminated by a * symbol.
*/
look_for_pin_names()
{ char the_name[MAX_NAME],look_buf[MAX_NAME];
  int pin_n;

if (names_file[0] == '\0' || names_file[0] == '*')
  return;
if (input_file != NULL)
  fclose(input_file);
input_file = exist(names_file,"c");
if (input_file == NULL)
  return;
var_init(NO);
dflt_names = NO;
lex(icon);
lex(icon);
fprintf(stderr,"\nReading Pin Names File [%s] for [%s]",names_file,icon);
for (lex(icon); icon[0] != '*'; lex(icon)) {
  strcpy(the_name,icon);
  if (icon[0] == '/') {
    lex(icon);
    strcat(the_name,icon);
  }
  lex(icon);
  if (icon[0] == '[') {
    strcat(the_name,icon);
    for (lex(icon);; lex(icon)) {
      if (strlen(the_name) > MAX_NAME)
        inform("NamePair too long",STOP);
      strcat(the_name,icon);
      if (icon[0] == ']')
        break;
    } /* FOR */
  } /* IF */
  else
    get_new_token = NO;
  pin_n = get_index(look_buf);
  for (lex(icon); icon[0] != '@'; lex(icon))
    ;
  store_name(the_name,pin_n);
} /* FOR */
if (input_file != NULL) {
  fclose(input_file);
  input_file = NULL;
}
} /* LOOK_FOR_PIN_NAMES */
/*  -   -   -   -   -   */
/* check if the pin is active low by checking the pin
   architecture list
*/
low_pin(dev_pin)
int dev_pin;
{ struct nmbr_list *lptr;
for (lptr = pin_arch_list; lptr != NULL; lptr = lptr->nxt_pin) {
  if (dev_pin == lptr->x_pin) {
    return(list_arch(lptr->list_features,(FILE *)NULL," active_low"));
  } /* IF */
} /* FOR lptr */
return(NO);
} /* LOW_PIN */
/*  -   -   -   -   -   */
/* write the information in the TEST_PIN_ELEMENTS list to the
   MAP_EQ_FILE ptr. This file will be written in the PLPL V2.2
   format, but can be modified in the future
*/
PLD_sw_format()
{ struct pin_element *run_pins;
  struct nmbr_list *xpoint;
  char hold;

if (MAP_EQ_file == NULL)
  inform("No MAP-EQ file",STOP);
fprintf(stderr,
       "\nConverting JEDEC Map [%s]\n=> into Logic Equation File [%s]",
        jmapfile,eq_file);
look_for_pin_names();
fprintf(MAP_EQ_file,"\"PLPL V2.2 File generated from JEDEC map [%s]\"",
        jmapfile);
hold = db_sim[0];
db_sim[0] = 'P';
fprintf(MAP_EQ_file,"\nDEVICE MAP_EQ_FILE (%s)\nPIN",db_sim);
db_sim[0] = hold;
/* specify the pin architecture features */
for (run_pins = test_pin_elements;
     run_pins != NULL;
     run_pins = run_pins->nxt_tst_pin) {
  if (run_pins->vctr_element != -1) {
    pairings(run_pins->vctr_element,icon);
    if ((low_pin(run_pins->vctr_element) == YES) &&
        (icon[0] != '/'))
      fprintf(MAP_EQ_file,"\n /");
    else
      fprintf(MAP_EQ_file,"\n  ");
    fprintf(MAP_EQ_file,"%s = %d",
            icon,run_pins->vctr_element);
    for (xpoint = pin_arch_list;
         xpoint != NULL;
         xpoint = xpoint->nxt_pin) {
       if (run_pins->vctr_element == xpoint->x_pin) {
         fprintf(MAP_EQ_file," (");
         list_arch(xpoint->list_features,MAP_EQ_file,"");
         fprintf(MAP_EQ_file,")");
       }
    } /* FOR xpoint */
  } /* IF */
} /* FOR all_list */
fprintf(MAP_EQ_file,"\n; \"End of Pin Definition Section");
fprintf(MAP_EQ_file," START LOGIC EQUATION SECTION\"\nBEGIN\n");
/* specify the logic equation section */
for (run_pins = test_pin_elements;
     run_pins != NULL;
     run_pins = run_pins->nxt_tst_pin) {
  if (run_pins->vctr_element != -1)
    eq_write(run_pins,MAP_EQ_file);
} /* FOR */
fprintf(MAP_EQ_file,"\nEND.\n");
fprintf(MAP_EQ_file,"\"-- Test Vector Section --\"\nTEST_VECTORS");
vctr_write(MAP_EQ_file);
fprintf(stderr,"\n=> JEDEC-to-Equation Conversion Complete");
} /* PLD_SW_FORMAT */
/*  -   -   -   -   -   */
