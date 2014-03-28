
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <grp.h>
#include <sys/wait.h> 
 
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <gtk/gtk.h>

#define	VERSION 0.1.0
#define	NOSHADOWUSER -1

static int   orig_ngroups=-1;
static gid_t orig_groups[NGROUPS_MAX];
static gid_t orig_rgid=-1;
static uid_t orig_ruid=-1;
static gid_t orig_egid=-1;
static uid_t orig_euid=-1;

bool	firstRun=true;
int retFromApp=77;

void drop_privileges(int permanent)
{
	if (geteuid()!=0)
		return;

	if (firstRun==true)
		{
			firstRun=false;
			orig_euid=geteuid();
			orig_egid=getegid();
			orig_ruid=getuid();
			orig_rgid=getgid();
			orig_ngroups=getgroups(NGROUPS_MAX,orig_groups);
		}

// drop groups
/* If root privileges are to be dropped, be sure to pare down the ancillary
* groups for the process before doing anything else because the setgroups(  )
* system call requires root privileges.  Drop ancillary groups regardless of
* whether privileges are being dropped temporarily or permanently.
*/

	gid_t newgid=orig_rgid;
	setgroups(1,&newgid);

	if (setregid(permanent ? newgid : -1,newgid)==-1) goto _drop_abort;

    // drop user
	if (setreuid((permanent ? orig_ruid : -1),orig_ruid)==-1) goto _drop_abort;

    // verify if not originally root
	if (orig_ruid!=0)
		{
			if (permanent)
				{
					if ((setegid(0)!=-1) || (getegid()!=newgid))
						goto _drop_abort;
					if ((seteuid(0)!=-1) || (geteuid()!=orig_ruid))
						goto _drop_abort;
				}
			else
				{
					if (getegid()!=newgid)
						goto _drop_abort;
					if (geteuid()!=orig_ruid)
						goto _drop_abort;
				}
		}

	return;

_drop_abort:
	printf("gtksuwrap: error 1: unable to drop priviledges - please report this problem\n");
    abort();
}

void restore_privileges(void)
{
	if (orig_euid!=0)
		return;

	seteuid(0);
	setegid(orig_egid);
	setgroups(orig_ngroups,orig_groups);
}

int checkPasswd(char* username,char* hashedpass)
{
	spwd*	shadow_entry=NULL;

	restore_privileges();
		shadow_entry=getspnam(username);
	drop_privileges(0);
	if(shadow_entry==NULL)
		return(NOSHADOWUSER);
	else
		return(strcmp(hashedpass,shadow_entry->sp_pwdp));
}

int sendHashBack(char* username)
{
	spwd*	shadow_entry=NULL;

	restore_privileges();
		shadow_entry=getspnam(username);
	drop_privileges(0);
	if(shadow_entry==NULL)
		{
			printf("XXX\n");
			return(NOSHADOWUSER);
		}
	else
		{
			printf("%s\n",shadow_entry->sp_pwdp);
			return(0);
		}
}

//const char* unsetVars[]=
//{
//};

struct passwd*	pwdata;

const char* setVars[][2]=
{
  {"IFS"," \t\n"},
  {"PATH",_PATH_STDPATH},
  {NULL,NULL}
};


char*	userHome;
char*	userName;
char*	userShell;
char*	userDisplay;
char*	userXAuth;

void keepEnvs(int theuid)
{
	pwdata=getpwuid(theuid);
	userHome=pwdata->pw_dir;
	userName=pwdata->pw_name;
	userShell=pwdata->pw_shell;
	userDisplay=getenv("DISPLAY");
	userXAuth=getenv("XAUTHORITY");
}

void cleanEnv(int theuid)
{
	keepEnvs(theuid);
	clearenv();
	if(theuid==0)
		setenv("PATH",_PATH_STDPATH,1);
	else
		setenv("PATH",_PATH_DEFPATH,1);

	setenv("IFS"," \t\n",1);
	setenv("HOME",userHome,1);
	setenv("USERNAME",userName,1);
	setenv("SHELL",userShell,1);
	setenv("DISPLAY",userDisplay,1);
	setenv("XAUTHORITY",userXAuth,1);
}

int main(int argc,char **argv)
{
	int			j;
	GString*	str;
	int			theuid;
	int			retval;

	drop_privileges(0);
	cleanEnv(geteuid());

	theuid=atoi(argv[1]);
	str=g_string_new(NULL);

	if(strncmp(argv[1],"gethash",7)==0)
		{	
			retval=sendHashBack(argv[2]);
			return(retval);
		}


	if(checkPasswd(argv[2],argv[3])==0)
		{		
			for(j=4;j<argc;j++)
				g_string_append_printf(str," \"%s\"",argv[j]);

			restore_privileges();
				setresuid(theuid,theuid,theuid);
				cleanEnv(geteuid());
				retFromApp=system(str->str);
			drop_privileges(0);

			g_string_free(str,true);

			return(WEXITSTATUS(retFromApp));
		}
	else
		{
			return(200);
		}
}
