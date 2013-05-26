
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <gtk/gtk.h>

#define VERSION 0.0.5

int checkPasswd(char* username,char* hashedpass)
{
	spwd*	shadow_entry=NULL;

	shadow_entry=getspnam(username);
	if(shadow_entry==NULL)
		return(-1);
	else
		return(strcmp(hashedpass,shadow_entry->sp_pwdp));
}

int sendHashBack(char* username)
{
	spwd*	shadow_entry=NULL;

	shadow_entry=getspnam(username);
	if(shadow_entry==NULL)
		return(-1);
	else
		{
			printf("%s\n",shadow_entry->sp_pwdp);
			return(0);
		}
}

int main(int argc, char **argv)
{
	int	j;
	int	theuid=atoi(argv[1]);

	GString*	str=g_string_new(NULL);

	if(strcmp(argv[1],"gethash")==0)
		{	
			sendHashBack(argv[2]);
			return(0);
		}


	if(checkPasswd(argv[2],argv[3])==0)
		{
			for(j=4;j<argc;j++)
				{
					if(argv[j][0]!='-')
						g_string_append_printf(str," \"%s\"",argv[j]);
				}
			g_string_append_printf(str," &");
			setresuid(theuid,theuid,theuid);
			system(str->str);
			g_string_free(str,true);
			sleep(1);
			return(0);
		}
	else
		{
			return(-200);
		}
	
}
//	-v, --version        Gives ktsuss version info
//	-u, --user USER      Runs the command as the given user
//	-m, --message MESG   Change default message in ktsuss window
//	-h, --help           Show this help
