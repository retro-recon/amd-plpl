/* ==> Logic Simulator (SIM)
June 22, 1987 HEADSIM.C contains the MAIN() function. The JEDEC
map to be simulated is read into a buffer, and the simulator
database is read here before simulation begins.
*/
#include "simdef.h" /* data definition/declaration module */
/*  -   -   -   -   -   -   */
inform(msg,status) /* inform the user of a file error and then exit */
char msg[];
int status;
{
fprintf(stderr,"\n[Message] %s",msg);
switch(status) {
  case BAD_JEDEC : fprintf(stderr,"\nInvalid JEDEC file\n");
                   exit(BAD_EXIT);
  case NO_FILE   : fprintf(stderr,"\nRestart program\n");
                   exit(BAD_EXIT);
  case STOP      : exit(BAD_EXIT);
  default        : break;
} /* SWITCH */
} /* INFORM */
/*  -   -   -   -   -   -   */
/* graphics characters to use when drawing the waveforms.
   On DOS systems, some of the extended graphics characters
   can be used; use ASCII characters (slashes and bars) for
   other systems such as the VAX: set using NO argument
*/
graph_forms(draw_type)
int draw_type;
{
if (draw_type == YES) {
  VECTOR_MARK = 176;
  SPACE       =  32;
  SPC_VCTR    = 216;
/* use these for inputs */
  VERT_BAR    = 179;
  LOW_LEFT    = 192;
  TOP_RIGHT   = 191;
  LOW_RIGHT   = 217;
  TOP_LEFT    = 218;
/* use these for outputs */
  DOUBLE_VERT   = 186;
  LEFT_LOW_OUT  = 200;
  RIGHT_TOP_OUT = 187;
  RIGHT_LOW_OUT = 188;
  LEFT_TOP_OUT  = 201;
/* use these for markers */
  IN_FLOAT  = 'F';
  OUT_HI_Z  = 'Z';
  NO_TEST   = 'N';
  DONT_CARE = 'X';
  CROSS     = 197;
  RIGHT_CROSS = 195;
  LEFT_CROSS  = 180;
  C_CLOCK     = 175;
  K_CLOCK     = 174;
} /* IF */
else { /* display ASCII characters */
  VECTOR_MARK = ':';
  SPACE       = ' ';
  SPC_VCTR    = '#';
/* use these for inputs */
  VERT_BAR   = '|';
  LOW_LEFT   = ' ';
  TOP_RIGHT  = ']';
  LOW_RIGHT  = ' ';
  TOP_LEFT   = '[';
/* use these for outputs */
  DOUBLE_VERT   = '|';
  LEFT_LOW_OUT  = ' ';
  RIGHT_TOP_OUT = ']';
  RIGHT_LOW_OUT = ' ';
  LEFT_TOP_OUT  = '[';
/* use these for markers */
  IN_FLOAT    = 'F';
  OUT_HI_Z    = 'Z';
  NO_TEST     = 'N';
  DONT_CARE   = 'X';
  CROSS       = '+';
  RIGHT_CROSS = '~';
  LEFT_CROSS  = '~';
  C_CLOCK     = '>';
  K_CLOCK     = '<';
} /* ELSE */
} /* GRAPH_FORMS */
/*  -   -   -   -   -   -   */
read_command_line(argc,argv)
int argc;
char *argv[];
{ char *ch;
input_file = NULL;
out_file = NULL;
waveptr = NULL;
jedecfile = NULL;  /* JEDEC file pointer */
m29file = NULL;    /* debugging pointer */
jedec_map = NULL;  /* full JEDEC map */
MAP_EQ_file = NULL; /* map-to-equation file */
calcvctr = NULL;
dfn_list = NULL;
pin_arch_list = NULL; /* architecture feature list */
name_pair_list = NULL; /* name and pin no list for MAP-to-EQ translation */
eq_frm_list = NULL;
dflt_test = 0;
dflt_names = YES;
trace = YES;
accurate = NO;
names_file[0] = '\0';
  /* use the PC/RT extended graphics characters; use NO
     argument for ASCII characters: see A option       */
graph_forms(YES);
debug_flag = NO;
db_sim[0] = '\0';
suppress_msg = NO;
while (--argc > 0 && (*++argv)[0] == '-')
  { for (ch = argv[0] + 1; *ch != '\0'; ch++)
      { if (islower(*ch))
      *ch = toupper(*ch);
    switch (*ch)
     { case 'I' : argc--;
              if (argc <= 0)
            inform("missing JEDEC file\n",STOP);
              strcpy(jmapfile,*++argv);
              jedecfile = exist(jmapfile,"r");
                      argc--;
              if (argc <= 0)
            inform("missing vector file\n",STOP);
              strcpy(vctr_file,*++argv);
              break;
      case 'P' : argc--;
             if (argc <= 0)
               inform("missing part file\n",STOP);
             strcpy(db_sim,*++argv);
                     if (input_file != NULL)
                       fclose(input_file);
             input_file = partsearch(db_sim);
                     break;
      case 'O' : argc--;
             if (argc <= 0)
               inform("missing simulation result file\n",STOP);
             strcpy(simfile,*++argv);
             out_file = exist(simfile,"w");
                     break;
      case 'F' : argc--;
             if (argc <= 0)
               inform("missing JEDEC-to-EQ file\n",STOP);
             strcpy(eq_file,*++argv);
             MAP_EQ_file = exist(eq_file,"w");
                     argc--;
                 if (argc <= 0)
               inform("No name file for MAP-to-EQ translation\n",
                               CONTINUE);
                     else
               strcpy(names_file,*++argv);
                     break;
      case 'B': argc--;
            if (argc <= 0)
              inform("debug value ?\n",STOP);
            debug_flag = atoi(*++argv);
                    break;
          case 'A': graph_forms(NO);
                    break;
      case 'W': argc--;
            if (argc <= 0)
              inform("missing WAVEFORM file\n",STOP);
            strcpy(wavefile,*++argv);
            waveptr = exist(wavefile,"w");
                    break;
      case 'C': argc--;
            if (argc <= 0)
              inform("specify file for calculated values\n",STOP);
            strcpy(calcfile,*++argv);
            calcvctr = exist(calcfile,"w");
                    break;
          case 'T': accurate = YES;
                    break;
          case 'N': trace = NO;
                    break;
          case 'Q': suppress_msg = YES;
                    break;
          case 'X': dflt_test = 1;
                    break;
      default : fprintf(stderr,"sim:[%c]?\n", *ch);
            exit(BAD_EXIT);
            break;
     } /* SWITCH */
     } /* FOR */
  } /* WHILE */
if (jedecfile == NULL)
 { inform("Using the Simulator:",CONTINUE);
   inform("sim -i <JEDEC map> <vectors>",CONTINUE);
   inform("Options:",CONTINUE);
   inform("\t-o <simulator result file>",CONTINUE);
   inform("\t-a (generate ASCII waveforms)",CONTINUE);
   inform("\t-b <debug mode #>",CONTINUE);
   inform("\t\t->  1 (show pin & product term states)",CONTINUE);
   inform("\t\t->200 (show intermediate calculations)",CONTINUE);
   inform("\t-c <calculated vector values>",CONTINUE);
   inform("\t-f <JEDEC-to-EQUATIONS> <pin name file>",CONTINUE);
   inform("\t-n (no simulation; list equations only)",CONTINUE);
   inform("\t-p <part to simulate>",CONTINUE);
   inform("\t-q (quiet mode/no warning msgs)",CONTINUE);
   inform("\t-w <waveform output file>",CONTINUE);
   inform("\t-x (set X/F/Z value to 1)\n",STOP);
 }
if (db_sim[0] == '\0' || input_file == NULL) {
  if (input_file != NULL)
    fclose(input_file);
  input_file = partsearch(db_sim);
} /* IF */
} /* end of READ_COMMAND_LINE */
/*  -   -   -   -   -   -   */
get_index(nmbr_chr)
/* get an index and return the number through NMBR_CHR */
char nmbr_chr[];
{ int i,p;
nmbr_chr[0] = '\0';
lex(icon);
i = HIGH;
if (icon[0] == '-') {
  lex(icon);
  i = LOW;
}
switch(token_type)
 { case NUMBER   : convert('d',icon,nmbr_chr);
                   p = (int)atoi(nmbr_chr);
                   if (i == LOW)
                     return(-p);
                   return(p);
   default       : err(FATAL,USE_CONSTANT,token_type,PUNCT_MARK,NUMBER);
                   return(NULL_VALUE);
 } /* SWITCH */
} /* GET_INDEX */
/*  -   -   -   -   -   -   */
/* Initialize global variables. If ACTION = YES, then
   do list_initialize also; if NO, then skip it. The NO case
   is done when PLD_sw_WRITE() is performed
*/
var_init(action)
int action;
{
if (action == YES) {
  list_initialize(); /* in SIMFUNC.C module */
  fuse_count = 0;
}
end_found = NO;
line_number = 0;
errors = 0;
get_new_token = YES;
end_of_file = NO;
get_line();  /* read in a source line */
} /* VAR_INIT */
/*  -   -   -   -   -   -   */
/* clean the DFN_CLASS data structures */
cln_dfn(point1)
struct dfn_class *point1;
{ struct dfn_class *dfnptr;
  struct node1class *cls1ptr,*cls2ptr;

for (dfnptr = point1;
     dfnptr != NULL;) {
  point1 = point1->nxt_dfn;
  for (cls1ptr = dfnptr->list_val; cls1ptr != NULL;) {
    cls2ptr = cls1ptr->node1next;
    free(cls1ptr);
    cls1ptr = cls2ptr;
  } /* FOR */
  free(dfnptr);
  dfnptr = point1;
} /* FOR */
} /* CLN_DFN */
/*  -   -   -   -   -   -   */
cln_nd2(point2)
struct node2class *point2;
{ struct node2class *ndptr;

for (ndptr = point2; ndptr != NULL;) {
 if (ndptr->symbol != NULL)
    free(ndptr->symbol);
  cln_nd2(ndptr->FWD);  /* do not clean backwards through RWD pointer */
  cln_nd2(ndptr->LEFT_0);
  cln_nd2(ndptr->RIGHT_1);
  free(ndptr);
  ndptr = NULL;
} /* FOR */
}/* CLN_ND2 */
/*  -   -   -   -   -   -   */
cln_PT_el(point3)
struct PT_element *point3;
{ struct PT_element *cln_ptr;

for (cln_ptr = point3;
     cln_ptr != NULL;) {
  point3 = point3->next_PT;
  cln_nd2(cln_ptr->pin_eq);
  free(cln_ptr);
  cln_ptr = point3;
} /* FOR */
} /* CLN_PT_EL */
/*  -   -   -   -   -   -   */
cln_mem_el(point4)
struct pin_element *point4;
{
if (point4 == NULL)
  return;
cln_PT_el(point4->reg_latch.PTreset);
cln_PT_el(point4->reg_latch.PTset);
cln_PT_el(point4->reg_latch.PTpreload);
cln_PT_el(point4->reg_latch.PTobserve);
cln_PT_el(point4->reg_latch.PTclock);
cln_PT_el(point4->reg_latch.PTenable);
cln_PT_el(point4->reg_latch.PTmem0_input);
cln_PT_el(point4->reg_latch.PTmem1_input);
} /* CLN_MEM_EL */
/*  -   -   -   -   -   -   */
/* close all files before ending the simulation and
   free all space used by the linked lists */
close_files()
{ struct pair_name_no *pairptr;
  struct nmbr_list *p_archptr;
  struct EQFORMAT *EQ_xtraptr;
  struct EQ_FRM *EQ2_ptr;
  struct pin_element *pinptr;

if (input_file != NULL) {
  fclose(input_file);
  input_file = NULL;
}
if (out_file != NULL) {
  fclose(out_file);
  out_file = NULL;
}
if (m29file != NULL) {
  fclose(m29file);
  m29file = NULL;
}
if (jedecfile != NULL) {
  fclose(jedecfile);
  jedecfile = NULL;
}
if (waveptr != NULL) {
  fclose(waveptr);
  waveptr = NULL;
}
if (calcvctr != NULL) {
  fclose(calcvctr);
  calcvctr = NULL;
}
if (MAP_EQ_file != NULL) {
  fclose(MAP_EQ_file);
  MAP_EQ_file = NULL;
}
for (pairptr = name_pair_list;
     pairptr != NULL;) {
  name_pair_list = name_pair_list->pair_next;
  free(pairptr);
  pairptr = name_pair_list;
} /* FOR */
cln_dfn(dfn_list);
for (p_archptr = pin_arch_list;
     p_archptr != NULL;) {
  pin_arch_list = pin_arch_list->nxt_pin;
  free(p_archptr);
  p_archptr = pin_arch_list;
} /* FOR */
for (EQ_xtraptr = eq_frm_list;
     EQ_xtraptr != NULL;) {
  eq_frm_list = eq_frm_list->MORE_EQ;
  free(EQ_xtraptr);
  EQ_xtraptr = eq_frm_list;
} /* FOR */
for (EQ2_ptr = pt_EQFRM;
     EQ2_ptr != NULL;) {
  pt_EQFRM = pt_EQFRM->nxt_EQ;
  free(EQ2_ptr);
  EQ2_ptr = pt_EQFRM;
} /* FOR */
cln_PT_el(product_terms);
for (pinptr = test_pin_elements; pinptr != NULL;) {
  test_pin_elements = test_pin_elements->nxt_tst_pin;
  cln_dfn(pinptr->poss_values);
  cln_mem_el(pinptr);
  cln_nd2(pinptr->out_element);
  cln_PT_el(pinptr->misc_fnc);
  free(pinptr);
  pinptr = test_pin_elements;
} /* FOR */
} /* CLOSE_FILES */
/*  -   -   -   -   -   -   */
main(argc,argv)
int argc;
char *argv[];
{
fprintf(stderr,"PLPL Logic Simulator Ver 2.2x December 1987\n");
read_command_line(argc,argv);
var_init(YES);   /* initialize data structures: lists, arrays, etc. */
read_data();  /* read data from the simulator and JEDEC files */
if (trace == YES)
  simulate();   /* do the simulation */

/* write the equations and architecture features to a file
   in a PLD development language syntax
*/
if (jedec_map != NULL)
  free(jedec_map);
jedec_map = NULL;
if (MAP_EQ_file != NULL)
  PLD_sw_format(); /* in ENHANCE.C */
close_files();  /* close the files properly before ending the simulation */
if (errors == 0)
  exit(GOOD_EXIT);
else
  exit(BAD_EXIT);
} /* MAIN */
