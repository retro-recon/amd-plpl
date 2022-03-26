/* OPR2.C */
#include "plpldef.h"
/* - - - - - - - - */
findstr(str,target) /* similar to STRCSPN VMS C function */
char str[];
char target;
{ int i;
for (i = 0 ; i <= strlen(str) && str[i] != '\0' ; ++i)
 if (str[i] == target)
    return(i+1);
return(-1);
} /* FINDSTR */
/* - - - - - - - - */
out_filename(dest,src,ext)  /* put name into DEST formed by appending EXT */
char dest[],src[],ext[];    /* to SRC */
{ int num;
dest[0] = '\0';
strcat(dest,src);
num = findstr(src,'.');
if (num == -1)
  { strcat(dest,".");
    strcat(dest,ext);
  }
else
  { strncpy(dest,src,num);
    dest[num] = '\0';
    strcat(dest,ext);
  }
} /* OUT_FILENAME */
/* - - - - - - - - */
opt_arg_attach(list_x,the_option,the_arg)
/* attach the OPTION and the ARGUMENT to the LIST_X in the following format
   [-THE_OPTION THE_ARG]. Note: THE_ARG can be NULL */
char list_x[],the_option,the_arg[];
{ char tmp[NAMELNG];
tmp[0] = ' ';
tmp[1] = '-';
tmp[2] = the_option;
tmp[3] = ' ';
tmp[4] = '\0';
strcat(list_x,tmp);  /* attach the option */
strcat(list_x,the_arg); /* attach the argument */
} /* OPT_ARG_ATTACH */
/* - - - - - - - - */
attach_arguments(comnd,list_commands,src_attach,dest_attach)
/* Attach the arguments from the ARG_ARRAY
   to the list LIST_COMMANDS for the COMND */
char comnd,list_commands[],src_attach[],dest_attach[];
{ int icnt;

for (icnt = 0; arg_array[icnt][0] != '\0' && icnt < MAXARG; ++icnt)
 { if (arg_buffer[icnt][0] != '\0' && arg_buffer[icnt][0] != '#')
     opt_arg_attach(list_commands,arg_array[icnt][0],arg_buffer[icnt]);
   else
    { if (((arg_array[icnt][2] == '0') || (arg_array[icnt][3] == '0')) &&
           (arg_buffer[icnt][0] == '#'))
        opt_arg_attach(list_commands,arg_array[icnt][0]," ");  /* no arg */
    } /* ELSE */
 } /* IF */
} /* ATTACH_ARGUMENTS */
/* - - - - - - - - */
clean_arg_buffers()
/* this function is called whenever a new command is called or when the
   command is executed */
{ int i;
for (i = 0; i < MAXARG; ++i)  /* clear the argument array buffers */
 { arg_array[i][0] = '\0';
   arg_statement[i][0] = '\0';
   arg_buffer[i][0] = '\0';
 } /* FOR */
} /* CLEAN_ARG_BUFFERS */
/* - - - - - - - - */
sys_call(prg_args)
/* call the program with the following program arguments */
char prg_args[];
{
fprintf(stderr,"Command line [%s]\n",prg_args);
system(prg_args);
} /* SYS_CALL */
/* - - - - - - - - */
execute_command(comnd,file_src,file_dest)
char comnd,file_src[],file_dest[];
{
scr_clr();
long_string[0] = '\0';
switch(comnd)
 { case 'c' : strcat(long_string,"plc");
              fprintf(stderr,"\nCompiling\n\n");
              break;
   case 'o' : strcat(long_string,"optimize");
              fprintf(stderr,"\nRunning Optimizer\n\n");
              break;
   case 'j' : strcat(long_string,"jm");
              fprintf(stderr,"\nGenerating Device Map\n\n");
              break;
   case 't' : strcat(long_string,"testv");
              fprintf(stderr,"\nGenerating Test Vectors\n\n");
              break;
   case 's' : strcat(long_string,"sim");
              fprintf(stderr,"\nSimulating\n\n");
              break;
   default  : fprintf(stderr,"\nProgram Error in EXECUTE COMMAND Call");
#ifdef unix
              endwin();
#endif
              exit(1);
 } /* SWITCH */
attach_arguments(comnd,long_string,file_src,file_dest);
sys_call(long_string);
} /* EXECUTE_COMMAND */
/* - - - - - - - - */
show_filenames(module,infile,outfile)
char module,infile[],outfile[];
{ int dd;
scr_clr();
fprintf(stderr,"\nInput File: ");
for (dd = 0; dd < MAXARG; ++dd) {
  if (arg_array[dd][0] == 'i') {
    if (arg_buffer[dd][0] == '\0')
      fprintf(stderr,"<No input file specified>\n");
    else
      fprintf(stderr,"%s\n",arg_buffer[dd]);
    break;
  } /* IF */
} /* FOR */
if (dd >= MAXARG)
  fprintf(stderr,"%s\n",infile);
switch(module)
 { case 'c':
     fprintf(stderr,"Compiler");
     break;
   case 'o':
     fprintf(stderr,"Optimizer");
     break;
   case 'j':
     fprintf(stderr,"Device Map");
     break;
   case 't':
     fprintf(stderr,"Test Vector");
     break;
   case 's':
     fprintf(stderr,"Simulator");
     break;
   default :
     fprintf(stderr,"Program Error in SHOW FILENAMES");
#ifdef unix  /* CURSES.H in UNIX */
     endwin();
#endif
     exit(1);
 } /* SWITCH */
fprintf(stderr," Output Destination: ");
for (dd = 0; dd < MAXARG; ++dd) {
  if (arg_array[dd][0] == 'o') {
    if (arg_buffer[dd][0] == '\0')
      fprintf(stderr,"<No output file specified>\n");
    else
      fprintf(stderr,"%s\n",arg_buffer[dd]);
    break;
  } /* IF */
} /* FOR */
if (dd >= MAXARG) {
  if (outfile[0] == '\0')
    fprintf(stderr,"<No output file specified>\n");
  else
    fprintf(stderr,"%s\n",outfile);
} /* IF */
} /* SHOW_FILENAMES */
/* - - - - - - - - */
check_outfile(act,an_outfile)
/* Check that the output file does not already exist. If it does, then query
   the user: should the file be deleted, a new output file specified, or
   should the whole operation be cancelled */
char act,an_outfile[];
{ int status;
  char chr_chk;
for (;;)
 { if (an_outfile[0] == '\0')
     return(NO);
   ptr = fopen(an_outfile,"r");
   if (ptr == NULL)
     return(YES);  /* the output file does not yet exist */
   fclose(ptr);
   fprintf(stderr,"File [%s] already exists\n\n",an_outfile);
   fprintf(stderr,"Type N to specify a new file\n");
   fprintf(stderr,"     D to delete [%s]\n",an_outfile);
   fprintf(stderr,"     <CR> to cancel the command\n\n");
   fprintf(stderr,"Enter: ");
   chr_chk = char_get();
   switch(chr_chk)
    { case 'n'
         : an_outfile[0] = '\0';
           fprintf(stderr,"Specify new destination filename: ");
           get_string(an_outfile,NAMELNG,NO);
           show_filenames(act,src_name,an_outfile);
           return(check_outfile(act,an_outfile));
      case '\n'
         : return(NO);
      case 'd'
         :
#ifdef vms   /* use DELETE in VAX/VMS-C to delete a file */
           status = delete(an_outfile);
#else        /* for UNIX and C86-C systems */
           status = unlink(an_outfile);
#endif
           if (status == -1)
             { fprintf(stderr,"Could not delete [%s]",an_outfile);
               return(NO);
             }
           show_filenames(act,src_name,an_outfile);
           return(YES);
    } /* SWITCH */
 } /* FOR */
} /* CHECK_OUTFILE */
/* - - - - - - - - */
type_2_menu()
{
fprintf(stderr,"\n\nType O <CR> to set options");
fprintf(stderr,"\n     X <CR> to cancel the command");
fprintf(stderr,"\n     <CR> to execute the command");
fprintf(stderr,"\n\nEnter: ");
} /* TYPE_2_MENU */
/*
- - - - - - - - */
level2_options(action,file2,destfile)
char action,file2[],destfile[];
{ char chr_2;
type_2_menu();
for (;;)
 { chr_2 = char_get();
   switch(chr_2)
   { case 'x'   /* cancel the command */
        : return(YES);
     case 'o'  /* return YES to cancel the command, NO to continue */
        : if (menu_3_level(action,destfile) == STOP)
           { sys_call(long_string);
             return(YES);
           }
          show_filenames(action,file2,destfile);
          type_2_menu();
          break;
     case '\n'
        : execute_command(action,file2,destfile);
          return(YES);
     default
        : fprintf(stderr,"\nType one of the above options: ");
   } /* SWITCH */
 } /* FOR */
} /* LEVEL2_OPTIONS */
/* - - - - - - - - */
menu_2_level(oper,tmpfile,out2)
char oper,tmpfile[],out2[];
{ int cnt;
switch(oper)
 { case 'c' :
      out_filename(out2,tmpfile,"int");
      break ;
   case 'o' :
      out_filename(out2,tmpfile,"opt");
      break ;
   case 'j' :
      out_filename(out2,tmpfile,"jed");
      break ;
   case 't' :
      out_filename(out2,tmpfile,"tst");
      break ;
   case 's' :
      out_filename(out2,tmpfile,"sim");
      break ;
   default  :
      fprintf(stderr,"MENU_2_LEVEL Error");
#ifdef unix
      endwin();
#endif
      exit(1);
 } /* SWITCH */
for (;;)  /* wait in this infinite loop */
 { for (cnt = 0; cnt < MAXARG; ++cnt) {
    if (arg_array[cnt][0] == 'o') {
      strcpy(arg_buffer[cnt],out2);
      break;
    } /* IF */
   } /* FOR */
   show_filenames(oper,tmpfile,out2);
   switch(check_outfile(oper,out2))
    { case YES : break;
      case NO  : return;
    } /* SWITCH */
   switch(level2_options(oper,tmpfile,out2))
    { case YES : return;
      case NO  : break;
    } /* SWITCH */
 } /* FOR (;;) */
} /* MENU_2_LEVEL */
/* - - - - - - - - */
check_default_file_name(chk_file)
/* If CHK_FILE is empty, then check the default file SRC_NAME. If this file
   is not empty, then copy this name into CHK_FILE. If SRC_NAME is also empty,
   then there is no default file and the function returns NO. Note that the
   only way to get a default file is to compile a file */
char chk_file[];
{
if (chk_file[0] == '\0')
  return(NO);
ptr = fopen(chk_file,"r");  /* check if the file exists */
if (ptr == NULL)
 { fprintf(stderr,"\n[%s] does not exist",chk_file);
   chk_file[0] = '\0';
   return(NO);
 }
fclose(ptr);
ptr = NULL;
return(YES);
} /* CHECK_DEFAULT_FILE_NAME */
/* - - - - - - - - */
/* this is common to the COMPILE and TESTV module */
common(a_command)
char a_command;
{
get_file(a_command,tmp1_name);
if (tmp1_name[0] == '\0')
  return;
else
  strcpy(src_name,tmp1_name);
if (check_default_file_name(tmp1_name) == NO)
 { src_name[0] = '\0';  /* this file does not exist */
   tmp1_name[0] = '\0';
   return;
 }
menu_2_level(a_command,tmp1_name,tmp2_name);
tmp1_name[0] = '\0';
tmp2_name[0] = '\0';
} /* COMMON */
/* - - - - - - - - */
compile()
{
common('c');
} /* COMPILE */
/* - - - - - - - - */
testv()
{
common('t');
} /* TESTV */
/* - - - - - - - - */
optimize()
{
get_file('o',tmp1_name);
if (tmp1_name[0] == '\0')
  return;
if (check_default_file_name(tmp1_name) == NO)
  return;
menu_2_level('o',tmp1_name,tmp2_name);
} /* OPTIMIZE */
/* - - - - - - - - */
fuse_map_eq_list()
{
get_file('j',tmp1_name);
if (tmp1_name[0] == '\0')
  return;
if (check_default_file_name(tmp1_name) == NO)
  return;
menu_2_level('j',tmp1_name,tmp2_name);
} /* FUSE_MAP */
/* - - - - - - - - */
simulate()
{
get_file('s',tmp1_name);
if (tmp1_name[0] == '\0')
  return;
if (check_default_file_name(tmp1_name) == NO)
  return;
menu_2_level('s',tmp1_name,tmp2_name);
} /* SIMULATE */
/* - - - - - - - - */
