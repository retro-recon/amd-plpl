/*
June 24, 1987: DBSIM.C reads the simulator database file for the
specified part. This contains the device description of the model
which depends on the JEDEC map pattern in the char buffer JEDEC_MAP.
*/
#define IN_DBSIM 1
#include "simdef.h"
/*  -   -   -   -   -  */
/*
Read data from the simulator database file and the JEDEC map file.
Note that the part's total fuse array size and pin count is read,
then the JEDEC map is read into a buffer. The rest of the simulator
database is then read, using data from the JEDEC map in the buffer.
*/
read_data()
{ char valbuf[MAX_NAME];  /* value buffer */

fprintf(stderr,"\nReading [%s] Database",db_sim);
lex(icon);
if (the_keyword != BEGIN_DEF)
  err(FATAL,"use BEGIN_DEF",0,0,0);
fuse_count = get_index(valbuf);
num_pins = get_index(valbuf);
physical_pins = get_index(valbuf);
input_array = get_index(valbuf);
read_JEDEC_map();   /* read JEDEC map into JEDEC_MAP buffer */
for (lex(icon); the_keyword != END_DEF; lex(icon)) {
  switch(the_keyword) {
  case PIN:
    fprintf(stderr,"\nConfiguring pins");
    configure_pins();
    break;
  case EQ:
    fprintf(stderr,"\nLoading equation formats");
    eq_set_up();
    break;
  case NODE:
    fprintf(stderr,"\nForming logic equations from JEDEC map");
    node_set_up();
    break;
  case DEFINE:
    fprintf(stderr,"\nReading definition statements");
    sim_define();
    break;
  default:
    if (icon[0] != ';')
      err(FATAL,"Check simulation database",0,0,0);
    break;
  } /* SWITCH */
} /* FOR */
} /* READ_DATA */
/* - - - - - - - - - - */
convert(base,nmbr,pattern)
/* convert using BASE the number NMBR and put the result in PATTERN */
char base;  /* can take the values B,O,D,H for Bin, Octal, Dec, and Hex */
char nmbr[],pattern[];
{ int i,j,k,value,num_type;
  char buf[MAX_NAME];

/* convert everything to decimal first */

num_type = DECIMAL;
if (nmbr[0] == '#')  /* take out the radices */
 { switch(nmbr[1])
    { case 'b' :
      case 'B' : num_type = BINARY;
                 break;
      case 'o' :
      case 'O' : num_type = OCTAL;
                 break;
      case 'd' :
      case 'D' : num_type = DECIMAL;
                 break;
      case 'h' :
      case 'H' : num_type = HEXADECIMAL;
                 break;
      default  : err(FATAL,"unknown number base",0,0,0);
    } /* SWITCH */
   for (i=2, j=0; nmbr[i] != '\0'; ++i)
     buf[j++] = nmbr[i];
   buf[j] = '\0';
 }
else /* NMBR was decimal */
  strcpy(buf,nmbr);
switch(num_type)
 { case OCTAL       : sscanf(buf,"%o",&value); break;
   case HEXADECIMAL : sscanf(buf,"%x",&value); break;
   case DECIMAL     : sscanf(buf,"%d",&value); break;
   case BINARY
       : i = strlen(buf);
         for (j=i-1,k=1,value=0; j >= 0; --j)
          { if (j!= i-1)
              k = k*2;
            if (buf[j] == '1')
              value = value+k;
          }
         break;
   default : value = (int)atoi(nmbr);
             break;
 } /* SWITCH */
switch(base)
 { case 'b' :
   case 'B' : if (num_type == BINARY)
               { strcpy(pattern,buf);
                 return;
               }
              for (i=0 ; i<16 ; ++i)
               { if (((value>>i)&(0x1)) == 1)
                   buf[i] = '1';
                 else
                   buf[i] = '0';
               }
              buf[i] = '\0';
              j = strlen(buf)-1;  /* reverse the string */
              for (i = j,pattern[j+1] = '\0';i >= 0; --i)
                pattern[j-i] = buf[i];
              break;
   case 'd' :
   case 'D' : sprintf(pattern,"%d",value);
              break;
   case 'o' :
   case 'O' : sprintf(pattern,"%o",value);
              break;
   case 'h' :
   case 'H' : sprintf(pattern,"%x",value);
              break;
 } /* SWITCH */
} /* CONVERT */
/* - - - - - - - - - - */
mem2_check(mem_0typ,mem_1typ,mem_2typ,mem_3typ,mem_4typ)
   /* check if memory was allocated */
struct node1class *mem_0typ;
struct node2class *mem_1typ;
struct PT_element *mem_2typ;
struct pin_element *mem_3typ;
struct dfn_class *mem_4typ;
{
if ((mem_0typ == NULL) && (mem_1typ == NULL) && (mem_2typ == NULL)
 && (mem_3typ == NULL) && (mem_4typ == NULL))
 { fprintf(stderr,"Not enough memory");
   exit(BAD_EXIT);
 }
} /* MEM2_CHECK */
/* - - - - - - - - - - */
/* check for more memory */
mem_size_chk(ltype1,ltype2)
struct nmbr_list *ltype1;
struct pair_name_no *ltype2;
{
if ((ltype1 == NULL) && (ltype2 == NULL)) {
  fprintf(stderr,"No memory MEM_SIZE");
  exit(BAD_EXIT);
}
} /* MEM_SIZE_CHK */
/* - - - - - - - - - - */
CORE_chk(mtype1,mtype2)
/* check if memory was allocated */
struct EQFORMAT *mtype1;
struct EQ_FRM *mtype2;
{
if ((mtype1 == NULL) && (mtype2 == NULL)) {
  fprintf(stderr,"No memory (CORE)");
  exit(BAD_EXIT);
}
} /* CORE_chk */
/* - - - - - - - - - - */
list_initialize()
/*
Initialize all the pointers. This routine is necessary
before using any list insertion routine because I did
not know how to send a pointer to a pointer. Future
programs will include this
*/
{
product_terms = (struct PT_element *)malloc(sizeof(struct PT_element));
mem2_check((struct node1class *)NULL,(struct node2class *)NULL,
           product_terms,(struct pin_element *)NULL,(struct dfn_class *)NULL);
strcpy(product_terms->term_name,LIST_HEAD);
product_terms->pinstates = 0;
product_terms->pin_eq = 0;
product_terms->next_PT = NULL;

dfn_list = (struct dfn_class *)malloc(sizeof(struct dfn_class));
mem2_check((struct node1class *)NULL,(struct node2class *)NULL,
    (struct PT_element *)NULL,(struct pin_element *)NULL,dfn_list);
strcpy(dfn_list->class1_name,LIST_HEAD);
dfn_list->list_val = NULL;
dfn_list->nxt_dfn = NULL;

test_pin_elements = (struct pin_element *)malloc(sizeof(struct pin_element));
mem2_check((struct node1class *)NULL,(struct node2class *)NULL,
    (struct PT_element *)NULL,test_pin_elements,(struct dfn_class *)NULL);
test_pin_elements->poss_values = NULL;
test_pin_elements->vctr_element = -1;  /* marks the head of this list */
test_pin_elements->pin_status = 0;
test_pin_elements->reg_latch.store_type = 0;
test_pin_elements->reg_latch.PTreset = NULL;
test_pin_elements->reg_latch.PTset = NULL;
test_pin_elements->reg_latch.PTpreload = NULL;
test_pin_elements->reg_latch.PTobserve = NULL;
test_pin_elements->reg_latch.PTclock = NULL;
test_pin_elements->reg_latch.PTenable = NULL;
test_pin_elements->reg_latch.PTmem0_input = NULL;
test_pin_elements->reg_latch.PTmem1_input = NULL;
test_pin_elements->reg_latch.mem_output = 0;
test_pin_elements->out_element = NULL;
test_pin_elements->misc_fnc = NULL;
test_pin_elements->nxt_tst_pin = NULL;

dfn_list = (struct dfn_class *)malloc(sizeof(struct dfn_class));
mem2_check((struct node1class *)NULL,(struct node2class *)NULL,
    (struct PT_element *)NULL,(struct pin_element *)NULL,dfn_list);
strcpy(dfn_list->class1_name,LIST_HEAD);
dfn_list->list_val = NULL;
dfn_list->nxt_dfn = NULL;

/* init the equation format list */
eq_frm_list = (struct EQFORMAT *)malloc(sizeof(struct EQFORMAT));
CORE_chk(eq_frm_list,(struct EQ_FRM *)NULL);
strcpy(eq_frm_list->EQNAME,LIST_HEAD);
eq_frm_list->EQWIDTH = 0;
eq_frm_list->EQ_OFFSET = 0;
eq_frm_list->EQFRM = NULL;
eq_frm_list->MORE_EQ = NULL;

} /* LIST_INITIALIZE */
/* - - - - - - - - - - */
/*
add a new pin (numbered by A_NEW_PIN) to the TEST_PIN_ELEMENTS list
*/
sim_new_pin(a_new_pin)
int a_new_pin;
{ struct pin_element *tmp1, *tmp2;

tmp1 = (struct pin_element *)malloc(sizeof(struct pin_element));
mem2_check((struct node1class *)NULL,(struct node2class *)NULL,
    (struct PT_element *)NULL,tmp1,(struct dfn_class *) NULL);
tmp1->vctr_element = a_new_pin;  /* marks the head of this list */
tmp1->poss_values = NULL;
tmp1->pin_status = 0;
tmp1->reg_latch.store_type = 0;
tmp1->reg_latch.PTreset = NULL;
tmp1->reg_latch.PTset = NULL;
tmp1->reg_latch.PTpreload = NULL;
tmp1->reg_latch.PTobserve = NULL;
tmp1->reg_latch.PTclock = NULL;
tmp1->reg_latch.PTenable = NULL;
tmp1->reg_latch.PTmem0_input = NULL;
tmp1->reg_latch.PTmem1_input = NULL;
tmp1->reg_latch.mem_output = 0;
tmp1->out_element = NULL;
tmp1->misc_fnc = NULL;
tmp1->nxt_tst_pin = NULL;
for (tmp2 = test_pin_elements;
     tmp2->nxt_tst_pin != NULL;
     tmp2 = tmp2->nxt_tst_pin)
  ;
tmp2->nxt_tst_pin = tmp1;
} /* SIM_NEW_PIN */
/* - - - - - - - - - - */
struct pin_element *sim_find_pin(which_pin)
int which_pin;
{ struct pin_element *sfind_ptr;
for (sfind_ptr = test_pin_elements;
     sfind_ptr != NULL;
     sfind_ptr = sfind_ptr->nxt_tst_pin) {
  if (sfind_ptr->vctr_element == which_pin)
    return(sfind_ptr);
}
if (sfind_ptr == NULL)
  err(FATAL,"Pin not defined in Dbase",0,0,0);
} /* SIM_FIND_PIN */
/* - - - - - - - - - - */
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
/* - - - - - - - - - - */
/* calculate ASCII checksum for CHKFILE */
file_checksum(chkfile)
char chkfile[];
{ unsigned int ascii_sum;
  int chr1;
  char temp1[MAX_NAME];
  FILE *chkptr;

chkptr = fopen(chkfile,"r"); /* reopen the file to get ASCII sum */
if (chkptr == NULL)
 { inform("Cannot reopen file to calculate",CONTINUE);
   inform("transmission checksum",STOP);
 }
for (chr1 = fgetc(chkptr);
     (chr1 != STX) && (!feof(chkptr));
     chr1 = fgetc(chkptr))
   ;  /* look for STX char */
if (chr1 != STX && feof(chkptr))
  err(FATAL,"cannot find STX",0,0,0);
for (chr1 = fgetc(chkptr),ascii_sum = STX;
     !feof(chkptr) && chr1 != ETX;
     chr1 = fgetc(chkptr))
 { if (chr1 == '\n')
     ascii_sum = ascii_sum + '\n' + '\r';
   else
     ascii_sum = ascii_sum + (unsigned int)chr1;
 } /* FOR */
if (chr1 != ETX && feof(chkptr))
  err(FATAL,"cannot find ETX",0,0,0);
ascii_sum = ascii_sum + ETX;  /* add ETX */
if (chkptr != NULL)
  fclose(chkptr);
chkptr = NULL;
chkptr = fopen(chkfile,"a");  /* append the ASCII sum to the file */
if (chkptr == NULL)
  inform("Cannot reopen file to append ASCII checksum",STOP);

ascii_sum = ascii_sum & 0x0FFFFL;
sprintf(temp1,"%04x\n",ascii_sum);
upr_case_convert(temp1);

fprintf(chkptr,"%s",temp1);  /* write link checksum */
fclose(chkptr);
chkptr = NULL;
} /* FILE_CHECKSUM */
/* - - - - - - - - - - */
