/*
tinyfiledialogs.c 
Unique code file of "tiny file dialogs" created [November 9, 2014]
Copyright (c) 2014 - 2015 Guillaume Vareille http://ysengrin.com
http://tinyfiledialogs.sourceforge.net

(changed for use in Chowdren, 2015)

- License -

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.  If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#undef WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tinyfiledialogs.h"

#ifdef _WIN32
 #ifndef _WIN32_WINNT
 #define _WIN32_WINNT 0x0500
 #endif
 #ifndef TINYFD_WIN_CONSOLE_ONLY
  #include <Windows.h>
  #include <Shlobj.h>
 #endif /* TINYFD_WIN_CONSOLE_ONLY */
 #include <sys/stat.h>
 #include <conio.h>
 #define SLASH "\\"
#else
 #include <limits.h>
 #include <unistd.h>
 #include <dirent.h> /* on old systems try <sys/dir.h> instead */
 #include <termios.h>
 #include <sys/utsname.h>
 #define SLASH "/"
#endif /* _WIN32 */

#define MAX_PATH_OR_CMD 1024 /* _MAX_PATH or MAX_PATH */
#define MAX_MULTIPLE 32

#pragma warning(disable:4996) 
/* allow usage of strncpy, strcpy, strcat, sprintf, fopen */

static int gWarningDisplayed = 0 ;
static char gTitle[]= "missing software! (so we switch to basic console input)";
static char gMessageWin[] = "tiny file dialogs on Windows needs:\n\t\
a graphic display\nor\tdialog.exe (enhanced console mode)\
\nor\ta console for basic input" ;

static char gMessageUnix[] = "tiny file dialogs on UNIX needs:\n\tapplescript\
\nor\tzenity (version 3 for the color chooser)\nor\tkdialog\
\nor\tXdialog\nor\tpython 2 with tkinter\
\nor\tdialog (opens a console if needed)\
\nor\twhiptail, gdialog or gxmessage (really?)\
\nor\tit will open a console (if needed) for basic input (you had it comming!)";

static char * getPathWithoutFinalSlash(
	char * const aoDestination, /* make sure it is allocated, use _MAX_PATH */
	char const * const aSource) /* aoDestination and aSource can be the same */
{
	char const * lTmp ;
	if ( aSource )
	{
		lTmp = strrchr(aSource, '/');
		if (!lTmp)
		{
			lTmp = strrchr(aSource, '\\');
		}
		if (lTmp)
		{
			strncpy(aoDestination, aSource, lTmp - aSource);
			aoDestination[lTmp - aSource] = '\0';
		}
		else
		{
			* aoDestination = '\0';
		}
	}
	else
	{
		* aoDestination = '\0';
	}
	return aoDestination;
}


static char * getLastName(
	char * const aoDestination, /* make sure it is allocated */
	char const * const aSource)
{
	/* copy the last name after '/' or '\' */
	char const * lTmp ;
	if ( aSource )
	{
		lTmp = strrchr(aSource, '/');
		if (!lTmp)
		{
			lTmp = strrchr(aSource, '\\');
		}
		if (lTmp)
		{
			strcpy(aoDestination, lTmp + 1);
		}
		else
		{
			strcpy(aoDestination, aSource);
		}
	}
	else
	{
		* aoDestination = '\0';
	}
	return aoDestination;
}


static void ensureFinalSlash ( char * const aioString )
{
	if ( aioString && strlen ( aioString ) )
	{
		char * lastcar = aioString + strlen ( aioString ) - 1 ;
		if ( strncmp ( lastcar , SLASH , 1 ) )
		{
			strcat ( lastcar , SLASH ) ;
		}
	}
}

static void replaceSubStr ( char const * const aSource ,
						    char const * const aOldSubStr ,
						    char const * const aNewSubStr ,
						    char * const aoDestination )
{
	char const * pOccurence ;
	char const * p ;
	char const * lNewSubStr = "" ;
	
	if ( ! aSource )
	{
		* aoDestination = '\0' ;
		return ;
	}
	if ( ! aOldSubStr )
	{
		strcpy ( aoDestination , aSource ) ;
		return ;
	}
	if ( aNewSubStr )
	{
		lNewSubStr = aNewSubStr ;
	}
	p = aSource ;
	int lOldSubLen = strlen ( aOldSubStr ) ;
	* aoDestination = '\0' ;
	while ( ( pOccurence = strstr ( p , aOldSubStr ) ) != NULL )
	{
		strncat ( aoDestination , p , pOccurence - p ) ;
		strcat ( aoDestination , lNewSubStr ) ;
		p = pOccurence + lOldSubLen ;
	}
	strcat ( aoDestination , p ) ;
}


static int replaceChr ( char * const aString ,
						char const aOldChr ,
						char const aNewChr )
{
	char * p ;
	int lRes = 0 ;

	if ( ! aString )
	{
		return 0 ;
	}
	if ( aOldChr == aNewChr )
	{
		return 0 ;
	}

	p = aString ;
	while ( (p = strchr ( p , aOldChr )) )
	{
		* p = aNewChr ;
		p ++ ;
		lRes = 1 ;
	}
	return lRes ;
}


static int filenameValid( char const * const aFileNameWithoutPath )
{
	if ( ! aFileNameWithoutPath
	  || ! strlen(aFileNameWithoutPath)
	  || strpbrk(aFileNameWithoutPath , "\\/:*?\"<>|") )
	{
		return 0 ;
	}
	return 1 ;
}


static int fileExists( char const * const aFilePathAndName )
{
	FILE * lIn ;
	if ( ! aFilePathAndName || ! strlen(aFilePathAndName) )
	{
		return 0 ;
	}
	lIn = fopen( aFilePathAndName , "r" ) ;
	if ( ! lIn )
	{

		return 0 ;
	}
	fclose ( lIn ) ;
	return 1 ;
}


/* source and destination can be the same or ovelap*/
static char const * ensureFilesExist( char * const aDestination ,
							 		  char const * const aSourcePathsAndNames)
{
	char * lDestination = aDestination ;
	char const * p ;
	char const * p2 ;
	int lLen ;

	if ( ! aSourcePathsAndNames )
	{
		return NULL ;
	}
	lLen = strlen( aSourcePathsAndNames ) ;
	if ( ! lLen )
	{
		return NULL ;
	}
	
	p = aSourcePathsAndNames ;
	while ( (p2 = strchr(p, '|')) != NULL )
	{
		lLen = p2-p ;		
		memmove(lDestination,p,lLen);
		lDestination[lLen] = '\0';
		if ( fileExists ( lDestination ) )
		{
			lDestination += lLen ;
			* lDestination = '|';
			lDestination ++ ;
		}
		p = p2 + 1 ;
	}
	if ( fileExists ( p ) )
	{
		lLen = strlen(p) ;		
		memmove(lDestination,p,lLen);
		lDestination[lLen] = '\0';
	}
	else
	{
		* (lDestination-1) = '\0';
	}
	return aDestination ;
}

#ifdef _WIN32

static int dirExists ( char const * const aDirPath )
{
	struct stat lInfo;
	if ( ! aDirPath || ! strlen ( aDirPath ) )
		return 0 ;
	if ( stat ( aDirPath , & lInfo ) != 0 )
		return 0 ;
	else if ( lInfo.st_mode & S_IFDIR )
		return 1 ;
	else
		return 0 ;
}

static char const * saveFileDialogWinGui (
	char * const aoBuff ,
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFilterPatterns , /* 0 */
    char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
    char const * const aSingleFilterDescription ) /* NULL or "image files" */
{
	char lDirname [ MAX_PATH_OR_CMD ] ;
	char lDialogString[MAX_PATH_OR_CMD];
	char lFilterPatterns[MAX_PATH_OR_CMD] = "";
    int i ;
    char * p;
	OPENFILENAME ofn ;

	getPathWithoutFinalSlash(lDirname, aDefaultPathAndFile);
	getLastName(aoBuff, aDefaultPathAndFile);
    
	if (aNumOfFilterPatterns > 0)
	{
		if ( aSingleFilterDescription && strlen(aSingleFilterDescription) )
		{
			strcpy(lFilterPatterns, aSingleFilterDescription);
			strcat(lFilterPatterns, "\n");
		}
		strcat(lFilterPatterns, aFilterPatterns[0]);
		for (i = 1; i < aNumOfFilterPatterns; i++)
		{
			strcat(lFilterPatterns, ";");
			strcat(lFilterPatterns, aFilterPatterns[i]);
		}
		strcat(lFilterPatterns, "\n");
		if ( ! (aSingleFilterDescription && strlen(aSingleFilterDescription) ) )
		{
			strcpy(lDialogString, lFilterPatterns);
			strcat(lFilterPatterns, lDialogString);
		}
		strcat(lFilterPatterns, "All Files\n*.*\n");
		p = lFilterPatterns;
		while ((p = strchr(p, '\n')) != NULL)
		{
			*p = '\0';
			p ++ ;
		}
	}
    
	ofn.lStructSize     = sizeof(OPENFILENAME) ;
	ofn.hwndOwner       = 0 ;
	ofn.hInstance       = 0 ;
	ofn.lpstrFilter     = lFilterPatterns ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter  = 0 ;
	ofn.nFilterIndex    = 1 ;
	ofn.lpstrFile		= aoBuff;

	ofn.nMaxFile        = MAX_PATH_OR_CMD ;
	ofn.lpstrFileTitle  = NULL ;
	ofn.nMaxFileTitle   = _MAX_FNAME + _MAX_EXT ;
	ofn.lpstrInitialDir = lDirname;
	ofn.lpstrTitle      = aTitle ;
	ofn.Flags           = OFN_OVERWRITEPROMPT ;
	ofn.nFileOffset     = 0 ;
	ofn.nFileExtension  = 0 ;
	ofn.lpstrDefExt     = NULL ;
	ofn.lCustData       = 0L ;
	ofn.lpfnHook        = NULL ;
	ofn.lpTemplateName  = NULL ;

	if ( GetSaveFileName ( & ofn ) == 0 )
	{
		return NULL ;
	}
	else 
	{ 
		return aoBuff ;
	}
}


static char const * openFileDialogWinGui (
	char * const aoBuff ,
    char const * const aTitle , /*  NULL or "" */
    char const * const aDefaultPathAndFile , /*  NULL or "" */
    int const aNumOfFilterPatterns , /* 0 */
    char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
    char const * const aSingleFilterDescription , /* NULL or "image files" */
    int const aAllowMultipleSelects ) /* 0 or 1 */
{
	char lDirname [ MAX_PATH_OR_CMD ] ;
	char lFilterPatterns[MAX_PATH_OR_CMD] = "";
	char lDialogString[MAX_PATH_OR_CMD] ;
	char * lPointers[MAX_MULTIPLE];
	size_t lLengths[MAX_MULTIPLE];
	int i , j ;
	char * p;
	OPENFILENAME ofn;
    size_t lBuffLen ;

	getPathWithoutFinalSlash(lDirname, aDefaultPathAndFile);
	getLastName(aoBuff, aDefaultPathAndFile);

	if (aNumOfFilterPatterns > 0)
	{
		if ( aSingleFilterDescription && strlen(aSingleFilterDescription) )
		{
			strcpy(lFilterPatterns, aSingleFilterDescription);
			strcat(lFilterPatterns, "\n");
		}
		strcat(lFilterPatterns, aFilterPatterns[0]);
		for (i = 1; i < aNumOfFilterPatterns; i++)
		{
			strcat(lFilterPatterns, ";");
			strcat(lFilterPatterns, aFilterPatterns[i]);
		}
		strcat(lFilterPatterns, "\n");
		if ( ! (aSingleFilterDescription && strlen(aSingleFilterDescription) ) )
		{
			strcpy(lDialogString, lFilterPatterns);
			strcat(lFilterPatterns, lDialogString);
		}
		strcat(lFilterPatterns, "All Files\n*.*\n");
		p = lFilterPatterns;
		while ((p = strchr(p, '\n')) != NULL)
		{
			*p = '\0';
			p ++ ;
		}
	}

	ofn.lStructSize     = sizeof ( OPENFILENAME ) ;
	ofn.hwndOwner       = 0 ;
	ofn.hInstance       = 0 ;
	ofn.lpstrFilter		= lFilterPatterns;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter  = 0 ;
	ofn.nFilterIndex    = 1 ;
	ofn.lpstrFile		= aoBuff ;
	ofn.nMaxFile        = MAX_PATH_OR_CMD ;
	ofn.lpstrFileTitle  = NULL ;
	ofn.nMaxFileTitle   = _MAX_FNAME + _MAX_EXT ;
	ofn.lpstrInitialDir = lDirname ;
	ofn.lpstrTitle      = aTitle ;
	ofn.Flags			= OFN_EXPLORER ;
	ofn.nFileOffset     = 0 ;
	ofn.nFileExtension  = 0 ;
	ofn.lpstrDefExt     = NULL ;
	ofn.lCustData       = 0L ;
	ofn.lpfnHook        = NULL ;
	ofn.lpTemplateName  = NULL ;

	if ( aAllowMultipleSelects )
	{
		ofn.Flags |= OFN_ALLOWMULTISELECT;
	}

	if ( GetOpenFileName ( & ofn ) == 0 )
	{
		return NULL ;
	}
	else 
	{
		lBuffLen = strlen(aoBuff) ;
		lPointers[0] = aoBuff + lBuffLen + 1 ;
		if ( !aAllowMultipleSelects || (lPointers[0][0] == '\0')  )
			return aoBuff ;
        
		i = 0 ;
		do
		{
			lLengths[i] = strlen(lPointers[i]);
			lPointers[i+1] = lPointers[i] + lLengths[i] + 1 ;
			i ++ ;
		}
		while ( lPointers[i][0] != '\0' );
		i--;
		p = aoBuff + MAX_MULTIPLE*MAX_PATH_OR_CMD - 1 ;
		* p = '\0';
		for ( j = i ; j >=0 ; j-- )
		{
			p -= lLengths[j];
			memmove(p, lPointers[j], lLengths[j]);
			p--;
			*p = '\\';
			p -= lBuffLen ;
			memmove(p, aoBuff, lBuffLen);
			p--;
			*p = '|';
		}
		p++;
		return p ;
	}
}

static int dialogPresent ( )
{
	static int lDialogPresent = -1 ;
	char lBuff [ MAX_PATH_OR_CMD ] ;
	FILE * lIn ;
	char const * lString = "dialog.exe";
	if ( lDialogPresent < 0 )
	{
		if (!(lIn = _popen("where dialog.exe","r")))
		{
			lDialogPresent = 0 ;
			return 0 ;
		}
		while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
		{}
		_pclose ( lIn ) ;
		if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
		{
			lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
		}
		if ( strcmp(lBuff+strlen(lBuff)-strlen(lString),lString) )
		{
			lDialogPresent = 0 ;
		}
		else
		{
			lDialogPresent = 1 ;
		}
	}
	return lDialogPresent ;
}

char const * tinyfd_saveFileDialog (
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFilterPatterns , /* 0 */
    char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
    char const * const aSingleFilterDescription ) /* NULL or "image files" */
{
    static char lBuff [ MAX_PATH_OR_CMD ] ;
	char lString[MAX_PATH_OR_CMD] ;
	char const * p ;
	lBuff[0]='\0';
	p = saveFileDialogWinGui(lBuff,
			aTitle,aDefaultPathAndFile,aNumOfFilterPatterns,aFilterPatterns,aSingleFilterDescription);
	if ( ! p || ! strlen ( p )  )
	{
		return NULL;
	}
	getPathWithoutFinalSlash ( lString , p ) ;
	if ( strlen ( lString ) && ! dirExists ( lString ) )
	{
		return NULL ;
	}
	getLastName(lString,p);
	if ( ! filenameValid(lString) )
	{
		return NULL;
	}
	return p ;
}

/* in case of multiple files, the separator is | */
char const * tinyfd_openFileDialog (
    char const * const aTitle , /*  NULL or "" */
    char const * const aDefaultPathAndFile , /*  NULL or "" */
    int const aNumOfFilterPatterns , /* 0 */
    char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
    char const * const aSingleFilterDescription , /* NULL or "image files" */
    int const aAllowMultipleSelects ) /* 0 or 1 */
{
	static char lBuff[MAX_MULTIPLE*MAX_PATH_OR_CMD];
	char const * p ;
	p = openFileDialogWinGui(lBuff,
			aTitle,aDefaultPathAndFile,aNumOfFilterPatterns,
			aFilterPatterns,aSingleFilterDescription,aAllowMultipleSelects);
	if ( ! p || ! strlen ( p )  )
	{
		return NULL;
	}
	if ( aAllowMultipleSelects && strchr(p, '|') )
	{
		p = ensureFilesExist( lBuff , p ) ;
	}
	else if ( ! fileExists (p) )
	{
		return NULL ;
	}
	/* printf ( "lBuff3: %s\n" , p ) ; //*/
	return p ;
}

#elif !defined(__APPLE__) /* unix */

static char gPython2Name[16];
							
static int isDarwin ( )
{
	static int lsIsDarwin = -1 ;
	struct utsname lUtsname ;
	if ( lsIsDarwin < 0 )
	{
		lsIsDarwin = !uname(&lUtsname) && !strcmp(lUtsname.sysname,"Darwin") ;
	}
	return lsIsDarwin ;
}


static int dirExists ( char const * const aDirPath )
{
	DIR * lDir ;
	if ( ! aDirPath || ! strlen ( aDirPath ) )
		return 0 ;
	lDir = opendir ( aDirPath ) ;
	if ( ! lDir )
	{
		return 0 ;
	}
	closedir ( lDir ) ;
	return 1 ;
}

									
static int detectPresence ( char const * const aExecutable )
{
    char lBuff [ MAX_PATH_OR_CMD ] ;
    char lTestedString [ MAX_PATH_OR_CMD ] = "which " ;
	FILE * lIn ;

    strcat ( lTestedString , aExecutable ) ;
    lIn = popen ( lTestedString , "r" ) ;
    if ( ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
        && ( ! strchr ( lBuff , ':' ) ) )
    {	/* present */
    	pclose ( lIn ) ;
    	return 1 ;
    }
    else
    {
    	pclose ( lIn ) ;
    	return 0 ;
    }
}


static int tryCommand ( char const * const aCommand )
{
    char lBuff [ MAX_PATH_OR_CMD ] ;
    FILE * lIn ;

    lIn = popen ( aCommand , "r" ) ;
    if ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) == NULL )
    {	/* present */
    	pclose ( lIn ) ;
    	return 1 ;
    }
    else
    {
    	pclose ( lIn ) ;
    	return 0 ;
    }

}


static char const * terminalName ( )
{
	static char lTerminalName[64] = "*" ;
    if ( lTerminalName[0] == '*' )
    {
		if ( isDarwin() )
		{
			if ( strcpy(lTerminalName , "/opt/X11/bin/xterm" )
		      && detectPresence ( lTerminalName ) )
			{
				strcat(lTerminalName , " -e bash -c " ) ;
			}
			else
			{
				strcpy(lTerminalName , "" ) ;
			}
		}
		else if ( strcpy(lTerminalName,"gnome-terminal")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -x bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"konsole")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"xterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
			//strcat(lTerminalName , " -e " ) ;
		}
		else if ( strcpy(lTerminalName,"lxterminal")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"xfce4-terminal")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -x bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"Terminal")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -x bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"rxvt")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"urxvt")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"mrxvt")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"wterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"eterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"aterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"terminology")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"multi-gnome-terminal")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -x bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"hpterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"winterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -c bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"roxterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"st")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"sakura")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"mlterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"vte")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"terminator")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -x bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"lilyterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -x bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"dtterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"nxterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"pterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"xgterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"evilvte")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"kterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"xiterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"termit")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"xvt")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"vala-terminal")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else if ( strcpy(lTerminalName,"osso-xterm")
			  && detectPresence(lTerminalName) )
		{
			strcat(lTerminalName , " -e bash -c " ) ;
		}
		else
		{
			strcpy(lTerminalName , "" ) ;
		}
    }
	if ( strlen(lTerminalName) )
	{
		return lTerminalName ;
	}
	else
	{
		return NULL ;
	}
}


static char const * dialogName ( )
{
	static char lDialogName[64] = "*" ;
    if ( lDialogName[0] == '*' )
    {
		if ( isDarwin() && strcpy(lDialogName , "/opt/local/bin/dialog" )
		  && detectPresence ( lDialogName ) )
		{}
		else if ( strcpy(lDialogName , "dialog" )
			&& detectPresence ( lDialogName ) )
		{}
		else
		{
			strcpy(lDialogName , "" ) ;
		}
    }
	if ( strlen(lDialogName) && ( isatty(1) || terminalName() ) )
	{
		return lDialogName ;
	}
	else
	{
		return NULL ;
	}
}


static int whiptailPresent ( )
{
    static int lWhiptailPresent = -1 ;
    if ( lWhiptailPresent < 0 )
    {
        lWhiptailPresent = detectPresence ( "whiptail" ) ;
    }
	return lWhiptailPresent && ( isatty(1) || terminalName() ) ;
}


static int graphicMode()
{
	return !( tinyfd_forceConsole && (isatty(1) || terminalName()) )
			&& getenv ( "DISPLAY" );
}


static int xmessagePresent ( )
{
    static int lXmessagePresent = -1 ;
    if ( lXmessagePresent < 0 )
    {
	 lXmessagePresent = detectPresence("xmessage");/*if not tty,not on osxpath*/
    }
    return lXmessagePresent && graphicMode ( ) ;
}


static int gxmessagePresent ( )
{
    static int lGxmessagePresent = -1 ;
    if ( lGxmessagePresent < 0 )
    {
        lGxmessagePresent = detectPresence("gxmessage") ;
    }
    return lGxmessagePresent && graphicMode ( ) ;
}


static int notifysendPresent ( )
{
    static int lNotifysendPresent = -1 ;
    if ( lNotifysendPresent < 0 )
    {
        lNotifysendPresent = detectPresence("notify-send") ;
    }
    return lNotifysendPresent && graphicMode ( ) ;
}


static int xdialogPresent ( )
{
    static int lXdialogPresent = -1 ;
    if ( lXdialogPresent < 0 )
    {
        lXdialogPresent = detectPresence("Xdialog") ;
    }
    return lXdialogPresent && graphicMode ( ) ;
}


static int gdialogPresent ( )
{
    static int lGdialoglPresent = -1 ;
    if ( lGdialoglPresent < 0 )
    {
        lGdialoglPresent = detectPresence ( "gdialog" ) ;
    }
    return lGdialoglPresent && graphicMode ( ) ;
}


static int osascriptPresent ( )
{
    static int lOsascriptPresent = -1 ;
    if ( lOsascriptPresent < 0 )
    {
        lOsascriptPresent = detectPresence ( "osascript" ) ;
    }
	return lOsascriptPresent && graphicMode ( ) ;
}


static int kdialogPresent ( )
{
	static int lKdialogPresent = -1 ;
	if ( lKdialogPresent < 0 )
	{
		lKdialogPresent = detectPresence("kdialog") ;
	}
	return lKdialogPresent && graphicMode ( ) ;
}


static int zenityPresent ( )
{
	static int lZenityPresent = -1 ;
	if ( lZenityPresent < 0 )
	{
		lZenityPresent = detectPresence("zenity") ;
	}
	return lZenityPresent && graphicMode ( ) ;
}


static int zenity3Present ( )
{
    static int lZenity3Present = -1 ;
    char lBuff [ MAX_PATH_OR_CMD ] ;
    FILE * lIn ;
	

	if ( lZenity3Present < 0 )
	{
		if ( ! zenityPresent() )
		{
			lZenity3Present = 0 ;
		}
	 	else
		{
			lIn = popen ( "zenity --version" , "r" ) ;
			if ( ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
			  && ( atoi(lBuff) >= 3 )
			  && ( atoi(strtok(lBuff,".")+1) >= 0 ) )
			{
				lZenity3Present = 1 ;
			}
			else
			{
				lZenity3Present = 0 ;
			}
			pclose ( lIn ) ;
		}
	}
    return lZenity3Present && graphicMode ( ) ;
}


static int tkinter2Present ( )
{
    static int lTkinter2Present = -1 ;
	char lPythonCommand[256];
	char lPythonParams[256] =
"-c \"try:\n\timport Tkinter;\nexcept:\n\tprint(0);\"";
	int i;

    if ( lTkinter2Present < 0 )
    {
		strcpy(gPython2Name , "python" ) ;
		sprintf ( lPythonCommand , "%s %s" , gPython2Name , lPythonParams ) ;
	    lTkinter2Present = tryCommand(lPythonCommand);		
        if ( ! lTkinter2Present )
	    {
			strcpy(gPython2Name , "python2" ) ;
			if ( detectPresence(gPython2Name) )
			{
sprintf ( lPythonCommand , "%s %s" , gPython2Name , lPythonParams ) ;
				lTkinter2Present = tryCommand(lPythonCommand);
			}
			else
			{
				for ( i = 9 ; i >= 0 ; i -- )
				{
					sprintf ( gPython2Name , "python2.%d" , i ) ;
					if ( detectPresence(gPython2Name) )
					{
sprintf ( lPythonCommand , "%s %s" , gPython2Name , lPythonParams ) ;
						lTkinter2Present = tryCommand(lPythonCommand);
						break ;
					}
				}
			}
	    }
    }
    /* printf ("gPython2Name %s\n", gPython2Name) ; //*/
    return lTkinter2Present && graphicMode ( ) ;
}

char const * tinyfd_saveFileDialog (
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFilterPatterns , /* 0 */
    char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
    char const * const aSingleFilterDescription ) /* NULL or "image files" */
{
    static char lBuff [ MAX_PATH_OR_CMD ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
    char lString [ MAX_PATH_OR_CMD ] ;
	int i ;
	int lWasGraphicDialog = 0 ;
	int lWasXterm = 0 ;
	char const * p ;
    DIR * lDir ;
    FILE * lIn ;
	lBuff[0]='\0';

	if ( osascriptPresent ( ) )
	{
		strcpy ( lDialogString ,
				"osascript -e 'POSIX path of ( choose file name " );
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "with prompt \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
		if ( strlen(lString) )
		{
			strcat(lDialogString, "default location \"") ;
			strcat(lDialogString, lString ) ;
			strcat(lDialogString , "\" " ) ;
		}
		getLastName ( lString , aDefaultPathAndFile ) ;
		if ( strlen(lString) )
		{
			strcat(lDialogString, "default name \"") ;
			strcat(lDialogString, lString ) ;
			strcat(lDialogString , "\" " ) ;
		}
		strcat ( lDialogString , ")'" ) ;
	}
    else if ( zenityPresent() )
    {
		strcpy ( lDialogString ,
				"zenity --file-selection --save --confirm-overwrite" ) ;
		if ( aTitle && strlen(aTitle) ) 
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) ) 
		{
			strcat(lDialogString, " --filename=\"") ;
			strcat(lDialogString, aDefaultPathAndFile) ;
			strcat(lDialogString, "\"") ;
		}		
		if ( aNumOfFilterPatterns > 0 )
		{
			strcat ( lDialogString , " --file-filter='" ) ;
			if ( aSingleFilterDescription && strlen(aSingleFilterDescription) )
			{
				strcat ( lDialogString , aSingleFilterDescription ) ;
				strcat ( lDialogString , " | " ) ;
			}
			for ( i = 0 ; i < aNumOfFilterPatterns ; i ++ )
			{
				strcat ( lDialogString , aFilterPatterns [ i ] ) ;
				strcat ( lDialogString , " " ) ;
			}
			strcat ( lDialogString , "' --file-filter='All files | *'" ) ;
		}
    }
    else if ( kdialogPresent() )
    {
		strcpy ( lDialogString , "kdialog --getsavefilename" ) ;
        if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
        {
			strcat(lDialogString, " \"") ;
			strcat(lDialogString, aDefaultPathAndFile ) ;
			strcat(lDialogString , "\"" ) ;
		}
		else
		{
			strcat(lDialogString, " :" ) ;
		}
	    if ( aNumOfFilterPatterns > 0 )
	    {
			strcat(lDialogString , " \"" ) ;
			for ( i = 0 ; i < aNumOfFilterPatterns ; i ++ )
			{
				strcat ( lDialogString , aFilterPatterns [ i ] ) ;
				strcat ( lDialogString , " " ) ;
			}
			if ( aSingleFilterDescription && strlen(aSingleFilterDescription) )
			{
				strcat ( lDialogString , " | " ) ;
				strcat ( lDialogString , aSingleFilterDescription ) ;
			}
			strcat ( lDialogString , "\"" ) ;
	    }
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
	    }
    }
    else if ( ! xdialogPresent() && tkinter2Present ( ) )
    {

		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) && isDarwin ( ))
		{
        	strcat ( lDialogString , " -i" ) ;  /* for osx without console */
		}
	    strcat ( lDialogString ,
" -c \"import Tkinter,tkFileDialog;root=Tkinter.Tk();root.withdraw();");

    	if ( isDarwin ( ) )
    	{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set\
 frontmost of process \\\"Python\\\" to true' ''');");
		}

		strcat ( lDialogString , "print tkFileDialog.asksaveasfilename(");
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
		}
	    if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
	    {
			getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialdir='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
			getLastName ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialfile='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
		}
	    if ( ( aNumOfFilterPatterns > 1 )
		  || ( (aNumOfFilterPatterns == 1) /* test because poor osx behaviour */
			&& ( aFilterPatterns[0][strlen(aFilterPatterns[0])-1] != '*' ) ) )
	    {
			strcat(lDialogString , "filetypes=(" ) ;
			strcat ( lDialogString , "('" ) ;
			if ( aSingleFilterDescription && strlen(aSingleFilterDescription) )
			{
				strcat ( lDialogString , aSingleFilterDescription ) ;
			}
			strcat ( lDialogString , "',(" ) ;
			for ( i = 0 ; i < aNumOfFilterPatterns ; i ++ )
			{
				strcat ( lDialogString , "'" ) ;
				strcat ( lDialogString , aFilterPatterns [ i ] ) ;
				strcat ( lDialogString , "'," ) ;
			}
			strcat ( lDialogString , "))," ) ;
			strcat ( lDialogString , "('All files','*'))" ) ;
	    }
		strcat ( lDialogString , ")\"" ) ;
	}
	else if ( xdialogPresent() || dialogName() )
	{
		if ( xdialogPresent ( ) )
		{
			lWasGraphicDialog = 1 ;
			strcpy ( lDialogString , "(Xdialog " ) ;
		}
		else if ( isatty ( 1 ) )
		{
			strcpy ( lDialogString , "(dialog " ) ;
		}
		else
		{
			lWasXterm = 1 ;
			strcpy ( lDialogString , terminalName() ) ;
			strcat ( lDialogString , "'(" ) ;
			strcat ( lDialogString , dialogName() ) ;
			strcat ( lDialogString , " " ) ;
		}

 		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		strcat ( lDialogString , "--fselect \"" ) ;
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			if ( ! strchr(aDefaultPathAndFile, '/') )
			{
				strcat(lDialogString, "./") ;
			}
			strcat(lDialogString, aDefaultPathAndFile) ;
		}
		else if ( ! isatty ( 1 ) && !lWasGraphicDialog )
		{
			strcat(lDialogString, getenv("HOME")) ;
			strcat(lDialogString, "/") ;
		}

		if ( lWasGraphicDialog )
		{
			strcat(lDialogString, "\" 0 60 ) 2>&1 ") ;
		}
		else
		{
			strcat(lDialogString, "\" 0 60  >/dev/tty) ") ;
			if ( lWasXterm )
			{
			  strcat ( lDialogString ,
				"2>/tmp/tinyfd.txt';cat /tmp/tinyfd.txt;rm /tmp/tinyfd.txt");
			}
			else
			{
				strcat(lDialogString, "2>&1 ; clear >/dev/tty") ;
			}
		}
	}
	else
	{
		p = tinyfd_inputBox ( aTitle , "Save file" , "" ) ;
		getPathWithoutFinalSlash ( lString , p ) ;
		if ( strlen ( lString ) && ! dirExists ( lString ) )
		{
			return NULL ;
		}
		getLastName(lString,p);
		if ( ! strlen(lString) )
		{
			return NULL;
		}
		return p ;
	}

	/* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return NULL ;
    }
    while ( fgets ( lBuff , sizeof ( lBuff ) , lIn ) != NULL )
    {}
    pclose ( lIn ) ;
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
	/* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	if ( ! strlen(lBuff) )
	{
		return NULL;
	}
    getPathWithoutFinalSlash ( lString , lBuff ) ;
    if ( strlen ( lString ) && ! dirExists ( lString ) )
    {
        return NULL ;
    }
	getLastName(lString,lBuff);
	if ( ! filenameValid(lString) )
	{
		return NULL;
	}
    return lBuff ;
}

                 
/* in case of multiple files, the separator is | */
char const * tinyfd_openFileDialog (
    char const * const aTitle , /* NULL or "" */
    char const * const aDefaultPathAndFile , /* NULL or "" */
    int const aNumOfFilterPatterns , /* 0 */
    char const * const * const aFilterPatterns , /* NULL or {"*.jpg","*.png"} */
    char const * const aSingleFilterDescription , /* NULL or "image files" */
    int const aAllowMultipleSelects ) /* 0 or 1 */
{
    static char lBuff [ MAX_MULTIPLE*MAX_PATH_OR_CMD ] ;
    char lDialogString [ MAX_PATH_OR_CMD ] ;
	char lString [ MAX_PATH_OR_CMD ] ;
	int i ;
    FILE * lIn ;
	char * p ;
	char const * p2 ;
	int lWasKdialog = 0 ;
	int lWasGraphicDialog = 0 ;
	int lWasXterm = 0 ;
    lBuff[0]='\0';

    if ( osascriptPresent ( ) )
    {
		strcpy ( lDialogString , "osascript -e '" );
	    if ( ! aAllowMultipleSelects )
	    {
			strcat ( lDialogString , "POSIX path of ( " );
		}
		else
		{
			strcat ( lDialogString , "set mylist to " );
		}
		strcat ( lDialogString , "choose file " );
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "with prompt \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
	    }
		getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
		if ( strlen(lString) )
		{
			strcat(lDialogString, "default location \"") ;
			strcat(lDialogString, lString ) ;
			strcat(lDialogString , "\" " ) ;
		}
		if ( aNumOfFilterPatterns > 0 )
		{
			strcat(lDialogString , "of type {\"" );
			strcat ( lDialogString , aFilterPatterns [ 0 ] + 2 ) ;
			strcat ( lDialogString , "\"" ) ;
			for ( i = 1 ; i < aNumOfFilterPatterns ; i ++ )
			{
				strcat ( lDialogString , ",\"" ) ;
				strcat ( lDialogString , aFilterPatterns [ i ] + 2) ;
				strcat ( lDialogString , "\"" ) ;
			}
			strcat ( lDialogString , "} " ) ;
		}
		if ( aAllowMultipleSelects )
		{
			strcat ( lDialogString , "multiple selections allowed true ' " ) ;
			strcat ( lDialogString ,
					"-e 'set mystring to POSIX path of item 1 of mylist' " );
			strcat ( lDialogString ,
					"-e 'repeat with  i from 2 to the count of mylist' " );
			strcat ( lDialogString , "-e 'set mystring to mystring & \"|\"' " );
			strcat ( lDialogString ,
			"-e 'set mystring to mystring & POSIX path of item i of mylist' " );
			strcat ( lDialogString , "-e 'end repeat' " );
			strcat ( lDialogString , "-e 'mystring'" );
		}
		else
		{
			strcat ( lDialogString , ")'" ) ;
		}
    }
    else if ( zenityPresent() )
    {
        strcpy ( lDialogString ,"zenity --file-selection" ) ;
		if ( aAllowMultipleSelects )
		{
			strcat ( lDialogString , " --multiple" ) ;
		}
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title=\"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			strcat(lDialogString, " --filename=\"") ;
			strcat(lDialogString, aDefaultPathAndFile) ;
			strcat(lDialogString, "\"") ;
		}
        if ( aNumOfFilterPatterns > 0 )
        {
	        strcat ( lDialogString , " --file-filter='" ) ; 
			if ( aSingleFilterDescription && strlen(aSingleFilterDescription) )
			{
				strcat ( lDialogString , aSingleFilterDescription ) ;
				strcat ( lDialogString , " | " ) ;
			}
            for ( i = 0 ; i < aNumOfFilterPatterns ; i ++ )
            {
                strcat ( lDialogString , aFilterPatterns [ i ] ) ;
                strcat ( lDialogString , " " ) ;
            }
 	        strcat ( lDialogString , "' --file-filter='All files | *'" ) ;
        }
    }
	else if ( kdialogPresent() )
	{
		lWasKdialog = 1 ;
		strcpy ( lDialogString , "kdialog --getopenfilename" ) ;
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			strcat(lDialogString, " \"") ;
			strcat(lDialogString, aDefaultPathAndFile ) ;

			strcat(lDialogString , "\"" ) ;
		}
		else
		{
			strcat(lDialogString, " :" ) ;
		}
		if ( aNumOfFilterPatterns > 0 )
		{
			strcat(lDialogString , " \"" ) ;
			for ( i = 0 ; i < aNumOfFilterPatterns ; i ++ )
			{
				strcat ( lDialogString , aFilterPatterns [ i ] ) ;
				strcat ( lDialogString , " " ) ;
			}
			if ( aSingleFilterDescription && strlen(aSingleFilterDescription) )
			{
				strcat ( lDialogString , " | " ) ;
				strcat ( lDialogString , aSingleFilterDescription ) ;
			}
			strcat ( lDialogString , "\"" ) ;
		}
		if ( aAllowMultipleSelects )
		{
			strcat ( lDialogString , " --multiple --separate-output" ) ;
		}
		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, " --title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\"") ;
		}
	}
    else if ( ! xdialogPresent() && tkinter2Present ( ) )
    {
		strcpy ( lDialogString , gPython2Name ) ;
		if ( ! isatty ( 1 ) && isDarwin ( ) )
		{
        	strcat ( lDialogString , " -i" ) ;  /* for osx without console */
		}
        strcat ( lDialogString ,
" -c \"import Tkinter,tkFileDialog;root=Tkinter.Tk();root.withdraw();");

    	if ( isDarwin ( ) )
    	{
			strcat ( lDialogString ,
"import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
		}
		strcat ( lDialogString , "lFiles=tkFileDialog.askopenfilename(");
        if ( aAllowMultipleSelects )
        {
	        strcat ( lDialogString , "multiple=1," ) ;
        }
	    if ( aTitle && strlen(aTitle) )
	    {
			strcat(lDialogString, "title='") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "',") ;
	    }
        if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
        {
			getPathWithoutFinalSlash ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialdir='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
			getLastName ( lString , aDefaultPathAndFile ) ;
			if ( strlen(lString) )
			{
				strcat(lDialogString, "initialfile='") ;
				strcat(lDialogString, lString ) ;
				strcat(lDialogString , "'," ) ;
			}
		}
        if ( ( aNumOfFilterPatterns > 1 )
          || ( ( aNumOfFilterPatterns == 1 ) /*test because poor osx behaviour*/
			&& ( aFilterPatterns[0][strlen(aFilterPatterns[0])-1] != '*' ) ) )
        {
			strcat(lDialogString , "filetypes=(" ) ;
			strcat ( lDialogString , "('" ) ;
			if ( aSingleFilterDescription && strlen(aSingleFilterDescription) )
			{
				strcat ( lDialogString , aSingleFilterDescription ) ;
			}
			strcat ( lDialogString , "',(" ) ;
			for ( i = 0 ; i < aNumOfFilterPatterns ; i ++ )
			{
				strcat ( lDialogString , "'" ) ;
				strcat ( lDialogString , aFilterPatterns [ i ] ) ;
				strcat ( lDialogString , "'," ) ;
			}
			strcat ( lDialogString , "))," ) ;
			strcat ( lDialogString , "('All files','*'))" ) ;
        }
		strcat ( lDialogString , ");\
\nif not isinstance(lFiles, tuple):\n\tprint lFiles\nelse:\
\n\tlFilesString=''\n\tfor lFile in lFiles:\n\t\tlFilesString+=str(lFile)+'|'\
\n\tprint lFilesString[:-1]\n\"" ) ;
    }
	else if ( xdialogPresent() || dialogName() )
	{
		if ( xdialogPresent ( ) )
		{
			lWasGraphicDialog = 1 ;
			strcpy ( lDialogString , "(Xdialog " ) ;
		}
		else if ( isatty ( 1 ) )
		{
			strcpy ( lDialogString , "(dialog " ) ;
		}
		else
		{
			lWasXterm = 1 ;
			strcpy ( lDialogString , terminalName() ) ;
			strcat ( lDialogString , "'(" ) ;
			strcat ( lDialogString , dialogName() ) ;
			strcat ( lDialogString , " " ) ;
		}

		if ( aTitle && strlen(aTitle) )
		{
			strcat(lDialogString, "--title \"") ;
			strcat(lDialogString, aTitle) ;
			strcat(lDialogString, "\" ") ;
		}
		strcat ( lDialogString , "--fselect \"" ) ;
		if ( aDefaultPathAndFile && strlen(aDefaultPathAndFile) )
		{
			if ( ! strchr(aDefaultPathAndFile, '/') )
			{
				strcat(lDialogString, "./") ;
			}
			strcat(lDialogString, aDefaultPathAndFile) ;
		}
		else if ( ! isatty ( 1 ) && !lWasGraphicDialog )
		{
			strcat(lDialogString, getenv("HOME")) ;
			strcat(lDialogString, "/");
		}

		if ( lWasGraphicDialog )
		{
			strcat(lDialogString, "\" 0 60 ) 2>&1 ") ;
		}
		else
		{
			strcat(lDialogString, "\" 0 60  >/dev/tty) ") ;
			if ( lWasXterm )
			{
				strcat ( lDialogString ,
				"2>/tmp/tinyfd.txt';cat /tmp/tinyfd.txt;rm /tmp/tinyfd.txt");
			}
			else
			{
				strcat(lDialogString, "2>&1 ; clear >/dev/tty") ;
			}
		}
	}
	else
	{
		p2 = tinyfd_inputBox(aTitle, "Open file","");
		if ( ! fileExists (p2) )
		{
			return NULL ;
		}
		return p2 ;
	}

    /* printf ( "lDialogString: %s\n" , lDialogString ) ; //*/
    if ( ! ( lIn = popen ( lDialogString , "r" ) ) )
    {
        return NULL ;
    }
	lBuff[0]='\0';
	p=lBuff;
	while ( fgets ( p , sizeof ( lBuff ) , lIn ) != NULL )
	{
		p += strlen ( p );
	}
    pclose ( lIn ) ;
    if ( lBuff[ strlen ( lBuff ) -1 ] == '\n' )
    {
    	lBuff[ strlen ( lBuff ) -1 ] = '\0' ;
    }
    /* printf ( "lBuff: %s\n" , lBuff ) ; //*/
	if ( lWasKdialog && aAllowMultipleSelects )
	{
		p = lBuff ;
		while ( ( p = strchr ( p , '\n' ) ) )
			* p = '|' ;
	}
	/* printf ( "lBuff2: %s\n" , lBuff ) ; //*/
	if ( ! strlen ( lBuff )  )
	{
		return NULL;
	}
	if ( aAllowMultipleSelects && strchr(lBuff, '|') )
	{
		p2 = ensureFilesExist( lBuff , lBuff ) ;
	}
	else if ( fileExists (lBuff) )
	{
		p2 = lBuff ;
	}
	else
	{
		return NULL ;
	}
	/* printf ( "lBuff3: %s\n" , p2 ) ; //*/

	return p2 ;
}

#endif

#pragma warning(default:4996)
