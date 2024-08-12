#include "commands.h"

#define BLUE_MSG "\033[34m"
#define GREEN_PATH "\033[32m"
#define RESET_COLOR "\033[0m"

char *args[10];
int argIndex;
char **argList1;
char **argList2;
char pwd[1024];

int main(int argc , char** argv)
{
	char command[100];
        char *token;
	ssize_t readSize=0;
	const char * shellMsg="Marhaba what do you want:) >";

         while(1) {

        printf("\n");
	/*print path in green*/
	printf("%s%s%s",GREEN_PATH,getcwd(pwd,sizeof(pwd)),RESET_COLOR);
	printf("$:");
	/*print shell msg in blue*/
	printf("%s%s%s",BLUE_MSG,shellMsg,RESET_COLOR);
	fflush(stdout);

	readSize=read(STDIN,command,100);

        command[readSize-1]='\0';

	token=strtok(command," ");

       /*when enter is pressed*/
        while(token==NULL)
      {
        printf("\n");
        /*print path in green*/
        printf("%s%s%s",GREEN_PATH,getcwd(pwd,sizeof(pwd)),RESET_COLOR);
        printf("$:");
        /*print shell msg in blue*/
        printf("%s%s%s",BLUE_MSG,shellMsg,RESET_COLOR);
        fflush(stdout);

        readSize=read(STDIN,command,100);

        command[readSize-1]='\0';

        token=strtok(command," ");

      }
        argIndex=0;
        while(token !=NULL && argIndex<10)
	{     args[argIndex++]=token;
	      token = strtok(NULL," ");
	}
        args[argIndex]=NULL;

	history_log();
/************* check for pipe **********/
	if(pipe_check())
	{
		pipe_command(argList1,argList2);
		free(argList1);
                free(argList2);
	}
/*********** check for redirection **********/	
	else if (redir_check())
	{
		//redir is done
	}
/********** internal echo command *************/
	else if(strcmp(args[0],"myecho")==0)
        {
		echo_command(args);
        }
/********** internal copy command *************/
        else if(strcmp(args[0],"mycp")==0)
       {/*to check if source and target file is passed or not*/
         if (argIndex < 3)
           fprintf(stderr, "Usage: %s <str1> <str2> [<len>]\n", args[0]);
         else
                copy_move_command(args);
       }
/********** internal move command *************/
        else if(strcmp(args[0],"mymv")==0)
       {/*to check if source and target file is passed  or not*/
         if (argIndex < 3)
           fprintf(stderr, "Usage: %s <str1> <str2> [<len>]\n", args[0]);
        else
	      copy_move_command(args);
	
       }

/********* exit command *********/
        else if(strcmp(args[0],"exit")==0)
	{
                exit_command();
	        return 0;
	}

/*********** internal pwd command *********/
         else if(strcmp(args[0],"mypwd")==0)
        {
                pwd_command();
	}
/*********** cd command *********/
         else if(strcmp(args[0],"cd")==0)
        {
	        if(argIndex<2)
			cd_command(NULL);
		else	  
			cd_command(args[1]);
        }

/*********** type command *********/
         else if(strcmp(args[0],"type")==0)
        {
                type_command(args);
        }

/*********** envir command *********/
         else if(strcmp(args[0],"envir")==0)
        {
		envir_command();
        }


/************ help command ********/
         else if(strcmp(args[0],"help")==0)
        {
		help_command();
        }

/************internal free command ********/
         else if(strcmp(args[0],"myfree")==0)
        {
		free_command();
        }

/************internal uptime command ********/
         else if(strcmp(args[0],"myuptime")==0)
        {
		uptime_command();
        }

/************internal phist command ********/
       else if(strcmp(args[0],"phist")==0)
        {
		phist_command();
        }
	  
/*********** not internal command ********/
       else
   	    
          external_command(args);
	  


/* to empty buffer from enterred command*/
	for(int i=0;i<readSize;i++)
	{
		command[i]='\0';
		
	}
}

}
