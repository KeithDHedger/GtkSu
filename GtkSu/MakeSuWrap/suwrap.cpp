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
	FILE*		fp;
	char		buffer[1024]={0,};
	char*		display;
	char*		key;
	char*		endPtr;
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

	xauthDir=mkdtemp(tname);
	if(xauthDir==NULL)
		exit(CANTMAKETMPDIR);

	asprintf(&xauthFile,"%s/.Xauthority",xauthDir);

	asprintf(&command,"%s list %s|head -1",xauthBinPath,userDisplay);
	fp=popen(command, "r");
	if(fp!=NULL)
		{
			fgets(buffer,1024,fp);
			pclose(fp);
		}
	else
		exit(CANTMAKEXAUTHFILE);

	endPtr=strrchr(buffer,' ');
	if(endPtr==NULL)
		exit(CANTMAKEXAUTHFILE);
	endPtr--;
	key=strndup(endPtr,strlen(endPtr)-1);

	endPtr=strchr(buffer,' ');
	if(endPtr==NULL)
		exit(CANTMAKEXAUTHFILE);
	*endPtr=0;
	display=strndup(buffer,strlen(buffer));

	asprintf(&command,"%s -f %s add \"%s\" . \"%s\" 2>/dev/null",xauthBinPath,xauthFile,display,key);
	system(command);
	if(!g_file_test(xauthFile,G_FILE_TEST_EXISTS))
		exit(CANTMAKEXAUTHFILE);
}

void cleanEnv(int theuid,bool createxauth)
{
	keepEnvs(theuid);
	if(createxauth==true)
		makeXauthFile();

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
	setenv("DISPLAY",userDisplay,1);
	if(createxauth==true)
		setenv("XAUTHORITY",xauthFile,1);

	setenv("TZ",userTz,1);
	setenv("LANG",userLang,1);
	setenv("LC_ALL",userLcAll,1);
	setenv("LC_COLLATE",userLCCol,1);
	setenv("LC_CTYPE",userLCCType,1);
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
				g_string_append_printf(str," \"%s\"",argv[j]);

			restore_privileges();
				setresuid(theuid,theuid,theuid);
				cleanEnv(geteuid(),true);
				retFromApp=system(str->str);
				unlink(xauthFile);
				rmdir(xauthDir);
			drop_privileges(0);
			g_string_free(str,true);

			return(WEXITSTATUS(retFromApp));
		}
	else
		{
			return(BADPASSWD);
		}
}
