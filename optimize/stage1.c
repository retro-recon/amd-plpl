/* Oct 24, 1987: STAGE1.C = optimizer stage 1 will use
the Quine-McCluskey algorithm to remove the redundant
variables in the logic function. DIFFERENTIATE() is then
run on the Q-M processed list
*/
#include "opt2def.h"
/* - - - - - - - */
check_constant(eqlist)
struct vartype *eqlist;
{ struct vartype *ptchk;

if (eqlist == NULL)
  inform("CHECK_CONST Err",STOP);
if (var_number == 0)
  return(YES);
if (eqlist->a_var == 0) {
  if (eqlist->next_minterm == NULL) {
    if (eqlist->nextvar != NULL) {
      for (ptchk = eqlist->nextvar; ptchk != NULL; ptchk = ptchk->nextvar) {
        if (ptchk->a_var != 0)
          return(NO);
      } /* FOR */
      return(YES);
    } /* IF */
    return(YES);
  } /* IF */
} /* IF */
return(NO);
} /* CHECK_CONSTANT */
/* - - - - - - - */
get_position(key,target_list)
int key;
struct vartype *target_list;
{ struct vartype *run;
  int location;

for (run = target_list,location = 0;
     run != NULL;
     run = run->nextvar,++location) {
  if (run->a_var == key)
    return(location);
} /* FOR */
return(location);
} /* GET_POSITION */
/* - - - - - - - */
/* compress the product terms in the HOLDER into the variable array
   by using the variable order in the ALL_VARS list to arrange the
   compressed variables: Ex: the variable order is 1/2/3/4, so the
   product term 1*4 will be arranged as 1-0-0-4, where the 0 is used
   as the don't care symbol.
*/
compress(wrklist,holder,all_vars,var_total)
struct varray **wrklist;
struct vartype **holder,*all_vars;
int var_total;
{ struct varray *tmp1comp,*tmp2comp;
  struct vartype *tmp1var,*tmp2var;
  int i;

if (var_total == 0)
  return;
for (tmp2var = *holder;
     tmp2var != NULL;
     tmp2var = tmp2var->next_minterm) {
  tmp1comp = (struct varray *)malloc(sizeof(struct varray));
  check_mem((struct vartype *)NULL,tmp1comp,(int *)NULL);
  tmp1comp->variables = (int *)malloc(var_total*(sizeof(int)));
  check_mem((struct vartype *)NULL,(struct varray *)NULL,tmp1comp->variables);
  tmp1comp->nextPT = NULL;
  for (i = 0; i < var_total; ++i)
    tmp1comp->variables[i] = 0;  /* set to all don't care */
  for (tmp1var = tmp2var;
       tmp1var != NULL;
       tmp1var = tmp1var->nextvar) {
    i = get_position(magnitude(tmp1var->a_var),all_vars);
    tmp1comp->variables[i] = tmp1var->a_var;
  } /* FOR */
  if (*wrklist == NULL)
    *wrklist = tmp1comp;
  else {
    for (tmp2comp = *wrklist;
         tmp2comp->nextPT != NULL;
         tmp2comp = tmp2comp->nextPT)
      ;
    tmp2comp->nextPT = tmp1comp;
  } /* ELSE */
} /* FOR */
} /* COMPRESS */
/* - - - - - - - */
/* check if there is a constant #1/TRUE here */
one_check(one_list,const_term)
struct varray *one_list;
int *const_term;
{ struct varray *run1;
  int i;

for (run1 = one_list,*const_term = NO;
     run1 != NULL && *const_term == NO;
     run1 = run1->nextPT) {
  *const_term = YES;
  for (i = 0; i < var_number; ++i) {
    if (run1->variables[i] != 0) {
      *const_term = NO;
      break;
    } /* IF */
  } /* FOR */
  if (i >= var_number)
    *const_term = YES;
} /* FOR */
} /* ONE_CHECK */
/* - - - - - - - */
dbg_wr_results(dbg_xlist,dbg_const_chk,dbg_value)
struct varray *dbg_xlist;
int dbg_const_chk,dbg_value;
{ int i,true_chk;
  struct varray *run1;

sprintf(main_msg,"\nDebug Format ->\n");
display(main_msg);
if (dbg_const_chk == YES) {
  sprintf(main_msg,"#%d",dbg_value);
  display(main_msg);
  return;
} /* IF */
if (dbg_xlist == NULL)
  return;
one_check(dbg_xlist,&true_chk);
if (true_chk == YES) {
  display("#1");
  return;
}
for (run1 = dbg_xlist; run1 != NULL; run1 = run1->nextPT) {
  if (run1 != dbg_xlist)
    display("\n");
  if (run1->variables != NULL) {
    for (i = 0; i < var_number; ++i) {
      if (i != 0)
        display("*");
      if (run1->variables[i] == 0)
        display(" --- ");
      else {
        sprintf(main_msg," %3d ",run1->variables[i]);
        display(main_msg);
      } /* ELSE */
    } /* FOR */
  } /* IF */
  else
    inform("Musical Instrument PIZZA",STOP);
} /* FOR */
} /* DBG_WR_RESULTS */
/* - - - - - - - */
wr_results(xlist,const_chk,value)
struct varray *xlist;
int const_chk,value;
{ int i,true_chk,firstVAR;
  struct varray *run1;

if (debug_flag > NO)
  display("\n==> Optimized Equation\n\t");
if (const_chk == YES) {
  sprintf(main_msg,"#%d",value);
  display(main_msg);
  return;
} /* IF */
if (xlist == NULL)
  return;
one_check(xlist,&true_chk);
if (true_chk == YES) {
  display("#1");
  return;
}
for (run1 = xlist; run1 != NULL; run1 = run1->nextPT) {
  if (run1 != xlist) {
    display(" +\n\t");
  }
  if (run1->variables != NULL) {
    for (i = 0,firstVAR = YES; i < var_number; ++i) {
      if (run1->variables[i] != 0) {  /* do not print DontCare symbol */
        if (firstVAR == NO)
          display("*");
        if (firstVAR == YES)
          firstVAR = NO;
        if (run1->variables[i] < 0)
          display("/");
        sprintf(main_msg,"%d",magnitude(run1->variables[i]));
        display(main_msg);
      } /* IF */
    } /* FOR */
  } /* IF */
  else
    inform("Omelette Du Fromage",STOP);
} /* FOR */
if (debug_flag > NO) /* write in DEBUG format */
  dbg_wr_results(xlist,const_chk,value);
} /* WR_RESULTS */
/* - - - - - - - */
/* add the ADDEND to the product term pointer of LIST_DEST */
add_PT(addend,list_dest)
struct vartype **addend,**list_dest;
{ struct vartype *search;

if (*addend == NULL)
  return;
if (*list_dest == NULL) {
  *list_dest = *addend;
  return;
}
for (search = *list_dest;
     search->next_minterm != NULL;
     search = search->next_minterm)
  ;
search->next_minterm = *addend;
} /* ADD_PT */
/* - - - - - - - */
/* clean the lists of type VARTYPE and VARRAY */
clean(vtyp1,vtyp2)
struct vartype *vtyp1;
struct varray  *vtyp2;
{ struct vartype *tmp1,*tmp2,*subptr1,*subptr2;
  struct varray  *tmp3,*tmp4;

for (tmp1 = vtyp1; tmp1 != NULL;) {
  tmp2 = tmp1->next_minterm;
  for (subptr1 = tmp1->nextvar,subptr2 = NULL; subptr1 != NULL;) {
    subptr2 = subptr1->nextvar;
    free(subptr1);
    subptr1 = subptr2;
  } /* FOR */
  free(tmp1);
  tmp1 = tmp2;
} /* FOR */
for (tmp3 = vtyp2; tmp3 != NULL;) {
  tmp4 = tmp3->nextPT;
  if (tmp3->variables != NULL)
    free(tmp3->variables);
  free(tmp3);
  tmp3 = tmp4;
} /* FOR */
} /* CLEAN */
/* - - - - - - - */
/* MAGNITUDE nee ABS(); written for portability considerations;
   if your system has ABS(), use it in place of MAGNITUDE() */
magnitude(a_number)
int a_number;
{
if (a_number < 0)
  return(-a_number);
return(a_number);
} /* MAGNITUDE */
/* - - - - - - - */
/* check if memory was allocated */
check_mem(mem1,mem2,mem3)
struct vartype *mem1;
struct varray *mem2;
int *mem3;
{
if (mem1 == NULL && mem2 == NULL && mem3 == NULL)
  err(FATAL,"Insufficient memory",0,0,0);
} /* CHECK_MEM */
/* - - - - - - - */
/* add the variable SIGNAL to the product term XPT; if ACTION  = YES, then
   increment the count UNIQUE if the value was added into the PT list */
add_item(xpt,signal,unique,action,a_constant,always_true)
struct vartype **xpt;
int signal,*unique,action,a_constant,*always_true;
{ struct vartype *point1,*point2;

*always_true = NO;
if (a_constant == YES) {
  switch(signal) {
    case 0:
      return(NO);
    case 1:
      lex(icon);
      if (*xpt == NULL && ((icon[0] == '+') || (icon[0] == ';'))) {
        if (icon[0] == ';')
          get_new_token = NO;
        *always_true = YES;
        return(YES);
      } /* IF */
      get_new_token = NO;
      return(YES);
    default:
      err(FATAL,"Const Err",0,0,0);
      break;
  } /* SWITCH */
} /* IF */

for (point2 = *xpt; point2 != NULL; point2 = point2->nextvar) {
/* check if the value is already in the list; if it is, then do not
   add if they are the same; if they are opposite signs, then remove
   this product term
*/
  if (magnitude(point2->a_var) == magnitude(signal)) {
    if (point2->a_var != signal)
      return(NO);
    else
      return(YES); /* the variable is already in the PT */
  }
} /* FOR */
/* the variable is not in the PT; add it */
point1 = (struct vartype *)malloc(sizeof(struct vartype));
check_mem(point1,(struct varray *)NULL,(int *)NULL);
point1->a_var = signal;
point1->nextvar = NULL;
point1->next_minterm = NULL;
if (*xpt == NULL) {
  if (action == YES)
    ++(*unique);
  *xpt = point1;
  return(YES);
}
for (point2 = *xpt; point2->nextvar != NULL; point2 = point2->nextvar)
  ;  /* find the end of this PT */
point2->nextvar = point1;
/* increment the number of unique variables in this list */
if (action == YES)
  ++(*unique);
return(YES);
} /* ADD_ITEM */
/* - - - - - - - */
/* Make a linked list consisting of the variables in a product term
   linked via the NEXTVAR ptr, and of all the product terms via the
   NEXT_MINTERM pointer. While parsing the equation, generate a list
   of all unique variables and return the total # of unique variables
   used in this equation.

   If a constant is parsed (#0/#1), then the TRUE_FLAG is passed to
   ADD_ITEM(). If TRUE_FLAG is set YES, then the function is always
   true (as in F = 2*3 + #1 + 3*4;) and then all previous product
   terms must be erased and set to the 0 or the don't care flag. This
   is similar to the previous optimizer which used the '-' for the
   don't care symbol.
*/
form_list(vptr)
struct vartype **vptr;
{ int polarity,count,cont,var_value,constant,true_flag;
  char buffer[MAX_NAME];
  struct vartype *onePT;

for (lex(icon),polarity = HIGH,count = 0,onePT = NULL,constant = NO;
     icon[0] != ';';
     lex(icon),polarity = HIGH,constant = NO) {
  switch(icon[0]) {
    case '#': /* a constant of the form #n (n = 0,1) */
      constant = YES;
      break;
    case '/':
      polarity = LOW;
      lex(icon);
      break;
    default:
      if (token_type != NUMBER)
        err(FATAL,"Missing operand",0,0,0);
      break;
  } /* SWITCH */
  if (icon[0] != '#')
    get_new_token = NO;
  var_value = get_index(buffer);
  if (var_value == 0 && constant == NO)
    err(FATAL,"Don't Care Symbol",0,0,0);
  if (polarity == LOW)
    var_value = -var_value;
/* if this variable cannot be added to the PT, skip this OR term */
  if (add_item(&onePT,var_value,&count,NO,constant,&true_flag) == NO) {
    clean(onePT,(struct varray *)NULL);
    onePT = NULL;
    for (lex(icon),cont = YES;cont == YES;) {
      switch(icon[0]) {
        case ';':
        case '+':
          cont = NO;
          get_new_token = NO;
          break;
        default:
          lex(icon);
          break;
      } /* SWITCH */
    } /* FOR */
  } /* IF */
  if (constant == YES && true_flag == YES) {
    clean(onePT,(struct varray *)NULL);
    clean(*vptr,(struct varray *)NULL);
    onePT = NULL;
    *vptr = NULL;
    onePT = (struct vartype *)malloc(sizeof(struct vartype));
    check_mem(onePT,(struct varray *)NULL,(int *)NULL);
    onePT->a_var = 0;
    onePT->nextvar = NULL;
    onePT->next_minterm = NULL;
    *vptr = onePT;
    for (lex(icon);icon[0] != ';';lex(icon))
      ;
    return(0);
  } /* IF */
  lex(icon);
  switch(icon[0]) {
    case ';':
    case '+':
      add_PT(&onePT,&*vptr);
      onePT = NULL;
      if (icon[0] == ';')
        get_new_token = NO;
      break;
    case '*':
      break;
    default:
      err(FATAL,"Use operator (+,*) between operands",0,0,0);
      break;
  } /* SWITCH */

  if (constant != YES) {
    if (add_item(&varlist,magnitude(var_value),
                 &count,YES,constant,&true_flag) == NO)
      err(FATAL,"PRG Err",0,0,0);
  } /* IF */
} /* FOR */
return(count);
} /* FORM_LIST */
/* - - - - - - - */
/* perform optimization by getting a single function of the form
          FUNCTION = 1*2*3 + 1*2*3*4 + .... + 1*2;
   performing the Quine-McCluskey minimization to remove redundant
   variables and then using the 2nd order logic differentiation to
   remove redundant minterms

   The variables must be represented by numbers, The functions
   begin in the file after the first [*] which is the START_EQ
   symbol, and are terminated by a [@] or END_EQ.
*/
optimize()
{ int piece;
for (piece = fgetc(input_file),line_number = 0;
     piece != START_EQ && piece != EOF;
     piece = fgetc(input_file)) {
  if (piece == '\n')
    ++line_number;
  icon[0] = piece;
  icon[1] = '\0';
  display(icon);
} /* FOR */
if (piece == EOF)
  err(FATAL,"Invalid Optimizer Input File",0,0,0);
else
  display("*");
var_init();
for (lex(icon); icon[0] != END_EQ;lex(icon)) {
  display("\n");
  display(icon);
  for (lex(icon); icon[0] != '='; lex(icon))
    display(icon);
  display(" = ");
  clean(tmplist,mainlist);
  clean(varlist,(struct varray *)NULL);
  varlist = NULL;
  tmplist = NULL;
  mainlist = NULL;
  var_number = form_list(&tmplist);
  if (tmplist != NULL) {  /* function is not a constant */
    if (check_constant(tmplist) != YES) {
      if (var_number <= 0)
        err(FATAL,"Var_Number <= 0",0,0,0);
      compress(&mainlist,&tmplist,varlist,var_number);
      use_Quine_McCluskey(&mainlist);
      differentiate(&mainlist);
      wr_results(mainlist,NO,0);
    }
    else
      wr_results((struct varray *)NULL,YES,1);
  } /* IF */
  else
    wr_results((struct varray *)NULL,YES,0);
  display(";");
} /* FOR */
display("\n@\n");
clean(tmplist,mainlist);
clean(varlist,(struct varray *)NULL);
varlist = NULL;
tmplist = NULL;
mainlist = NULL;
} /* OPTIMIZE */
/* - - - - - - - */
