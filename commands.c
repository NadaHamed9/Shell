#include "commands.h"
char **argList; 
char *hist[1024];
long int histIndex=0;



/******* exit command function**********/
void exit_command ()
{
          printf("GoodBye<3\n");
}
/***********internal pwd command function ********/
void pwd_command()
{
          char buff[100];
          getcwd(buff, 100);
          printf("\n");
          printf("%s\n",buff);
}
/********* help command function*******/
void help_command()
{
          printf("\n");
          printf("Supported internal commands are:\n");
          printf("1- mycp:Copy file to another file.\n");
          printf("2- mymv:Move file to another place.\n");
          printf("3- mypwd:Print working directory.\n");
          printf("4- myecho:Print a user input string on stdout\n");
          printf("5- exit:Terminate the shell.\n");
	  printf("6- cd:change the current directory.\n");
	  printf("7- type:return the type of the command.\n");
	  printf("8- envir:print all enviroment variables.\n");
	  printf("9- myfree:print Mem and Swap available and used and total sizes.\n");
	  printf("10- myuptime:print uptime sys and time idle.\n");
	  printf("11- phist:print history of last 10 commands.\n");

}
/********internal echo command function********/
void echo_command(char *args[])
{      /*remove'"' if it exist in first string to echo*/
	if (args[1]!=NULL && args[1][0]=='\"'){
		memmove(args[1],args[1]+1,strlen(args[1]));
	}
	
	int lastIndex=0;
	while(args[lastIndex+1]!=NULL){
		lastIndex++;
	}
	if (args[lastIndex]!=NULL){
		size_t len=strlen(args[lastIndex]);
		 /*remove'"' if it exist in first string to echo*/
		if(len>0 &&args[lastIndex][len-1]=='\"'){
			args[lastIndex][len-1]='\0';
		}
	}
         printf("\n");
        for (int i=1;args[i]!=NULL;i++)
       { 
          printf("%s ",args[i]);
       }
}

/*********internal copy and move command function********/
void copy_move_command(char*args[])
{
    /*if user didnt give filename of targetpath use file name of src path*/
    char*src_filename=basename(strdup(args[1]));

    struct stat dest_stat;

    int is_dir=(stat(args[2],&dest_stat)==0 && S_ISDIR(dest_stat.st_mode));
    int ends_with_sepr=args[2][strlen(args[2])-1]=='/';

    char final_dest[1024];

   /***construct final target path***/

    if(is_dir || ends_with_sepr)
    /*put file name in src path in target path*/
    snprintf(final_dest, sizeof(final_dest),"%s/%s",args[2],src_filename);

    else/*leave target path as it is*/
    snprintf(final_dest, sizeof(final_dest),"%s",args[2]);

     /*to check if source path is the same as target path*/
     if (strcmp(args[1],args[2])==0) 
           fprintf(stderr, "Target file is already exists\n");
           
	   else{

       int  fd_in =open(args[1], O_RDONLY);

       /* to check it got source file path */
       if (fd_in<0)
       perror("open error in source file:");
       else{
       int fd_out =open(final_dest, O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);

       /* to check it got target file path*/
       if (fd_out<0)
       perror("open error in dest file:");
       else{
       char buff[100];
       int size =0;

        do
        {
                size = read(fd_in,buff,100);
                write(fd_out, buff, size);


        } while(size==100);
        close (fd_in);
        close (fd_out);

       /* if its move command delete file in src address*/
       if(strcmp(args[0],"mymv")==0)
        {
          if(remove(args[1])==0)

	  printf("moved successfully:) \n");
        }   
}}}}

/***************** cd command function*************/
void cd_command(const char*path)
{
      
	static char prevDir[1024]="";
	char currDir[1024];
	char newPath[1024];

       /*get current directory*/
	if(getcwd(currDir,sizeof(currDir))==NULL)
	{
		perror("getcwd");
		return;
	}
	/*check if path is empty , put it default to home directory*/
        if(path==NULL || strcmp(path,"")==0 )
		path="~";

	/*change to prev dir*/
	if(path!=NULL && strcmp(path,"-")==0)
	{       
		if(prevDir[0]=='\0')
		{
			fprintf(stderr,"cd: oldpwd not set\n");
		}
		else if(chdir(prevDir)==0){
//                printf("directory changed to prev dir %s\n",prevDir);
		/*update prev dir to the current dir*/
		strncpy(prevDir,currDir,sizeof(prevDir));
		/*store it to global var to appear in pwd function and shell message*/
		strncpy(pwd,currDir,sizeof(pwd));
		}
                else
                perror("chdir");

	}
        /*if path have ~*/
	else if(path[0]=='~' )
        {
		struct passwd *pw=getpwuid(getuid());
		const char *homeDir=pw->pw_dir;
                /*path is only ~ so change to home dir*/
		if(path[1]=='\0'){

                if(chdir(homeDir)==0){
//                printf("directory changed to home dir %s\n",homeDir);
                /*update prev dir to the current dir*/
                strncpy(prevDir,currDir,sizeof(prevDir));
                }
                else
                perror("chdir");
                }

               else{/*path have ~ plus path to new dir*/
	       snprintf(newPath,sizeof(newPath),"%s%s",homeDir,path+1);
               path=newPath;
		       
               if(chdir(path)==0){
//		printf("directory changed to %s\n",path);
		strncpy(prevDir,currDir,sizeof(prevDir));

	       }
	       else
		perror("chdir");
        }}
	else {/*if path is not null,~,-*/
		if(chdir(path)==0){
//                printf("directory changed to %s\n",path);
                strncpy(prevDir,currDir,sizeof(prevDir));

               }
               else
                perror("chdir");
	}
}

/**************check if command is external function**************/
int is_external(const char* cmd)
{

	char *path=getenv("PATH");
	/*check if path enviroment var not set*/
	if(path==NULL)
	return 0;
	
	char *path_cp=strdup(path);
	char *dir=strtok(path_cp,":");
       
	while(dir){
		char fullPath[1024];
		snprintf(fullPath,sizeof(fullPath),"%s/%s",dir,cmd);
		if(access(fullPath,X_OK)==0)
		{/*this command found in path means its external*/
			free(path_cp);
			return 1;
		}

		dir =strtok(NULL,":");
	}
	/*command not found in enviroment so its unsupported*/
	free(path_cp);
	return 0;
	}

/************** type command function ***********/
void type_command(char *args[])
{ /*if user enterred type without command */

	if(args[1]==NULL)
	   fprintf(stderr, "please enter it in form type (command)\n");
        
	else{
        int flag=0;
        char *internal_commands[12]={"myfree","mycp","mymv","myecho","mypwd",
		"help","exit", "type","cd","envir","myuptime","phist"};

        for(int i=0;i<12;i++)
        {      /*internal command*/
                if(strcmp(args[1],internal_commands[i])==0)
                { 
                 flag=1;
                 printf("this is internal command\n");
                }
        }
       /* flag=0 means that maybe external or unsupported command*/        
        if(flag==0)
         {
          int state =is_external(args[1]);
	  if(state==1)
		  printf("this is external command\n");
	  else
		  printf("this is unsupported command\n");
	 }

	}
}

/************ envir command function*************/
void envir_command()
{
/* print enviroment variables */
    int i=0;
    while(environ[i]!= NULL)
    {
	    printf("%s\n",environ[i++]);
    }   
}

/********* child handle function**********/
void child( )
{       /*dynamically allocates memory for argList*/
	argList=malloc((argIndex)*sizeof(char* ));
        /*if theres failure in malloc*/
        if(argList==NULL)
	perror("malloc");
	
	for (int i=0;i<argIndex;i++)
	{/*storing tokenised commands in argList*/
		argList[i]=args[i];
	}

       /*terminate argList by NULL*/	
	argList[argIndex]=NULL;
        execvpe(argList[0], argList,environ);

        perror("execvpe failed");
     /*in case of non supported command or execvpe fails*/
        exit(EXIT_FAILURE);

}

/*********** external command function**********/
void  external_command( )
{
int retpid =fork();

/************parent***********/
        if(retpid>0);
        {
        int status =0;
	/*wait for child process to finish*/
        wait(&status);

        if(WIFEXITED(status))
        {
        int exit_status=WEXITSTATUS(status);
      /*if status =0 means succefully exit other than 0 means fail*/
        if (exit_status!=0)
        {
           printf("sorry thats is wrong/unsupported command\n");
        } //return exit_status;

        }else if (WIFSIGNALED(status))
        {
	   printf("child terminated by signal%d\n",WTERMSIG(status));
        }
        else printf("child terminated abnormally\n");
	free(argList);
        }

/*************child*************/
        if(retpid==0)
        {

	if (strcmp(args[0],"echo")==0){
	/*remove'"' if it exist in first string to echo*/
        if (args[1]!=NULL && args[1][0]=='\"'){
                memmove(args[1],args[1]+1,strlen(args[1]));
        }

        int lastIndex=0;
        while(args[lastIndex+1]!=NULL){
                lastIndex++;
        }
        if (args[lastIndex]!=NULL){
                size_t len=strlen(args[lastIndex]);
                 /*remove'"' if it exist in first string to echo*/
                if(len>0 &&args[lastIndex][len-1]=='\"'){
                        args[lastIndex][len-1]='\0';
                }
        }

		}
        child();
        }
/*in case failure in fork */
        if(retpid<0)
        perror("fork");
}

/****************** pipe command function **********/
void pipe_command(char **argList1,char **argList2)
{
     
        int pipefd[2];
	int retpid1;
	int retpid2;

	if(pipe(pipefd)==-1)
		perror("pipe");

        retpid1=fork();

	if(retpid1==-1)
		perror("fork");

        /*child1 process*/
	if(retpid1==0)
	{
		close(pipefd[0]);
		dup2(pipefd[1],STDOUT_FILENO);
		close(pipefd[1]);

		execvpe(argList1[0],argList1,environ);
		perror("execvpe");
	}
	/*parent1 process*/
	else 
        {
                retpid2=fork();

		if(retpid2==-1)
		perror("fork");

		/*child2 process*/
		if(retpid2==0){

                close(pipefd[1]);
                dup2(pipefd[0],STDIN_FILENO);
                close(pipefd[0]);

                execvpe(argList2[0],argList2,environ);
                perror("execvpe");
		}
        
	else{/*parent2 process*/
		close(pipefd[0]);
		close(pipefd[1]);

		/*wait for child1 to finish*/
		if(waitpid(retpid1,NULL,0)==-1)
		{
			perror("waitpid");
			exit(EXIT_FAILURE);
		}
		/*wait for child2 to finish*/
		if(waitpid(retpid2,NULL,0)==-1)
                {
                        perror("waitpid");
                        exit(EXIT_FAILURE);
                }

	}

}}

/*****************internal free command function*********/
void free_command()
{
	
	char buff[1024];
	ssize_t size=0;
	unsigned long MemTotal=0;
	unsigned long MemFree=0;
	unsigned long Buffers=0;
	unsigned long Cached=0;
	unsigned long SwapTotal=0;
	unsigned long SwapFree=0;

	int  fd_in =open("/proc/meminfo", O_RDONLY);

       /* to check it got source file path */
       if (fd_in<0)
       perror("open error in source file:");
       else{
	       while((size = read(fd_in,buff,sizeof(buff)-1))>0){
                       /*terminates buff by null*/
		       buff[size]='\0';
		       /*to read line by line of buff*/
		       char *line=strtok(buff,"\n");

		       while(line!=NULL)
		       {
			       if(sscanf(line,"MemTotal: %lu KB",&MemTotal)==1)
				       printf("MemTotal: %lu KB\n",MemTotal);
					      
			       if(sscanf(line,"MemFree: %lu KB",&MemFree)==1)
                                       printf("MemFree: %lu KB\n",MemFree);

			       if(sscanf(line,"Buffers: %lu KB",&Buffers)==1)
                                       printf("Buffers: %lu KB\n",Buffers);

                               if(sscanf(line,"Cached: %lu KB",&Cached)==1)
                                       printf("Cached: %lu KB\n",Cached);

                               if(sscanf(line,"SwapTotal: %lu KB",&SwapTotal)==1)
                                       printf("SwapTotal: %lu KB\n",SwapTotal);

                               if(sscanf(line,"SwapFree: %lu KB",&SwapFree)==1)
                                       printf("SwapFree: %lu KB\n",SwapFree);
			       /*new line*/
                               line=strtok(NULL,"\n");
		       }
	       }
       }
       printf("MemUsed: %lu KB\n",MemTotal-MemFree-Buffers-Cached);
       printf("SwapUsed: %lu KB\n",SwapTotal-SwapFree);

       close(fd_in);
}

/**************** internal myuptime command function ***************/
void uptime_command()
{
	char buff[1024];
	ssize_t size=0;
	float uptime_seconds=0.0;
	float idle_seconds=0.0;

	int  fd_in =open("/proc/uptime", O_RDONLY);

       /* to check it got source file path */
       if (fd_in<0)
       perror("open error in source file:");
       else{
	       do{
		       size = read(fd_in,buff,sizeof(buff));
	       }while(size==sizeof(buff));

	       /*gets uptime and idletime from /proc/uptime*/
	       sscanf(buff,"%f %f",&uptime_seconds,&idle_seconds);

	       long uptime=(long) uptime_seconds;
	       long days=uptime/(24*3600);
	       uptime %=(24*3600);

	       long hours=uptime /3600;
	       uptime %=3600;

	       long min= uptime/60;
	       long sec= uptime % 60;


               long idletime=(long) idle_seconds;
               long idle_days=idletime/(24*3600);
               idletime %=(24*3600);

               long idle_hours=idletime /3600;
               idletime %=3600;

               long idle_min= idletime/60;
               long idle_sec= idletime % 60;

               printf("UpTime: %ld Days, %ld Hours, %ld Mins, %ld Sec\n",days,hours,min,sec);

               printf("IdleTime: %ld Days, %ld Hours, %ld Mins, %ld Sec\n",idle_days,idle_hours,
				idle_min,idle_sec);

	       close(fd_in);

       }
       
}

/****************** history_log function******************/
void history_log()
{
	if(histIndex>1023)
                printf("HISTORY IS FULL CAN NOT ADD MORE COMMANDS");

	for(int i=0;i<argIndex;i++){
		
		hist[histIndex]=strdup(args[i]);

		histIndex++;
		}

	hist[histIndex]=NULL;

	histIndex++;
	
}

/************* internal phist command function *********/
void phist_command()
{

	int start=histIndex;
	int last=histIndex;

	if (hist[last]==NULL){

	printf("[         HISTORY OF THE LAST 10 ENTERRED COMMANDS         ]\n");

	for(int i =0;i<11;i++)
	{
		last=last-1;
		while(hist[last] != NULL)
		{
			start=last;
			last--;
		}
	
		while(hist[start]!=NULL)
		{
			printf("%s ",hist[start]);
			start++;
		}
		printf("\n");
	}
	}
	else
		printf("error\n");
}

/*********************** redir command function ***********/
void redir_command(int type,char *file,int location)
{
	if(file==NULL)
	{
		fprintf(stderr,"NO FILE FOR REDIRECTION\n");
		return;
	}

	/*if command is echo to remove "" from atring */
	if( strcmp(args[0],"echo")==0)
	{
		/*remove'"' if it exist in first string to echo*/
		if (args[1]!=NULL && args[1][0]=='\"'){
                memmove(args[1],args[1]+1,strlen(args[1]));
        }
		int lastIndex=0;
		while(args[lastIndex+1]!=NULL){
                lastIndex++;
		}
		if (args[lastIndex]!=NULL){
			size_t len=strlen(args[lastIndex]);
                /*remove'"' if it exist in last string to echo*/
                if(len>0 &&args[lastIndex][len-1]=='\"'){
                        args[lastIndex][len-1]='\0';}
		}
	}

	int saved_fd;
	int oldfd;

	/*remove redirection symbol from args*/
	args[location]=NULL;
	/*remove file from args*/
	args[location+1]=NULL;
	char *cleaned_args[100];
	int j=0;

	 for(int i=0;i<argIndex;i++)
	 {
		 if(args[i]!=NULL)
		 {
			 cleaned_args[j]=args[i];
			 j++;
		 }
	 }
	 cleaned_args[j]=NULL;

	 /*********** check if command is built in ***************/
         if (strcmp(args[0], "myecho") == 0 ||strcmp(args[0], "mycp") == 0 ||strcmp(args[0], "mymv") == 0 ||
             strcmp(args[0], "exit") == 0 ||strcmp(args[0], "mypwd") == 0 ||strcmp(args[0], "cd") == 0 ||
             strcmp(args[0], "type") == 0 ||strcmp(args[0], "envir") == 0 ||strcmp(args[0], "help") == 0 ||
             strcmp(args[0], "myfree") == 0 ||strcmp(args[0], "myuptime") == 0 ||strcmp(args[0], "phist") == 0)
           {
		   
		   if(type==0){
			 /*redirect file to 0 instead of oldfd*/
			 oldfd =open(file, O_RDONLY);

			 /*save STDIN before redir*/
			 saved_fd=dup(STDIN_FILENO);

			 if(oldfd<3)
			 {
				 printf("error fd is < 3\n");
			 }
			 if(dup2(oldfd,STDIN_FILENO)<0)
				 perror("dup2");
			 close(oldfd);
		 }
		 else if(type==1){
                         /*redirect file to 1 instead of oldfd*/
                         oldfd =open(file,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);

			  /*save STDOUT before redir*/
                         saved_fd=dup(STDOUT_FILENO);
			 

                         if(oldfd<3)
                         {
                                 printf("error fd is < 3\n");
                         }
                         if(dup2(oldfd,STDOUT_FILENO)<0)
                                 perror("dup2");
			 close(oldfd);
			 
                 }
                 else if(type==2){
                         /*redirect file to error instead of oldfd*/
			 oldfd =open(file,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);

			 /*save STDERR before redir */
                         saved_fd=dup(STDERR_FILENO);

                         if(oldfd<3)
                         {
                                 printf("error fd is < 3\n");
                         }
                         if(dup2(oldfd,STDERR_FILENO)<0)
                                 perror("dup2");
			 close(oldfd);
                 }
                 else if(type==3){
                         /*redirect file to append instead of oldfd*/
                         oldfd =open(file,O_WRONLY|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);

			 /*save STDOUT  before redir*/
                         saved_fd=dup(STDOUT_FILENO);


                         if(oldfd<3)
                         {
                                 printf("error fd is < 3\n");
                         }
                         if(dup2(oldfd,STDOUT_FILENO)<0)
                                 perror("dup2");
			 close(oldfd);
		 }

		 /* internal echo command */
		 if(strcmp(args[0],"myecho")==0)
			 echo_command(cleaned_args);

		 /* internal copy command */
		 else if(strcmp(args[0],"mycp")==0)
		 {/*to check if source and target file is passed or not*/
			 if (argIndex < 3)
				 fprintf(stderr, "Usage: %s <str1> <str2> [<len>]\n", args[0]);
			 else
				 copy_move_command(cleaned_args);
		 }

		 /* internal move command*/
		 else if(strcmp(args[0],"mymv")==0)
		 {/*to check if source and target file is passed  or not*/
			 if (argIndex < 3)
				 fprintf(stderr, "Usage: %s <str1> <str2> [<len>]\n", args[0]);
			 else
				 copy_move_command(cleaned_args);
		 }

		 /* exit command */
		 else if(strcmp(args[0],"exit")==0)
			 exit_command();
		 /*internal pwd command */
		 else if(strcmp(args[0],"mypwd")==0)
		         pwd_command();
		 /* cd command */
		 else if(strcmp(args[0],"cd")==0)
		 {
			 if(argIndex<2)
				 cd_command(NULL);
			 else
				 cd_command(cleaned_args[1]);
		 }
		 /*type command */
		 else if(strcmp(args[0],"type")==0)
			 type_command(cleaned_args);

                /* envir command */
                else if(strcmp(args[0],"envir")==0)
		         envir_command();

		/* help command */
                else if(strcmp(args[0],"help")==0)
		         help_command();

                /*internal free command */
                else if(strcmp(args[0],"myfree")==0)
		         free_command();

               /*internal uptime command */
                else if(strcmp(args[0],"myuptime")==0)
		         uptime_command();

                /*internal phist command */
                else if(strcmp(args[0],"phist")==0)
	                 phist_command();

		/*restore again fd of the file*/
		switch(type)
		{
			case 0:/*in case of input redir*/
				if(dup2(saved_fd,STDIN_FILENO)<0)
                                 perror("dup2");
				close(saved_fd);
				break;
			case 1:/*incase of output redir*/
                                if(dup2(saved_fd,STDOUT_FILENO)<0)
                                 perror("dup2");
                                close(saved_fd);
                                break;
			case 2:/*incase of error redir */
                                if(dup2(saved_fd,STDERR_FILENO)<0)
                                 perror("dup2");
                                close(saved_fd);
                                break;
			case 3:/*incase of append redir */
                                if(dup2(saved_fd,STDOUT_FILENO)<0)
                                 perror("dup2");
                                close(saved_fd);
                                break;


		}

           }

	  else{

         /*external command*/
	 int retpid=fork();

	 if(retpid==0)
	 {
		 /*CHILD PROCESS*/
		 printf("child");
		 if(type==0){
			 /*redirect file to INPUT instead of oldfd*/
			 oldfd =open(file, O_RDONLY);

			 if(oldfd<3)
			 {
				 printf("error fd < 3\n");
			 }
			 if(dup2(oldfd,STDIN_FILENO)<0)
				 perror("dup2");
			 close(oldfd);
		 }
		 else if(type==1){
                         /*redirect file to OUTPUT instead of oldfd*/
                         oldfd =open(file,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);

                         if(oldfd<3)
                         {
                                 printf("error fd < 3\n");
                         }
                         if(dup2(oldfd,STDOUT_FILENO)<0)
                                 perror("dup2");
			 close(oldfd);
                 }
                 else if(type==2){
                         /*redirect file to ERROR instead of oldfd*/
                         oldfd =open(file,O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR);

                         if(oldfd<3)
                         {
                                 printf("error fd < 3\n");
                         }
                         if(dup2(oldfd,STDERR_FILENO)<0)
                                 perror("dup2");
			 close(oldfd);
                 }
                 else if(type==3){
                         /*redirect file to APPEND instead of oldfd*/
                         oldfd =open(file,O_WRONLY|O_CREAT|O_APPEND,S_IRUSR|S_IWUSR);

                         if(oldfd<3)
                         {
                                 printf("error fd < 3\n");
                         }
                         if(dup2(oldfd,STDOUT_FILENO)<0)
                                 perror("dup2");
			 close(oldfd);
                 }

		 execvpe(cleaned_args[0],cleaned_args,environ);
		 perror("execvpe");
	 
	 }
	 if(retpid<0)
	 {
		 perror("fork");
	 }
	 else {/*PARENT PROCESS*/
		 wait(NULL);
	 }
}
}
/*check for pipe function */
int  pipe_check()
{
	for(int i=0;i<argIndex;i++)
	{
		/*its pipe command*/
		if(strcmp(args[i],"|")==0)
		{
			int pipe_pos=i;
			/*dynamically allocates memory for argList1*/
			argList1=malloc((pipe_pos)*sizeof(char* ));
                       /*if theres failure in malloc*/
			if(argList1==NULL)
				perror("malloc");
			for (int i=0;i<pipe_pos;i++)
			{/*storing tokenised commands before pipe in argList*/
				argList1[i]=args[i];
			}
			/*terminate argList by NULL*/
			argList1[pipe_pos]=NULL;
			/*dynamically allocates memory for argList2*/
			argList2=malloc(((argIndex-pipe_pos)-1)*sizeof(char* ));
			/*if theres failure in malloc*/
			if(argList2==NULL)
				perror("malloc");
			int initial =pipe_pos+1;
			for (int i=0;i<((argIndex-pipe_pos)-1);i++)
			{/*storing tokenised commands after pipe in argList*/
				argList2[i]=args[initial];
				printf("%s ",argList2[i]);
			}/*terminate argList2 by NULL*/
			argList2[((argIndex-pipe_pos)-1)]=NULL;
			return 1; //theres pipe
	       }
	}
	return 0; //no pipe
}

/*redirection check function */
int redir_check()
{
	for(int i=0;i<argIndex;i++){
	/*its input redirection command*/
          if(strcmp(args[i],"<")==0)
         {
                 redir_command(0,args[i+1],i);
                 if(strcmp(args[0],"exit")==0)
                         return 0;

                 return 1;//theres redir
         }

          /*its output redirection command*/
         else if(strcmp(args[i],">")==0)
         {
                 redir_command(1,args[i+1],i);
                 if(strcmp(args[0],"exit")==0)
                         return 0;

                 return 1;//theres redir
         }

          /*its error redirection command*/
         else if(strcmp(args[i],"2>")==0)
         {
                 redir_command(2,args[i+1],i);
                 if(strcmp(args[0],"exit")==0)
                         return 0;

                 return 1;//theres redir
         }

        /*its append redirection command*/
         else if(strcmp(args[i],">>")==0)
         {
                 redir_command(3,args[i+1],i);
                 if(strcmp(args[0],"exit")==0)
                         return 0;
		 return 1;//theres redir
	 }
	}
	return 0; //no redir
}
