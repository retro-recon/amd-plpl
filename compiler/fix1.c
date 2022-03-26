/* September 18, 1986  FIX1.C
   FIX() will fix the intermediate files created by GLS (PLC rev 2.1).
   These files contain logic equations that have been split up by the
   modification done on 9/18/86 to reduce the amount of memory used by
   PLC. This means that an output pin 15 can appear more than once in
   the intermediate file because it was written out right after the
   statement was compiled.

   Example:  15 = 4*5*6;
             15 = 7*8*9;

   After running FIX, the final file should look as follows:
      15 = 4*5*6 + 7*8*9;
   */
#include "glsdef.h"
/* - - - - - - - - - */
add_pin(list_x,pin_number,names)
int list_x,pin_number;
char names[];
{ struct fixtype *a_pin,*tmp;
a_pin = (struct fixtype *)malloc(sizeof(struct fixtype));
if (a_pin == NULL)
  inform("Not enough memory",STOP);
a_pin->pin = pin_number;
strcpy(a_pin->function_name,names);
a_pin->next = NULL;
switch(list_x)
 { case OUTPUT
    : if (known_pins == NULL)
       { known_pins = a_pin;
         current_ptr = a_pin;
       }
      else
       { for (tmp = known_pins; tmp->next != NULL; tmp = tmp->next)
          ;
         tmp->next = a_pin;
         current_ptr = a_pin;
       }
      break;
   case FUNCTION
    : if (fname == NULL)
       { fname = a_pin;
         curnt_fname = a_pin;
       }
      else
       { for (tmp = fname; tmp->next != NULL; tmp = tmp->next)
          ;
         tmp->next = a_pin;
         curnt_fname = a_pin;
       }
      break;
 } /* SWITCH */
} /* ADD_PIN */
/* - - - - - - - - - */
/*
initialize lists and pointers before calling DO_FIX()
*/
fix_initialize(file_a,file_b)
char file_a[],file_b[];
{
first_PT = YES;
fnc_first = YES;
known_pins = NULL;
current_ptr = NULL;
fname = NULL;
curnt_fname = NULL;
add_pin(OUTPUT,NULL_VALUE,"$$$$");
add_pin(FUNCTION,NULL_VALUE,"$$$$");
fnc_file = fopen(FNC_FILE,"w");
if (fnc_file == NULL)
  file_error(FNC_FILE,1);
input_file = fopen(file_a,"r");
if (input_file == NULL)
  file_error(file_a,1);
out_file = fopen(file_b,"a");
if (out_file == NULL)
  file_error(file_b,1);
} /* FIX_INITIALIZE */
/* - - - - - - - - - */
/* arrange the output signals found in the file INPUT_FILE */
arrange()
{ char name[80];
  int change,status,type_signal;

   /* read until the beginning of the equations '*' */
for (fscanf(input_file,"%s",name);
     !feof(input_file) && name[0] != '@';
     fscanf(input_file,"%s",name))
 { status = check_token(name,VALUE,OUTPUT);
   if (status < 0)
     skip_over(OUTPUT,';');
   else
    { add_pin(OUTPUT,status,"$$$$");
      break;
    }
 } /* FOR */
for (change = NO,first_PT = YES,type_signal = NULL_VALUE;
     !feof(input_file) && name[0] != '@';
     fscanf(input_file,"%s",name))
 { switch(check_token(name,PARSE,OUTPUT))
    { case OUTPUT
        : out_name(name,OUTPUT);
          type_signal = OUTPUT;
          change = YES;
          break;
      case FUNCTION
        : out_name(name,FUNCTION);
          change = YES;
          break;
      case SKIP  /* FIX is not looking for this output signal name */
        : skip_over(OUTPUT,';');
          break;
      default
        : inform("Can't program FIX right",STOP);
          break;
    } /* SWITCH */
 } /* FOR */
if (name[0] != '@')
  inform("Invalid plc file",STOP);
if (input_file != NULL)
 { fclose(input_file);
   input_file = NULL;
 }
fprintf(fnc_file,"\n@\n");
if (type_signal == OUTPUT)
 { fprintf(stderr,";");
   fprintf(out_file,";");
 }
fix_function();
if (fnc_file != NULL)
  fclose(fnc_file);
fnc_file = fopen(FNC_FILE,"w");
if (fnc_file == NULL)
  inform("Can't reopen FNC_FILE",STOP);
return(change);
} /* ARRANGE */
/* - - - - - - - - - */
/*
remove/delete the file RNAME from the current directory;
possible portability issue
*/
remove_file(rname)
char rname[];
{ int status;
  char buf[80];
#ifdef vms   /* use DELETE in VAX/VMS-C to delete a file */
status = delete(rname);
#else        /* for UNIX and C86-C systems */
status = unlink(rname);
#endif
if (status == -1)
 { sprintf(buf,"Could not delete [%s]",rname);
   inform(buf,CONTINUE);
 }
} /* REMOVE_FILE */
/* - - - - - - - - - */
/*
fix the input file IN_FILE
*/
do_fix(in_file)
char in_file[];
{ int item;
for (item = fgetc(input_file);
     !feof(input_file) && item != '*';
     item = fgetc(input_file))
 { fprintf(stderr,"%c",item);
   fprintf(out_file,"%c",item);
 }
fprintf(stderr,"*");
fprintf(out_file,"*");
for (;arrange() == YES;skip_over(OUTPUT,'*'))
 { if (input_file != NULL)
    { fclose(input_file);
      input_file = NULL;
    }
   input_file = fopen(in_file,"r");
   if (input_file == NULL)
     inform("Can't reopen input file",STOP);
 } /* FOR */
fprintf(stderr,"\n@");
fprintf(out_file,"\n@");
clean_space_used();
remove_file(in_file);
remove_file(FNC_FILE);
} /* DO_FIX */
/* - - - - - - - - - */
/*
fix the PLC_FILE and convert it into the PLPL 2.1
format before writing to PLC_INT_FILE
*/
fix(plc_file,plc_int_file)
char plc_file[], plc_int_file[];
{
fix_initialize(plc_file,plc_int_file);
do_fix(plc_file);
} /* MAIN */
/* - - - - - - - - - */
