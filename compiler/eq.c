/* [April 18, 1986] EQ.C : Equation Module manipulates
   sum-of-products logic equations for the output pins
Notes:
 (1) A common list insert routine is made possible by first initializing
     the PIN list ptr in LIST_INITIALIZE with a void item. This list ptr
     can now be passed and any new items added to the list get added after
     this void item (cannot be NULL). See list insert routine LIST_INSERT.
*/
#include "glsdef.h"
/* - - - - - - - - - - */
mem_check(mem_typ0,mem_typ1,mem_typ2) /* check if memory was allocated */
struct type0 *mem_typ0;
struct type1 *mem_typ1;
struct type2 *mem_typ2;
{
if (mem_typ0 == NULL && mem_typ1 == NULL && mem_typ2 == NULL)
 { fprintf(stderr,"Not enough memory\nCompiled until source line [%d]\n",
           line_number);
   fprintf(stderr,"[%s]\n",source_line);
   exit(BAD_EXIT);
 }
} /* MEM_CHECK */
/* - - - - - - - - - - */
invert(log_exp) /* invert operators and operands because of '/' */
struct type1 *log_exp;
{
if (log_exp == NULL)
  return;
switch(log_exp->operator)
 { case '#' : log_exp->pin_num = -(log_exp->pin_num);
              return;
   case '$' : if (log_exp->pin_num == 1)
                log_exp->pin_num = 0;
              else
                log_exp->pin_num = 1;
              return;
   case '*' : log_exp->operator = '+';
              break;
   case '+' : log_exp->operator = '*';
              break;
   default  : inform("Error in INVERT routine\n",STOP);
              break;
 } /* SWITCH */
invert(log_exp->rtptr);
invert(log_exp->ltptr);
} /* INVERT */
/* - - - - - - - - - - */
eval_pair(eval_cond) /* evaluate top 2 operands and top operator */
int eval_cond;
  /* eval conditions : if COMPLETE, then evaluate the OR (+) also */
{ struct type1 *top, *next, *pt_to_next, *top_oprtr;
   /* FIRST and SECOND will be used when processing XOR:
      XOR will be converted to FIRST*(not SECOND) + (not FIRST)*SECOND */

if (errors > 0)
  return;
if (oprnd->typ1_next == NULL)
 { if (oprtr->typ1_next == NULL)
     return;
   else
     inform("too many operators\n",STOP);
 }
for (typ1_temp = oprnd; typ1_temp->typ1_next != NULL;
     typ1_temp = typ1_temp->typ1_next)
  ; /* looking for the tail of this list */
top = typ1_temp;
for (typ1_temp = oprtr ; typ1_temp->typ1_next != NULL;
     typ1_temp = typ1_temp->typ1_next)
  ;
top_oprtr = typ1_temp;
switch(top_oprtr->operator)
 { case '?' : if (top->backptr->pin_num != NULL_VALUE)
                inform("Fatal Error: No operator\n",STOP);
              else
                return;
              break;
   case '/' : if (top->pin_num == NULL_VALUE)
                inform("no operand in OPRND\n",STOP);
              invert(top); /* remove '/' */
              clean_list((struct type0 *)NULL,top_oprtr->backptr->typ1_next);
              top_oprtr->backptr->typ1_next = NULL;
              eval_pair(PAIR);
              return;
   case '%' : /* XOR operator */
   case '+' : if (eval_cond != COMPLETE && eval_cond != LBRACKET)
                return;
              break;
   case '*' : break;
   case '(' : if (eval_cond == COMPLETE)
                err(FATAL,"mismatched parentheses",0,0,0);
              return;
   default  : inform("Not a valid operator\n",STOP);
              break;
 } /* SWITCH */
if (top->backptr->pin_num == NULL_VALUE) /* there is only one operand */
  inform("Only one operand",CONTINUE);
if (top_oprtr->operator == '%')
 { do_xor(); /* process the XOR function */
   for (typ1_temp = oprnd; typ1_temp->typ1_next != NULL;
        typ1_temp = typ1_temp->typ1_next)
     ; /* looking for the tail of this list */
   top = typ1_temp;
   for (typ1_temp = oprtr ; typ1_temp->typ1_next != NULL;
        typ1_temp = typ1_temp->typ1_next)
     ;
   top_oprtr = typ1_temp;
 } /* process XOR */
next = top->backptr;
if (next->backptr == NULL)
  inform("Backtracking error 1: EVAL\n",STOP);
pt_to_next = next->backptr;

top_oprtr->rtptr = next;
top_oprtr->ltptr = top;
top->backptr->typ1_next = NULL;
next->backptr->typ1_next = NULL;
top->backptr = top_oprtr;
next->backptr = top_oprtr;
pt_to_next->typ1_next = top_oprtr;
top_oprtr->backptr->typ1_next = NULL;
top_oprtr->backptr = pt_to_next;

/* DeMorganize only if there is 1 operand in the OPRND list
   and no operator in OPRTR
*/
if (oprnd == NULL)
  inform("PRG Err 1\n",STOP);
if (oprnd->typ1_next == NULL)
  return;
if (oprnd->typ1_next->typ1_next == NULL) {
  if (oprtr == NULL)
    inform("PRG Err 2\n",STOP);
  if (oprtr->typ1_next != NULL)
    return;
}
if (eval_cond != COMPLETE)
  return;
/* Reuse the TOP ptr variable to make a pseudo root node
   for the expression to be DeMorganized. After
   DeMorganization, remove this pseudo root
*/
top = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,top,(struct type2 *)NULL);
top->pin_num = NULL_VALUE;
top->operator = '?';
top->ltptr = NULL;
top->rtptr = NULL;
top->backptr = pt_to_next;
top->typ1_next = pt_to_next->typ1_next;
pt_to_next->typ1_next->backptr = top;
pt_to_next->typ1_next = top;
for (;DeMorgan(top->typ1_next) != STOP;)
  ;
if (top->operator != '?')
  err(FATAL,"DeMorganization Error",0,0,0);
pt_to_next->typ1_next = top->typ1_next;
pt_to_next->typ1_next->backptr = pt_to_next;
top->ltptr = NULL;
top->rtptr = NULL;
top->backptr = NULL;
top->typ1_next = NULL;
clean_list((struct type0 *)NULL,top);
} /* EVAL_PAIR */
/* - - - - - - - - - - */
eval(extent)
/* evaluate top 2 items on OPRND stack with top item in OPRTR stack */
int extent; /* COMPLETE : evaluate the entire operand and operator stack
               LBRACKET : evaluate until the left bracket is found
               PAIR     : evaluate the top operand pair */
{
if (errors > 0)
 { clean_list((struct type0 *)NULL,oprnd->typ1_next);
   clean_list((struct type0 *)NULL,oprtr->typ1_next);
   oprnd->typ1_next = NULL;
   oprtr->typ1_next = NULL;
   return;
 }
if (oprtr == NULL || oprnd == NULL)
  inform("Fatal Error: Eval\n",STOP);
if (oprnd->typ1_next != NULL && oprtr->typ1_next == NULL)
 { if (extent == LBRACKET)
     err(FATAL,"too many )",0,0,0);
   if (oprnd->typ1_next->typ1_next == NULL)
     return;
   else
     inform("too many operands on OPRND stack",CONTINUE);
 }
switch(extent)
 { case PAIR : for (typ1_temp = oprtr ; typ1_temp->typ1_next != NULL;
                    typ1_temp = typ1_temp->typ1_next)
                 ;
               if (typ1_temp->operator != '+' && typ1_temp->operator != '%')
                 eval_pair(PAIR); /* ==> evaluate top 2 operands */
               break;
   case COMPLETE
             : eval_pair(COMPLETE);
               eval(COMPLETE);
               break;
   case LBRACKET
             : for (typ1_temp = oprtr ; typ1_temp->typ1_next != NULL;
                    typ1_temp = typ1_temp->typ1_next)
                 ;
               if (typ1_temp->typ1_next == oprtr->typ1_next)
                { err(FATAL,"unbalanced parentheses",token_type,
                      WHITE_SPACE,WHITE_SPACE);
                  return;
                }
               if (typ1_temp->operator != '(')
                 { eval_pair(LBRACKET);
                   eval(LBRACKET);
                 }
               else
                 { clean_list((struct type0 *)NULL,
                               typ1_temp->backptr->typ1_next);
                   typ1_temp->backptr->typ1_next = NULL;
                   eval_pair(PAIR);
                 }
               break;
 } /* SWITCH extent */
} /* EVAL */
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
    mem_check((struct type0 *)NULL,typ1_entry,(struct type2 *)NULL);
    typ1_entry->pin_num = a_pin;
    typ1_entry->operator = item;
    typ1_entry->rtptr = NULL;
    typ1_entry->ltptr = NULL;
    typ1_entry->backptr = NULL;
    typ1_entry->typ1_next = NULL;
 } /* IF PTLIST == NULL ==> note that if it is not NULL, then the
      PTLIST is already in the copied subtree rooted by TYP1_ENTRY */
for (typ1_temp = list;  /* ====> find the tail of this list */
     typ1_temp->typ1_next != NULL;
     typ1_temp = typ1_temp->typ1_next)
  ;
typ1_temp->typ1_next = typ1_entry;
typ1_entry->backptr = typ1_temp;
} /* PUSH */
/* - - - - - - - - - - */
/* attach a new pin to the PIN list */
new_pin(pin_x)
int pin_x;
{ struct type2 *tmp2,*xptr;

tmp2 = (struct type2 *)malloc(sizeof(struct type2));
mem_check((struct type0 *)NULL,(struct type1 *)NULL,tmp2);
tmp2->pin_nmbr = pin_x;
tmp2->pin_type = I_O;
tmp2->pname[0] = '\0';
tmp2->pt_list = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,tmp2->pt_list,(struct type2 *)NULL);
tmp2->pt_list->pin_num = NULL_VALUE;
tmp2->pt_list->operator = '?';
tmp2->pt_list->rtptr = NULL;
tmp2->pt_list->ltptr = NULL;
tmp2->pt_list->backptr = NULL;
tmp2->pt_list->typ1_next = NULL;

tmp2->special = (struct type0 *)malloc(sizeof(struct type0));
mem_check(tmp2->special,(struct type1 *)NULL,(struct type2 *)NULL);
tmp2->special->fn_name[0] = '\0';
tmp2->special->fuse_numbers = NULL_VALUE;
tmp2->special->pterms = NULL;
tmp2->special->typ0_next = NULL;

tmp2->oprtr_stk = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,tmp2->oprtr_stk,(struct type2 *)NULL);
tmp2->oprtr_stk->pin_num = NULL_VALUE;
tmp2->oprtr_stk->operator = '?';
tmp2->oprtr_stk->rtptr = NULL;
tmp2->oprtr_stk->ltptr = NULL;
tmp2->oprtr_stk->backptr = NULL;
tmp2->oprtr_stk->typ1_next = NULL;

tmp2->oprnd_stk = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,tmp2->oprnd_stk,(struct type2 *)NULL);
tmp2->oprnd_stk->pin_num = NULL_VALUE;
tmp2->oprnd_stk->operator = '?';
tmp2->oprnd_stk->rtptr = NULL;
tmp2->oprnd_stk->ltptr = NULL;
tmp2->oprnd_stk->backptr = NULL;
tmp2->oprnd_stk->typ1_next = NULL;

tmp2->typ2_next = NULL;

if (pins == NULL)
 { pins = tmp2;
   return;
 }
for (xptr = pins; xptr->typ2_next != NULL; xptr = xptr->typ2_next)
  ;
xptr->typ2_next = tmp2;
} /* NEW_PIN */
/* - - - - - - - - - - */
list_initialize()
/* initialize the PIN list array and other ptrs; this routine is necessary
   before using the LIST_INSERT routine */
{
cond_list = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,cond_list,(struct type2 *)NULL);
cond_list->pin_num = NULL_VALUE;
cond_list->operator = '?';
cond_list->rtptr = NULL;
cond_list->ltptr = NULL;
cond_list->backptr = NULL;
cond_list->typ1_next = NULL;

vctr_temp = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,vctr_temp,(struct type2 *)NULL);
vctr_temp->pin_num = NULL_VALUE;
vctr_temp->operator = '?';
vctr_temp->rtptr = NULL;
vctr_temp->ltptr = NULL;
vctr_temp->backptr = NULL;
vctr_temp->typ1_next = NULL;

func_list = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,func_list,(struct type2 *)NULL);
func_list->pin_num = NULL_VALUE;
func_list->operator = '?';
func_list->rtptr = NULL;
func_list->ltptr = NULL;
func_list->backptr = NULL;
func_list->typ1_next = NULL;

pin_vctr = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,pin_vctr,(struct type2 *)NULL);
pin_vctr->pin_num = NULL_VALUE;
pin_vctr->operator = '?';
pin_vctr->rtptr = NULL;
pin_vctr->ltptr = NULL;
pin_vctr->backptr = NULL;
pin_vctr->typ1_next = NULL;

vctr2 = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,vctr2,(struct type2 *)NULL);
vctr2->pin_num = NULL_VALUE;
vctr2->operator = '?';
vctr2->rtptr = NULL;
vctr2->ltptr = NULL;
vctr2->backptr = NULL;
vctr2->typ1_next = NULL;

oprtr = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,oprtr,(struct type2 *)NULL);
oprtr->pin_num = NULL_VALUE;
oprtr->operator = '?';
oprtr->rtptr = NULL;
oprtr->ltptr = NULL;
oprtr->backptr = NULL;
oprtr->typ1_next = NULL;

oprnd = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,oprnd,(struct type2 *)NULL);
oprnd->pin_num = NULL_VALUE;
oprnd->operator = '?';
oprnd->rtptr = NULL;
oprnd->ltptr = NULL;
oprnd->backptr = NULL;
oprnd->typ1_next = NULL;

tmp_oprnd = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,tmp_oprnd,(struct type2 *)NULL);
tmp_oprnd->pin_num = NULL_VALUE;
tmp_oprnd->operator = '?';
tmp_oprnd->rtptr = NULL;
tmp_oprnd->ltptr = NULL;
tmp_oprnd->backptr = NULL;
tmp_oprnd->typ1_next = NULL;

vctr_list = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,vctr_list,(struct type2 *)NULL);
vctr_list->pin_num = NULL_VALUE;
vctr_list->operator = '?';
vctr_list->rtptr = NULL;
vctr_list->ltptr = NULL;
vctr_list->backptr = NULL;
vctr_list->typ1_next = NULL;

caselist = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,caselist,(struct type2 *)NULL);
caselist->pin_num = NULL_VALUE;
caselist->operator = '?';
caselist->rtptr = NULL;
caselist->ltptr = NULL;
caselist->backptr = NULL;
caselist->typ1_next = NULL;

macro_temp = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,macro_temp,(struct type2 *)NULL);
macro_temp->pin_num = NULL_VALUE;
macro_temp->operator = '?';
macro_temp->rtptr = NULL;
macro_temp->ltptr = NULL;
macro_temp->backptr = NULL;
macro_temp->typ1_next = NULL;

macro_list = (struct type0 *)malloc(sizeof(struct type0));
mem_check(macro_list,(struct type1 *)NULL,(struct type2 *)NULL);
macro_list->fn_name[0] = '\0';
macro_list->fuse_numbers = NULL_VALUE;
macro_list->pterms = NULL;
macro_list->typ0_next = NULL;

pins = NULL;
new_pin(NULL_VALUE);
} /* LIST_INITIALIZE */
/* - - - - - - - - - - */
clean_list(list0,list1)  /* free the space used by this list */
struct type0 *list0;
struct type1 *list1;
{
if (list0 != NULL)
 { if (list0->typ0_next != NULL)
     clean_list(list0->typ0_next,(struct type1 *)NULL);
   if (list0->pterms != NULL)
     clean_list((struct type0 *)NULL,list0->pterms);
   free(list0);
   list0 = NULL;
   return;
 } /* LIST0 != NULL */
if (list1 != NULL)
 { if (list1->rtptr != NULL)
     clean_list((struct type0 *)NULL,list1->rtptr);
   if (list1->ltptr != NULL)
     clean_list((struct type0 *)NULL,list1->ltptr);
   if (list1->typ1_next != NULL)
     clean_list((struct type0 *)NULL,list1->typ1_next);
   free(list1);
   list1 = NULL;
 }
} /* CLEAN_LIST */
/* - - - - - - - - - - */
