/* August 1, 1987 APPLY.C contains the routines which read the vectors from
   the specified file (could be the same as the JEDEC map file) and then
   applies these vectors to the logic model created from the JEDEC map.
*/
#include "simdef.h"
/*  -   -   -   -   -   */
/* convert the vector element V_ELEMENT to either a 0 or 1 */
chk_a_vctr(v_grp,v_element,action)
char v_grp[];
int v_element,action;
{
if (v_grp[0] == '\0')
  return(dflt_test);
switch(v_grp[v_element-1]) {
  case 'K':
    switch(action) {
      case CLOCK:
      case LOW:
        return(4);
      case INIT_PT:
      case PRELOAD:
        return(0);
      default:
        inform("Clock signal 1-0-1",CONTINUE);
        return(0);
    } /* SWITCH */
  case '0':
    return(0);
  case 'C':
    switch(action) {
      case CLOCK:
      case LOW:
        return(5);
      case INIT_PT:
        return(1);
      case PRELOAD:
        return(0);
      default:
        inform("Clock signal 0-1-0",CONTINUE);
        return(1);
    } /* SWITCH */
  case '1':
    return(1);
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    sprintf(main_msg,"Super-Voltages (pin %d)",v_element);
    inform(main_msg,STOP);
  case 'L': return(0);
  case 'H': return(1);
  case 'X': return(dflt_test);
  case 'B':
  case 'P':
    if (debug_flag == YES || debug_flag == 200) {
      sprintf(main_msg,"\nNote: Preload (pin %d)",v_element);
      pr_msgs(main_msg,YES);
    }
/* a CLOCK or LOW means the program is checking the previous vector
   element state; return a 0 since the previous B/P will have returned
   to 0
*/
    if (action == CLOCK || action == LOW)
      return(0);
    if (v_grp[v_element-1] == 'P')
      return(2);
    return(3);  /* return B value */
  case 'F':
    if (debug_flag == YES || debug_flag == 200) {
      sprintf(main_msg,"\nNote: Pin floating (pin %d)",v_element);
      pr_msgs(main_msg,YES);
    }
    return(dflt_test);
  case 'Z':
    if (debug_flag == YES || debug_flag == 200) {
      sprintf(main_msg,"\nNote: High-Z state (pin %d)",v_element);
      pr_msgs(main_msg,YES);
    }
    return(dflt_test);
  case 'N':
    if (debug_flag == YES || debug_flag == 200) {
      sprintf(main_msg,"\nNote: Pin [%d] not tested",v_element);
      pr_msgs(main_msg,YES);
    }
    return(dflt_test);
  default:
    sprintf(main_msg,"Invalid CHK_VCTR [%c]",v_grp[v_element-1]);
    inform(main_msg,STOP);
    break;
} /* SWITCH */
} /* CHK_A_VCTR */
/*  -   -   -   -   -   */
/* check if the C_PIN has been configured as an IO pin or as a dedicated
   input pin; if it is an i/o or output pin, return the previous state
   through P_STATE. REG_CLASS is either REG_CELL/0 or LATCH_CELL/1
*/
chk_io_pin(c_pin,p_state,vctr_grp,last_vctr_grp,chk_act,reg_class)
int c_pin,*p_state;
char vctr_grp[],last_vctr_grp[];
int chk_act,reg_class;
{ struct pin_element *chk_ptr;
  int tmp_pin;

if (vctr_grp[0] == '\0')
  inform("CHK_IO_PIN Err",STOP);
for (chk_ptr = test_pin_elements;
     chk_ptr != NULL;
     chk_ptr = chk_ptr->nxt_tst_pin) {
  if (chk_ptr->vctr_element == c_pin)
    break;
} /* FOR */
if (chk_ptr == NULL) {
  sprintf(main_msg,"Pin [%d] not found",c_pin);
  inform(main_msg,STOP);
} /* IF */
/* If the OUT_ELEMENT field in the structure is not NULL, then this pin
   has output or I/O capability. If OUT_ELEMENT is NULL, then only inputs
   must be sent to this pin */
if (chk_ptr->out_element == NULL) {
  switch (vctr_grp[c_pin-1]) {
    case 'H':
    case 'L':
    case 'Z':
      sprintf(main_msg,"\nPin [%d] is an INPUT",c_pin);
      pr_msgs(main_msg,YES);
    case 'F':
    case 'C':
    case 'K':
    case 'N':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case 'X':
    case 'P':
    case 'B':
      return(NO);
    default:
      sprintf(main_msg,"Invalid symbol [%c] in ->[%s]",
              vctr_grp[c_pin-1],vctr_grp);
      inform(main_msg,STOP);
  } /* SWITCH */
} /* IF */
switch (vctr_grp[c_pin-1]) {
  case 'H':
  case 'L':
  case 'Z':
  case 'N':
  case 'X':
    tmp_pin = calcPT(chk_ptr->reg_latch.PTenable->pin_eq,
                     vctr_grp,last_vctr_grp,chk_act,reg_class);
    bin_test(tmp_pin);
    if (tmp_pin == 1 || chk_act == INIT_PT) {
      *p_state = (unsigned)((chk_ptr->pin_status >> 1) & 1);
      return(YES);
    } /* IF */
    if (vctr_grp[c_pin-1] != 'X' && vctr_grp[c_pin-1] != 'Z') {
      sprintf(main_msg,"\nPin [%2d] configured as INPUT; use 0/1/F",c_pin);
      pr_msgs(main_msg,YES);
    }
    return(NO);
  case 'F':
  case 'C':
  case 'K':
  case '0':
  case '1':
  case 'P':
  case 'B':
    tmp_pin = calcPT(chk_ptr->reg_latch.PTenable->pin_eq,
                     vctr_grp,last_vctr_grp,chk_act,reg_class);
    if (chk_act != PRELOAD && chk_act != CLOCK)
      bin_test(tmp_pin);
    if (tmp_pin == 0)
      return(NO);
    if (chk_act != PRELOAD && chk_act != INIT_PT) {
      sprintf(main_msg,"\nI/O Pin [%d] configured as OUTPUT",c_pin);
      pr_msgs(main_msg,YES);
    }
    return(NO);
  default:
    sprintf(main_msg,"Invalid symbol [%c] in -> [%s]",
            vctr_grp[c_pin-1],vctr_grp);
    inform(main_msg,STOP);
} /* SWITCH */
} /* CHK_IO_PIN */
/*  -   -   -   -   -   */
/* check if a_number is a binary number */
bin_test(a_number)
int a_number;
{
switch(a_number) {
  case 0:
  case 1:  return;
  default:
    sprintf(main_msg,"Non-binary # [%d] calculated",a_number);
    inform(main_msg,STOP);
} /* SWITCH */
} /* BIN_TEST */
/*  -   -   -   -   -   */
/* calculate product terms; if REG_CLASS is LATCH_CELL and
   ACTION = CLOCK, return the calculated value, else if
   REG_CELL, then check for 0-1-0 or 1-0-1 transitions
*/
calcPT(eval_eq,x_vctr,y_vctr,action,reg_class)
struct node2class *eval_eq;
char x_vctr[],y_vctr[];
int action,reg_class;
{ char mbuf[80];
  struct pin_element *Q_PTR;
  struct PT_element *runner;
  int io_p_state,act2;

if (eval_eq == NULL)
  return(0);
if (eval_eq->symbol == NULL)
  inform("Cannot evaluate",STOP);
switch(eval_eq->symbol[0]) {
  case '^':
    if (eval_eq->pin_no != 0 && eval_eq->pin_no != 1)
      inform("Use 0/1 as constants",STOP);
    if (action == CLOCK)
      return(0);
    return(eval_eq->pin_no);
  case EQNMBR:
    if (eval_eq->pin_no < 1 || eval_eq->pin_no > num_pins)
      inform("Invalid pin number in equation; check database",STOP);
    if (chk_io_pin(eval_eq->pin_no,&io_p_state,x_vctr,
                   y_vctr,action,reg_class) == YES)
      return(io_p_state);
    else {
      io_p_state = chk_a_vctr(x_vctr,eval_eq->pin_no,action);
      if ((io_p_state == 2   || io_p_state == 3) &&
          (action != PRELOAD && action != CLOCK && action != LOW))
        inform("Invalid PRELOAD procedure",STOP);
      if (action == PRELOAD)
        return(io_p_state);
      switch(io_p_state) {
        case 4: /* negative clock K 1-0-1 pulse */
        case 5: /* positive clock C 0-1-0 pulse */
          if (action == CLOCK || action == LOW)
            break;
        default:
          bin_test(io_p_state);
          break;
      } /* SWITCH */
      if (action == LOW || action == CLOCK) {
        if (reg_class == LATCH_CELL)
          return(io_p_state);
/* prev state Y_VCTR */
        act2 = chk_a_vctr(y_vctr,eval_eq->pin_no,INIT_PT);
        bin_test(act2);
        switch(action) {
          case CLOCK:
            if (act2 == 0 && io_p_state == 1)
              return(1);
            if (io_p_state == 5)
              return(1);
            if (io_p_state == 4) {
              switch(act2) {
                case 0:
                  inform("K (1-0-1) clock pulse will cause 2 clocks",CONTINUE);
                  inform("-->SIM will use only 1. Try 0/1 for clock",CONTINUE);
                  return(1);
                case 1:
                  return(1);
              } /* SWITCH */
            } /* IF */
            return(0);
          case LOW:
            if (act2 == 1 && io_p_state == 0)
              return(0);
            if (io_p_state == 4)
              return(0);
            if (io_p_state == 5) {
              switch(act2) {
                case 0:
                  return(0);
                case 1:
                  inform("C (0-1-0) clock pulse will cause 2 clocks",CONTINUE);
                  inform("-->SIM will use only 1. Try 0/1 for clock",CONTINUE);
                  return(0);
              } /* SWITCH */
            } /* IF */
            return(1);
          default: inform("!! calcPT",STOP);
        } /* SWITCH */
      } /* IF */
      return(io_p_state);
    } /* ELSE */
  case Q_OUTPUT:
    if (eval_eq->pin_no < 1 || eval_eq->pin_no > num_pins)
      inform("Invalid register feedback # in eq; check database",STOP);
    for (Q_PTR = test_pin_elements;
         Q_PTR != NULL;
         Q_PTR = Q_PTR->nxt_tst_pin) {
      if (Q_PTR->vctr_element == eval_eq->pin_no) {
        return(Q_PTR->reg_latch.mem_output);
      } /* IF */
    } /* FOR */
    sprintf(main_msg,"\nFeedback [%d] not found",eval_eq->pin_no);
    inform(main_msg,STOP);
  case '@': /* use a temp buffer to hold the PT name without the @ */
    for (io_p_state = 1; eval_eq->symbol[io_p_state] != '\0'; ++io_p_state)
      mbuf[io_p_state-1] = eval_eq->symbol[io_p_state];
    mbuf[io_p_state-1] = '\0';
    for (runner = product_terms; runner != NULL; runner = runner->next_PT) {
      if (!strcmp(runner->term_name,mbuf)) {
/* get the CURRENT state (in bit position 1 starting from 0) of the PT */
        io_p_state = (runner->pinstates & 2) >> 1;
        if ((eval_eq->LEFT_0 != NULL) || (eval_eq->RIGHT_1 != NULL)) {
          switch (io_p_state) {
            case 0: return(calcPT(eval_eq->LEFT_0 ,x_vctr,
                           y_vctr,action,reg_class));
            case 1: return(calcPT(eval_eq->RIGHT_1,x_vctr,
                           y_vctr,action,reg_class));
            default: inform("Bit operation err",STOP);
          } /* SWITCH */
        } /* IF */
        else {
          if (action == CLOCK || action == LOW) {
            if (reg_class == LATCH_CELL)
              return(io_p_state);
            act2 = runner->pinstates & 1; /* get previous state */
/* ------------- on the 0-1 or 1-0 transitions, a 1 is returned if
   a CLOCK or NOT CLOCK/LOW is the specified action ------------- */
            switch(action) {
              case CLOCK:
                if (act2 == 0 && io_p_state == 1)
                  return(1);
                else
                  return(0);
              case LOW:
                if (act2 == 1 && io_p_state == 0)
                  return(0);
                else
                  return(1);
              default: inform("!# calcPT",STOP);
            } /* SWITCH */
          } /* IF */
          return(io_p_state); /* return CURRENT value */
        } /* ELSE */
      } /* IF */
    } /* FOR */
    sprintf(main_msg,"\nCannot find PT [%s]",eval_eq->symbol);
    inform(main_msg,STOP);
    break;
  case '/':
    if (action == CLOCK)
      act2 = LOW;
    else
      act2 = action;
    io_p_state = calcPT(eval_eq->RIGHT_1,x_vctr,y_vctr,act2,reg_class);
    bin_test(io_p_state);
    switch(io_p_state) {
      case 0: return(1);
      case 1: return(0);
    } /* SWITCH */
    break;
  case '*':
    io_p_state = calcPT(eval_eq->LEFT_0,x_vctr,y_vctr,action,reg_class) &
                 calcPT(eval_eq->RIGHT_1,x_vctr,y_vctr,action,reg_class);
    bin_test(io_p_state);
    return(io_p_state);
  case '+':
    io_p_state = calcPT(eval_eq->LEFT_0,x_vctr,y_vctr,action,reg_class) |
                 calcPT(eval_eq->RIGHT_1,x_vctr,y_vctr,action,reg_class);
    bin_test(io_p_state);
    return(io_p_state);
  case '%': /* EXOR */
    io_p_state = calcPT(eval_eq->LEFT_0,x_vctr,y_vctr,action,reg_class) ^
                 calcPT(eval_eq->RIGHT_1,x_vctr,y_vctr,action,reg_class);
    bin_test(io_p_state);
    return(io_p_state);
  case '|': /* XNOR */
    io_p_state = calcPT(eval_eq->LEFT_0,x_vctr,y_vctr,action,reg_class) ^
                 calcPT(eval_eq->RIGHT_1,x_vctr,y_vctr,action,reg_class);
    bin_test(io_p_state);
    switch(io_p_state) {
      case 0: return(1);
      case 1: return(0);
    } /* SWITCH */
    break;
  default :
    for (runner = product_terms; runner != NULL; runner = runner->next_PT) {
      if (!strcmp(runner->term_name,eval_eq->symbol)) {
        return(calcPT(runner->pin_eq,x_vctr,y_vctr,action,reg_class));
      } /* IF */
    } /* FOR */
    sprintf(mbuf,"Invalid symbol [%s] in eq",eval_eq->symbol);
    inform(mbuf,STOP);
    break;
} /* SWITCH */
} /* CALCPT */
/*  -   -   -   -   -   */
/* evaluate all the PTs in the PRODUCT_TERM list and store the
   result in the corresponding X_STATE: either Previous (0),
   Current (1), or Future (2) state. Note that REG_CELL is
   actually used as a dummy argument for CALCPT; it is only
   in DO_PIN_EVAL() that this parameter varies between REG_CELL
   and LATCH_CELL
*/
do_PTeval(x_vectors,x_state,y_vectors,do_action)
char x_vectors[],y_vectors[];
int x_state,do_action;
{ unsigned int PTstatus;
  int a_val;
  struct PT_element *runPTR;

for (runPTR = product_terms; runPTR != NULL; runPTR = runPTR->next_PT) {
  if (strcmp(runPTR->term_name,LIST_HEAD)) {
    PTstatus = runPTR->pinstates;
    PTstatus = (PTstatus >> 1) & 7;  /* preserve lower 3 bits */
    a_val = calcPT(runPTR->pin_eq,x_vectors,y_vectors,do_action,REG_CELL);
    update(&(runPTR->pinstates),a_val,x_state);
    if (debug_flag == YES && runPTR->pin_eq != NULL) {
      sprintf(main_msg,"\n:- PT [%s] state [%u]",
              runPTR->term_name,runPTR->pinstates);
      pr_msgs(main_msg,NO);
    } /* IF */
  } /* IF not the LIST_HEAD */
} /* FOR */
} /* DO_PTEVAL */
/*  -   -   -   -   -   */
/* get vectors from the file FROM_FILE and assign them to the character
   array VTARGET */
vctr_srch(from_file,vtarget)
FILE **from_file;
char vtarget[];
{ int i;
if (*from_file == NULL)
  inform("Vctr file NULL",STOP);
vtarget[0] = '\0';
read_map(from_file,vtarget,1,num_pins);
if (vtarget[0] == '\0')
  return(NO);
else {
  if (strlen(vtarget) < num_pins) {
    for (i = strlen(vtarget); i < num_pins; ++i)
      vtarget[i] = 'X';
    vtarget[i] = '\0';
  }
  return(YES);
} /* ELSE */
} /* VCTR_SRCH */
/*  -   -   -   -   -   */
/* update the DEST with the SRC in the bit position specified by STATUS */
update(dest,src,status)
unsigned int *dest,src,status;
{ unsigned int tmpr0,tmpr1;
bin_test(src);
tmpr0 = (src << status) & 7;
tmpr1 = (*dest >> 1) & 7;
*dest = tmpr0 | tmpr1;
} /* UPDATE */
/*  -   -   -   -   -   */
/* FF-CTRL = flip-flop control function
   CELL_TYPE determines how IN0 and IN1 are used: if a D or T flip-flop,
   then only IN0 is used; if J-K or S-R, then IN0 and IN1 are used.
*/
FF_ctrl(in0,in1,cell_type,pin_nmbr)
int in0,in1;
struct pin_element *cell_type;
int pin_nmbr;
{ unsigned int reg_class;

bin_test(in0);
bin_test(in1);
reg_class = cell_type->reg_latch.store_type & 0x0F;
switch(reg_class) {
  case D_FF:
    switch(in0) {
      case 0: cell_type->reg_latch.mem_output = 0;
              break;
      case 1: cell_type->reg_latch.mem_output = 1;
              break;
    }
    break;
  case JK_FF:
    if (in0 == 0 && in1 == 1)
      cell_type->reg_latch.mem_output = 0;
    if (in0 == 1 && in1 == 0)
      cell_type->reg_latch.mem_output = 1;
    if (in0 == 1 && in1 == 1) {
      if (cell_type->reg_latch.mem_output == 1)
        cell_type->reg_latch.mem_output = 0;
      else
        cell_type->reg_latch.mem_output = 1;
    }
    break;
  case SR_FF:
    if (in0 == 0 && in1 == 1)
      cell_type->reg_latch.mem_output = 0;
    if (in0 == 1 && in1 == 0)
      cell_type->reg_latch.mem_output = 1;
    if (in0 == 1 && in1 == 1) {
      sprintf(main_msg,"\nWarning: Both inputs to S-R FF [%d] on",pin_nmbr);
      pr_msgs(main_msg,YES);
    }
    break;
  case T_FF:
    if (in0 == 1) {
      if (cell_type->reg_latch.mem_output == 1)
        cell_type->reg_latch.mem_output = 0;
      else
        cell_type->reg_latch.mem_output = 1;
    }
    break;
  default:
    sprintf(main_msg,"Invalid FF type: [%u] in FF_CTRL",reg_class);
    inform(main_msg,STOP);
} /* SWITCH */
} /* FF_CTRL */
/*  -   -   -   -   -   */
/* Check if the macrocell is a dedicated input: all the control PTs are
   NULL
*/
JAN_test(macrocell)
struct pin_element *macrocell;
{
if ( (macrocell->reg_latch.PTreset      == NULL) &&
     (macrocell->reg_latch.PTset        == NULL) &&
     (macrocell->reg_latch.PTpreload    == NULL) &&
     (macrocell->reg_latch.PTobserve    == NULL) &&
     (macrocell->reg_latch.PTclock      == NULL) &&
     (macrocell->reg_latch.PTenable     == NULL) &&
     (macrocell->reg_latch.PTmem0_input == NULL) &&
     (macrocell->reg_latch.PTmem1_input == NULL))
  return(NO);
return(YES);
} /* JAN_TEST */
/*  -   -   -   -   -   */
/* Apply the D_VCTR to all the pins on the device; any calculated values
   get sent to the corresponding location in D_CALC_V. The updated state
   of the pin is put in the D_STATE state.
*/
do_PIN_eval(d_vctr,d_prev_vctr,d_calc_v,d_state)
char d_vctr[],d_prev_vctr[],d_calc_v[];
int d_state;
{ int rst_act,set_act,pload_act,p1_extra,clk_pulse,obs_act,
      oe_act,FF0_in,FF1_in,out_st;
  struct pin_element *JAN; /* just a ptr name */
  struct node2class *handle0,*handle1;

for (JAN = test_pin_elements; JAN != NULL; JAN = JAN->nxt_tst_pin) {
  if ((JAN->vctr_element != -1) && (JAN_test(JAN) == YES)) {
    rst_act = 0;
    set_act = 0;
    pload_act = 4000;
    p1_extra = 4001;
    obs_act = 0;
    clk_pulse = 0;
    oe_act = 0;
    FF0_in = 0;
    FF1_in = 0;
    out_st = 0;  /* output state */

    if (JAN->reg_latch.PTpreload != NULL) {
      if (JAN->reg_latch.PTpreload->pin_eq != NULL) {
        handle0 = JAN->reg_latch.PTpreload->pin_eq->LEFT_0;
        JAN->reg_latch.PTpreload->pin_eq->LEFT_0 = NULL;
        handle1 = JAN->reg_latch.PTpreload->pin_eq->RIGHT_1;
        JAN->reg_latch.PTpreload->pin_eq->RIGHT_1 = NULL;
      }
      pload_act = calcPT(JAN->reg_latch.PTpreload->pin_eq,
                         d_vctr,d_prev_vctr,PRELOAD,REG_CELL);
      if (pload_act != 2 && pload_act != 3)
        update(&(JAN->reg_latch.PTpreload->pinstates),pload_act,d_state);
      else /* if zener PLOAD_ACT active, then set the status to 0 */
        update(&(JAN->reg_latch.PTpreload->pinstates),0,d_state);
      if (JAN->reg_latch.PTpreload->pin_eq != NULL) {
        JAN->reg_latch.PTpreload->pin_eq->LEFT_0 = handle0;
        JAN->reg_latch.PTpreload->pin_eq->RIGHT_1 = handle1;
      }
    } /* IF PRELOAD */
    switch(pload_act) {
      case 1: /* TTL-level preload */
        if (JAN->reg_latch.PTpreload->pin_eq != NULL) {
          JAN->reg_latch.mem_output =
                 calcPT(JAN->reg_latch.PTpreload->pin_eq->RIGHT_1,
                        d_vctr,d_prev_vctr,0,REG_CELL);
          pr_msgs("\nPT preload",YES);
        } /* IF */
        break;
      case 0: /* check for ZENER-level preload */
        if (JAN->reg_latch.PTpreload->pin_eq->LEFT_0 != NULL) {
          handle0 = JAN->reg_latch.PTpreload->pin_eq->LEFT_0->LEFT_0;
          JAN->reg_latch.PTpreload->pin_eq->LEFT_0->LEFT_0 = NULL;
          handle1 = JAN->reg_latch.PTpreload->pin_eq->LEFT_0->RIGHT_1;
          JAN->reg_latch.PTpreload->pin_eq->LEFT_0->RIGHT_1 = NULL;
        }
        p1_extra = calcPT(JAN->reg_latch.PTpreload->pin_eq->LEFT_0,
                           d_vctr,d_prev_vctr,PRELOAD,REG_CELL);
        if (p1_extra != 2 && p1_extra != 3)
          update(&(JAN->reg_latch.PTpreload->pinstates),p1_extra,d_state);
        else /* if zener PLOAD_ACT active, then set the status to 0 */
          update(&(JAN->reg_latch.PTpreload->pinstates),0,d_state);
        if (JAN->reg_latch.PTpreload->pin_eq->LEFT_0 != NULL) {
          JAN->reg_latch.PTpreload->pin_eq->LEFT_0->LEFT_0 = handle0;
          JAN->reg_latch.PTpreload->pin_eq->LEFT_0->RIGHT_1 = handle1;
        }
        switch(p1_extra) {
          case 2: /* zener-level register preload */
            if (JAN->reg_latch.PTpreload->pin_eq->LEFT_0 != NULL) {
              if (JAN->reg_latch.PTpreload->pin_eq->LEFT_0->RIGHT_1 == NULL)
                break;
              JAN->reg_latch.mem_output =
                     calcPT(JAN->reg_latch.PTpreload->pin_eq->LEFT_0->RIGHT_1,
                            d_vctr,d_prev_vctr,PRELOAD,REG_CELL);
            }
            break;
          case 3: /* zener-level internal register preload */
            if (JAN->reg_latch.PTpreload->pin_eq->LEFT_0 != NULL) {
              if (JAN->reg_latch.PTpreload->pin_eq->LEFT_0->LEFT_0 == NULL)
                break;
              JAN->reg_latch.mem_output =
                     calcPT(JAN->reg_latch.PTpreload->pin_eq->LEFT_0->LEFT_0,
                            d_vctr,d_prev_vctr,PRELOAD,REG_CELL);
            }
            break;
          default: bin_test(p1_extra);
        } /* SWITCH */
        break;
      case 4000: break;
      default: bin_test(pload_act);
    } /* SWITCH for PLOAD */
/* preload operation performed */
    if (pload_act == 1 || p1_extra == 2 || p1_extra == 3) {
      bin_test(JAN->reg_latch.mem_output);
/*      d_calc_v[(JAN->vctr_element)-1] = JAN->reg_latch.mem_output + '0'; */
    }
    else { /* calculate the next output */
      if (JAN->reg_latch.PTreset != NULL) {
        rst_act = calcPT(JAN->reg_latch.PTreset->pin_eq,d_vctr,
                         d_prev_vctr,0,REG_CELL);
        update(&(JAN->reg_latch.PTreset->pinstates),rst_act,d_state);
      }
      if (JAN->reg_latch.PTset != NULL) {
        set_act = calcPT(JAN->reg_latch.PTset->pin_eq,d_vctr,
                         d_prev_vctr,0,REG_CELL);
        update(&(JAN->reg_latch.PTset->pinstates),set_act,d_state);
      }
      if (JAN->reg_latch.PTobserve != NULL) {
        if (JAN->reg_latch.PTobserve->pin_eq != NULL) {
          handle0 = JAN->reg_latch.PTobserve->pin_eq->LEFT_0;
          JAN->reg_latch.PTobserve->pin_eq->LEFT_0 = NULL;
          handle1 = JAN->reg_latch.PTobserve->pin_eq->RIGHT_1;
          JAN->reg_latch.PTobserve->pin_eq->RIGHT_1 = NULL;
        }
        obs_act = calcPT(JAN->reg_latch.PTobserve->pin_eq,d_vctr,
                         d_prev_vctr,0,REG_CELL);
        update(&(JAN->reg_latch.PTobserve->pinstates),obs_act,d_state);
        if (JAN->reg_latch.PTobserve->pin_eq != NULL) {
          JAN->reg_latch.PTobserve->pin_eq->LEFT_0 = handle0;
          JAN->reg_latch.PTobserve->pin_eq->RIGHT_1 = handle1;
        }
      }
  /* for the clocks, the cell type (REG_CELL/0 or LATCH_CELL/1)
     determines if the cell must be clocked or latched. Note
     that the cell type is in the upper nibble of the field STORE_TYPE
  */
      if (JAN->reg_latch.PTclock != NULL) {
        clk_pulse = calcPT(JAN->reg_latch.PTclock->pin_eq,
                           d_vctr,d_prev_vctr,CLOCK,
                           ((JAN->reg_latch.store_type >> 4) & 0x0F));
        update(&(JAN->reg_latch.PTclock->pinstates),clk_pulse,d_state);
      }
      if (JAN->reg_latch.PTenable != NULL) {
        oe_act = calcPT(JAN->reg_latch.PTenable->pin_eq,d_vctr,
                        d_prev_vctr,0,REG_CELL);
        update(&(JAN->reg_latch.PTenable->pinstates),oe_act,d_state);
      }
      if (JAN->reg_latch.PTmem0_input != NULL) {
        FF0_in = calcPT(JAN->reg_latch.PTmem0_input->pin_eq,
                        d_vctr,d_prev_vctr,0,REG_CELL);
        update(&(JAN->reg_latch.PTmem0_input->pinstates),FF0_in,d_state);
      }
      if (JAN->reg_latch.PTmem1_input != NULL) {
        FF1_in = calcPT(JAN->reg_latch.PTmem1_input->pin_eq,d_vctr,
                         d_prev_vctr,0,REG_CELL);
        update(&(JAN->reg_latch.PTmem1_input->pinstates),FF1_in,d_state);
      }

      if (rst_act == 1) {
        if (set_act == 1 || pload_act == 1) {
          sprintf(main_msg,"\nWarning: SET or PRELOAD active with RESET");
          pr_msgs(main_msg,YES);
        }
        JAN->reg_latch.mem_output = 0;
      } /* IF */
      if (set_act == 1) {
        if (pload_act == 1) {
          sprintf(main_msg,"Warning: PRELOAD active with SET");
          pr_msgs(main_msg,YES);
        }
        JAN->reg_latch.mem_output = 1;
      } /* IF */
      if (clk_pulse == 1) {
        FF_ctrl(FF0_in,FF1_in,JAN,JAN->vctr_element);
      } /* IF */
      if (JAN->out_element != NULL) {
        out_st = calcPT(JAN->out_element,d_vctr,d_prev_vctr,0,REG_CELL);
        update(&(JAN->pin_status),out_st,d_state);
      }
      if (obs_act == 1) { /* override the computed value */
        if (JAN->reg_latch.PTobserve->pin_eq != NULL) {
          out_st = calcPT(JAN->reg_latch.PTobserve->pin_eq->RIGHT_1,
                          d_vctr,d_prev_vctr,0,REG_CELL);
          update(&(JAN->pin_status),out_st,CURRENT);
        }
      } /* IF */
      if (oe_act == 1) {
        switch (d_vctr[(JAN->vctr_element)-1]) {
          case 'X':
          case 'N':
          case 'L':
          case 'H':
            switch(out_st) {
               case 0:
                 d_calc_v[(JAN->vctr_element)-1] = 'L';
                 break;
               case 1:
                 d_calc_v[(JAN->vctr_element)-1] = 'H';
                 break;
               default:
                 inform("Must always be L/H",STOP);
            } /* SWITCH */
            break;
          default:
            sprintf(main_msg,"\nWarning: Output pin [%d] is driving",
                    JAN->vctr_element);
            pr_msgs(main_msg,YES);
            if (out_st == 0)
              d_calc_v[(JAN->vctr_element)-1] = 'L';
            else
              d_calc_v[(JAN->vctr_element)-1] = 'H';
        } /* SWITCH */
      } /* IF */
      else { /* the OE = 0 */
        switch(d_vctr[(JAN->vctr_element)-1]) {
          case 'X':
            if (accurate == NO)
              d_calc_v[(JAN->vctr_element)-1] = 'Z';
            else
              d_calc_v[(JAN->vctr_element)-1] = dflt_test + '0';
            break;
          case 'F':
          case 'N':
          case 'Z':
          case 'C':
          case 'K':
          case 'P':
          case 'B':
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            break;
          default:
            sprintf(main_msg,"\nWarning: High-Z on pin [%d]",
                             JAN->vctr_element);
            pr_msgs(main_msg,YES);
            d_calc_v[(JAN->vctr_element)-1] = 'Z';
            break;
        } /* SWITCH */
      } /* ELSE */
    } /* ELSE for PRELOAD */
  } /* IF */
} /* FOR */
} /* DO_PIN_EVAL */
/*  -   -   -   -   -   */
/* split the clocks in the test vector: this means separating the
   0-1-0 clocks (C) from the 1-0-1 clocks (K). Save this in the temporary
   vector so that when the K clocks come to be processed (when the ACTION is
   a NO), put these K symbols back while removing the C symbols.
*/
split_clocks(C_vctr,K_vctr,action)
char C_vctr[],K_vctr[];
int action;
{ int i,b;

for (i = 0,b = NO; C_vctr[i] != '\0'; ++i) {
  if (action == YES) {
    if (C_vctr[i] == 'K') {
      K_vctr[i] = 'K';
      C_vctr[i] = '0';
      b = YES;
    }
    else
      K_vctr[i] = ' ';
  } /* IF */
  else {
    if (K_vctr[i] == 'K') {
      C_vctr[i] = 'K';
    }
  } /* ELSE */
} /* FOR */
K_vctr[i] = '\0';
return(b);
} /* SPLIT_CLOCKS */
/*  -   -   -   -   -   */
/* remove all the clocks and replace them with 0s (for C) or
   1s (for Ks).
*/
remove_clocks(CLK_VCTR)
char CLK_VCTR[];
{ int i;
for (i = 0; CLK_VCTR[i] != '\0'; ++i) {
  switch(CLK_VCTR[i]) {
   case 'C':
     CLK_VCTR[i] = '0';
     break;
   case 'K':
     CLK_VCTR[i] = '1';
     break;
  } /* SWITCH */
} /* FOR */
} /* REMOVE_CLOCKS */
/*  -   -   -   -   -   */
/* apply the vector stimuli to the logic model. If there are any clocks
   in the VECTOR, evaluate them once (when DO_CLOCK is -1) and then
   remove them from the STIMULATION vector. The SPLIT_CLOCKS() function
    will always return a value other than -1
*/
apply_vectors(stimulation,which_state,prev_stimuli,calc_vctr,vctr_count)
char stimulation[];
int which_state;
char prev_stimuli[],calc_vctr[];
{ int loop_count,do_clocks;
  char answer[4],*tmp0_vctr,*tmp1_vctr,*prev_wave;

prev_wave = malloc((unsigned)(num_pins+1));
read_mem_check(prev_wave,"PREV_WAVE");
tmp0_vctr = malloc((unsigned)(num_pins+1));
read_mem_check(tmp0_vctr,"TMP0_VCTR");
tmp1_vctr = malloc((unsigned)(num_pins+1));
read_mem_check(tmp1_vctr,"TMP1_VCTR");
prev_wave[0] = '\0';
tmp0_vctr[0] = '\0';
tmp1_vctr[0] = '\0';
for (loop_count = 15, do_clocks = -1;;--loop_count) {
  if (loop_count < 0) {
    inform("Values haven't stabilized; continue resolving ? (Y/N):",NOTICE);
    get_string(answer,3);
    if (isalpha(answer[0])) {
      if (islower(answer[0]))
        answer[0] = toupper(answer[0]);
    }
    if (answer[0] == 'N') {
      free(tmp0_vctr);
      free(tmp1_vctr);
      free(prev_wave);
      strcpy(prev_stimuli,stimulation);
      return;
    }
    loop_count = 15;  /* go another 15 more */
  }
/* ---> 5 was arbitrarily chosen */
  if ((loop_count < 5) && (debug_flag == NO)) {
    sprintf(main_msg,"Iteration [%2d]: Resolving",loop_count);
    inform(main_msg,NOTICE);
  } /* IF */
/* DO_CLOCKS IF statement is executed once, SPLIT_CLOCKS will
   return YES/NO and never a -1
*/
  if (do_clocks == -1) {
    strcpy(prev_wave,prev_stimuli);
    strcpy(tmp1_vctr,stimulation);
    do_clocks = split_clocks(stimulation,tmp0_vctr,YES);
    do_PTeval(stimulation,which_state,prev_stimuli,PRELOAD);
    do_PIN_eval(stimulation,prev_stimuli,calc_vctr,CURRENT);
    strcpy(prev_stimuli,calc_vctr);
    if (debug_flag == 200 || debug_flag == YES) {
      pr_msgs("\n  Intermediate C: ",NO);
      wr_space(calc_vctr,4,0);
    }
    if (do_clocks == YES) {
      do_clocks = split_clocks(stimulation,tmp0_vctr,NO);
      do_PTeval(stimulation,which_state,prev_stimuli,0);
      do_PIN_eval(stimulation,prev_stimuli,calc_vctr,CURRENT);
      strcpy(prev_stimuli,calc_vctr);
      if (debug_flag == 200 || debug_flag == YES) {
        pr_msgs("\n  Intermediate K: ",NO);
        wr_space(calc_vctr,4,0);
      }
    }
    remove_clocks(stimulation);
    if (waveptr != NULL && debug_flag != NO) {
      strcpy(tmp0_vctr,calc_vctr);
      draw_waves(waveptr,prev_wave,calc_vctr,tmp1_vctr,vctr_count);
      strcpy(calc_vctr,tmp0_vctr);
      strcpy(prev_wave,tmp0_vctr);
      strcpy(tmp1_vctr,tmp0_vctr);
    }
  } /* IF */
  else {
    do_PTeval(stimulation,which_state,prev_stimuli,0);
    do_PIN_eval(stimulation,prev_stimuli,calc_vctr,CURRENT);
    strcpy(prev_stimuli,calc_vctr);
    if (debug_flag == 200 || debug_flag == YES) {
      sprintf(main_msg,"\n  Loop [%3d]-> A: ",loop_count);
      pr_msgs(main_msg,NO);
      wr_space(calc_vctr,4,0);
    }
    if (waveptr != NULL && debug_flag != NO) {
      strcpy(prev_wave,calc_vctr);
      draw_waves(waveptr,stimulation,calc_vctr,stimulation,vctr_count);
    }
  } /* ELSE */
  remove_clocks(calc_vctr);
  strcpy(tmp0_vctr,calc_vctr);
  strcpy(stimulation,calc_vctr);
  do_PTeval(stimulation,CURRENT,prev_stimuli,PRELOAD);
  do_PIN_eval(stimulation,prev_stimuli,calc_vctr,CURRENT);
  strcpy(prev_stimuli,calc_vctr);
  strcpy(stimulation,calc_vctr);
  if (waveptr != NULL) {
    draw_waves(waveptr,prev_wave,calc_vctr,tmp1_vctr,vctr_count);
    strcpy(prev_wave,stimulation);
    strcpy(calc_vctr,stimulation);
  }
  if (vctr_compare(tmp0_vctr,calc_vctr,tmp1_vctr,0) == NO) {
    free(tmp0_vctr);
    free(tmp1_vctr);
    free(prev_wave);
    strcpy(prev_stimuli,stimulation);
    return;
  }
  strcpy(tmp1_vctr,stimulation);
  if (debug_flag == 200 || debug_flag == YES) {
    sprintf(main_msg,"\n  Loop [%3d]-> B: ",loop_count);
    pr_msgs(main_msg,NO);
    wr_space(calc_vctr,4,0);
  }
} /* FOR */
} /* APPLY_VECTORS */
/*  -   -   -   -   -   */
/* DO_SPACE() will space the STR_ARRAY in the file with a spacing factor
   specified by SPACING. If there is a non-zero PAD value, then PAD
   the file with spaces and then return.
*/
do_space(file_ptr,str_array,spacing,pad)
FILE *file_ptr;
char str_array[];
int spacing,pad;
{ int i;

if (pad > 0) {
  fprintf(file_ptr,"\n");
  for (i = 0; i < pad; ++i)
    fprintf(file_ptr," ");
  return;
} /* IF PAD */
if (pad <= 0)
  fprintf(file_ptr,"[");
for (i = 0; str_array[i] !=  '\0'; ++i) {
  if (i != 0 && (i%spacing == 0))
    fprintf(file_ptr," ");
  fprintf(file_ptr,"%c",str_array[i]);
} /* FOR */
if (pad <= 0)
  fprintf(file_ptr,"]");
if (pad == -1) {
  for (i = 0; str_array[i] != '\0'; ++i) {
    if (str_array[i] == '?')
      fprintf(file_ptr,"\n  *!* Pin <%d>",i+1);
  } /* FOR */
} /* IF */
} /* D0_SPACE */
/*  -   -   -   -   -   */
pr_msgs(p_msg,override)
char p_msg[];
int override;
{
if (out_file != NULL)
  fprintf(out_file,"%s",p_msg);
if (override == NO || suppress_msg == NO)
  fprintf(stderr,"%s",p_msg);
} /* PR_MSGS */
/*  -   -   -   -   -   */
/* write the vector according to the spacing specified */
wr_space(str_array,spacing,pad)
char str_array[];
int spacing,pad;
{
if (out_file != NULL)
  do_space(out_file,str_array,spacing,pad);
do_space(stderr,str_array,spacing,pad);
} /* SPACE */
/*  -   -   -   -   -   */
/* Compare the user-specified vector VCTR_A with the
   SIMULATOR calculated vector VCTR_B. Any inconsistencies
   between the 2 vectors will be marked in the corresponding
   position in VCTR_MARK. If ERR_COUNT = 0, then do not count
   the vector differences since the simulator will still try
   to resolve these.
*/
vctr_compare(vctr_a,vctr_b,vctr_mark,err_count)
char vctr_a[],vctr_b[],vctr_mark[];
int err_count;
{ int i,mark;

for (i = 0,mark = NO; vctr_a[i] != '\0'; ++i) {
  vctr_mark[i] = ' ';
  if (vctr_a[i] != vctr_b[i]) {
    switch(vctr_a[i]) {
      case '0':
        if (vctr_b[i] == 'C')
          break;
      case '1':
        if (vctr_b[i] == 'K')
          break;
      case 'Z':
      case 'H':
      case 'L':
        vctr_mark[i] = '?';
        if (err_count == 1)
          ++errors;
        mark = YES;
        break;
    } /* SWITCH */
  } /* IF */
} /* FOR */
vctr_mark[i] = '\0';
return(mark);
} /* VCTR_COMPARE */
/*  -   -   -   -   -   */
/* Read the test vectors from the JEDEC map file or a separate file
   and apply them to the logic model. The vectors must be in ascending
   order (one possible improvement). The P field in the JEDEC 3-A
   standard, which lets the user define the vector pin order, is not
   supported, therefore the vectors muyst be in ascending order.

   If ACT == YES, then apply the vectors. If NO, then just print the
   vectors out to the FPTR file (called from VCTR_WRITE module).
*/
rd_apply_vctrs(fptr,act)
FILE *fptr;
int act;
{ FILE *vptr;
  int more_vectors,sim_count,i;
  char *stimuli,  /* character pointer pointing to the vector array */
       *last_stimuli,
       *calculated,
       *tmpvctr,   /* holds original STIMULI value */
       *mark_buf;

if (act == YES) {
  last_stimuli = malloc((unsigned)(num_pins+1));
  read_mem_check(last_stimuli,"Last STIMULI");
  calculated = malloc((unsigned)(num_pins+1));
  read_mem_check(calculated,"CALCULATED");
  mark_buf = malloc((unsigned)(num_pins+1));
  read_mem_check(mark_buf,"MARK_BUFFER");
  tmpvctr = malloc((unsigned)(num_pins+1));
  read_mem_check(tmpvctr,"TMPVCTR");
  last_stimuli[0] = '\0';
  calculated[0] = '\0';
  mark_buf[0] = '\0';
  tmpvctr[0] = '\0';
} /* IF */
vptr = exist(vctr_file,"r");
stimuli = malloc((unsigned)(num_pins+1));
read_mem_check(stimuli,"STIMULI");
stimuli[0] = '\0';

fprintf(stderr,"\nReading test vectors in [%s]",vctr_file);
for (more_vectors = YES,sim_count = 1,errors = 0;
     more_vectors != NO; ++sim_count) {
  more_vectors = vctr_srch(&vptr,stimuli);
  if (act == NO && more_vectors == YES) {
    if (fptr == NULL)
      inform("NULL VCTR file",STOP);
    for (i = 0; stimuli[i] != '\0'; ++i)
      fprintf(fptr,"%c ",stimuli[i]);
    fprintf(fptr,"; \"%3d\"\n",sim_count);
  } /* IF act == NO */
  if (more_vectors == YES && act == YES) {
    if (sim_count == 1) {
      if (waveptr != NULL) {
        fprintf(waveptr,"\t<< Waveform Diagram >>");
        fprintf(waveptr,"\n- simulation model  (JEDEC Map) [%s]",jmapfile);
        fprintf(waveptr,"\n- inputs/stimuli (test vectors) [%s]",vctr_file);
        fprintf(waveptr,"\n- device [%s]\n",db_sim);
        fprintf(waveptr,
           "Symbols: Output         [%c] Input       [%c] Not tested [N]\n",
            DOUBLE_VERT,VERT_BAR);
        fprintf(waveptr,
           "         Input Floating [F] Output Hi-Z [Z] Don't Care [X]\n");
        fprintf(waveptr,
           "         Positive Clock [-%c]     Negative Clock [%c-]\n",
           C_CLOCK,K_CLOCK);
        fprintf(waveptr,"V #%c",VECTOR_MARK);
        for (i = 0; i < num_pins; ++i) {
          fprintf(waveptr,"%2d",(i+1)/10);
          if ((i+1)%4 == 0)
            fprintf(waveptr,"%c",VECTOR_MARK);
        } /* FOR */
        fprintf(waveptr,"\n");
        for (i = 0; i < num_pins; ++i) {
          if (i == 0)
            fprintf(waveptr,"  #%c",VECTOR_MARK);
          fprintf(waveptr,"%2d",(i+1)%10);
          if ((i+1)%4 == 0)
            fprintf(waveptr,"%c",VECTOR_MARK);
        } /* FOR */
      } /* IF WAVEPTR != NULL */
      sprintf(main_msg,
      "\nSimulating [%s] (device file [%s])\n\twith vectors in file [%s]\n",
         jmapfile,db_sim,vctr_file);
      pr_msgs(main_msg,NO);

/* ---> get tens value; good for only 99 pins */
      pr_msgs("\n==> Device Pin #: ",NO);
      for (i = 0,mark_buf[0] = '\0',tmpvctr[0] = '\0'; i < num_pins; ++i) {
        sprintf(mark_buf,"%d",(i+1)/10);
        tmpvctr[i] = mark_buf[0];
      } /* FOR */
      tmpvctr[i] = '\0';
      wr_space(tmpvctr,4,0);
/* ---> get ones position value */
      pr_msgs("\n- - - - - - - - - ",NO);
      for (i = 0,mark_buf[0] = '\0',tmpvctr[0] = '\0'; i < num_pins; ++i) {
        sprintf(mark_buf,"%d",(i+1)%10);
        tmpvctr[i] = mark_buf[0];
      } /* FOR */
      tmpvctr[i] = '\0';
      wr_space(tmpvctr,4,0);

      do_PTeval(stimuli,CURRENT,last_stimuli,INIT_PT);
               /* initialize PTs & put result in CURRENT state */
      do_PTeval(stimuli,CURRENT,last_stimuli,0);
               /* eval PTs & put result in CURRENT state */
    } /* IF first_vector */
    sprintf(main_msg,"\n- Applying [%3d]: ",sim_count);
    pr_msgs(main_msg,NO);
    wr_space(stimuli,4,0);
    pr_msgs(" -",NO);
    strcpy(tmpvctr,stimuli);
    strcpy(calculated,stimuli);
    apply_vectors(stimuli,CURRENT,last_stimuli,calculated,sim_count);
    pr_msgs("\n  Calculated   => ",NO);
    wr_space(calculated,4,0);
    if (vctr_compare(tmpvctr,calculated,mark_buf,1) == YES) {
      wr_space("",0,18);
      wr_space(mark_buf,4,-1);
    }
/* --> send program-calculated vectors to CALCVCTR file ptr */
    if (calcvctr != NULL)
      prg_calculated_vectors(calcvctr,tmpvctr,calculated,sim_count);
    strip_PRELOAD(last_stimuli);
  } /* IF more_vectors */
} /* FOR */
if (act == YES) {
  sprintf(main_msg,"\nSimulation Completed: Errors [%3d]",errors);
  pr_msgs(main_msg,NO);
  free(last_stimuli);
  free(calculated);
  free(mark_buf);
  free(tmpvctr);
  pr_msgs("\n",NO);
} /* IF act == YES */
free(stimuli);
fclose(vptr);
vptr = NULL;
if (calcvctr != NULL) {
  fprintf(calcvctr,"%c",ETX);
  fclose(calcvctr);
  calcvctr = NULL;
  file_checksum(calcfile);
} /* IF */
} /* RD_VCTRS */
/*  -   -   -   -   -   */
