/* September 2, 1986
     : VCTRFNC2.C contains auxiliary functions for
       making vectors
*/
#include "testdef.h"
/*  -   -   -   -   -   -   */
/* assign the VALUE into BUF1 if SIGNAL_TYPE is 3, else into BUF2 using the
   index INDX1 or INDX2 respectively */
assign_tv(signal_type,value,buf1,buf2,indx1)
char signal_type,value,buf1[],buf2[];
int indx1;
{ int i;
if (signal_type == '3')
 { i = indx1%physical_pins;
   if ((i < 0) || (i >= (num_pins-physical_pins)))
     inform("invalid index in ASSIGN_TV",STOP);
   buf1[i] = value;
 }
else
  buf2[indx1] = value;
} /* ASSIGN_TV */
/*  -   -   -   -   -   -   */
/* set the power and gnd pins in BUF to 'N' */
fill_power(buf)
char buf[];
{ int i;
  struct type3 *pin_key;

for (i = 0; (buf[i] != '\0') && (i < physical_pins); ++i)
 { pin_key = (struct type3 *)find_pin(i+1);
   switch(pin_key->pin_type)
    { case VCC:
      case GND
         : switch(buf[i])
            { case 'n':
              case 'N':
                  break;
              case 'x':
              case 'X':
                  buf[i] = 'N';
                  break;
              default:
                  err(FATAL,"use N for power and GND pins",0,0,0);
            } /* SWITCH */
           break;
      default: break;
    } /* SWITCH */
 } /* FOR */
if (!(buf[i] == '\0' && i == physical_pins))
  inform("Using incorrect vector buffer",STOP);
} /* FILL_POWER */
/*  -   -   -   -   -   -   */
vctr_print(a_vctr,an_int_reg,a_vct_num)
char a_vctr[],an_int_reg[];
int a_vct_num;
{
fprintf(stderr,"\nV%04d %s*",a_vct_num,a_vctr);
if (an_int_reg[0] != '\0')
  fprintf(stderr,"   N $ %s*",an_int_reg);
if (out_file != NULL && errors == 0)
 { fprintf(out_file,"\nV%04d %s*",a_vct_num,a_vctr);
   if (an_int_reg[0] != '\0')
     fprintf(out_file,"   N $ %s*",an_int_reg);
 }
} /* VCTR_PRINT */
/*  -   -   -   -   -   -   */
/* put the B symbol in the firest location of VCTR_BUF, and then put all
   succeeding characters into the vctr_buf and int_buf */
load_internal(vctr_buf,int_buf,intgr)
char vctr_buf[],int_buf[];
int intgr;
{ int i,b,c,curnt_vctr_count,max_int;
  char buf[80];

if (int_buf[0] == '\0')
  err(FATAL,"No internal registers",0,0,0);
vctr_init(vctr_buf,'X',physical_pins);
vctr_init(int_buf,'X',num_pins-physical_pins);
max_int = num_pins-physical_pins;
curnt_vctr_count = intgr;
vctr_buf[0] = 'B';
vctr_buf[1] = '0';
for (i = 0,lex(icon),c = 0,b = 2; i < max_int; ++i,++c,++screen_position,++b)
 { if (icon[c] == '\0')
    { lex(icon);
      if (icon[0] == ';' || the_keyword == END)
       { if (b > 2)
          { vctr_print(vctr_buf,int_buf,curnt_vctr_count);
            return(curnt_vctr_count);
          }
         return(curnt_vctr_count);
       }
      c = 0;
    }
   if (b == physical_pins)
    { vctr_print(vctr_buf,int_buf,curnt_vctr_count);
      vctr_init(vctr_buf,'X',physical_pins);
      vctr_init(int_buf,'X',num_pins-physical_pins);
      ++curnt_vctr_count;
      vctr_buf[0] = 'B';
      vctr_buf[1] = '1';
      b = 2;
    }
   switch (icon[c])
    { case 'x' :
      case 'n' : icon[c] = toupper(icon[c]);
      case 'X' :
      case 'N' :
      case '0' :
      case '1' : break;
      default : err(FATAL,"preload only with 0/1,X,N",0,0,0);
                break;
    } /* SWITCH */
   vctr_buf[b] = icon[c];
   int_buf[i] = icon[c];
 } /* FOR */
vctr_print(vctr_buf,int_buf,curnt_vctr_count);
if (icon[c] != '\0')
 { sprintf(buf,"only [%d] internal registers",max_int);
   err(WARNING,buf,token_type,PUNCT_MARK,0);
 }
lex(icon);
if (icon[0] != ';')
  err(FATAL,"';' expected",0,0,0);
return(curnt_vctr_count);
} /* LOAD_INTERNAL */
/*  -   -   -   -   -   -   */
upr_case_convert(chrbuf)
/* convert the string in TEMP to upper case */
char chrbuf[];
{ int i;

for (i = 0; chrbuf[i] != '\0' && i < MAX_NAME; ++i)
 { if (isalpha(chrbuf[i]))
    { if (islower(chrbuf[i]))
        chrbuf[i] = toupper(chrbuf[i]);
    }
 } /* FOR */
} /* UPR_CASE_CONVERT */
/*  -   -   -   -   -   -   */
file_checksum(tptr)
FILE **tptr;
{ unsigned int ascii_sum;
  int chrf;
  char temp1[MAX_NAME];

if (tptr == NULL)
  inform("FILE CHKSUM Err",STOP);

if (testfile[0] == '\0')
  return;
if (*tptr != NULL)
 { fclose(*tptr);
   *tptr = NULL;
 }
*tptr = fopen(testfile,"r"); /* reopen the file to get ASCII sum */
if (*tptr == NULL)
 { inform("Cannot reopen vector file",CONTINUE);
   inform("to calculate file checksum",STOP);
 }
for (chrf = fgetc(*tptr);
     (chrf != STX) && (!feof(*tptr));
     chrf = fgetc(*tptr))
   ;  /* look for STX char */
if (chrf != STX && feof(*tptr))
  err(FATAL,"cannot find STX",0,0,0);
for (chrf = fgetc(*tptr),ascii_sum = STX;
     !feof(*tptr) && chrf != ETX;
     chrf = fgetc(*tptr))
 { if (chrf == '\n')
     ascii_sum = ascii_sum + '\n' + '\r';
   else
     ascii_sum = ascii_sum + (unsigned int)chrf;
 } /* FOR */
if (chrf != ETX && feof(*tptr))
  err(FATAL,"cannot find ETX",0,0,0);
ascii_sum = ascii_sum + ETX;  /* add ETX */
if (*tptr != NULL)
  fclose(*tptr);
*tptr = NULL;
*tptr = fopen(testfile,"a");  /* append the ASCII sum to the file */
if (*tptr == NULL)
  inform("Cannot reopen file to append ASCII checksum",STOP);

ascii_sum = ascii_sum & 0x0FFFFL;
sprintf(temp1,"%04x\n",ascii_sum);
upr_case_convert(temp1);

fprintf(*tptr,"%s",temp1);  /* write link checksum */
fprintf(stderr,"%s",temp1);  /* send to screen also */
fclose(*tptr);
*tptr = NULL;
} /* FILE_CHECKSUM */
/*  -   -   -   -   -   -   */
/* start making the test vectors by getting a character at a time and
   checking that the character is valid for the test vector signal; i.e.,
   if the first char is a 'C' and the first symbol in the PIN_VCTR list is
   a clock signal, then only 'C' or 'P' (for preloading) is valid.
 Valid settings:
   for input : 0,1
   for output: L,H,Z
   for IO    : 0,1,L,H
   for clock : C,K,P
   for breg  : L,H
 Note:
  for BREGs, a B is put in the first position regardless of the pin type;
  a 0 or 1 follows, then the register states. The 0 in the second position
  means that the first n-2 internal registeres are loaded; a 1 means load
  the next n-2 internal registers. This is handled by LOAD_INTERNAL()
*/
make_vectors()
{ struct type1 *make_ptr;
  struct type3 *pin_ptra;
  char the_vector[INPUT_ARRAY_SIZE],breg_buf[INPUT_ARRAY_SIZE];
  int vctr_nmbr,i;

fprintf(stderr,"N Vectors from File [%s] for PLD [%s]*\n",spec_name,part_name);
fprintf(stderr,"X%c*",((dflt_test == 0) ? '0' : '1'));
if (out_file != NULL && errors == 0)
 { fprintf(out_file,"%c",STX);
   fprintf(out_file,"N Vectors from File [%s] for PLD [%s]*\n",
           spec_name,part_name);
   fprintf(out_file,"X%c*",((dflt_test == 0) ? '0' : '1'));
 }
vctr_init(the_vector,'X',physical_pins);
if (num_pins-physical_pins == 0)
  breg_buf[0] = '\0';
else
  vctr_init(breg_buf,'X',num_pins-physical_pins);
if (pin_vctr == NULL)
  inform("PIN_VCTR list is NULL",STOP);
for (make_ptr = pin_vctr->typ1_next,vctr_nmbr = 1,lex(icon),i = 0;
     the_keyword != END;
     ++vctr_nmbr,lex(icon))
 { if (!strcmp(icon,"load_internal"))
    { if (make_ptr != pin_vctr->typ1_next)
        err(FATAL,"LOAD_INTERNAL must be on a line by itself",0,0,0);
      vctr_nmbr = load_internal(the_vector,breg_buf,vctr_nmbr);
           /* load internal reg */
      vctr_init(the_vector,'X',physical_pins);
      if (num_pins-physical_pins == 0)
        breg_buf[0] = '\0';
      else
        vctr_init(breg_buf,'X',num_pins-physical_pins);
    } /* IF */
   else {
   for (;icon[0] != ';';++i,++screen_position)
    { if (icon[i] == '\0')
       { lex(icon);
         i = 0;
         if (icon[0] == ';' || the_keyword == END)
          { if (make_ptr != NULL)
              err(FATAL,
                  "number of symbols does not match vector specification",
                  0,0,0);
            else
             { make_ptr = pin_vctr->typ1_next;
               break;
             }
          }
       } /* IF */
      if (make_ptr == NULL)
        err(FATAL,"number of symbols does not match vector specification",
            0,0,0);
      if (abs(make_ptr->pin_num) > physical_pins)
       { if (make_ptr->operator != '3' || abs(make_ptr->pin_num) > num_pins)
           inform("Indexing beyond THE_VECTOR range",STOP);
       } /* IF */
      if (isalpha(icon[i]))
       { if (islower(icon[i]))
           icon[i] = toupper(icon[i]);
       }
      valid_test_symbol(icon[i],abs(make_ptr->pin_num)-1,the_vector);
      pin_ptra = (struct type3 *)find_pin(abs(make_ptr->pin_num));
      switch(pin_ptra->polarity)
       { case HIGH
           : assign_tv(make_ptr->operator,icon[i],breg_buf,
                       the_vector,abs(make_ptr->pin_num)-1);
             break;
         case LOW
           : switch(icon[i])
              { case '0'
                   : assign_tv(make_ptr->operator,'1',breg_buf,
                               the_vector,abs(make_ptr->pin_num)-1);
                     break;
                case '1'
                   : assign_tv(make_ptr->operator,'0',breg_buf,
                               the_vector,abs(make_ptr->pin_num)-1);
                     break;
                case 'L'
                   : assign_tv(make_ptr->operator,'H',breg_buf,
                               the_vector,abs(make_ptr->pin_num)-1);
                     break;
                case 'H'
                   : assign_tv(make_ptr->operator,'L',breg_buf,
                               the_vector,abs(make_ptr->pin_num)-1);
                     break;
                default
                   : assign_tv(make_ptr->operator,icon[i],breg_buf,
                               the_vector,abs(make_ptr->pin_num)-1);
                     break;
              } /* SWITCH */
             break;
         case PROGRAMMABLE
           : inform("Polarity in MAKE_VECTORS cannot be Programmable",STOP);
             break;
         default
           : inform("Polarity must be HIGH/LOW",STOP);
             break;
       } /* SWITCH */
      make_ptr = make_ptr->typ1_next;
    } /* FOR */
   fill_power(the_vector);  /* set power/gnd locations */
   vctr_print(the_vector,breg_buf,vctr_nmbr);
  } /* ELSE */
 } /* FOR */
fprintf(stderr,"\n");
if (out_file != NULL && errors == 0) {
  fprintf(out_file,"\n%c",ETX);
  file_checksum(&out_file);
}
} /* MAKE_VECTORS */
/*  -   -   -   -   -   -   */
/* check that the items in PLIST_A are not duplicated in PLIST_B */
duplicate_check(plist_a,plist_b)
struct type1 *plist_a,*plist_b;
{ struct type1 *pt1,*pt2;
if (plist_a == NULL || plist_b == NULL)
  inform("A list is NULL",STOP);
for (pt1 = plist_a->typ1_next; pt1 != NULL; pt1 = pt1->typ1_next)
 { for (pt2 = plist_b->typ1_next; pt2 != NULL; pt2 = pt2->typ1_next)
    { if (pt1->pin_num == pt2->pin_num)
        err(FATAL,"redefinition of pin in Test Vector section",0,0,0);
    }
 } /* FOR */
} /* DUPLICATE_CHECK */
/*  -   -   -   -   -   -   */
msg_chk2()
{
err(FATAL,"incompatible with PIN definition",0,0,0);
} /* MSG_CHK2 */
/*  -   -   -   -   -   -   */
/* check that the PTYPE_CONST is compatible with the pin_type associated
   with the pin number A_NMBR. Ex: PTYPE_CONST is INPUT, and this check will
   pass only if the pin_type of A_NMBR is INPUT,IO,CLOCK,CLK_INPUT or
   CONTROL */
chk2_pin(ptype_const,a_nmbr)
int ptype_const,a_nmbr;
{ int x;
  struct type3 *pin_1key;
x = abs(a_nmbr);
if (x <= 0)
  inform("pin cannot be negative",STOP);
pin_1key = (struct type3 *)find_pin(x);
if (x > physical_pins)
 { if (pin_1key->pin_type != BREG)
     inform("pin is not a BREG",STOP);
 } /* IF */
if (ptype_const != pin_1key->pin_type)
 { switch(ptype_const)
    { case INPUT
       : switch(pin_1key->pin_type)
          { case CLOCK:
            case CLK_INPUT:
            case IO:
            case VCC:
            case GND:
            case CONTROL: break;
            default: msg_chk2();
                     break;
          }
         break;
      case OUTPUT
       : switch(pin_1key->pin_type)
          { case IO: break;
            default: msg_chk2();
                     break;
          }
         break;
      case IO
       :
      case BREG
       :
      default
       : msg_chk2();
         break;
    } /* SWITCH */
 } /* IF */
} /* CHK2_PIN */
/*  -   -   -   -   -   -   */
/* get the pins listed under the test vector pin type TVPIN_TYPE and add it
   to the PIN_VCTR list. This list will be used to check the test vector
   value arguments later (e.g., H,L valid for IO and output pins only.
   PTYPE_CONST also indicates the pin type but uses the constant definitions
   from the H files. */
push_testvector(tvpin_type,ptype_const)
char tvpin_type;
int ptype_const;
{ int high_pin,tmp_int;
  char bufa[MAX_NAME], bufb[MAX_NAME];
  struct type1 *tmp;
  struct type3 *pin_2key;

for (lex(icon),high_pin = YES; icon[0] != ';'; lex(icon),high_pin = YES)
 { if (icon[0] == '/')
    { high_pin = NO;
      lex(icon);
    } /* IF */
   switch(check_name(icon,bufa,bufb))
    { case NOT_EXIST
       : err(FATAL,"Name not defined in PIN definition section",0,0,0);
         break;
      case VCTR
       : duplicate_check(pin_vctr,vctr2);
         for (tmp = vctr2->typ1_next; tmp != NULL; tmp = tmp->typ1_next)
          { tmp->operator = tvpin_type;  /* set the test vector type */
            chk2_pin(ptype_const,tmp->pin_num);
            pin_2key = (struct type3 *)find_pin(abs(tmp->pin_num));
            if (high_pin == NO) /* complement the polarity of the signal */
             { if (pin_2key->polarity == HIGH)
                 pin_2key->polarity = LOW;
               else
                 pin_2key->polarity = HIGH;
             } /* IF */
          } /* FOR */
         for (tmp = pin_vctr; tmp->typ1_next != NULL; tmp = tmp->typ1_next)
           ;  /* find the end of the PIN_VCTR list */
         tmp->typ1_next = vctr2->typ1_next;
         vctr2->typ1_next->backptr = tmp;
         vctr2->typ1_next = NULL;
         break;
      case EXIST
       : convert('d',bufb,bufa);
         tmp_int = (int)atoi(bufa);
         push(vctr2,tvpin_type,tmp_int,(struct type1 *)NULL);
         duplicate_check(pin_vctr,vctr2);
         chk2_pin(ptype_const,tmp_int);
         pin_2key = (struct type3 *)find_pin(abs(tmp_int));
         if (high_pin == NO)  /* complement the polarity of the signal */
          { if (pin_2key->polarity == HIGH)
              pin_2key->polarity = LOW;
            else
              pin_2key->polarity = HIGH;
          } /* IF */
         for (tmp = pin_vctr; tmp->typ1_next != NULL; tmp = tmp->typ1_next)
           ;
         tmp->typ1_next = vctr2->typ1_next;
         vctr2->typ1_next->backptr = tmp;
         vctr2->typ1_next= NULL;
         break;
      default
       : err(FATAL,"nonexistent pin name(s)",0,0,0);
    } /* SWITCH */
  lex(icon);
  switch(icon[0])
   { case ';' : return;
     case ',' : break;
     case '/' : get_new_token = NO;
                break;
     default
       : if (token_type == VARIABLE || token_type == KEYWORD)
           get_new_token = NO;
         else
           err(WARNING,"separate names with ','",token_type,VARIABLE,KEYWORD);
   } /* SWITCH */
 } /* FOR */
} /* PUSH_TESTVECTOR */
/*  -   -   -   -   -   -   */
def_find_pins()
/* read the pin definition sections; check that they match with the pins in
   the PIN definition section. If they do, then push the items onto the
   PIN_VCTR list with the pin number and appropriate OPERATOR argument:
    0 = input pin
    1 = output pin
    2 = io pin
    3 = breg pin
*/
{
for (lex(icon); the_keyword != BEGIN; lex(icon))
 { switch(the_keyword)
    { case IN
        : push_testvector('0',INPUT);
          break;
      case OUT
        : push_testvector('1',OUTPUT);
          break;
      case I_O
        : push_testvector('2',IO);
          break;
      default
        : if (strcmp(icon,"breg"))
            err(FATAL,"invalid pin declaration in test vector section",0,0,0);
          push_testvector('3',BREG);
          break;
    } /* SWITCH */
 } /* FOR */
} /* FIND_PINS */
/*  -   -   -   -   -   -   */
testvector_parse()
{
for (lex(icon);;lex(icon))  /* find the beginning of the test vector table */
 if (!strcmp("test_vectors",icon))
    break;
def_find_pins();
make_vectors();
if (out_file != NULL)
 { fclose(out_file);
   out_file = NULL;
 }
if (input_file != NULL)
 { fclose(input_file);
   input_file = NULL;
 }
} /* TESTVECTOR_PARSE */
/*  -   -   -   -   -   -   */
