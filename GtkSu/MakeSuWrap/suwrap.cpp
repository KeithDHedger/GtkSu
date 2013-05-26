
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

int checkPasswd(char* username,char* passwd)
{
	char	*result=NULL;
	spwd*	shadow_entry=NULL;

	shadow_entry=getspnam(username);
	if(shadow_entry==NULL)
		return(-1);

	result=crypt(passwd,shadow_entry->sp_pwdp);
	if(result!=NULL)
		return(strcmp(result,shadow_entry->sp_pwdp));
	else
		return(-100);
}

int main(int argc, char **argv)
{
	int	ret;
	int	j;
	int	theuid=atoi(argv[1]);

	GString*	str=g_string_new(NULL);

	if(strcmp(argv[1],"checkpassword")==0)
		return(checkPasswd(argv[2],argv[3]));

	for(j=2;j<argc;j++)
		{
			if(argv[j][0]!='-')
				g_string_append_printf(str," \"%s\"",argv[j]);
		}
	g_string_append_printf(str," ");
	setresuid(theuid,theuid,theuid);
	ret=system(str->str);
	printf("%s\n",str->str);
	return(ret);
}
//	-v, --version        Gives ktsuss version info
//	-u, --user USER      Runs the command as the given user
//	-m, --message MESG   Change default message in ktsuss window
//	-h, --help           Show this help
