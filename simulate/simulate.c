/*
July 19, 1987: SIMULATE.C contains the simulation routines
and PT construction function S_PAIR and S_EVAL
*/
#include "simdef.h"
/*  -   -   -   -   -   -   */
s_traverse(nd2list)
struct node2class *nd2list;
{

if (nd2list == NULL)
  return;

if (nd2list->symbol != NULL) {
  if (nd2list->symbol[0] == '@') {
    fprintf(stderr,"%s",nd2list->symbol);
    if (nd2list->LEFT_0 != NULL) {
      fprintf(stderr," => F branch: ");
      s_traverse(nd2list->LEFT_0);
    }
    if (nd2list->RIGHT_1 != NULL) {
      fprintf(stderr," => T branch: ");
      s_traverse(nd2list->RIGHT_1);
    }
    return;
  }
} /* IF */

if (nd2list->symbol[0] != EQNMBR)
  s_traverse(nd2list->LEFT_0);

if (nd2list->symbol != NULL) {
  switch(nd2list->symbol[0]) {
    case EQNMBR:
      fprintf(stderr,"%d",nd2list->pin_no);
      if (nd2list->LEFT_0 != NULL) {
        fprintf(stderr," => F' branch: ");
        s_traverse(nd2list->LEFT_0);
      }
      if (nd2list->RIGHT_1 != NULL) {
        fprintf(stderr," => T' branch: ");
        s_traverse(nd2list->RIGHT_1);
      }
      break;
    case Q_OUTPUT:
      fprintf(stderr,"%d.Q",nd2list->pin_no);
      break;
    case '+':
    case '*':
    case '/':
    case '|':
    case '%':
      fprintf(stderr,"%s",nd2list->symbol);
      break;
    case '^':
      fprintf(stderr,"#%d",nd2list->pin_no);
      break;
    default:
      fprintf(stderr,"[%s]",nd2list->symbol);
  } /* SWITCH */
} /* IF */
else
  fprintf(stderr,"Node is NULL");
if (nd2list->symbol[0] != EQNMBR)
  s_traverse(nd2list->RIGHT_1);
} /* S_TRAVERSE */
/*  -   -   -   -   -   -   */
/* simulate the device according to the vectors supplied
   in the JEDECFILE or in another test vector file
*/
simulate()
{
if (debug_flag == YES)
  show_set_up();
rd_apply_vctrs((FILE *)NULL,YES);
} /* SIMULATE */
/*  -   -   -   -   -   -   */
/* list the PTs and architecture features of the pins */
show_set_up()
{
struct PT_element *A_PTLIST;
struct pin_element *PKG_PIN;
struct node2class *pin_runner;

fprintf(stderr,"\nPRODUCT TERM list is -------");
for (A_PTLIST = product_terms;
     A_PTLIST != NULL;
     A_PTLIST = A_PTLIST->next_PT) {
  if (A_PTLIST->pin_eq != NULL) {
    fprintf(stderr,"\nPT name [%s] = ",A_PTLIST->term_name);
    for (pin_runner = A_PTLIST->pin_eq;
         pin_runner != NULL;
         pin_runner = pin_runner->FWD)
      if (pin_runner != NULL)
        s_traverse(pin_runner);
  } /* IF */
} /* FOR */

for (PKG_PIN = test_pin_elements;
     PKG_PIN != NULL;
     PKG_PIN = PKG_PIN->nxt_tst_pin) {
  fprintf(stderr,"\nPin [%d] configured as = ",PKG_PIN->vctr_element);
  if (PKG_PIN->out_element != NULL) {
    fprintf(stderr,"\n== OUT_element = ");
    s_traverse(PKG_PIN->out_element);
  }
  if (PKG_PIN->misc_fnc != NULL) {
    if (PKG_PIN->misc_fnc->pin_eq != NULL) {
      fprintf(stderr,"\n== MISC_FNC = ");
      s_traverse(PKG_PIN->misc_fnc->pin_eq);
    }
  }
  if (PKG_PIN->reg_latch.PTreset != NULL) {
    if (PKG_PIN->reg_latch.PTreset->pin_eq != NULL) {
      fprintf(stderr,"\n== PTreset = ");
      s_traverse(PKG_PIN->reg_latch.PTreset->pin_eq);
    }
  }
  if (PKG_PIN->reg_latch.PTset != NULL) {
    if (PKG_PIN->reg_latch.PTset->pin_eq != NULL) {
      fprintf(stderr,"\n== PTset = ");
      s_traverse(PKG_PIN->reg_latch.PTset->pin_eq);
    }
  }
  if (PKG_PIN->reg_latch.PTpreload != NULL) {
    if (PKG_PIN->reg_latch.PTpreload->pin_eq != NULL) {
      fprintf(stderr,"\n== PTpreload = ");
      s_traverse(PKG_PIN->reg_latch.PTpreload->pin_eq);
    }
  }
  if (PKG_PIN->reg_latch.PTobserve != NULL) {
    if (PKG_PIN->reg_latch.PTobserve->pin_eq != NULL) {
      fprintf(stderr,"\n== PTobserve = ");
      s_traverse(PKG_PIN->reg_latch.PTobserve->pin_eq);
    }
  }
  if (PKG_PIN->reg_latch.PTclock != NULL) {
    if (PKG_PIN->reg_latch.PTclock->pin_eq != NULL) {
      fprintf(stderr,"\n== PTclock = ");
      s_traverse(PKG_PIN->reg_latch.PTclock->pin_eq);
    }
  }
  if (PKG_PIN->reg_latch.PTenable != NULL) {
    if (PKG_PIN->reg_latch.PTenable->pin_eq != NULL) {
      fprintf(stderr,"\n== PTenable = ");
      s_traverse(PKG_PIN->reg_latch.PTenable->pin_eq);
    }
  }
  if (PKG_PIN->reg_latch.PTmem0_input != NULL) {
    if (PKG_PIN->reg_latch.PTmem0_input->pin_eq != NULL) {
      fprintf(stderr,"\n== PTmem0_input = ");
      s_traverse(PKG_PIN->reg_latch.PTmem0_input->pin_eq);
    }
  }
  if (PKG_PIN->reg_latch.PTmem1_input != NULL) {
    if (PKG_PIN->reg_latch.PTmem1_input->pin_eq != NULL) {
      fprintf(stderr,"\n== PTmem1_input = ");
      s_traverse(PKG_PIN->reg_latch.PTmem1_input->pin_eq);
    }
  }
  fprintf(stderr,"\n== register class and type [%d]",
                       PKG_PIN->reg_latch.store_type);
} /* FOR */
} /* SHOW_SET_UP */
/*  -   -   -   -   -   -   */
/*
Configure the pins: based on control links or product terms
*/
configure_pins()
{ char CFG_BUF[MAX_NAME];
  int cfg_pin,saved_keyword;
  struct pin_element *s_pin_ptr;
  struct dfn_class *cls1;

srch_icon("begin",WARNING,YES,"start PIN def");
for (lex(icon); the_keyword != END; lex(icon)) {
  get_new_token = NO; /* for GET_INDEX() call */
  cfg_pin = get_index(CFG_BUF);
  sim_new_pin(cfg_pin);  /* add a new pin to the pin list */
  s_pin_ptr = sim_find_pin(cfg_pin);
  srch_icon("=",WARNING,YES,"start def");
  srch_icon("begin",WARNING,YES,"start a def");

  for (lex(icon);the_keyword != END;lex(icon)) {
    saved_keyword = the_keyword;
    srch_icon("=",WARNING,YES,"get =");
    switch(saved_keyword) {
    case VAL:
      srch_icon("\\",FATAL,YES,"define possible values");
      lex(icon);
      if (chk_dfn_name(icon,dfn_list) == NO)
        err(FATAL,"Invalid value range",0,0,0);
      cls1 = (struct dfn_class *)malloc(sizeof(struct dfn_class));
      mem2_check((struct node1class *)NULL,(struct node2class *)NULL,
          (struct PT_element *)NULL,(struct pin_element *)NULL,cls1);
      strcpy(cls1->class1_name,icon);
      cls1->list_val = NULL;
      cls1->nxt_dfn = NULL;
      copy_dfn(icon,cls1);
      s_pin_ptr->poss_values = cls1;
      srch_icon(";",FATAL,YES,"end def");
      break;
    case RESET:
      s_pin_ptr->reg_latch.PTreset = get_PT("reset",1);
      break;
    case SET:
      s_pin_ptr->reg_latch.PTset = get_PT("set",1);
      break;
    case PRELOAD:
      s_pin_ptr->reg_latch.PTpreload = get_PT("preload",1);
      break;
    case OBSERVE:
      s_pin_ptr->reg_latch.PTobserve = get_PT("observe",1);
      break;
    case CLOCK:
      s_pin_ptr->reg_latch.PTclock = get_PT("clock",1);
      break;
    case FF_TYPE:
      cfg_pin = fuse_test();
      switch (cfg_pin) {
        case 0: s_pin_ptr->reg_latch.store_type =
                     (s_pin_ptr->reg_latch.store_type & 0x0F0) | D_FF;
                break;
        case 1: s_pin_ptr->reg_latch.store_type =
                     (s_pin_ptr->reg_latch.store_type & 0x0F0) | JK_FF;
                break;
        case 2: s_pin_ptr->reg_latch.store_type =
                     (s_pin_ptr->reg_latch.store_type & 0x0F0) | SR_FF;
                break;
        case 3: s_pin_ptr->reg_latch.store_type =
                     (s_pin_ptr->reg_latch.store_type & 0x0F0) | T_FF;
                break;
        default: err(FATAL,"Unknown FF type",0,0,0);
      } /* SWITCH */
      srch_icon(";",FATAL,YES,USE_DELIMITER);
      break;
    case STORAGE_TYPE:
      cfg_pin = fuse_test();
      switch (cfg_pin) {
        case 0: s_pin_ptr->reg_latch.store_type =
                     (s_pin_ptr->reg_latch.store_type & 0x0F) |
                            (REG_CELL << 4);
                break;
        case 1: s_pin_ptr->reg_latch.store_type =
                     (s_pin_ptr->reg_latch.store_type & 0x0F) |
                            (LATCH_CELL << 4);
                break;
        default: err(FATAL,"Use REG or LATCH",0,0,0);
      } /* SWITCH */
      srch_icon(";",FATAL,YES,USE_DELIMITER);
      break;
    case ENABLE:
      s_pin_ptr->reg_latch.PTenable = get_PT("enable",1);
      break;
    case MEM0_INPUT:
      s_pin_ptr->reg_latch.PTmem0_input = get_PT("mem0input",1);
      break;
    case MEM1_INPUT:
      s_pin_ptr->reg_latch.PTmem1_input = get_PT("mem1input",1);
      break;
    case MEM_OUTPUT:
      s_pin_ptr->reg_latch.mem_output = get_index(CFG_BUF);
      srch_icon(";",FATAL,YES,USE_DELIMITER);
      break;
    case OUT_ELEMENT:
      get_new_token = NO; /* for ND_EQ_PARSE() call */
      s_pin_ptr->out_element = nd_eq_parse();
      break;
    case MISC_FNC:
      s_pin_ptr->misc_fnc = get_PT("misc_functions",1);
      break;
    default:
      err(FATAL,"Unsupported macrocell",0,0,0);
    } /* SWITCH */
  } /* FOR */
  srch_icon(";",FATAL,YES,USE_DELIMITER);
} /* FOR */
srch_icon(";",FATAL,YES,USE_DELIMITER);
} /* CONFIGURE_PINS */
/*  -   -   -   -   -   -   */
/* evaluate the operands and operators by pairing an operator
with 2 operands.
*/
s_pair(x_oprtr,x_oprnd)
struct node2class *x_oprtr,*x_oprnd;
{ struct node2class *tmp1rtr,*tmp1rnd,*tmp2rtr,*tmp3,*tmp4;

for (tmp1rtr = x_oprtr; tmp1rtr->FWD != NULL; tmp1rtr = tmp1rtr->FWD)
  ; /* find last item in s_oprtr */
for (tmp1rnd = x_oprnd; tmp1rnd->FWD != NULL; tmp1rnd = tmp1rnd->FWD)
  ; /* find last item in s_oprnd */
if (tmp1rtr->symbol[0] == '/') {  /* perform unary operation */
  if (tmp1rnd->symbol == NULL)
    err(FATAL,"No operand for /",0,0,0);
  tmp2rtr = tmp1rtr->RWD;
  tmp1rtr->RIGHT_1 = tmp1rnd;
  tmp1rnd->RWD->FWD = tmp1rtr;
  tmp1rtr->RWD = tmp1rnd->RWD;
  tmp1rnd->RWD = tmp1rtr;
  tmp2rtr->FWD = NULL;
  tmp1rtr->FWD = NULL;
  return;
} /* IF */
if (tmp1rnd->symbol == NULL)
  err(FATAL,"NULL operand",0,0,0);
if (tmp1rnd->RWD == NULL)
  err(FATAL,"Need operands",0,0,0);
if (tmp1rnd->RWD->symbol == NULL)
  return;  /* only 1 operand */
else {
  if (tmp1rtr->symbol == NULL)
    err(FATAL,"No operator",0,0,0);
}
for (tmp3 = x_oprtr, tmp4 = tmp3; tmp3->FWD != NULL; tmp3 = tmp3->FWD)
  tmp4 = tmp3;
tmp3->RWD = tmp4;
tmp3->RIGHT_1 = tmp1rnd;
tmp1rnd->FWD = NULL;
tmp3->LEFT_0 = tmp1rnd->RWD;
tmp3->RIGHT_1->RWD = tmp3;
tmp3->LEFT_0->RWD->FWD = tmp3;
tmp3->RWD->FWD = NULL;
tmp3->RWD = tmp3->LEFT_0->RWD;
tmp3->LEFT_0->RWD = tmp3;
tmp3->LEFT_0->FWD = NULL;
} /* S_PAIR */
/*  -   -   -   -   -   -   */
/* evaluate the operands and operators in the respective
stacks/linked lists. Operator prioritization is as follows:
0) NOT /, and parenthesis (
1) AND *
2) XOR %, XNOR |
3) OR +
*/
s_eval(current,s_oprtr,s_oprnd)
char current;
struct node2class *s_oprtr,*s_oprnd;
{ struct node2class *tmp1rtr,*tmp1rnd,*tmp2rtr,*tmp3,*tmp4;

/* fprintf(stderr,"\n in S_EVAL"); */

if (s_oprtr == NULL || s_oprnd == NULL)
  err(FATAL,"NULL S_EVAL",0,0,0);

for (;;) {


/*
fprintf(stderr,"\ninside S_EVAL loop");

fprintf(stderr,"OPERATOR :");
  for (tmp1rtr = s_oprtr; tmp1rtr != NULL; tmp1rtr = tmp1rtr->FWD)
    fprintf(stderr,"\ntmp1rtr->symbol [%s]",
       ((tmp1rtr->symbol==NULL)?("NULL"):(tmp1rtr->symbol)) );

fprintf(stderr,"OPERAND :");
  for (tmp1rnd = s_oprnd; tmp1rnd != NULL; tmp1rnd = tmp1rnd->FWD)
    fprintf(stderr,"\ntmp1rnd->symbol [%s][%d]",
       ((tmp1rnd->symbol==NULL)?("NULL"):(tmp1rnd->symbol)),tmp1rnd->pin_no);
*/



  for (tmp1rtr = s_oprtr; tmp1rtr->FWD != NULL; tmp1rtr = tmp1rtr->FWD)
    ; /* find last item in s_oprtr */
  for (tmp1rnd = s_oprnd; tmp1rnd->FWD != NULL; tmp1rnd = tmp1rnd->FWD)
    ; /* find last item in s_oprnd */

  if (tmp1rtr->symbol == NULL) {
    if (tmp1rnd->symbol != NULL) {
      if (tmp1rnd->RWD != NULL) {
        if (tmp1rnd->RWD->symbol != NULL) /* there are 2 operands on stack */
          err(FATAL,"No operator (!)",0,0,0);
        else {
          if (current == '=')
            return;
        } /* ELSE */
      }
    }
    else {
      if (current == '=')
        return;  /* no operators or operands */
    } /* ELSE */
  } /* IF */

  switch(current) {
    case '+': /* OR has lowest priority */
      switch(tmp1rtr->symbol[0]) { /* check last item on the operator stk */
        case '/':  /* if any of these, then eval first before pushing OR */
        case '*':
        case '%':
        case '|':
        case '+':
          s_pair(s_oprtr,s_oprnd);
          for (tmp3 = s_oprnd,tmp4 = tmp3;
               tmp3->FWD != NULL; tmp3 = tmp3->FWD)
            tmp4 = tmp3;
          tmp3->RWD = tmp4;
          break;
      } /* SWITCH */
      s_push(s_oprtr,current,(struct node2class *)NULL);
      for (tmp3 = s_oprtr,tmp4 = tmp3; tmp3->FWD != NULL; tmp3 = tmp3->FWD)
        tmp4 = tmp3;
      tmp3->RWD = tmp4;
      break;
    case '/': /* push the NOT operator after checking
                 that previous one is not a '/' also */
      switch(tmp1rtr->symbol[0]) {
        case '/':
          s_pair(s_oprtr,s_oprnd); /* eval current oprtrs; fix oprnd stk*/
          for (tmp3 = s_oprnd,tmp4 = tmp3;
               tmp3->FWD != NULL; tmp3 = tmp3->FWD)
            tmp4 = tmp3;
          tmp3->RWD = tmp4;
          break;
      } /* SWITCH */
      s_push(s_oprtr,current,(struct node2class *)NULL);
      for (tmp3 = s_oprtr,tmp4 = tmp3;
           tmp3->FWD != NULL; tmp3 = tmp3->FWD)
        tmp4 = tmp3;
      tmp3->RWD = tmp4;
      break;
    case '(': /* push onto operator stack */
      s_push(s_oprtr,current,(struct node2class *)NULL);
      for (tmp3 = s_oprtr,tmp4 = tmp3; tmp3->FWD != NULL; tmp3 = tmp3->FWD)
        tmp4 = tmp3;
      tmp3->RWD = tmp4;
      break;
    case '*': /* AND has highest priority */
      switch(tmp1rtr->symbol[0]) {
        case '*':
        case '/':
          s_pair(s_oprtr,s_oprnd); /* eval current oprtrs; fix oprnd stk*/
          for (tmp3 = s_oprnd,tmp4 = tmp3;
               tmp3->FWD != NULL; tmp3 = tmp3->FWD)
            tmp4 = tmp3;
          tmp3->RWD = tmp4;
          break;
      } /* SWITCH */
      s_push(s_oprtr,current,(struct node2class *)NULL);
      for (tmp3 = s_oprtr,tmp4 = tmp3;
           tmp3->FWD != NULL; tmp3 = tmp3->FWD)
        tmp4 = tmp3;
      tmp3->RWD = tmp4;
      break;
    case '%':
    case '|':
      switch(tmp1rtr->symbol[0]) {
        case '*': /* if any of these, then eval first before pushing */
        case '/':
        case '%':
        case '|':
          s_pair(s_oprtr,s_oprnd);
          for (tmp3 = s_oprnd,tmp4 = tmp3;
               tmp3->FWD != NULL; tmp3 = tmp3->FWD)
            tmp4 = tmp3;
          tmp3->RWD = tmp4;
          break;
      } /* SWITCH */
      s_push(s_oprtr,current,(struct node2class *)NULL);
      for (tmp3 = s_oprtr,tmp4 = tmp3;
           tmp3->FWD != NULL; tmp3 = tmp3->FWD)
        tmp4 = tmp3;
      tmp3->RWD = tmp4;
      break;
    case '=':
      for (;;) {
        for (tmp1rtr = s_oprtr; tmp1rtr->FWD != NULL; tmp1rtr = tmp1rtr->FWD)
          ; /* find last item in s_oprtr */
        for (tmp1rnd = s_oprnd; tmp1rnd->FWD != NULL; tmp1rnd = tmp1rnd->FWD)
          ; /* find last item in s_oprnd */
        if (tmp1rtr->symbol == NULL) {
          if (tmp1rnd->symbol == NULL) {
            err(FATAL,"Missing operands",0,0,0);
          if (tmp1rnd->RWD != NULL)
            { if (tmp1rnd->RWD->symbol == NULL
                     && tmp1rnd->pin_no == NULL_VALUE)
                return;
              else
                err(FATAL,"Operator between operands",0,0,0);
            } /* IF */
          } /* IF */
        } /* IF */
        if (tmp1rtr->symbol[0] == '(')
          err(FATAL,"Unmatched (",0,0,0);
        s_pair(s_oprtr,s_oprnd);
        for (tmp3 = s_oprnd,tmp4 = tmp3;
             tmp3->FWD != NULL; tmp3 = tmp3->FWD)
          tmp4 = tmp3;
        tmp3->RWD = tmp4;
        for (tmp1rtr = s_oprtr; tmp1rtr->FWD != NULL; tmp1rtr = tmp1rtr->FWD)
          ; /* find last item in s_oprtr */
        for (tmp1rnd = s_oprnd;; tmp1rnd = tmp1rnd->FWD) {
          if (tmp1rnd->symbol != NULL)
            break; /* find 1st non NULL symbol which will be first operand */
        } /* FOR */
        if (tmp1rtr->symbol == NULL) {
          if (tmp1rnd->symbol == NULL)
            err(FATAL,"No operand",0,0,0);
          else {
            if (tmp1rnd->FWD != NULL)
              err(FATAL,"Not enough operators",0,0,0);
            else
              return;
          } /* ELSE */
        } /* IF TMP1RTR->SYMBOL == NULL */
      } /* FOR */
    case ')':
      for (tmp2rtr = tmp1rtr;;) {
        if (tmp2rtr->symbol == NULL)
          err(FATAL,"No (",0,0,0);
        if (tmp2rtr->symbol[0] == '(')
          break;
        s_pair(s_oprtr,s_oprnd);
        for (tmp3 = s_oprnd,tmp4 = tmp3;
             tmp3->FWD != NULL; tmp3 = tmp3->FWD)
          tmp4 = tmp3;
        tmp3->RWD = tmp4;
        for (tmp2rtr = s_oprtr; tmp2rtr->FWD != NULL; tmp2rtr = tmp2rtr->FWD)
          ; /* find last item in s_oprtr */
      } /* FOR */
      if (tmp2rtr->symbol != NULL) {
        tmp3 = tmp2rtr->RWD;
        tmp3->FWD = NULL;
        free(tmp2rtr); /* remove ( */
      } /* IF */
      break;
    default :
      fprintf(stderr,"\n[%s] S_EVAL",tmp1rtr->symbol);
      exit(BAD_EXIT);
  } /* SWITCH */
  if (current != '=')
    return;
} /* FOR */
} /* S_EVAL */
