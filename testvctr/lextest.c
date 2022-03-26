/* LEXTEST.C  August 29, 1986
   Lexical Analyzer returns tokens from a file */
#include "testdef.h"
/* - - - - - - - - - - - - - - - */
write_err(degree,dest,msgx)
int degree;
FILE *dest;
char msgx[];
{ int i;

switch(degree) {
  case NOTICE:
    fprintf(dest,"\n=> Note: ");
    break;
  case WARNING:
    fprintf(dest,"\n=> Warning: ");
    break;
  default:
    fprintf(dest,"\n=> Error %d: ",errors);
    break;
} /* SWITCH */
fprintf(dest,"%s",msgx);
fprintf(dest,"\nLine #%d:\n%s\n",line_number,source_line);
if (screen_position <= strlen(source_line)+2)
 { for (i = 0 ; i <= screen_position-2 ; ++i)
   { if (source_line[i] != '\t')
    fprintf(dest," ");
     else
    fprintf(dest,"\t");
    }
 }
else
 { fprintf(dest,"\n<Msg Err>");
   return;
 }
fprintf(dest,"^\n");
} /* WRITE_ERR */
/* - - - - - - - - - - - - - - - */
err(severity,msg,got_this,alt1,alt2)
  /* print out the error message and pinpoint the error on the screen.
     Error messages are written to any output file opened. Rudimentary
     error recovery also done: the token_type obtained (GOT_THIS)
     is compared with the next item to be taken. If GOT_THIS is the same type
     as alternates ALT1 or ALT2, then use the same token next time by setting
     GET_NEW_TOKEN to NO. If not, then just return */
int severity;
char *msg;
int got_this,alt1,alt2;
{
if (severity == WARNING || severity == FATAL)
  ++errors;
if (out_file != NULL && severity != NOTICE)
  write_err(severity,out_file,msg);
write_err(severity,stderr,msg);
if (severity == FATAL)
  exit(BAD_EXIT);
if (got_this == alt1 || got_this == alt2)
  get_new_token = NO;
return;
} /* end of ERR */
/* - - - - - - - - - - - - - - - */
get_line()
{ int i,lexi_char;

++line_number;
for ( i = 0, lexi_char = fgetc(input_file);
      i < 256 && lexi_char != '\n' && lexi_char != EOF;
      ++i, lexi_char = fgetc(input_file))
  source_line[i] = lexi_char;
source_line[i] = ' ';
source_line[i+1] = '\0';
if (lexi_char == EOF)
  end_of_file = YES;
screen_position = 0;
lexi_i = 0;
} /* end of GET_LINE */
/* - - - - - - - - - - - - - - - */
char get_char()  /* return a character */
{
if (end_of_file == YES && lexi_i >= strlen(source_line)+1)
  { if (end_found == YES)
      { ++screen_position;
        err(FATAL,"Unexpected EOF",0,0,0);
      }
    else
      { end_found = YES;
        return(' ');
      }
  }
if (lexi_i >= strlen(source_line) + 1)
 { get_line();
   return(' ');
 }
return(source_line[lexi_i++]);
} /* end of GET_CHAR */
/* - - - - - - - - - - - - - - - */
check_validity(valid_digits,comparand)
char valid_digits[];
char comparand[];
{ int i,j;
(comparand[0] == '#') ? (i=2) : (i=0);
for (; i != strlen(comparand) - 1; ++i)
  { for (j=0; j != strlen(valid_digits); ++j)
     { if (comparand[i] == valid_digits[j])
     break;
     }
    if (j == strlen(valid_digits))
      return(NO);
  }
return(YES);
} /* end of CHECK_VALIDITY */
/* - - - - - - - - - - - - - - - */
work_on_number(a_number)
char a_number[];
{
token_type = NUMBER;
number_type = NO_BASE;
switch (a_number[1])
 { case 'b' :
   case 'B' : if (check_validity("01",a_number) == YES)
        number_type = BINARY;
          else
        err(FATAL,"not a binary number",token_type,0,0);
          break;
   case 'o' :
   case 'O' : if (check_validity("01234567",a_number) == YES)
        number_type = OCTAL;
          else
        err(FATAL,"not an octal number",token_type,0,0);
          break;
   case 'd' :
   case 'D' : if (check_validity("0123456789",a_number) == YES)
        number_type = DECIMAL;
          else
        err(FATAL,"not a decimal number",token_type,0,0);
          break;
   case 'h' :
   case 'H' : if (check_validity("0123456789abcdefABCDEF",a_number) == YES)
        number_type = HEXADECIMAL;
          else
        err(FATAL,"not a hexadecimal number",token_type,0,0);
          break;
   default  : err(FATAL,"not a valid number base",token_type,
                  KEYWORD,VARIABLE);
          break;
 } /* end of SWITCH on last character in number array */
} /* end of WORK_ON_NUMBER */
/* - - - - - - - - - - - - - - - */
skip() /* skip over the rest of the token
          until a space/punct mark seen */
{ char tmp;
while (tmp = get_char())
 { if (isspace(tmp) || (ispunct(tmp) && tmp != '_'))
     break;
 }
} /* SKIP */
/* - - - - - - - - - - - - - - - */
number_parse(a_number)
char a_number;
{ int i;
  char a_char;
token[0] = a_number;
the_keyword = NOT_A_KEYWORD;
a_char = get_char();
for ( i = 1;
      ((!(isspace(a_char) || ispunct(a_char))) && i < 40);
      ++i, a_char = get_char())
   token[i] = a_char;
if (i >= 40)
  { err(FATAL,"number too large",token_type,VARIABLE,WHITE_SPACE);
    skip();
  }
token[i] = '\0';
if (i == 1 && token[0] == '#')
 { token_type = PUNCT_MARK;
   number_type = NOT_A_NUMBER;
   return;
 }
if ((isspace(a_char) || ispunct(a_char)) && lexi_i > 0)
   --lexi_i;
token_type = NUMBER;
number_type = NOT_A_NUMBER;
switch (token[1])
 { case 'b' : case 'B' : break;
   case 'o' : case 'O' : break;
   case 'd' : case 'D' : break;
   case 'h' : case 'H' : break;
   default  : if (check_validity("0123456789#",token) == YES)
             number_type = DECIMAL;
          else
                { err(FATAL,"not a decimal number",token_type,
                      WHITE_SPACE,VARIABLE);
          number_type = NO_BASE;
                }
          return;
 } /* end of switch */
if (token[0] == '#')
  work_on_number(token);
else
  err(FATAL,"put # for numbers",token_type,VARIABLE,PUNCT_MARK);
} /* end of NUMBER_PARSE */
/* - - - - - - - - - - - - - - - */
comments()
{ int line;
line = line_number;
for (chr = get_char() ; chr != '"'; chr = get_char())
 { screen_position = lexi_i;
   if (end_found == YES)
     { fprintf(stderr,"\n [Close comments in line %d]",line);
       err(FATAL,"Unexpected EOF",0,0,0);
     }
 }
++screen_position;  /* increment SCREEN_POS to cover " symbol */
} /* end of COMMENTS */
/* - - - - - - - - - - - - - - - */
check_keyword(thing_to_check) /* check if this is a keyword */
char thing_to_check[];
{ char temp[80];
  int i;
the_keyword = NOT_A_KEYWORD;
for (i = 0 ; i != strlen(thing_to_check) ; ++i)
 { if (isupper(thing_to_check[i]))
    temp[i] = tolower(thing_to_check[i]);
   else
    temp[i] = thing_to_check[i];
 }
temp[i] = '\0';
if (!strcmp(temp,"device"))
 { the_keyword = DEVICE;
   return(YES);
 }
if (!strcmp(temp,"define"))
 { the_keyword = DEFINE;
   return(YES);
 }
if (!strcmp(temp,"pin"))
 { the_keyword = PIN;
   return(YES);
 }
if (!strcmp(temp,"in"))
 { the_keyword = IN;
   return(YES);
 }
if (!strcmp(temp,"out"))
 { the_keyword = OUT;
   return(YES);
 }
if (!strcmp(temp,"i_o") || !strcmp(temp,"io"))
 { the_keyword = I_O;
   return(YES);
 }
if (!strcmp(temp,"begin"))
 { the_keyword = BEGIN;
   return(YES);
 }
if (!strcmp(temp,"end"))
 { the_keyword = END;
   return(YES);
 }
return(NO);
} /* end of CHECK_KEYWORD */
/* - - - - - - - - - - - - - - - */
lexi()
{ int i;
while (chr = get_char())
 { if (isdigit(chr) || chr == '#')
      { screen_position = lexi_i;
    number_parse(chr);
    return;
      }
   if (isalnum(chr) || chr == '_')     /* read in an alphanumeric token */
      { for (i = 0, screen_position = lexi_i;
        (isalnum(chr) || chr == '_') && i < MAX_NAME-1;
         ++i, chr = get_char())
      token[i] = chr;
    if (i >= MAX_NAME-1)
      { err(FATAL,"name too long, max 28 characters",
                token_type,WHITE_SPACE,PUNCT_MARK);
            skip();
      }
    token[i] = '\0';
    if (!(isalnum(chr) || chr == '_'))
      --lexi_i;  /* return char for future GET_CHAR call by decrementing
                        lexi_i (used to access the line buffer) */
    if (check_keyword(token) == YES)
      token_type = KEYWORD;
    else
      token_type = VARIABLE;
        number_type = NOT_A_NUMBER;
    return;
      } /* end of IF is an alphanumeric character */
   if (ispunct(chr))
     { token_type = PUNCT_MARK;
       the_keyword = NOT_A_KEYWORD;
       number_type = NOT_A_NUMBER;
       switch(chr)
    { case ' ' : token_type = WHITE_SPACE;
             token[0] = '\0';
             return;
      case '"' : last_token[0] = '"' ;
                     last_token[1] = '\0';
             comments() ;
                     token[0] = '\0';
             token_type = WHITE_SPACE;
             return;
      default  : screen_position = lexi_i;
             break;
    } /* end of SWITCH */
       token[0] = chr; token[1] = '\0';
       return;
     } /* end of IF is punctuation */
   if (isspace(chr))
     { switch (chr)
      { case '\n' :
        case '\r' : screen_position = 0;
                        break;
        case '\t' :
        default   : break;
       } /* end of SWITCH on character */
       token[0] = ' ';
       token[1] = '\0';
       the_keyword = NOT_A_KEYWORD;
       token_type = WHITE_SPACE;
       number_type = NOT_A_NUMBER;
       return;
      }
  } /* end of WHILE character != END */
} /* end of LEXI */
/* - - - - - - - - - - - - - - - */
get_token(item)
char item[];
{ int i;
for (lexi() ;; lexi())
  { if (token[0] != ' ' && token[0] != '\0')
      { for (i = 0 ; i != strlen(token) ; ++i)
     { if (isupper(token[i]))
          item[i] = tolower(token[i]);
       else
          item[i] = token[i];
     }
    item[strlen(token)] = '\0';
    strcpy(last_token,item);
    return;
      }
  }
} /* end of GET_TOKEN */
/* - - - - - - - - - - - - - - - */
lex(jea) /* lex returns a token through JEA */
char jea[];
{
if (get_new_token == YES)
  get_token(jea);
else
  get_new_token = YES;
} /* LEX */
/* - - - - - - - - - - - - - - - */
