/* Nov 23, 1986: XORFILE.C contains the routines for generating the
                 sum-of-products form of the XOR (%) operator.
These routines are necessary to avoid the lengthy calculations of the
other approach: A%B = /A&B + A&/B. It becomes very slow and memory
intensive once the number of XOR reaches 4 or more.

This laternate approach is described succintly:
(1) The number of PTs in the XOR expression is equivalent to the
    number of variables in the expression separated by %. A%B%C has
    4 PTs => /A&/B&C, /A&B&/C, A&/B&/C, A&B&C.
    Note that the number of uncomplemented terms (no '/') is always odd.
    This will be the trick used to speed up XOR PT generation.
(2) Count the number of consecutive % operators on the operator stack.
    This will speed up the calculation if we know that the XOR
    expression is A%B%C%D.
(3) The number of variables to be process is n+1 where n is the number
    of consecutive % operators.
(4) A FOR loop is executed pow(2,n+1) times. An XORCOUNT function is
    called to determine if this count has an odd number of 1s. If it
    has, then the variables are logically ANDed according to the count
    and added to the operand list.
(5) This works a bit slower for expressions because they will may have
    to be inverted depending on the variable/expressions's position in
    the count sequence.
    Ex: (A+B)%C will be evaluated as /(A+B)&C + (A+B)&/C.
*/
#include "glsdef.h"
/*  -   -   -   -   -   */
/* Check if there are an odd number of 1s in the binary count; this will
   determine if this is part of the XOR count sequence. Check only the
   number of bits specified by CNT_WIDTH */
xorcount(cnt,cnt_width)
int cnt,cnt_width;
{ int amount,i,b;
amount = 0;
for (i = 0; i < cnt_width; ++i)
 { b = (cnt>>i)&(0x01);
   if (b == 1)
     ++amount;
 } /* FOR */
if (amount%2 == 1)  /* odd number */
  return(YES);
return(NO);
} /* XORCOUNT */
/*  -   -   -   -   -   */
/* add the PT described by the binary count CNT: if there are two
   variables A,B, and the count is 2, then add the PT A&/B which
   represents the binary number 2 */
add_xor(cnt,xor_varlist)
int cnt;
struct type1 *xor_varlist;
{ struct type1 *add1,*add2;
  int logshift,b;
if (xor_varlist == NULL)
  return;
for (logshift = 0,add1 = xor_varlist,add2 = NULL;
     add1 != NULL;
     add1 = add1->typ1_next,++logshift)
 { copytree(add1,add2,'t');
   add2 = typ1_entry;
   b = (cnt >> logshift)&(0x01);
   if (b == 0)
     invert(add2);
   typ1_entry = add2;
   push(oprnd,'#',NULL_VALUE,typ1_entry);
   if (add1 != xor_varlist) /* if not first item then push a '*' */
     push(oprtr,'*',OPERATOR,(struct type1 *)NULL);
   eval_pair(PAIR);
 } /* FOR */
} /* ADD_XOR */
/*  -   -   -   -   -   */
do_xor()
{ struct type1 *xor1,*pos1,*pos2;
  int i,count,maxcount;
  double x,y,b;
for (xor1 = oprtr; xor1->typ1_next != NULL; xor1 = xor1->typ1_next)
  ;
if (xor1->operator != '%')
  return;
for (count = 1;  /* count the topmost % */
     xor1->backptr != NULL;
     xor1 = xor1->backptr)
 { if (xor1->backptr->operator == '%')
     ++count;
   else
     break;
 } /* FOR */
pos1 = xor1->backptr;
pos1->typ1_next = NULL;
xor1->backptr = NULL;
clean_list((struct type0 *)NULL,xor1);
for (xor1 = oprnd; xor1->typ1_next != NULL; xor1 = xor1->typ1_next)
  ;
x = (double)2;
y = (double)(count+1);
for (; count > 0; xor1 = xor1->backptr,--count)
  ;
pos2 = xor1->backptr;
pos2->typ1_next = NULL;
xor1->backptr = NULL;
b = pow(x,y);
maxcount = (int)b;
/* the next 2 lines of code can be removed if the POW(x,y) returns the
   exact number; C-code on the RT-PC gives 15.9999999998 for POW(2,4);
   this is truncated to 15 when cast as an INT. Only when Y >= 5 does
   the answer become precise enough */
if (maxcount%2 == 1)
  ++maxcount;
for (i = 1; i < maxcount; ++i)
 { if (xorcount(i,(int)y) == YES)
    { push(oprtr,'+',OPERATOR,(struct type1 *)NULL);
      add_xor(i,xor1);
    }
 } /* FOR */
clean_list((struct type0 *)NULL,xor1);
/* remove the very last + added in the above loop */
for (xor1 = oprtr; xor1->typ1_next != NULL; xor1 = xor1->typ1_next)
  ;
xor1->backptr->typ1_next = NULL;
clean_list((struct type0 *)NULL,xor1);
} /* DO_XOR */
