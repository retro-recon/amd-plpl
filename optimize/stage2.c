/* Oct 26, 1987: STAGE2.C = optimizer stage2 will use
   2nd order logic differentiation. Quine-McCluskey
   routine is also in this file
*/
#include "opt2def.h"
/* - - - - - - - */
/* find the difference between the 2 PTs according to
   the ff rules:
- If SRC and CMP are the same, then go to the next variable position.
- If SRC is a var and CMP is a 0/X, then mark this DontCare position.
- If SRC, CMP are var, then mark this var difference; if there is more
  than 1 var difference, then these 2 PTs cannot be reduced, so get a
  new PT pair.
- If after comparing, there is only a DontCare comparison successful,
  then this var can be marked off, as in 1*2*3 + 1*2 (3 can be removed).
- If SRC is a 0/X and CMP is a var, then this is also considered a
  var difference; if there is more than 1, then get the next PT pair.
*/
PTdiff(srclist,cmplist)
int *srclist,*cmplist;
{ int t,dont_care,total_diff,xdiff,var_diff;

if (debug_flag == YES) {
  fprintf(stderr,"\nSRCLIST:");
  for (t = 0; t < var_number; ++t)
    fprintf(stderr," %3d|",srclist[t]);
  fprintf(stderr,"\ncmplist: ");
  for (t = 0; t < var_number; ++t)
    fprintf(stderr," %3d|",cmplist[t]);
}

for (t = 0,xdiff = -1,dont_care = -1,total_diff = 0,var_diff = -1;
     t < var_number; ++t) {
  if (srclist[t] != cmplist[t]) {
    if (cmplist[t] == 0) { /* get the 1st dont care */
      if (dont_care == -1)
        dont_care = t;
    }
    else {
      if (srclist[t] == 0)
        xdiff = t;
      else
        var_diff = t;
      ++total_diff;
      if (total_diff > 1) {
        if (debug_flag == YES)
          fprintf(stderr,"\nTOTAL DIFF [%d]",total_diff);
        return(NO);
      }
    } /* ELSE */
  }
} /* FOR */
if (debug_flag == YES)
  fprintf(stderr,"\nXdiff [%d] DontCare [%d] VarDiff [%d]",
           xdiff,dont_care,var_diff);
if ((xdiff == -1) && (dont_care == -1) && (var_diff == -1))
  return(NO);
if (xdiff == -1) {
  if (var_diff == -1) {
    var_diff = dont_care;
    srclist[var_diff] = 0;
    return(YES);
  }
  else {
    srclist[var_diff] = 0;
    return(YES);
  } /* ELSE */
}
return(NO);
} /* PTDIFF */
/* - - - - - - - */
/* check if these PTs are the same */
same(term1,term2)
int *term1,*term2;
{ int x;
for (x = 0; x < var_number; ++x) {
  if (term1[x] != term2[x])
    return(NO);
} /* FOR */
return(YES);
} /* SAME */
/* - - - - - - - */
/* copy the variables in DESTPT to the MAINPT list; if ACT is YES,
   then use the SIGNAL_INDEX to mark the signal which is redundant;
   if NO, then just copy the variable
*/
QMcopy(mainPT,QMsrcPT,signal_index,act)
struct varray **mainPT,*QMsrcPT;
int signal_index,act;
{ struct varray *tmp1cpy,*tmp2cpy;
  int x;

tmp2cpy = (struct varray *)malloc(sizeof(struct varray));
check_mem((struct vartype *)NULL,tmp2cpy,(int *)NULL);
tmp2cpy->variables = (int *)malloc(var_number*(sizeof(int)));
check_mem((struct vartype *)NULL,(struct varray *)NULL,tmp2cpy->variables);
tmp2cpy->nextPT = NULL;
for (x = 0; x < var_number; ++x)
  tmp2cpy->variables[x] = QMsrcPT->variables[x];
if (act == YES)
  tmp2cpy->variables[signal_index] = 0;
if (*mainPT == NULL) {
  *mainPT = tmp2cpy;
  return;
}
for (tmp1cpy = *mainPT; tmp1cpy != NULL; tmp1cpy = tmp1cpy->nextPT) {
  if (same(tmp1cpy->variables,tmp2cpy->variables) == YES) {
    clean((struct vartype *)NULL,tmp2cpy);
    return;
  }
} /* FOR */
for (tmp1cpy = *mainPT;
     tmp1cpy->nextPT != NULL;
     tmp1cpy = tmp1cpy->nextPT)
  ;
tmp1cpy->nextPT = tmp2cpy;
} /* QMCOPY */
/* - - - - - - - */
/* remove/cull any redundant terms in this list */
cull_list(to_cull)
struct varray **to_cull;
{ struct varray *cptr1,*cptr2,*cptr3;
  int cullflag;

if (*to_cull == NULL)
  return;
cptr1 = (*to_cull)->nextPT;
(*to_cull)->nextPT = NULL;
for (cullflag = 0; cptr1 != NULL; cullflag = 0) {
  cptr2 = cptr1->nextPT;
  cptr1->nextPT = NULL;
  for (cptr3 = *to_cull; cptr3 != NULL; cptr3 = cptr3->nextPT) {
    if (same(cptr3->variables,cptr1->variables) == YES) {
      clean((struct vartype *)NULL,cptr1);
      cullflag = 1;
      break;
    } /* IF */
  } /* FOR cptr3 */
  if (cullflag == 0) {  /* record was not culled from the list */
    for (cptr3 = *to_cull; cptr3->nextPT != NULL; cptr3 = cptr3->nextPT)
      ;
    cptr3->nextPT = cptr1;  /* add it to the other list */
  } /* IF */
  cptr1 = cptr2;
} /* FOR */
} /* CULL_LIST */
/* - - - - - - - */
use_Quine_McCluskey(srclist)
struct varray **srclist;
{ struct varray *ptA,*ptB;
  int change;

change = YES;
for (;change == YES;) {
  change = NO;
  for (ptA = *srclist; ptA != NULL; ptA = ptA->nextPT) {
    for (ptB = *srclist; ptB != NULL; ptB = ptB->nextPT) {
      if (ptA != ptB) {
        if (PTdiff(ptA->variables,ptB->variables) == YES) {
          change = YES;
        } /* IF PTdiff */
      } /* IF */
    } /* sub-FOR */
  } /* inner FOR */
  cull_list(&*srclist);
} /* FOR */
} /* USE_QUINE_MCCLUSKEY */
/* - - - - - - - */
/* do the 2nd order logic differentiation by getting the variables
   that are different between XTERM and the variables in the PLIST
   list; put the differences into a list DEST2
*/
order2(xterm,Plist,dest2)
struct varray *xterm,*Plist,**dest2;
{ struct varray *temp1,*temp2,*g1;
  int xcnt,add_this,nullify;

for (g1 = Plist,temp1 = NULL; g1 != NULL; g1 = g1->nextPT) {
  add_this = NO;
  if (same(xterm->variables,g1->variables) == NO) {
    QMcopy(&temp1,g1,0,NO);
    for (xcnt = 0; xcnt < var_number; ++xcnt)
      temp1->variables[xcnt] = 0;
    for (xcnt = 0,nullify = NO;
         (xcnt < var_number) && (nullify == NO);
         ++xcnt) {
      if (xterm->variables[xcnt] != g1->variables[xcnt]) {
        if (xterm->variables[xcnt] == 0) {
          add_this = YES;
          temp1->variables[xcnt] = g1->variables[xcnt];
        }
        else {
          if (g1->variables[xcnt] != 0) {
            nullify = YES;
            add_this = NO;
          }
        } /* ELSE */
      } /* IF XTERM != G1 */
    } /* FOR xcnt < var_number */
    if (add_this == YES) {
      if (*dest2 == NULL)
        *dest2 = temp1;
      else {
        for (temp2 = *dest2;
             temp2->nextPT != NULL;
             temp2 = temp2->nextPT)
          ;
        temp2->nextPT = temp1;
      } /* ELSE of *dest2 == NULL */
    } /* IF add_this */
    else
      clean((struct vartype *)NULL,temp1);
    temp1 = NULL;
  } /* IF same(xterm,g1) */
} /* FOR */
} /* ORDER2 */
/* - - - - - - - */
/* Perform 2nd order logic differentiation; perform Q-M on the
   differentiated equation list; if it is #1, then this term is
   redundant.
*/
differentiate(dlist)
struct varray **dlist;
{ struct varray *d1_run,*d3_run,*d4_run,*prev_ptr;
  int xflag,modify,out_count;

for (modify = YES,out_count = 0;modify == YES;) {
  d1_run = *dlist;
  prev_ptr = *dlist;
  for (xflag = NO,modify = NO,d3_run = NULL; d1_run != NULL;xflag = NO) {
    d4_run = d1_run->nextPT;
    order2(d1_run,*dlist,&d3_run);
    use_Quine_McCluskey(&d3_run);
    one_check(d3_run,&xflag);
    if (xflag == YES) { /* ---- */
      modify = YES;
      if (d1_run == *dlist) { /* PT to remove is head of list */
        *dlist = d1_run->nextPT;
        prev_ptr = *dlist;
      } /* IF */
      else {
        prev_ptr->nextPT = d1_run->nextPT;
      } /* ELSE */
      d1_run->nextPT = NULL;
      ++out_count;
      if (debug_flag == 200) {
        display("\n\t!! Removed PT !!");
        wr_results(d1_run,NO,0);
        display("\n\t>==<");
      }
      clean((struct vartype *)NULL,d1_run);
      d1_run = NULL;
    } /* IF of XFLAG*/
    else                /* ---- */
      prev_ptr = d1_run;
    d1_run = d4_run;
    clean((struct vartype *)NULL,d3_run);
    d3_run = NULL;
  } /* FOR */
} /* FOR */
if (debug_flag != NO) {
  sprintf(main_msg,"\n=> Removed [%d] PTs",out_count);
  display(main_msg);
}
} /* DIFFERENTIATE */
/* - - - - - - - */
