/* OPR3.C : August 18, 1986 */
#include "plpldef.h"
/* - - - - - - - - */
read_options(prg_action,ACTION)
 /* Read the options available for the program option PRG_ACTION from
    the option file PLPLOPT.TXT, storing the options into the array
    ARG_ARRAY and messages into the array ARG_STATEMENT. */
char prg_action;
int ACTION;
{ char key[NAMELNG],tmp_buf[256];
  int i,b,prg_state,stop_bit;

ptr = fopen(OPTN_FILE,"r");
if (ptr == NULL) {
  if (ACTION == YES) {
  /* stop at this point in the menu program, get options and
     immediately execute the program */
    fprintf(stderr,"\n=> Options list file not available.");
    fprintf(stderr,"\n\tType program name with options.");
    fprintf(stderr,"\n\tRefer to manual for details.");
    fprintf(stderr,"\n\nEnter: ");
  } /* IF */
  return(STOP);
} /* IF */
sprintf(key,"#%c",prg_action);
for (i = -1,prg_state = STOP;;)
 { fscanf(ptr,"%s",tmp_buf);
   if (feof(ptr))
     { fprintf(stderr,"Invalid option");
       if (ptr != NULL)
         fclose(ptr);
       if (ACTION == NO)
         return(prg_state);
       return(NO);
     }
   if (!strncmp(tmp_buf,key,2)) /* find the key #x */
     { for (;;)
        { tmp_buf[0] = fgetc(ptr);
          switch(tmp_buf[0])
           { case '~'    /* read until '~' */
                : if (ptr != NULL)
                    fclose(ptr);
                  if (ACTION == NO)
                    return(prg_state);
                  return(YES);
             case '-'
                : for (++i,b = 0,stop_bit = NO;
                       !feof(ptr)&& stop_bit == NO;) {
                    chr = fgetc(ptr);
                    if (isalpha(chr)) {
                      if (isupper(chr))
                        chr = tolower(chr);
                    } /* IF */
                    switch(chr) {
                      case '.'
                        : arg_array[i][b] = '\0';
                          stop_bit = YES;
                          break;
                      case '-'  /* this is a necessary flag */
                        : arg_array[i][b++] = chr;
                          prg_state = YES;
                          break;
                      default
                        : arg_array[i][b++] = chr;
                          break;
                    } /* SWITCH */
                  } /* FOR */
                  break;
             case '='
                : for (b = 0,stop_bit = NO;
                       !feof(ptr) && stop_bit == NO;) {
                    chr = fgetc(ptr);
                    switch(chr) {
                      case '!'
                        : arg_statement[i][b] = '\0';
                          stop_bit = YES;
                          break;
                      default
                        : arg_statement[i][b++] = chr;
                          break;
                    } /* SWITCH */
                  } /* FOR */
                  break;
           } /* SWITCH */
          if (feof(ptr))
           { if (ptr != NULL)
               fclose(ptr);
             if (ACTION == NO)
               return(prg_state);
             return(NO);
           }
        } /* FOR */
     } /* IF strncmp */
 } /* FOR */
} /* READ_OPTIONS */
/* - - - - - - - - */
valid_option(an_opt,a1_file)
 /* check if AN_OPT exists in the array ARG_ARRAY; if it does, then
    get the corresponding arguments and store it in ARG_ARRAY */
char an_opt,a1_file[];
{ int indx,b;
  char num_args[NAMELNG],buf[NAMELNG];
for (indx = 0; indx < MAXARG; ++indx)
 { if (arg_array[indx][0] == an_opt)
    { arg_buffer[indx][0] = '\0';  /* clear the line of any options */
      num_args[0] = arg_array[indx][2];
      if (num_args[0] == '-')
        num_args[0] = arg_array[indx][3];
      num_args[1] = '\0';
      if (atoi(num_args) == 0)
        strcpy(arg_buffer[indx],"#");
      else
        fprintf(stderr,"\n%s\n\n",arg_statement[indx]);  /* print arg prompt */
      for (b = 0; b < atoi(num_args); ++b) {
        fprintf(stderr,"Enter [arg %d]: ",b+1);
        get_string(buf,NAMELNG,NO);
        strcpy(a1_file,buf);
        if (buf[0] == '\0') {
          arg_buffer[indx][0] = '\0';
          return(YES);
        } /* IF */
        strcat(arg_buffer[indx],buf);
        strcat(arg_buffer[indx]," ");
      } /* FOR */
      return(YES);
    } /* IF */
 } /* FOR */
return(NO);  /* option did not exist */
} /* VALID_OPTION */
/* - - - - - - - - */
option_display()
{ int i,b,x;
  char a_buf[MAXLNG];
scr_clr();
fprintf(stderr,"\nAvailable Options\n");
for (i = 0; i < MAXARG; ++i)
 { if (arg_array[i][0] == '\0')
     break;
   else
    { fprintf(stderr,"\n  [%c]   ",arg_array[i][0]);
      for (x = 0,b = 2; arg_array[i][b] != '\0' && b < MAXLNG;++b) {
        if (arg_array[i][b] == '-')
          ++x;
        else
          a_buf[b-x-2] = arg_array[i][b];
      } /* FOR */
      a_buf[b-x-2] = '\0';
      fprintf(stderr,"%s",a_buf);
    }
 } /* FOR */
fprintf(stderr,"\n  <CR> to return to previous menu");
} /* OPTION_DISPLAY */
/* - - - - - - - - */
menu_3_level(action,result_file)
char action,result_file[];
{ char chr_3;

switch(read_options(action,YES))
 { case STOP: get_string(long_string,MAXLNG,YES);
              return(STOP);
   case NO  : return(NO);
   case YES : option_display();
              break;
 } /* SWITCH */
fprintf(stderr,"\n\n");
for (;;)
 { fprintf(stderr,"Enter: ");
   chr_3 = char_get();
   if (chr_3 == '\n')
     return(YES);
   if (valid_option(chr_3,result_file) == NO)
     fprintf(stderr,"\nInvalid option [%c]\n",chr_3);
 } /* FOR */
} /* MENU_3_LEVEL */
/* - - - - - - - - */
