/*
 *
 * K.D.Hedger 2012-2014 <kdhedger68713@gmail.com>
 *
 * Parts of this code are from udevil.c available here:
 * https://nodeload.github.com/IgnorantGuru/udevil/zip/master
 * Mucked about by me :)
 *
 * And libgksu.c available here:
 * http://people.debian.org/~kov/gksu/libgksu-2.0.12.tar.gz
 * Mucked about by me :)
 *
 * All code is suppled 'as is' use it at your own peril!
 * Released under GPLv3
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <grp.h>
#include <sys/wait.h> 

#include "config.h"

#include <glib.h>

#include <pwd.h>
#include <shadow.h>
#include <crypt.h>

#define	NOSHADOWUSER 255
#define CANTDROPPRIVS 254
#define NOXAUTHBIN 253
#define NOCLEANENV 252
#define CANTCHDIR 251
#define BADPASSWD 250
#define CANTMAKETMPDIR 249
#define CANTMAKEXAUTHFILE 248

static int		orig_ngroups=-1;
static gid_t	orig_groups[NGROUPS_MAX];
static gid_t	orig_rgid=-1;
static uid_t	orig_ruid=-1;
static gid_t	orig_egid=-1;
static uid_t	orig_euid=-1;

bool			firstRun=true;
int				retFromApp=-1;
char*			userHome;
char*			userName;
char*			userShell;
char*			userDisplay;
char*			userXAuth;
char*			userTz;
char*			userLang;
char*			userLcAll;
char*			userLCCol;
char*			userLCCType;

struct passwd*	pwdata;
extern char**	environ;
char*			xauthDir=NULL;
char*			xauthFile=NULL;

void setEnvTest(const char *env,const char *envvar)
{
	if(envvar!=NULL)
		setenv("DISPLAY",envvar,1);
}

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
	fprintf(stderr,"gtksuwrap: error 1: unable to drop priviledges - please report this problem\n");
    exit(CANTDROPPRIVS);
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

void keepEnvs(int theuid)
{
	pwdata=getpwuid(theuid);
	userHome=pwdata->pw_dir;
	userName=pwdata->pw_name;
	userShell=pwdata->pw_shell;
	userDisplay=getenv("DISPLAY");
	userXAuth=getenv("XAUTHORITY");
	userTz=getenv("TZ");
	userLang=getenv("LANG");
	userLcAll=getenv("LC_ALL");
	userLCCol=getenv("LC_COLLATE");
	userLCCType=getenv("LC_CTYPE");
}

void makeXauthFile(void)
{
	char*		command;
	gchar		tname[]="/tmp/GtkSu-XXXXXX";
	const char*	xauthBinPath=NULL;

	if(g_file_test("/usr/bin/xauth",G_FILE_TEST_IS_EXECUTABLE))
		xauthBinPath="/usr/bin/xauth";
	if(g_file_test("/usr/X11R6/bin/xauth",G_FILE_TEST_IS_EXECUTABLE))
		xauthBinPath="/usr/X11R6/bin/xauth";

	if(xauthBinPath==NULL)
		{
			fprintf(stderr,"Can't find xauth binary in /usr/bin/ or /usr/X11R6/bin/xauth\n");
			exit(NOXAUTHBIN);
		}

	xauthDir=strdup(mkdtemp(tname));
	if(xauthDir==NULL)
		exit(CANTMAKETMPDIR);

	asprintf(&xauthFile,"%s/.Xauthority",xauthDir);
	asprintf(&command,"chmod 777 %s",xauthDir);
	system(command);
	free(command);
	asprintf(&command,"%s extract - %s > %s",xauthBinPath,userDisplay,xauthFile);
	system(command);
	free(command);
}

void cleanEnv(int theuid,bool createxauth)
{
	keepEnvs(theuid);

#ifdef _GOTCLEARENV_
	if(clearenv()!=0)
		{
			fprintf(stderr,"Can't clean environment, aborting ...");
			exit(NOCLEANENV);
		}
#else
	environ=NULL;
#endif

	if(theuid==0)
		setenv("PATH",_PATH_STDPATH,1);
	else
		setenv("PATH",_PATH_DEFPATH,1);

	setenv("IFS"," \t\n",1);
	setenv("HOME",userHome,1);
	setenv("USERNAME",userName,1);
	setenv("SHELL",userShell,1);


	setEnvTest("DISPLAY",userDisplay);
	setEnvTest("TZ",userTz);
	setEnvTest("LANG",userLang);
	setEnvTest("LC_ALL",userLcAll);
	setEnvTest("LC_COLLATE",userLCCol);
	setEnvTest("LC_CTYPE",userLCCType);
	if(chdir(userHome)!=0)
		{
			if(chdir("/")!=0)
				{
					fprintf(stderr,"Can't change PWD, aborting ...");
					exit(CANTCHDIR);
				}
		}
}

int main(int argc,char **argv)
{
	int			j;
	GString*	str;
	int			theuid;
	int			retval;

	drop_privileges(0);
	cleanEnv(geteuid(),false);

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
				{
					if(j==4)
						g_string_append_printf(str," %s",argv[j]);
					else
						g_string_append_printf(str," \"%s\"",argv[j]);
				}
			makeXauthFile();
			restore_privileges();
				setresuid(theuid,theuid,theuid);
				cleanEnv(geteuid(),false);
				setEnvTest("XAUTHORITY",xauthFile);
				retFromApp=system(str->str);
			drop_privileges(0);
			unlink(xauthFile);
			rmdir(xauthDir);
			g_string_free(str,true);
			free(xauthDir);
			free(xauthFile);

			return(WEXITSTATUS(retFromApp));
		}
	else
		{
			return(BADPASSWD);
		}
}
