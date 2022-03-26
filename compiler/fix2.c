/* September 22, 1986: FIX2.C */
#include "glsdef.h"
/* - - - - - - - - - */
fnc_send(fnc_name)
char fnc_name[];
{ int fnc_item;

if (fnc_first == YES)
 { fprintf(out_file,"\n%s =",fnc_name);
   fprintf(stderr,"\n%s =",fnc_name);
   fnc_first = NO;
 }
else {
  fprintf(out_file," +\n    ");
  fprintf(stderr,  " +\n    ");
}
for (fnc_item = fgetc(fnc_file);
     !feof(fnc_file) && fnc_item != '=';
     fnc_item = fgetc(fnc_file))
  ;
for (fnc_item = fgetc(fnc_file);
     fnc_item != ';' && !feof(fnc_file);
     fnc_item = fgetc(fnc_file)) {
  fprintf(stderr,"%c",fnc_item);
  fprintf(out_file,"%c",fnc_item);
} /* FOR */
if (fnc_item != ';')
  inform("Can't find ;",STOP);
} /* FNC_SEND */
/* - - - - - - - - - */
fnc_arrange()
{ char name[80];
  int status,change;
for (fscanf(fnc_file,"%s",name);
     !feof(fnc_file) && name[0] != '@';
     fscanf(fnc_file,"%s",name))
 { status = check_token(name,VALUE,FUNCTION);
   if (status < 0)
     skip_over(FUNCTION,';');
   else
    { add_pin(FUNCTION,status,name);
      break;
    }
 } /* FOR */
if (name[0] == '@')
  return(NO);
for (change = NO,fnc_first = YES;
     !feof(fnc_file) && name[0] != '@';
     fscanf(fnc_file,"%s",name))
 { switch(check_token(name,PARSE,FUNCTION))
    { case FUNCTION
        : fnc_send(name);
          change = YES;
          break;
      case SKIP  /* FIX is not looking for this output signal name */
        : skip_over(FUNCTION,';');
          break;
      default
        : inform("fnc_arrange()",STOP);
          break;
    } /* SWITCH */
 } /* FOR */
fprintf(out_file,";");
return(change);
} /* FNC_ARRANGE */
/* - - - - - - - - - */
fix_function()
{
if (fnc_file != NULL)
  fclose(fnc_file);
fnc_file = fopen(FNC_FILE,"r");
if (fnc_file == NULL)
  inform("Cannot reopen FNC_FILE",STOP);
for (;fnc_arrange() == YES;)
 { if (fnc_file != NULL)
     fclose(fnc_file);
   fnc_file = fopen(FNC_FILE,"r");
   if (fnc_file == NULL)
     inform("Cannot reopen FNC_FILE",STOP);
 } /* FOR */
} /* FIX_FUNCTION */
/* - - - - - - - - - */
/*
checks if the name taken contains a pin number that is the current pin
being processed; if not, skip it. If it is, then add the product terms
to the output file. Check also if the name is a function, in which case
return the keyword FUNCTION.
*/
check_token(iconx,action,a_file_type)
char iconx[];
int action,a_file_type;
{ char chr_nmbr[8];
  int pin_nmbr,i,flag_function;
  struct fixtype *xptr;

if (iconx[0] == '\0')
  inform("CHECK_TOKEN arg empty",STOP);
for (i = 0,flag_function = NO; iconx[i] != '.' && iconx[i] != '\0'; ++i)
  chr_nmbr[i] = iconx[i];
if (iconx[i] == '.')
  flag_function = YES;
chr_nmbr[i] = '\0';
pin_nmbr = (int)atoi(chr_nmbr);
if (action == VALUE)
 { if (a_file_type == FUNCTION)
     xptr = fname;
   else
     xptr = known_pins;
   if (xptr->next == NULL)
     return(pin_nmbr);
   for (; xptr != NULL; xptr = xptr->next)
    { if (a_file_type == OUTPUT)
       { if (xptr->pin == pin_nmbr)
           return(-1);
       }
      else
       { if (!strcmp(xptr->function_name,iconx))
           return(-1);
       } /* ELSE */
    } /* FOR */
   return(pin_nmbr);
 }
if (current_ptr == NULL)
  add_pin(OUTPUT,pin_nmbr,"$$$$");
if (a_file_type == OUTPUT)
 { if (pin_nmbr == current_ptr->pin)
    { if (flag_function == YES)
        return(FUNCTION);
      else
        return(OUTPUT);
    }
   else
     return(SKIP);
 }
else
 { if (!strcmp(iconx,curnt_fname->function_name))
     return(FUNCTION);
   else
     return(SKIP);
 } /* ELSE */
} /* CHECK_TOKEN */
/* - - - - - - - - - */
get_nmbr(a_string)
char a_string[];
{ char tmp[20];
  int i;
for (i = 0;
     a_string[i] != '.' && a_string[i] != '\0';
     ++i)
  tmp[i] = a_string[i];
tmp[i] = '\0';
return((int)atoi(tmp));
} /* GET_NMBR */
/* - - - - - - - - - */
/*
send all the characters in the input file to the file specified by the
FILETYPE until the target character A_TARGET
*/
send(filetype,a_target)
int filetype;
char a_target;
{ int item;

for (item = fgetc(input_file);
     !feof(input_file) && item != '=';
     item = fgetc(input_file))
  ;
for (item = fgetc(input_file);
     !feof(input_file) && item != a_target;
     item = fgetc(input_file))
 { if (filetype == OUTPUT)
     fprintf(stderr,"%c",item);
   switch(filetype)
    { case OUTPUT
       : fprintf(out_file,"%c",item);
         break;
      case FUNCTION
       : fprintf(fnc_file,"%c",item);
         break;
      default
       : inform("Must be OUTPUT/FUNCTION",STOP);
         break;
    } /* SWITCH */
 } /* FOR */
if (feof(input_file))
  inform("Cannot be EOF yet",STOP);
} /* SEND */
/* - - - - - - - - - */
out_name(a_name,pin_type)
char a_name[];
int pin_type;
{ int x;

x = get_nmbr(a_name);
switch(pin_type)
 { case OUTPUT
    : if (first_PT == YES)
        { fprintf(stderr,"\n%d =",x);
          fprintf(out_file,"\n%d =",x);
          first_PT = NO;
        }
      else
        { fprintf(stderr," +\n    ");
          fprintf(out_file," +\n    ");
        }
      send(pin_type,';');
      break;
   case FUNCTION
    : fprintf(fnc_file,"\n%s =",a_name);
      fnc_first = NO;
      send(pin_type,';');
      fprintf(fnc_file,";");
      break;
   default
    : inform("OUT_NAME",STOP);
      break;
 } /* SWITCH */
} /* OUT_NAME */
/* - - - - - - - - - */
/* skip all characters until the char TARGET */
skip_over(which_file,target)
int which_file;
char target;
{ int a_chr;
  FILE *tmp;

if (which_file == FUNCTION)
  tmp = fnc_file;
else
  tmp = input_file;
if (feof(tmp))
  inform("File not generated by PLC 1987",STOP);
for (a_chr = fgetc(tmp);
     a_chr != target && !feof(tmp);
     a_chr = fgetc(tmp))
  ;
if (a_chr != target)
  inform("Not a PLC 1987 file",STOP);
} /* SKIP_OVER */
/* - - - - - - - - - */
