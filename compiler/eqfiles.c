/* [May 14, 1986] : EQFILES.C contains utlities/functions used by the EQ.C
   module. Included is the COPYTREE() routine used in the DeMorgan()
   function
*/
#include "glsdef.h"
/*  -  -  -  -  -  -  -  -  */
copytree(subtree,subroot,node_type)
/* copy the SUBTREE into a tree rooted to SUBROOT */
struct type1 *subtree, *subroot;
char node_type;
{ struct type1 *entry;

if (errors > 0)
  return;
if (subtree == NULL)
  return;
entry = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,entry,(struct type2 *)NULL);
entry->pin_num = subtree->pin_num;
entry->operator = subtree->operator;
entry->rtptr = NULL;
entry->ltptr = NULL;
entry->backptr = subroot;
entry->typ1_next = NULL;
switch(node_type)
 { case 't' : typ1_entry = entry;
              break;
   case 'r' : subroot->rtptr = entry;
              break;
   case 'l' : subroot->ltptr = entry;
              break;
 } /* SWITCH */
copytree(subtree->rtptr,entry,'r');
copytree(subtree->ltptr,entry,'l');
} /* COPYTREE */
/*  -  -  -  -  -  -  -  -  */
DeMorgan(eqlist)
/* DeMorganize the equation list into sum-of-products (SOP) form */
struct type1 *eqlist;
{ int status1, status2, flagset;
  struct type1 *ptr1, *ptr2, *tmptr;  /* see GLS.TXT for explanation */

if (errors > 0)
  return(STOP);
flagset = NO;
if (eqlist == NULL)
  return(STOP);
switch(eqlist->operator)
 { case '#' :
   case '$' : return(STOP); /* eqlist is at the leaf level already */
   case '+' : status1 = DeMorgan(eqlist->rtptr);
              status2 = DeMorgan(eqlist->ltptr);
              if (status1 == CONTINUE || status2 == CONTINUE)
                return(CONTINUE); /* continue DeMorganizing the equation */
             else
                return(STOP);
   case '*' : if (eqlist->rtptr->operator == '#' &&
                  eqlist->ltptr->operator == '#')
                return(STOP);
              if (eqlist->rtptr->operator != '+' &&
                  eqlist->ltptr->operator != '+')
               { status1 = DeMorgan(eqlist->rtptr);
                 status2 = DeMorgan(eqlist->ltptr);
                 if (status1 == CONTINUE || status2 == CONTINUE)
                   return(CONTINUE); /* continue DeMorganizing equation */
                 else
                   return(STOP);
               } /* IF */
              else /* if one is '+', treat only the right ptr */
               { if (eqlist->rtptr->operator == '+')
                  { ptr1 = eqlist->rtptr;
                    ptr2 = eqlist->ltptr;
                  } /* rtptr is '+' */
                 else
                  { ptr1 = eqlist->ltptr;
                    ptr2 = eqlist->rtptr;
                  }
               } /* ELSE */
              break;
   case '?' : return(DeMorgan(eqlist->typ1_next));
   default  : inform("DeMorganizing error\n",STOP);
 } /* SWITCH */
   /* see GLS.TXT for description of DeMorganization routine */
if (eqlist->backptr == NULL)
  inform("No backptr for eqlist->operator\n",STOP);
if (eqlist->backptr->rtptr != NULL || eqlist->backptr->ltptr != NULL)
 { if (eqlist->backptr->rtptr->operator == '*' &&
       eqlist->backptr->ltptr->operator == '*')
     { flagset = YES;
       eqlist->operator = '+';
       eqlist->rtptr = ptr1->rtptr;  /* see GLS.TXT for explanation */
       eqlist->ltptr = ptr1->ltptr;
       eqlist->rtptr->backptr = eqlist;
       eqlist->ltptr->backptr = eqlist;
       ptr1->rtptr = NULL;
       ptr1->ltptr = NULL;
       ptr1->typ1_next = NULL;
       ptr1->backptr = NULL;
       clean_list((struct type0 *)NULL,ptr1);
       ptr1 = eqlist;
       ptr2->backptr = NULL;
     }
   else
     { if (eqlist->backptr->rtptr->operator == '*')
         eqlist->backptr->rtptr = ptr1;
       else
        { if (eqlist->backptr->ltptr->operator == '*')
            eqlist->backptr->ltptr = ptr1;
          else
            inform("Error in backtracking for '*' operator\n",STOP);
        }
     }
 }
else /* the root must be changed */
 { if (eqlist->backptr->operator == '?')
     eqlist->backptr->typ1_next = ptr1;
   else
     inform("FATAL error: Root exchange\n",STOP);
 }
if (flagset == NO)
 { ptr1->backptr = eqlist->backptr;
   eqlist->rtptr = NULL;
   eqlist->ltptr = NULL;
   eqlist->backptr = NULL;
   clean_list((struct type0 *)NULL,eqlist);
 }
typ1_entry = NULL;
copytree(ptr2,typ1_entry,'t');
tmptr = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,tmptr,(struct type2 *)NULL);
tmptr->pin_num = OPERATOR;
tmptr->operator = '*';
tmptr->typ1_next = NULL;
tmptr->ltptr = typ1_entry;
tmptr->rtptr = ptr1->ltptr;
tmptr->ltptr->backptr = tmptr;
tmptr->rtptr->backptr = tmptr;
tmptr->backptr = ptr1;
ptr1->ltptr = tmptr;
tmptr->typ1_next = NULL;

tmptr = (struct type1 *)malloc(sizeof(struct type1));
mem_check((struct type0 *)NULL,tmptr,(struct type2 *)NULL);
tmptr->pin_num = OPERATOR;
tmptr->operator = '*';
tmptr->typ1_next = NULL;
tmptr->ltptr = ptr2;
tmptr->rtptr = ptr1->rtptr;
tmptr->ltptr->backptr = tmptr;
tmptr->rtptr->backptr = tmptr;
tmptr->backptr = ptr1;
ptr1->rtptr = tmptr;
tmptr->typ1_next = NULL;

return(CONTINUE);  /* continue DeMorganizing */
} /* DeMorgan */
