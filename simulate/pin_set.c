/* June 25, 1987: PIN_SET.C contains routines used
to configure and set the device pins. This configuration
depends on the link information in the JEDEC map and
the equation formats available in the device. For example,
a 22V10 can have an equation with 44 possible variables
in a product term. This format is specified in the
device simulation database, but the actual PT is dependent
on the links specified in the JEDEC map: a 0 in a particular
position in the map will leave a variable intact in the PT,
a 1 will remove it.
*/
#define FILE_PIN_SET 1
#include "simdef.h"
/*  -   -   -   -   -   -   */
/* Put the CHR or STR into the SYMBOL field of the PUTPTR */
s_put_symbol(putptr,chrput,str)
struct node2class *putptr;
char chrput,str[];
{ char tmp[2];

if (putptr == NULL)
  err(FATAL,"ptr NULL",0,0,0);
if (str[0] == '\0') {
  if((putptr->symbol = malloc(sizeof(tmp))) == NULL)
    err(FATAL,"PMF right",0,0,0);
  tmp[0] = chrput;
  tmp[1] = '\0';
  strcpy(putptr->symbol,tmp);
}
else {
  if ((putptr->symbol = malloc((unsigned)(strlen(str) + 1))) == NULL)
    err(FATAL,"PMF right",0,0,0);
  strcpy(putptr->symbol,str);
} /* ELSE */
} /* S_PUT_SYMBOL */
/*  -   -   -   -   -   -   */
struct node2class *new_ND2()
{ struct node2class *newtmp;
newtmp = (struct node2class *)malloc(sizeof(struct node2class));
mem2_check((struct node1class *)NULL,newtmp,(struct PT_element *)NULL,
    (struct pin_element *)NULL,(struct dfn_class *)NULL);
newtmp->symbol = NULL;
newtmp->pin_no = NULL_VALUE;
newtmp->FWD = NULL;
newtmp->RWD = NULL;
newtmp->LEFT_0 = NULL;
newtmp->RIGHT_1 = NULL;
return(newtmp);
} /* NEW_ND2 */
/*  -   -   -   -   -   -   */
/* architecture parse: get the architecture feature set by this
   fuse/PT and put it in the list for that pin. If ACT is YES,
   then parse the arch feature; if NO, then skip until the <.
*/
arch_parse(act)
int act;
{ int pin_no;
  char nbuffer[MAX_NAME],text_buf[MAX_NAME];
  struct dfn_class *arch_data,*xptr;
  struct nmbr_list *ptr_all_pin,*xtmp;

lex(icon);
if (icon[0] != '>') {
  get_new_token = NO;
  return;
}
pin_no = get_index(nbuffer);
lex(icon);
switch(token_type) {
 case VARIABLE:
 case KEYWORD : break;
 default      : sprintf(main_msg,"[%s] Invalid feature name",icon);
                inform(main_msg,CONTINUE);
                return;
} /* SWITCH */
strcpy(nbuffer,icon);
srch_icon("<",FATAL,YES,"Use > < for Feature List");
if (act == NO)
  return;
arch_data = (struct dfn_class *)malloc(sizeof(struct dfn_class));
mem2_check((struct node1class *)NULL,(struct node2class *)NULL,
    (struct PT_element *)NULL,(struct pin_element *)NULL,arch_data);
strcpy(arch_data->class1_name,"$");
strcat(arch_data->class1_name,nbuffer);
arch_data->list_val = NULL;
arch_data->nxt_dfn = NULL;
for (ptr_all_pin = pin_arch_list;
     ptr_all_pin != NULL;
     ptr_all_pin = ptr_all_pin->nxt_pin) {
  if (ptr_all_pin->x_pin == pin_no) {
    if (ptr_all_pin->list_features == NULL) {
      ptr_all_pin->list_features = arch_data;
      return;
    }
    strcpy(text_buf,"$");
    strcat(text_buf,nbuffer);
    for (xptr = ptr_all_pin->list_features;;xptr = xptr->nxt_dfn) {
      if (!strcmp(xptr->class1_name,text_buf))
         err(FATAL,"Name redefinition",0,0,0);
      if (xptr->nxt_dfn == NULL)
        break;
    }
    xptr->nxt_dfn = arch_data;
    return;
  } /* IF */
} /* FOR */
if (ptr_all_pin == NULL) {
  xtmp = (struct nmbr_list *)malloc(sizeof(struct nmbr_list));
  mem_size_chk(xtmp,(struct pair_name_no *)NULL);
  xtmp->x_pin = pin_no;
  xtmp->list_features = arch_data;
  xtmp->nxt_pin = NULL;
  if (pin_arch_list == NULL) {
    pin_arch_list = xtmp;
    return;
  }
} /* IF */
for (ptr_all_pin = pin_arch_list;
     ptr_all_pin->nxt_pin != NULL;
     ptr_all_pin = ptr_all_pin->nxt_pin)
  ; /* goto end of list */
ptr_all_pin->nxt_pin = xtmp;
} /* ARCH_PARSE */
/*  -   -   -   -   -   -   */
/*
push either an S_ITEM or an element pointed to by S_PTR onto the list
SLIST
*/
s_push(slist,s_item,s_ptr)
struct node2class *slist;
char s_item;
struct node2class *s_ptr;
{ struct node2class *sp1,*sp2;

if (slist == NULL)
  err(FATAL,"SLIST NULL",0,0,0);
if (s_item == '\0' && s_ptr == NULL)
  err(FATAL,"S_PUSH Err",0,0,0);

for (sp2 = slist; sp2->FWD != NULL; sp2 = sp2->FWD)
  ;
if (s_item != '\0') {
  sp1 = new_ND2();
  s_put_symbol(sp1,s_item,"");
  sp2->FWD = sp1;
  sp1->RWD = sp2;
  return;
} /* IF */
sp2->FWD = s_ptr;
s_ptr->RWD = sp2;
} /* S_PUSH */
/*  -   -   -   -   -   -   */
struct node2class *s_prse_pin(s_last_item)
char s_last_item;
{ struct node2class *s_pinptr;
  char sevalbuf[MAX_NAME];

s_pinptr = new_ND2();
lex(icon);
switch(icon[0]) {
  case ')':
    if (s_last_item == '@' || s_last_item == '/')
      err(FATAL,"Put a PT name or pin",0,0,0);
    get_new_token = NO;
    free(s_pinptr);
    return(NULL);
  case '@':
    if (s_last_item != '/')
      err(FATAL,"Unknown expr",0,0,0);
    strcpy(sevalbuf,"@");
    lex(icon);
    if (icon[0] == '.') {
      strcat(sevalbuf,".");
      lex(icon);
    } /* IF */
    if (token_type != VARIABLE)
      err(FATAL,"Var name required",0,0,0);
    strcat(sevalbuf,icon);
    s_put_symbol(s_pinptr,'\0',sevalbuf);
    return(s_pinptr);
  default:
    switch(token_type) {
      case NUMBER:
        if (s_last_item == '@' || s_last_item == '.')
          err(FATAL,"Unknown pin",0,0,0);
        get_new_token = NO;  /* push token back for GET_INDEX() */
        s_pinptr->pin_no = get_index(sevalbuf);
        lex(icon);
        if (icon[0] == '.') {
          lex(icon);
          if (icon[0] != 'q') /* for output Q */
            err(FATAL,"Use Q",0,0,0);
          s_put_symbol(s_pinptr,Q_OUTPUT,"");
        }
        else {
          s_put_symbol(s_pinptr,EQNMBR,"");
          get_new_token = NO;
        }
        return(s_pinptr);
      case VARIABLE:
        if (s_last_item == '@')
          sprintf(sevalbuf,"@%s",icon);
        else
          strcpy(sevalbuf,icon);
        s_put_symbol(s_pinptr,'\0',sevalbuf);
        return(s_pinptr);
      default:
        err(FATAL,"Var/number or )",0,0,0);
    } /* SWITCH */
} /* SWITCH */
} /* S_PRSE_PIN */
/*  -   -   -   -   -   -   */
/* skip over the definition enclosed in parentheses;
   this will also handle nested definitions */
skip_case()
{ int nest_levels;
for (nest_levels = 0,lex(icon);;lex(icon)) {
  switch(icon[0]) {
    case '(':
      ++nest_levels;
      break;
    case ')':
      --nest_levels;
      break;
  } /* SWITCH */
  if (nest_levels == 0)
    break;
} /* FOR */
} /* SKIP_CASE */
/*  -   -   -   -   -   -   */
/* FUSE_EVAL will return the value associated with the fuse state: for
   example, !1992 : 0 (1) : 1 (0); means if 1992 is 0, then return a 1,
   else return a 0
*/
fuse_test()
{
  int fuse_no,state0,state1;
  char xxbuf[MAX_NAME];

lex(icon);
switch(icon[0]) {
case '!':
  fuse_no = get_index(xxbuf);
  srch_icon(":",FATAL,YES,"Put 0/1 cases");
  for (state1 = get_index(xxbuf);
       xxbuf[0] != jedec_map[fuse_no];
       state1 = get_index(xxbuf)) {
    arch_parse(NO);
    skip_case();
    srch_icon(":",FATAL,YES,USE_COLON);
  } /* FOR */
  arch_parse(YES);
  srch_icon("(",FATAL,YES,"feature specified by fuse");
  state1 = fuse_test();
  lex(icon);
  switch(icon[0]) {
    case ')':
      get_new_token = NO;
      srch_icon(")",FATAL,YES,"use )");
      break;
  } /* SWITCH */
  lex(icon);
  switch(icon[0]) {
    case ':' :
      state0 = get_index(xxbuf);
      skip_case();
      break;
    case ';' :
      get_new_token = NO;
      break;
  } /* SWITCH */
  return(state1);
case '&':
case '@':
case '/':
  err(FATAL,"Specify a decimal number",0,0,0);
  break;
case '^':  /* return the constant */
  state1 = get_index(xxbuf);
  if (state1 != 0 && state1 != 1)
    err(FATAL,"Use 0/1",0,0,0);
  return(state1);
case ')':
  get_new_token = NO;
  return(-100);
default :
  if (token_type != NUMBER)
    err(FATAL,"Use numbers",0,0,0);
  get_new_token = NO;
  state1 = get_index(xxbuf);
  return(state1);
} /* SWITCH */
} /* FUSE_TEST */
/*  -   -   -   -   -   -   */
/* obtain the architecture feature defined by the fuse cell */
struct node2class *fuse_eval()
{ struct node2class *xxptr,*xtmp_ptr;
  int fuse_no,state1,cases;
  char xxbuf[MAX_NAME],xtrasymb;

lex(icon);
switch(icon[0]) {
case '!':
  fuse_no = get_index(xxbuf);
  srch_icon(":",FATAL,YES,"Put 0/1 cases");
  for (state1 = get_index(xxbuf);
       xxbuf[0] != jedec_map[fuse_no];
       state1 = get_index(xxbuf)) {
    arch_parse(NO);
    skip_case();
    srch_icon(":",FATAL,YES,USE_COLON);
  } /* FOR */
  arch_parse(YES);
  srch_icon("(",FATAL,YES,"feature specified by fuse");
  xxptr = fuse_eval();
  lex(icon);
  switch(icon[0]) {
    case ':':  /* skip over the other case */
      state1 = get_index(xxbuf);
      skip_case();
      break;
    default :
      get_new_token = NO;
      srch_icon(")",FATAL,YES,"use )");
      break;
  } /* SWITCH */
  return(xxptr);
case '&':
case '@':
case '/':
  xtrasymb = icon[0];
  if (icon[0] == '/') {
    xxptr = new_ND2();
    s_put_symbol(xxptr,'/',"");
    xxptr->RIGHT_1 = s_prse_pin('/');
    if (xxptr->RIGHT_1 != NULL)
      xxptr->RIGHT_1->RWD = xxptr;
  }
  else
    xxptr = s_prse_pin(icon[0]);
  arch_parse(YES);
  lex(icon);
  if (icon[0] != ':') {
    get_new_token = NO;
    return(xxptr);
  }
  if (xtrasymb == '/') {
    xtmp_ptr = xxptr->RIGHT_1;
    if (xtmp_ptr == NULL)
      err(FATAL,"No name",0,0,0);
  }
  else
    xtmp_ptr = xxptr;

/* handle the 2 cases that this product term can take: 0/1. Put the 0
   case in the LEFT_0 subtree and the 1 case in the RIGHT_1 subtree */

  for (cases = 0; cases < 2; ++cases) {
    if (cases == 1)
      srch_icon(":",FATAL,YES,USE_COLON);
    state1 = get_index(xxbuf);
    arch_parse(YES);
    srch_icon("(",FATAL,YES,"Put (");
    if (state1 == 0)  {
      if (xtmp_ptr->LEFT_0 != NULL)
        err(FATAL,"Define 0 value once",0,0,0);
      xtmp_ptr->LEFT_0 = fuse_eval();
      if (xtmp_ptr->LEFT_0 != NULL)
        xtmp_ptr->LEFT_0->RWD = xtmp_ptr;
    } /* IF */
    else {
      if (xtmp_ptr->RIGHT_1 != NULL)
        err(FATAL,"Define 1 value once",0,0,0);
      xtmp_ptr->RIGHT_1 = fuse_eval();
      if (xtmp_ptr->RIGHT_1 != NULL)
        xtmp_ptr->RIGHT_1->RWD = xtmp_ptr;
    } /* ELSE */
    srch_icon(")",FATAL,YES,"use )");
  } /* FOR */
  return(xxptr);
case '^':  /* return the constant */
  xxptr = new_ND2();
  s_put_symbol(xxptr,'^',"");
  state1 = get_index(xxbuf);
  if (state1 != 0 && state1 != 1)
    err(FATAL,"Use 0/1",0,0,0);
  xxptr->pin_no = state1;
  return(xxptr);
case ')':
  get_new_token = NO;
  return(NULL);
default :
  if (token_type != NUMBER)
    err(FATAL,"Use PTs/links",0,0,0);
  get_new_token = NO;
  xxptr = s_prse_pin(' ');
  lex(icon);
  if (icon[0] != ':') {
    get_new_token = NO;
    return(xxptr);
  }
  if (xxptr == NULL)
    err(FATAL,"No name",0,0,0);

/* handle the 2 cases that this pin can take: 0/1. Put the 0
   case in the LEFT_0 subtree and the 1 case in the RIGHT_1 subtree */

  for (cases = 0; cases < 2; ++cases) {
    if (cases == 1)
      srch_icon(":",FATAL,YES,USE_COLON);
    state1 = get_index(xxbuf);
    arch_parse(YES);
    srch_icon("(",FATAL,YES,"Put (");
    if (state1 == 0)  {
      if (xxptr->LEFT_0 != NULL)
        err(FATAL,"Define 0 value once",0,0,0);
      xxptr->LEFT_0 = fuse_eval();
      if (xxptr->LEFT_0 != NULL)
        xxptr->LEFT_0->RWD = xxptr;
    } /* IF */
    else {
      if (xxptr->RIGHT_1 != NULL)
        err(FATAL,"Define 1 value once",0,0,0);
      xxptr->RIGHT_1 = fuse_eval();
      if (xxptr->RIGHT_1 != NULL)
        xxptr->RIGHT_1->RWD = xxptr;
    } /* ELSE */
    srch_icon(")",FATAL,YES,"use )");
  } /* FOR */
  return(xxptr);
} /* SWITCH */
} /* FUSE_EVAL */
/*  -   -   -   -   -   -   */
/* get the PT referenced by the & and @ operator. Note that the name
can take the form @.NAME where the . is used to force evaluation of the
PT node referenced by the [.] operator. The ACTION is used by the
PTNAME_EVAL() routine to return a pointer with the parsed names when
the JEDEC map has a connection there or simply parsing the names when
skipping the unconnected links */
struct node2class *s_get_name(action,name_type)
int action;
char name_type[];
{ struct node2class *gtname;
  char Xname[MAX_NAME];

if (action == YES)
  gtname = new_ND2();
lex(icon);
if (icon[0] == '.') /* [.] is the eval operator */ {
  if (action == YES) {
    strcpy(Xname,name_type);
    strcat(Xname,".");
  }
  lex(icon);
  if (token_type != VARIABLE)
    err(FATAL,"Var/name",0,0,0);
  if (action == YES) {
    strcat(Xname,icon);
    s_put_symbol(gtname,'\0',Xname);
    return(gtname);
  } /* IF action == YES */
} /* IF */
switch(token_type) {
  case VARIABLE:
    break;
  case NUMBER:
    if (icon[0] == '0' || icon[0] == '1')
      break;
  default:
    err(FATAL,"Var/name",0,0,0);
} /* SWITCH */
if (action == YES) {
  switch(token_type) {
    case VARIABLE:
      strcpy(Xname,name_type);
      strcat(Xname,icon);
      s_put_symbol(gtname,'\0',Xname);
      return(gtname);
    case NUMBER:
      if ((gtname->symbol = malloc(2)) == NULL)
        err(FATAL,"SGN mem",0,0,0);
      strcpy(gtname->symbol,"^");
      gtname->pin_no = (int)atoi(icon);
      return(gtname);
    default:
      err(FATAL,"Action: Var/name",0,0,0);
  } /* SWITCH */
} /* IF */
free(gtname);
return(NULL);
} /* S_GET_NAME */
/*  -   -   -   -   -   -   */
/*
generate a PT from the EQTYPE and the starting address used.
Get the starting fuse address and find the corresponding bit in
the JEDECMAP buffer. Continue until the EVAL_MAX count (maximum
number of inputs in a PT) is reached. EVAL_DIR is used to add or
subtract from the starting address by the specified amount.

The FIRST_OPR is a flag which will indicates if an operand has been
pushed on the operator stack. When a binary operator is seen, at
least one operand must be on the stack, else skip the operator.
*/
struct node2class *PTname_eval()
{ struct EQFORMAT *eqptr1;
  struct EQ_FRM *eq1form;
  int eval_cnt,eval_max,eval_dir,stop,first_opr,fuse_cntr;
  char ebuf[MAX_NAME];
  struct node2class *PToprtr,*PToprnd,*cpy,*tmp3,*tmp4;

srch_icon("(",FATAL,YES,"enclose EQTYPE name");
lex(icon);
if (token_type != VARIABLE)
  err(FATAL,"Use EQ type name",0,0,0);
for (eqptr1 = eq_frm_list; eqptr1 != NULL; eqptr1 = eqptr1->MORE_EQ) {
  if (!strcmp(icon,eqptr1->EQNAME)) {
    eval_max = eqptr1->EQWIDTH;
    eval_dir = eqptr1->EQ_OFFSET;
    eq1form = eqptr1->EQFRM;
    break;
  }
} /* FOR */
if (eqptr1 == NULL)
  err(FATAL,"Unknown EQ type",0,0,0);
srch_icon(")",FATAL,YES,"close EQtype name");
srch_icon(":",FATAL,YES,"specify link number next");
eval_cnt = get_index(ebuf);
if (eval_cnt >= fuse_count)
  err(FATAL,"Out of Range",0,0,0);

/* --------------
check if an equation row is all 0s; if all connected,
(all 0s), then return a NULL pointer, meaning no PT
needed here since it will always be a logic 0
*/
for (stop = 0; stop < eval_max; stop = stop+eval_dir) {
  if ((stop+eval_cnt) >= fuse_count)
    err(FATAL,"+Out of Range",0,0,0);
  if (jedec_map[stop+eval_cnt] == '1')
    break;
} /* FOR */
if (stop >= eval_max) {
  if (debug_flag == YES)
    fprintf(stderr,"\nPT [%3d] all connected",eval_cnt/eval_max);
  return(NULL);
}
/* --------------
check if an equation row is all 1s; if all disconnected,
(all 1s), then return a constant 1 pointer, meaning a
constant 1 is here since it will always be a logic 1/TRUE
*/
for (stop = 0; stop < eval_max; stop = stop+eval_dir) {
  if (jedec_map[stop+eval_cnt] == '0')
    break;
} /* FOR */
if (stop >= eval_max) {
  if (debug_flag == YES)
    fprintf(stderr,"\nPT [%d] disconnected/TRUE",eval_cnt/eval_max);
  cpy = new_ND2();
  if ((cpy->symbol = malloc((unsigned)(strlen("^")+1))) == NULL)
     err(FATAL,"Eval mem",0,0,0);
   strcpy(cpy->symbol,"^");
   cpy->pin_no = 1; /* return a constant 1 */
   return(cpy);
}

PToprtr = new_ND2();
PToprnd = new_ND2();
for (first_opr = NO,fuse_cntr = 0; fuse_cntr < eval_max && eq1form != NULL;
       eval_cnt = eval_cnt + eval_dir,++fuse_cntr) {

  if (jedec_map[eval_cnt] == '0')  /* JEDEC link is still intact */ {
    for (stop = NO; stop != YES; ) {
      if (eq1form == NULL)
        err(FATAL,"EQTYPE Err",0,0,0);
      switch(eq1form->EQ_opr) {
      case '/':
        s_eval('/',PToprtr,PToprnd);
        for (tmp3 = PToprtr,tmp4 = tmp3;
             tmp3->FWD != NULL;
             tmp3 = tmp3->FWD)
          tmp4 = tmp3;
        tmp3->RWD = tmp4;
        break;
      case '(':
        s_push(PToprtr,eq1form->EQ_opr,(struct node2class *)NULL);
        for (tmp3 = PToprtr,tmp4 = tmp3;
             tmp3->FWD != NULL;
             tmp3 = tmp3->FWD)
          tmp4 = tmp3;
        tmp3->RWD = tmp4;
        break;
      case '&':  /* a reference to a node */
      case '@':  /* a reference to a PT */
        cpy = new_ND2();
        strcpy(ebuf,eq1form->a_PT_name);
        s_put_symbol(cpy,'\0',ebuf);
        s_push(PToprnd,'\0',cpy);
        for (tmp3 = PToprnd, tmp4 = tmp3;
             tmp3->FWD != NULL;
             tmp3 = tmp3->FWD)
          tmp4 = tmp3;
        tmp3->RWD = tmp4;
        stop = YES;
        first_opr = YES;
        break;
      case ')':
        s_eval(')',PToprtr,PToprnd);
        break;
      case EQNMBR:
        cpy = new_ND2();
        s_put_symbol(cpy,EQNMBR,"");
        cpy->pin_no = eq1form->EQ_pin;
        s_push(PToprnd,'\0',cpy);
        for (tmp3 = PToprnd,tmp4 = tmp3;
             tmp3->FWD != NULL;
             tmp3 = tmp3->FWD)
          tmp4 = tmp3;
        tmp3->RWD = tmp4;
        first_opr = YES;
        stop = YES;
        break;
      case '*':
      case '+':
      case '|':
      case '%':
        if (first_opr == YES)
          s_eval(eq1form->EQ_opr,PToprtr,PToprnd);
        break;
      default :
        err(FATAL,"check 0:EQ_opr type",0,0,0);
      } /* SWITCH */
      switch(eq1form->EQ_opr) {
        case '*':
        case '+':
        case '/':
        case '|':
        case '%':
        case '(':
        case ')':
        case EQNMBR:
        case '@':
        case '&':
          eq1form = eq1form->nxt_EQ;
          break;
      } /* SWITCH */
    } /* FOR */
  } /* IF */
  else {
    for (stop = NO; stop != YES && eq1form != NULL;) {
      switch(eq1form->EQ_opr) {
        case '(':
           s_push(PToprtr,'(',(struct node2class *)NULL);
           for (tmp3 = PToprtr,tmp4 = tmp3;
                tmp3->FWD != NULL;
                tmp3 = tmp3->FWD)
             tmp4 = tmp3;
           tmp3->RWD = tmp4;
           eq1form = eq1form->nxt_EQ;
           break;
        case ')':
           s_eval(')',PToprtr,PToprnd);
           eq1form = eq1form->nxt_EQ;
           break;
        case '*':
        case '+':
        case '|':
        case '%':
           eq1form = eq1form->nxt_EQ;
           break;
        case '/':
           if (eq1form->nxt_EQ != NULL) {
             if (eq1form->nxt_EQ->EQ_opr == '(') {
               s_push(PToprtr,'/',(struct node2class *)NULL);
               for (tmp3 = PToprtr,tmp4 = tmp3;
                    tmp3->FWD != NULL;
                    tmp3 = tmp3->FWD)
                 tmp4 = tmp3;
               tmp3->RWD = tmp4;
             } /* IF */
           } /* IF */
           eq1form = eq1form->nxt_EQ;
           break;
        case '&':
        case '@': /* do not use parsed name; skip it */
          for (; eq1form != NULL && stop != YES ;eq1form = eq1form->nxt_EQ) {
            switch(eq1form->EQ_opr) {
              case '*':
              case '+':
              case '%':
              case '|':
              case '/':
              case '&':
              case ')':
              case '(': stop = YES;
                        break;
            } /* SWITCH */
          } /* FOR */
          break;
        case EQNMBR:
          eq1form = eq1form->nxt_EQ;
          stop = YES;
          break;
        default :
          err(FATAL,"check 1:EQ_opr type",0,0,0);
      } /* SWITCH */
    } /* FOR */
  } /* ELSE */
} /* FOR */
s_eval('=',PToprtr,PToprnd);
if (PToprtr->FWD != NULL)
  err(FATAL,"opr stk not NULL",0,0,0);
else {
  free(PToprtr);
  PToprtr = NULL;
}
if (PToprnd->symbol == NULL && PToprnd->pin_no == NULL_VALUE) {
  PToprtr = PToprnd->FWD;
  if (PToprtr != NULL)
    PToprtr->RWD = NULL;
  free(PToprnd);
  PToprnd = PToprtr;
  PToprtr = NULL;
}
return(PToprnd);
} /* PTNAME_EVAL */
/*  -   -   -   -   -   -   */
/* Make the node logic equations and return a pointer
*/
struct node2class *nd_eq_parse()
{ struct node2class *s_oprstk,*s_oprndstk,*tmp1,*tmp2,*tmp3,*tmp4;

s_oprstk = new_ND2();
s_oprndstk = new_ND2();
srch_icon("=",FATAL,YES,"eq width and offset");
for (lex(icon);;lex(icon)) {
  switch(icon[0]) {
  case '(':
  case '*':
  case '+':
  case '/':
  case '|':
  case '%':
    s_eval(icon[0],s_oprstk,s_oprndstk);
    break;
  case '>':
    get_new_token = NO;
    arch_parse(YES);
    break;
  case '!':
    get_new_token = NO; /* use the ! token again in FUSE_EVAL() */
    tmp2 = fuse_eval();
    for (lex(icon); icon[0] != ';'; lex(icon))
      ;
    s_push(s_oprndstk,'\0',tmp2);
    for (tmp3 = s_oprndstk,tmp4 = tmp3; tmp3->FWD != NULL; tmp3 = tmp3->FWD)
      tmp4 = tmp3;
    tmp3->RWD = tmp4;
    break;
  case ')':
    s_eval(')',s_oprstk,s_oprndstk);
    break;
  case ';':
    s_eval('=',s_oprstk,s_oprndstk);
    break;
  case '^': /* constant 0/1 */
  case '@':
    if (icon[0] == '^')
      tmp1 = s_get_name(YES,"^");
    else
      tmp1 = s_get_name(YES,"@");
    s_push(s_oprndstk,'\0',tmp1);
    for (tmp3 = s_oprndstk,tmp4 = tmp3; tmp3->FWD != NULL; tmp3 = tmp3->FWD)
      tmp4 = tmp3;
    tmp3->RWD = tmp4;
    break;
  default:
    if (token_type == NUMBER) {
      get_new_token = NO;
      tmp2 = s_prse_pin(' ');
      if (icon[0] == ';') /* S_PRSE_PIN will modify ICON */
        get_new_token = NO;
      if (tmp2 == NULL)
        break;
      s_push(s_oprndstk,'\0',tmp2);
      for (tmp3 = s_oprndstk,tmp4 = tmp3; tmp3->FWD != NULL; tmp3 = tmp3->FWD)
        tmp4 = tmp3;
      tmp3->RWD = tmp4;
      break;
    } /* IF */
    if (the_keyword != EQTYPE)
      err(FATAL,"DB error",0,0,0);
    tmp2 = PTname_eval();
    if (tmp2 == NULL)
      break;
    s_push(s_oprndstk,'\0',tmp2);
    for (tmp3 = s_oprndstk,tmp4 = tmp3; tmp3->FWD != NULL; tmp3 = tmp3->FWD)
      tmp4 = tmp3;
    tmp3->RWD = tmp4;
    break;
  } /* SWITCH */
  if (icon[0] == ';' && get_new_token == YES)
    break; /* leave FOR */
} /* FOR */
 /* return ptr to S_OPRNDSTK which contains evaluated eq */
if (s_oprstk->FWD != NULL)  /* reach this point only if icon[0] == ';' */
  err(FATAL,"ND_EQ Err: Database Err",0,0,0);
else {
  free(s_oprstk);
  s_oprstk = NULL;
}
if (s_oprndstk->symbol == NULL && s_oprndstk->pin_no == NULL_VALUE) {
  s_oprstk = s_oprndstk->FWD;
  if (s_oprstk != NULL)
    s_oprstk->RWD = NULL;
  free(s_oprndstk);
  s_oprndstk = s_oprstk;
  s_oprstk = NULL;
}
return(s_oprndstk);
} /* ND_EQ_PARSE */
/*  -   -   -   -   -   -   */
struct PT_element *get_PT(gtname,get_type)
char gtname[];
int get_type;  /* if 0 then get an expression, else
                  use FUSE_EVAL(); FUSE_EVAL() is used
                  when in the CONFIGURE_PINS() function */

{ struct PT_element *Xptr;
Xptr = (struct PT_element *)malloc(sizeof(struct PT_element));
mem2_check((struct node1class *)NULL,(struct node2class *)NULL,Xptr,
    (struct pin_element *)NULL,(struct dfn_class *)NULL);
strcpy(Xptr->term_name,gtname);
Xptr->pinstates = 0;
if (get_type == 0)
  Xptr->pin_eq = nd_eq_parse();
else {
  Xptr->pin_eq = fuse_eval();
  for (lex(icon); icon[0] != ';'; lex(icon))
    ;
} /* ELSE */
Xptr->next_PT = NULL;
if (Xptr->pin_eq != NULL) {
  if ( (Xptr->pin_eq->FWD == NULL) && (Xptr->pin_eq->RIGHT_1 == NULL) &&
       (Xptr->pin_eq->LEFT_0 == NULL) ) {
    if (Xptr->pin_eq->symbol == NULL && Xptr->pin_eq->pin_no == NULL_VALUE) {
      free(Xptr->pin_eq);
      Xptr->pin_eq = NULL;
    }
  }
}
return(Xptr);
} /* GET_PT */
/*  -   -   -   -   -   -   */
/* Set up the nodes, which are usually PTs and after getting the
   PT describing the node, attach it to the end of the PRODUCT_TERMS
   list
*/
node_set_up()
{ struct PT_element *NDPTR,*set_tmp;

srch_icon("begin",WARNING,YES,"Use BEGIN-END");
for (lex(icon); the_keyword != END; lex(icon)) {
  if (token_type != VARIABLE)
    err(FATAL,"Use NAMES",0,0,0);
  NDPTR = get_PT(icon,0);
  if (NDPTR == NULL && debug_flag == YES)
    err(WARNING,"No PT/link ctrl",1,0,0);
  if (product_terms == NULL)
    err(FATAL,"Set up PR_TERMS",0,0,0);
  for (set_tmp = product_terms;;set_tmp = set_tmp->next_PT) {
    if (!strcmp(set_tmp->term_name,NDPTR->term_name))
      err(FATAL,"PT Redefinition",0,0,0);
    if (set_tmp->next_PT == NULL)
      break;
  } /* FOR */
  set_tmp->next_PT = NDPTR;
} /* FOR */
} /* NODE_SET_UP */
/*  -   -   -   -   -   -   */
/* check if the next token is the target */
srch_icon(target,response,use_up,a_msg)
char target[];
int response,use_up;
char a_msg[];
{ char msg[80];
lex(icon);
if (strcmp(icon,target)) {
  sprintf(msg,"Looking for [%s]: %s\n",target,a_msg);
  err(response,msg,0,0,0);
}
if (use_up == NO)
  get_new_token = NO;
} /* SRCH_ICON */
/*  -   -   -   -   -   -   */
EQ_dfn(atype)
char atype[];
{ struct EQFORMAT *tmp1_EQ,*tmp2_EQ;
  struct EQ_FRM *EQxptr;
  int num_tmp;
  char dfnbuf[MAX_NAME];

tmp1_EQ = (struct EQFORMAT *)malloc(sizeof(struct EQFORMAT));
CORE_chk(tmp1_EQ,(struct EQ_FRM *)NULL);
strcpy(tmp1_EQ->EQNAME,atype);
srch_icon("(",FATAL,YES,"eq width and offset");
tmp1_EQ->EQWIDTH = get_index(dfnbuf);
srch_icon(",",FATAL,YES,"separate number pairs");
tmp1_EQ->EQ_OFFSET = get_index(dfnbuf);
srch_icon(")",FATAL,YES,"close pair def");
srch_icon("=",WARNING,YES,"specify eq format");
tmp1_EQ->EQFRM = NULL;
tmp1_EQ->MORE_EQ = NULL;
if (eq_frm_list == NULL)
  err(FATAL,"init EQ_FRM_LIST",0,0,0);
for (tmp2_EQ = eq_frm_list;
     tmp2_EQ->MORE_EQ != NULL;
     tmp2_EQ = tmp2_EQ->MORE_EQ)
  ;
tmp2_EQ->MORE_EQ = tmp1_EQ;
for (lex(icon),num_tmp = 0;
     icon[0] != ';' && num_tmp < tmp1_EQ->EQWIDTH;
     lex(icon)) {
  pt_EQFRM = (struct EQ_FRM *)malloc(sizeof(struct EQ_FRM));
  CORE_chk((struct EQFORMAT *)NULL,pt_EQFRM);
  pt_EQFRM->EQ_opr = NO_EQ_CHR;
  pt_EQFRM->EQ_pin = NULL_VALUE;
  pt_EQFRM->a_PT_name[0] = '\0';
  pt_EQFRM->nxt_EQ = NULL;
  switch(token_type) {
    case NUMBER
      : get_new_token = NO;
        pt_EQFRM->EQ_opr = EQNMBR;
        pt_EQFRM->EQ_pin = get_index(dfnbuf);
    ++num_tmp;
        break;
    case PUNCT_MARK
      : switch(icon[0]) {
        case '/':
        case '(':
        case ')':
        case '*':
        case '+':
        case '%': /* EXOR */
        case '|': /* XNOR or equivalence */
                  pt_EQFRM->EQ_opr = icon[0];
                  break;
        case '@':
        case '&': dfnbuf[0] = '\0';
                  pt_EQFRM->EQ_opr = icon[0];
                  lex(icon);
                  if (token_type == PUNCT_MARK) {
                    if (icon[0] != '.')
                      err(FATAL,"Use . for eval control",0,0,0);
                    strcat(dfnbuf,".");
                    lex(icon); /* get another token */
                  } /* if PUNCT_MARK */
                  if (token_type == VARIABLE)
                    strcat(dfnbuf,icon);
                  else
                    err(FATAL,"Use a PT name",0,0,0);
                  strcpy(pt_EQFRM->a_PT_name,dfnbuf);
          ++num_tmp;
                  break;
        default : err(FATAL,"Check opr type",0,0,0);
        } /* SWITCH icon[0] */
        break;
    case VARIABLE
      : err(FATAL,"Preface PT name with &",0,0,0);
    default
      : err(FATAL,"Use only numbers/operators",0,0,0);
  } /* SWITCH */
  if (tmp1_EQ->EQFRM == NULL)
    tmp1_EQ->EQFRM = pt_EQFRM;
  else {
    for (EQxptr = tmp1_EQ->EQFRM;
         EQxptr->nxt_EQ != NULL;
         EQxptr = EQxptr->nxt_EQ)
      ;
    EQxptr->nxt_EQ = pt_EQFRM;
  } /* ELSE */
} /* FOR */
if (num_tmp >= tmp1_EQ->EQWIDTH && icon[0] != ';')
  err(FATAL,"Incorrect PT width",0,0,0);
} /* EQ_dfn */
/*  -   -   -   -   -   -   */
pr_eq_set_up()
{ struct EQFORMAT *setptr;
  struct EQ_FRM *setsubptr;

fprintf(stderr,"\nEQ_FRM_LIST contents:");
for (setptr = eq_frm_list; setptr != NULL; setptr = setptr->MORE_EQ) {
  fprintf(stderr,"\nEQNAME [%s] W[%d] Off[%d] =\n",
          setptr->EQNAME,setptr->EQWIDTH,setptr->EQ_OFFSET);
  for (setsubptr = setptr->EQFRM;
       setsubptr != NULL;
       setsubptr = setsubptr->nxt_EQ) {
    switch(setsubptr->EQ_opr) {
    case '/':
    case '(':
    case ')':
    case '*':
    case '+':
    case '%':
    case '|': fprintf(stderr," %c",setsubptr->EQ_opr);
              break;
    case '&': fprintf(stderr," &[%s]",setsubptr->a_PT_name);
              break;
    case EQNMBR: fprintf(stderr," %d",setsubptr->EQ_pin);
              break;
    default : err(FATAL,"Invalid EQTYPE",0,0,0);
    } /* SWITCH */
  } /* FOR */
} /* FOR */
} /* PR_EQ_SET_UP */

/*  -   -   -   -   -   -   */
/*
Set up the equation format. This function will read in the format.
This format will be referred to when processing EQTYPE calls.
*/
eq_set_up()
{
lex(icon);
if (the_keyword != BEGIN)
  err(FATAL,"EQ:BEGIN",0,0,0);
for (lex(icon);token_type == VARIABLE;lex(icon))
  EQ_dfn(icon);  /* define the values for the variable */
if (the_keyword != END)
  err(FATAL,"EQ:END",0,0,0);
srch_icon(";",FATAL,YES,"EQ_SET_UP");
if (debug_flag == YES)
  pr_eq_set_up();
} /* EQ_SET_UP */
/*  -   -   -   -   -   -   */
/* check if ANAME is in the list CLS1_LIST */
chk_dfn_name(aname,cls1_list)
char aname[];
struct dfn_class *cls1_list;
{ struct dfn_class *tmp1;

for (tmp1 = cls1_list; tmp1 != NULL; tmp1 = tmp1->nxt_dfn) {
  if (!strcmp(tmp1->class1_name,aname))
    return(YES);
} /* FOR */
return(NO);
} /* CHK_DFN_NAME */
/*  -   -   -   -   -   -   */
/* add the ITEM for the XNAME variable to XLIST */
cls1_add(item,xname,xlist)
char item,xname[];
struct dfn_class *xlist;
{ struct node1class *nd1,*nd2;

nd1 = (struct node1class *)malloc(sizeof(struct node1class));
mem2_check(nd1,(struct node2class *)NULL,(struct PT_element *)NULL,
    (struct pin_element *)NULL,(struct dfn_class *)NULL);
nd1->node_item = item;
nd1->node1next = NULL;
if (xlist->list_val != NULL) {
  for (nd2 = xlist->list_val; nd2 != NULL; nd2 = nd2->node1next) {
    if (nd2->node_item == item)  /* item already defined */
      return;
    if (nd2->node1next == NULL)
      break;
  } /* FOR */
  nd2->node1next = nd1;
} /* IF */
else
  xlist->list_val = nd1;
} /* CLS1_ADD */
/*  -   -   -   -   -   -   */
/* copy the items associated with SRC to the list pointed to by CLIST */
copy_dfn(src,clist)
char src[];
struct dfn_class *clist;
{ struct dfn_class *ptr1;
  struct node1class *nd_1,*nd_3,*nd_4;

/* point to SRC in the DFN_LIST */
for (ptr1 = dfn_list; ptr1 != NULL; ptr1 = ptr1->nxt_dfn) {
  if (!strcmp(ptr1->class1_name,src))
     break;
} /* FOR */
if (ptr1 == NULL)
  err(FATAL,"Check COPY_DFN",0,0,0);
/* check destination list for this item */
for (nd_1 = ptr1->list_val; nd_1 != NULL; nd_1 = nd_1->node1next) {
  for (nd_3 = clist->list_val; nd_3 != NULL; nd_3 = nd_3->node1next) {
    if (nd_1->node_item == nd_3->node_item)
      break;
  } /* FOR */
  if (nd_3 == NULL) {  /* item is not in the list; add it */
    nd_4 = (struct node1class *)malloc(sizeof(struct node1class));
    mem2_check(nd_4,(struct node2class *)NULL,(struct PT_element *)NULL,
        (struct pin_element *)NULL,(struct dfn_class *)NULL);
    nd_4->node_item = nd_1->node_item;
    nd_4->node1next = NULL;
    if (clist->list_val == NULL)
    clist->list_val = nd_4;
    else {
    for (nd_3 = clist->list_val;
             nd_3->node1next != NULL;
             nd_3 = nd_3->node1next)
      ;
    nd_3->node1next = nd_4;
    }
  } /* IF nd_3 == NULL */
} /* FOR */
} /* COPY_DFN */
/*  -   -   -   -   -   -   */
/*
Store the following value definition(s) for the variable
DFN_NAME in a linked list
*/
dfn_val(dfn_name)
char dfn_name[];
{ struct dfn_class *cls1,*cls2;
  int elmnt;
  char dfnbuf[MAX_NAME];

if (chk_dfn_name(dfn_name,dfn_list) == YES)
  err(FATAL,"Name already defined",0,0,0);
if (strlen(dfn_name) == 1)
  err(FATAL,"Use names with 2+ char",0,0,0);

cls1 = (struct dfn_class *)malloc(sizeof(struct dfn_class));
mem2_check((struct node1class *)NULL,(struct node2class *)NULL,
    (struct PT_element *)NULL,(struct pin_element *)NULL,cls1);
strcpy(cls1->class1_name,dfn_name);
cls1->list_val = NULL;
cls1->nxt_dfn = NULL;
if (dfn_list == NULL)
  err(FATAL,"Init DFN_LIST",0,0,0);
for (cls2 = dfn_list; cls2->nxt_dfn != NULL; cls2 = cls2->nxt_dfn)
  ;
cls2->nxt_dfn = cls1;

lex(icon);
if (icon[0] != '=')
  err(WARNING,"= expected",token_type,VARIABLE,KEYWORD);
for (lex(icon); icon[0] != ';'; lex(icon)) {
  switch(token_type) {
  case NUMBER:
    get_new_token = NO;
    elmnt = get_index(dfnbuf);
    if (elmnt < 0 && elmnt > 9)
      err(FATAL,"Use 0:9",0,0,0);
    cls1_add(icon[0],dfn_name,cls1);
    break;
  case VARIABLE:
    if (strlen(icon) == 1) {
      switch(icon[0]) {
      case 'b':
      case 'c':
      case 'f':
      case 'h':
      case 'k':
      case 'l':
      case 'n':
      case 'p':
      case 'x':
      case 'z': cls1_add(icon[0],dfn_name,cls1);
            break;
      default: err(FATAL,"Non-standard JEDEC symbol",0,0,0);
           break;
      } /* SWITCH */
    } /* IF */
    else {
      if (chk_dfn_name(icon,dfn_list) == NO)
         err(FATAL,"Undefined variable",0,0,0);
        /* copy the value definitions for this name into
           the definition list of DFN_NAME */
      copy_dfn(icon,cls1);
    } /* ELSE */
    break;
  case PUNCT_MARK:
    if (icon[0] != ',')
      err(WARNING,"Use , to separate items",0,0,0);
    break;
  case KEYWORD:
  default:
    err(FATAL,"Use number/name",0,0,0);
    break;
  } /* SWITCH */
} /* FOR */
} /* DFN_VAL */
/*  -   -   -   -   -   -   */
/*
Define symbols: used to define possible pin values. For
example: INPUT is defined as being 0:9,X,N,Z and F.
*/
sim_define()
{
struct dfn_class *dfnshow;
struct node1class *dfnd1;

lex(icon);
if (the_keyword != BEGIN)
  err(FATAL,"DB:BEGIN",0,0,0);
for (lex(icon);token_type == VARIABLE;lex(icon))
  dfn_val(icon);  /* define the values for the variable */
if (the_keyword != END)
  err(FATAL,"DB:END",0,0,0);
lex(icon);
if (icon[0] != ';')
  get_new_token = NO;
if (debug_flag == YES) {
  fprintf(stderr,"\nDFN_LIST has: ");
  for (dfnshow = dfn_list; dfnshow != NULL; dfnshow = dfnshow->nxt_dfn) {
    fprintf(stderr,"\nList_name [%s]",dfnshow->class1_name);
    for (dfnd1 = dfnshow->list_val; dfnd1 != NULL; dfnd1 = dfnd1->node1next)
      fprintf(stderr,"\ndfnd1->node_item [%c]",dfnd1->node_item);
  } /* FOR */
} /* IF */
} /* SIM_DEFINE */
