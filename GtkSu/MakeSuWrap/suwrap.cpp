
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <gtk/gtk.h>
#define VERSION 0.0.3
	char	*result=NULL;

int checkPasswd(char* username,char* hashedpass)
{
	spwd*	shadow_entry=NULL;

	shadow_entry=getspnam(username);
	if(shadow_entry==NULL)
		return(-1);

//	result=crypt(passwd,shadow_entry->sp_pwdp);
//	if(result!=NULL)
	printf("XXXX\nhashed %s\nentry %s\nXXX\n",hashedpass,shadow_entry->sp_pwdp);
		return(strcmp(hashedpass,shadow_entry->sp_pwdp));
//	else
//		return(-100);
}

int sendHashBack(char* username)
{
	spwd*	shadow_entry=NULL;

	shadow_entry=getspnam(username);
	if(shadow_entry==NULL)
		return(NULL);
	else
		{
			printf("%s\n",shadow_entry->sp_pwdp);
		//return(shadow_entry->sp_pwdp);
		//	g_free(shadow_entry);
			return(0);
		}
}

//argv[1]=user [2]=username [3]=passwd [n]=command
int main(int argc, char **argv)
{

if(strcmp(argv[1],"gethash")!=0)
	printf("arg1 %s arg2 %s arg1 %s3\n",argv[1],argv[2],argv[3]);
 
	int	ret;
	int	j;
	int	theuid=atoi(argv[1]);

	GString*	str=g_string_new(NULL);

	if(strcmp(argv[1],"gethash")==0)
		{	
			sendHashBack(argv[2]);
			return(0);
			//return(sendHashBack(argv[2]));
			//return(checkPasswd(argv[2],argv[3]));
		}

printf("going to check passwd with\nname %s\nhash %s\n",argv[2],argv[3]);
	if(checkPasswd(argv[2],argv[3])==0)
		{
			for(j=4;j<argc;j++)
				{
					if(argv[j][0]!='-')
						g_string_append_printf(str," \"%s\"",argv[j]);
				}
			g_string_append_printf(str," ");
			setresuid(theuid,theuid,theuid);
			ret=system(str->str);
			g_string_free(str,true);
//	printf("%s\n",str->str);
			return(ret);
		}
	else
		{
			printf("XXXXno\n");
			printf("\n\n%s\n%s\n",argv[3],result);
		return(-200);
		}
	
}
//	-v, --version        Gives ktsuss version info
//	-u, --user USER      Runs the command as the given user
//	-m, --message MESG   Change default message in ktsuss window
//	-h, --help           Show this help
