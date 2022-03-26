/* Operations Processor for PIZZA Project : Created March 26, 1986
   Consists of files OPR1.C, OPR2.C and OPR3.C
Changes:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define MENU_MAIN 1
#include "plpldef.h"
/* - - - - - - - - */
#ifdef unix   /* UNIX-C systems have CLEAR() in CURSES.H */
scr_clr()
{
clear();
refresh();
}
#else
scr_clr()
{
crtclr();  /* clear function for PC in SCREEN.H : clear screen */
} /* CLEAR */
#endif
/* - - - - - - - - */
main_screen() /* clear screen and show main menu */
{
#ifdef unix
initscr();  /* initialize screen for UNIX CURSES.H calls */
#endif
scr_clr();
fprintf(stderr,"PLPL V2.2 1987\t\t\tMain Menu\t\tRelease A.01\n\n");
fprintf(stderr,"Operations : C : Logic Compiler      ");
fprintf(stderr,"System Operations : R : Run OS Commands\n");
fprintf(stderr,"             O : Optimizer           ");
fprintf(stderr,"                    E : Exit to OS\n");
fprintf(stderr,"             J : JEDEC Output/List Equations");
fprintf(stderr,"             H : Help\n");
fprintf(stderr,"             T : Test Vector Generator\n");
fprintf(stderr,"             S : Simulator\n\n");
} /* end of CLEAR_SCREEN */
/* - - - - - - - - */
buf_clear(tmp1) /* clear the input buffer : until a <CR> is seen */
char tmp1;
{
for (;tmp1 != '\n';)
  tmp1 = getchar();
} /* BUF_CLEAR */
/* - - - - - - - - */
char_get()  /* get the first character from stdin terminated by '\n' */
{ char tmp;
tmp = getchar();
if (isupper(tmp))  /* make it upper case */
  tmp = tolower(tmp);
buf_clear(tmp);   /* disregard the rest of the characters */
return(tmp);
} /* CHAR_GET */
/* - - - - - - - - */
get_string(a_file,length,ADD_BLANKS)
/* get a string up to the length of LENGTH; if ADD_BLANKS = YES then
   include blanks also */
char a_file[];
int length,ADD_BLANKS;
{ char tmp;
  int i;
for (i=0,tmp = 'x'; i < length && tmp != '\n'; ++i)
 { tmp = getchar();
   switch(tmp)
    { case '\n'
         : a_file[i] = '\0';
           break;
      case ' ' :
      case '\t': if (ADD_BLANKS == YES)
                   a_file[i] = tmp;
                 else
                  { a_file[i] = '\0';
                    buf_clear(tmp);
                    tmp = '\n';
                    break;
                  }
      default
         : a_file[i] = tmp;
           break;
    } /* SWITCH */
 } /* FOR */
if (tmp != '\n')
  buf_clear('x');
} /* GET_STRING */
/* - - - - - - - - */
get_file(operation,a_file)
char operation;
char a_file[];
{ char tmpstring[80];
  int g_state,opt_count;

clean_arg_buffers();
switch(operation)
 { case 'c':
   case 'o':
   case 't':
   case 's':
   case 'j'
     : g_state = read_options(operation,NO);
       switch(g_state) {
         case STOP
           : break;
         case YES
           : for (opt_count = 0;
                  arg_array[opt_count][0] != '\0';
                  ++opt_count) {
               if (arg_array[opt_count][2] == '-')
                 valid_option(arg_array[opt_count][0],a_file);
             } /* FOR */
             return;
         default
           : fprintf(stderr,"\n!");
             break;
       } /* SWITCH */
/* do next 2 statements if the $PLPLOPT.TXT file is in error */
       fprintf(stderr,"\nSpecify File: ");
       get_string(a_file,NAMELNG,NO);
       break;
   case 'h'
     : fprintf(stderr,"\nFor help on a command, type the letter: ");
       get_string(a_file,NAMELNG,NO);
       break;
   default
     : fprintf(stderr,"\nInvalid command");
       break;
 } /* SWITCH */
} /* GET_FILE */
/* - - - - - - - - */
run_os()  /* run os command; will vary with your system */
{
fprintf(stderr,"\nOS command (<CR> return to PLPL): ");
for (get_string(long_string,MAXLNG,YES);
     long_string[0] != '\0';
     get_string(long_string,MAXLNG,YES)) {
 if (long_string[0] != '\0')
   system(long_string);
 fprintf(stderr,"System (<CR> to return)> ");
} /* FOR */
} /* RUN_OS */
/* - - - - - - - - */
help() /* DOS specific command */
{ char tmp_strng[MAXLNG+1];
  char key_strng[MAXLNG+1];
  FILE *tmp;
  int line_count;

tmp_strng[0] = '\0';
tmp = NULL;
tmp = fopen(HELP_FILE,"r");
if (tmp == NULL)
#ifdef unix
 { fprintf(stderr,"\n\tNo help file in current directory");
   return;
 }
#else
 { fprintf(stderr,"\n=> No help file on disk; specify drive: ");
   chr = char_get();
   sprintf(tmp_strng,"%c:%s",chr,"plplhelp.txt");
   tmp = fopen(tmp_strng,"r");  /* second chance to open a file */
   if (tmp == NULL)
     { fprintf(stderr,"\nNo help file on drive %c:",chr);
       return;
     }
 }
#endif
fprintf(stderr,"\nHelp on what command: ");
chr = char_get();
if (isalpha(chr)) {
  if (isupper(chr))
    chr = tolower(chr);
}
switch(chr) {
  case 'c':
  case 'o':
  case 'j':
  case 't':
  case 's':
  case 'r':
  case 'h':
  case 'e':
    break;
  default  : return;
} /* SWITCH */
sprintf(key_strng,"#%c",chr);
for (;;)
 { fscanf(tmp,"%s",tmp_strng);
   if (feof(tmp))
     { fprintf(stderr,"No help message for this command");
       if (tmp != NULL)
         fclose(tmp);
       return;
     }
   if (!strncmp(tmp_strng,key_strng,2)) { /* find the key #x */
     for (line_count = 0;;) {
       tmp_strng[0] = fgetc(tmp);
       if (tmp_strng[0] == '\n')
         ++line_count;
       if (tmp_strng[0] == '~' || feof(tmp)) /* print until '~' */
        { if (tmp != NULL)
            fclose(tmp);
          return;
        }
       fprintf(stderr,"%c",tmp_strng[0]);
       if (line_count == 20) {
         fprintf(stderr,"\n<CR> to continue, S to stop: ");
         get_string(tmp_strng,MAXLNG,YES);
         switch(tmp_strng[0]) {
           case 'S' :
           case 's' :
              if (tmp != NULL)
                fclose(tmp);
              return;
           case '\n':
           default
             : line_count = 0;
               break;
         } /* SWITCH */
       } /* IF line_count == 20 */
     } /* FOR */
   } /* IF strncmp */
 } /* FOR */
} /* HELP */
/* - - - - - - - - */
tmp_pause()  /* user hits a key to continue */
{ char chrtmp;
fprintf(stderr,"\nHit return to continue ...");
for (chrtmp = getchar(); chrtmp != '\n';chrtmp = getchar())
  ;
} /* PAUSE */
/* - - - - - - - - */
init_var()  /* initialize some variables */
{ int i;
src_name[0] = '\0';
tmp1_name[0] = '\0';
tmp2_name[0] = '\0';
tmp3_name[0] = '\0';
long_string[0] = '\0';
arg_array[0][0] = '\0';
ptr = NULL;
for (i = 0; i < MAXARG; ++i)
 { arg_buffer[i][0] = '\0';
   arg_statement[i][0] = '\0';
   arg_array[i][0] = '\0';
 } /* FOR */
} /* INIT_VAR */
/* - - - - - - - - */
main()
{
init_var();
for (chr = ' ';;) {
  if (chr != '\n')
    main_screen();
  fprintf(stderr,"Enter Command: ");
  chr = char_get();
  switch(chr) {
  case 'c' :
      compile();
      break;
   case 'o' :
      optimize();
      break;
   case 'j' :
      fuse_map_eq_list();
      break;
   case 't' :
      testv();
      break;
   case 's' :
      simulate();
      break;
   case 'r' :
      run_os();
      break;
   case 'h' :
      help();
      break;
   case 'e' :
#ifdef unix
      endwin();  /* used for UNIX-C systems with CURSES.H */
#endif
      exit(0);
   case '\n':
      break;
   default  :
      fprintf(stderr,"\n=> Unknown command [%c]",chr);
      break ;
  } /* SWITCH */
  switch(chr) {
  case '\n':
  case 'r'
    : break;
  default
    : tmp_pause();
      break;
  } /* SWITCH */
} /* FOR */
} /* MAIN */
