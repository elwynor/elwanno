/*****************************************************************************
 *                                                                           *
 *       EZEANNO.C - An advertisement broadcaster                            *
 *                                                                           *
 * Developed 09/22/96 by 'Easy Ed' Erdman.   Major BBS developer code 'EZE'. *
 *                                                                           *
 *       Updates : 1/29/97  - Version 1.0W                                   *
 *                 Upgrade for compatibility with Worldgroup 3 WIN NT/95     *
 *                 7/17/97  - Version 1.1W                                   *
 *                 Fix bug in database file of seen ads                      *
 *                 Fix bug where ANSI codes were displayed to C/S users      *
 *                 5/14/2021 - Version 1.2                                   *
 *                 Worldgroup 3.2 version                                    *
 *                                                                           *
 *       Copyright (C) 1996 by EzSoft. All rights reserved.                  *
 *       Copyright (C) 2004-2021 Elwynor Technologies. All Rights Reserved.  *
 *                                                                           *
 * This program is free software: you can redistribute it and/or modify      *
 * it under the terms of the GNU Affero General Public License as published  *
 * by the Free Software Foundation, either version 3 of the License, or      *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU Affero General Public License for more details.                       *
 *                                                                           *
 * You should have received a copy of the GNU Affero General Public License  *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.     *
 *                                                                           *
 * Additional Terms for Contributors:                                        *
 * 1. By contributing to this project, you agree to assign all right, title, *
 *    and interest, including all copyrights, in and to your contributions   *
 *    to Rick Hadsall and Elwynor Technologies.                              *
 * 2. You grant Rick Hadsall and Elwynor Technologies a non-exclusive,       *
 *    royalty-free, worldwide license to use, reproduce, prepare derivative  *
 *    works of, publicly display, publicly perform, sublicense, and          *
 *    distribute your contributions                                          *
 * 3. You represent that you have the legal right to make your contributions *
 *    and that the contributions do not infringe any third-party rights.     *
 * 4. Rick Hadsall and Elwynor Technologies are not obligated to incorporate *
 *    any contributions into the project.                                    *
 * 5. This project is licensed under the AGPL v3, and any derivative works   *
 *    must also be licensed under the AGPL v3.                               *
 * 6. If you create an entirely new project (a fork) based on this work, it  *
 *    must also be licensed under the AGPL v3, you assign all right, title,  *
 *    and interest, including all copyrights, in and to your contributions   *
 *    to Rick Hadsall and Elwynor Technologies, and you must include these   *
 *    additional terms in your project's LICENSE file(s).                    *
 *                                                                           *
 * By contributing to this project, you agree to these terms.                *
 *                                                                           *
 *****************************************************************************/

#include "gcomm.h"
#include "majorbbs.h"
#include "elwanno.h"
#include <limits.h>
#include "gcspsrv.h"

#define MAXTERMS 256                    /* maximum number of simultaneous users */
#define DEMODAYS 7                      /* number of days to run in DEMO mode */
#define PATHSIZ 50                      /* maximum number of chars in path and text file name */
#define NUMFLAGS 8                      /* number of flags available */

/* definition of flags for annodat.adtype */
#define ONCE 1                          /* display ad only once per day to user */
#define RANDOM 2                        /* display ad to random single user only */
#define CHANNEL 4                       /* display ad only to users on certain channel range */
#define MINOR 8                         /* display ad only to minors */
#define ADULT 16                        /* display ad only to adults */
#define MALE 32                         /* display ad only to males */
#define FEMALE 64                       /* display ad only to females */
#define PRIME 128                       /* display ad only during prime time */

VOID EXPORT init__elwanno( VOID );                              /* module initializer */
GBOOL annolog( VOID );                                          /* user log-on routine */
GBOOL annoinput( VOID );                                        /* module user input handler */
VOID annodel( CHAR *deluser );                                  /* delete user routine */
VOID clsanno( VOID );                                           /* BBS shutdown routine */
VOID anno( VOID );                                              /* the main routine - bug the DEMOs */
static VOID beep( INT numbeeps );                               /* "beep" user's computer numbeeps times */
INT annoglob( VOID );                                           /* global command handler */
VOID mark_seen( CHAR *pn );                                     /* marks ad pn as seen by user today */
INT show_ad( CHAR *pn, CHAR flags );                            /* returns 1 if show othuap ad pn, 0 if don't */
INT primetime( VOID );                                          /* return 1 if during "prime time", 0 if not */
INT channel_ok( VOID );                                         /* return 1 if user's channel is to be displayed to, 0 if not */
INT seen_ad( CHAR *pn );                                        /* returns 1 if othuap->userid has seen ad pn, 0 if not */
VOID list_flags( CHAR f );                                      /* displays flags in f */
CHAR get_flags( VOID );                                         /* returns flags of ad as CHAR from input */
INT user_to( VOID );                                            /* returns a usrnum in the range 0 to nterms-1 to display an ad to */
INT valid_viewer( VOID );                                       /* returns 1 if othusn is a potential viewer of ad, 0 if not */
VOID show_displays( CHAR *pn );                                 /* shows ad displays in the Audit Trail and/or a text file */

INT annostat;                                              /* the module state no. */

struct module annomod={
	"",                                                     /* name used to refer to this module */
	annolog,                                                /* user logon supplemental routine */
	annoinput,                                              /* input routine if selected */
	dfsthn,                                                 /* status-input routine if selected */
	NULL,                                                   /* "injoth" routine for this module */
	NULL,                                                   /* user logoff supplemental routine */
	NULL,                                                   /* hangup (lost carrier) routine */
	NULL,                                                   /* midnight cleanup routine */
	annodel,                                                /* delete-account routine */
	clsanno,                                                /* finish-up (sys shutdown) routine */
	};

struct annodat                                          /* data for each ad stored to disk */
	{
	INT adnum;                                      /* key value */
	CHAR pathname[PATHSIZ];                         /* path and name of text file */
	CHAR adtype;                                    /* flags for type of ad */
	}; //55

struct adseen                                           /* save that ad has been seen by user */
	{
	CHAR userid[UIDSIZ];                            /* handle of user */
	CHAR ad[PATHSIZ];                               /* ad seen */
	USHORT dateseen;                                /* date ad was seen */
	}; //82

DFAFILE *annobb;                                        /* list of advertisement data */
DFAFILE *seenbb;                                        /* users who have seen a given ad */
HMCVFILE annombk;                                       /* module message file pointer */
INT annosecs;                                           /* number of seconds between broadcasts */
CHAR *pesterkey;                                        /* key needed to not be bothered with ads */
CHAR *editkey;                                          /* key needed to be able to view and edit ads */
INT dobeeps;                                            /* 1 if "beep" with message, 0 if dont */
INT sendedit;                                           /* 1 if send announcement to users in Editor, 0 if dont */
CHAR *editorname;                                       /* module name of The Editor */
INT base=15094;                                         /* number to add values to before placing in accesscode */
CHAR *ANN_TYPE;                                         /* version of program */
CHAR *ANN_VERSION="1.2";                                /* version number */
CHAR *ANN_NAME="Advertiser";                            /* the module name */
CHAR modnamestring[33];                                 /* module name string identifier */
CHAR xorchar='ž';                                       /* value for encoding module name in genbb */
INT num_ads=0;                                          /* number of ads there are */
CHAR *liststring;                                       /* global command to list ad info */
CHAR *editstring;                                       /* global command to edit ad info */
USHORT starttime;                                       /* starting time of prime time */
USHORT endtime;                                         /* ending time of prime time */
INT firstline;                                          /* line number to begin call back delays at */
INT lastline;                                           /* line number to end call back delays at */
CHAR *flag_names[NUMFLAGS] = { "ONCE",
			       "RANDOM",
			       "CHANNEL",
			       "MINOR",
			       "ADULT",
			       "MALE",
			       "FEMALE",
			       "PRIME" };
CHAR flag_values[NUMFLAGS] = { 1,2,4,8,16,32,64,128 };
INT showaudit;                                          /* 1 if show ads displayed in Audit Trail; 0 if don't */
INT showtext;                                           /* 1 if show ads displayed in text file; 0 if don't */
CHAR *pathname;                                         /* path and file name of text file to show ads displayed */

VOID EXPORT init__elwanno( VOID )          /* the module initialization routine */
{
	INT found;                              /* 1 if ad found, 0 if no more */

	stzcpy(annomod.descrp, gmdnam("ELWANNO.MDF"),MNMSIZ);
	annostat=register_module(&annomod);
	annombk=opnmsg("elwanno.mcv");
	
   shocst(spr("ELW Advertiser v%s",ANN_VERSION),"(C) Copyright 2021-2024 Elwynor Technologies - www.elwynor.com");
	
	annobb=dfaOpen("elwanno.dat", sizeof(struct annodat), NULL);
	seenbb=dfaOpen("elwannos.dat", sizeof(struct adseen), NULL);
	globalcmd(annoglob);
	annosecs=numopt(PSTSEC, 30, 3600);
	pesterkey=stgopt(PSTKEY);
	editkey=stgopt(EDITKEY);
	dobeeps=ynopt(DOBEEP);
	sendedit=ynopt(EDITBUG);
	editorname=stgopt(EDTNAM);
	liststring=stgopt(VIEWSTG);
	editstring=stgopt(EDITSTG);
	starttime=dctime(stgopt(PRMSTR));
	endtime=dctime(stgopt(PRMEND));
	firstline=hexopt(FSTLINE, 0, 0xFF);
	lastline=hexopt(LSTLINE, 0, 0xFF);
	showaudit=ynopt(SHOWAUD);
	showtext=ynopt(SHOWTEXT);
	pathname=stgopt(FILNAM);
	dfaSetBlk(annobb);
	found=dfaStepLO(NULL);
	while ( found ) {
	   num_ads++;
	   found=dfaStepNX(NULL);
	}
	dfaRstBlk();
#ifdef __BUILDV10MODULE  
	srand((unsigned)time(NULL)); // BBSV10
#else
	randomize(); // WG32
#endif
	strcpy(modnamestring, ANN_NAME);
	strcat(modnamestring, " v");
	strcat(modnamestring, ANN_VERSION);

   rtkick( annosecs, anno );
}

GBOOL annolog( VOID )                           /* user log-on routine */
	{
	if ( haskey(editkey) )	{       /* display help on module usage */
		setmbk(annombk);
		prfmsg(ADLOG, modnamestring, liststring, editstring);
		outprf(usrnum);
		rstmbk();
	}
	return(0);
}

GBOOL annoinput( VOID )                         /* module user input handler */
{
    return(0);
}

VOID annodel( CHAR *deluser )                   /* delete user routine */
{
	dfaSetBlk(seenbb);
	while ( dfaAcqEQ(NULL, deluser, 0) ) {       /* delete all ads seen */
		dfaDelete();
	}
	dfaRstBlk();
}

VOID clsanno( VOID )                            /* BBS shutdown routine */
{
	clsmsg(annombk);
	free(ANN_TYPE);
}

VOID anno( VOID )                               /* the main routine - bug the DEMOs */
{
	INT display;            /* key number of ad to show */
	CHAR *buffer;           /* buffer to place ad into */
	CHAR *cs_buffer;	/* stripped buffer for C/S users */
	FILE *adtext;           /* the ad text file */
	struct annodat adat;    /* data for ad to display */
        struct ffblk fildat;    /* get file's data including file length */
	INT showuser;           /* user to randomly display ad to */
	INT shown=0;            /* 1 if ad was shown to someone, 0 if not */

	if ( !num_ads ) {       /* no ads to show */
      rtkick( annosecs, anno );
		return;
	}
#ifdef __BUILDV10MODULE
	display=(rand() % num_ads);       // same idea as random(n) which is 0..n-1, but for Visual Studio
#else
	display=random(num_ads);          // returns 0 through num_ads-1, but a Borland macro
#endif
	dfaSetBlk(annobb);
	if ( !dfaAcqEQ(&adat, &display, 0) ) {       /* cannot display ad */
		dfaRstBlk();
		shocst("ELW Advertiser Error", "Cannot get ad data file");
		return;
	}
	dfaRstBlk();
	if ( !fndfile(&fildat, adat.pathname, 0) ) {	/* can't get file information */
		shocst("ELW Advertiser Error", "Cannot open %s", adat.pathname);
		return;
	}
	if ( fildat.ff_fsize > (UINT_MAX-1) ) {       /* file too big to display */
		shocst("ELW Advertiser Error", "%s too big", adat.pathname);
		return;
	}
	buffer=alczer((USHORT)fildat.ff_fsize+1);
	cs_buffer=alczer((USHORT)fildat.ff_fsize+1);
	if ( (adtext=fopen(adat.pathname, FOPRA)) == NULL ) {       /* can't open file */
		shocst("ELW Advertiser Error","Cannot open %s", adat.pathname);
		return;
	}
	fread(buffer, (USHORT)fildat.ff_fsize, 1, adtext);
	strcpy(cs_buffer, buffer);
	stp4cs(cs_buffer);
	fclose(adtext);
	if ( adat.adtype&RANDOM ) {       /* display to only one selected user */
		showuser=user_to();
		othusn=showuser;
		othusp=usroff(othusn);
		othuap=uacoff(othusn);
		if ( valid_viewer() && show_ad(adat.pathname, adat.adtype) ) {      /* make sure someone is on channel, and not in editor */
			mark_seen(adat.pathname);
			if ( dobeeps ) {
				beep(1);
			}
			if ( (othusp->flags&ISGCSU) != 0L )	{	/* C/S user */
				prf(cs_buffer);
			} else {	/* A/A user */
				prf(buffer);
			}
			if ( injoth() ) {       /* note ad was shown */
				show_displays(adat.pathname);
			}
		}
	} else for ( othusn=0, othusp=usroff(othusn); othusn < nterms; othusn++, othusp=usroff(othusn) ) {       /* announce to everyone without key */
		othuap=uacoff(othusn);
		if ( valid_viewer() && show_ad(adat.pathname, adat.adtype) ) {      /* make sure someone is on channel, and not in editor */
			mark_seen(adat.pathname);
			if ( dobeeps )	{
				beep(1);
			}
			if ( (othusp->flags&ISGCSU) != 0L )	{	/* C/S user */
				prf(cs_buffer);
			} else {	/* A/A user */
				prf(buffer);
			}
			if ( injoth() ) {       /* ad was shown to someone */
				shown=1;
			}
		}
	}
	if ( shown ) {       /* note that ad was displayed */
		show_displays(adat.pathname);
	}
	free(buffer);
   free(cs_buffer);
	rtkick( annosecs, anno );
}

static VOID beep( INT numbeeps )                /* "beep" user's computer numbeeps times */
{
	INT beepcount;                  /* looping counter for beeping */

	for ( beepcount=0; beepcount < numbeeps; beepcount++ ) {
		prf("\a");
	}
}

INT annoglob( VOID )                            /* global command handler */
{
	struct annodat adat;    /* data for ad to display */
	INT found;              /* 1 if records found, 0 if not */

	if ( sameas( margv[0], liststring ) )	{       /* list ad info */
		if ( !haskey(editkey) )	{       /* has no access to this */
			setmbk(annombk);
			prfmsg(SORRY);
			outprf(usrnum);
			rstmbk();
			return(1);
		}
		if ( ( margc == 2 ) && sameas( margv[1], "?" ) ) {       /* user wants help */
			setmbk(annombk);
			prfmsg(HELP, liststring, editstring, editstring);
			outprf(usrnum);
			rstmbk();
			return(1);
		}
		if ( margc == 1 ) {       /* display info on all ads */
			setmbk(annombk);
			dfaSetBlk(annobb);
			found=dfaAcqLO(&adat, 0);
			if ( found ) {       /* display appropriate header */
				prfmsg(RECS);
			} else {
				prfmsg(NORECS);
			}
			outprf(usrnum);
			while ( found ) {
				prfmsg(RECDAT, adat.adnum, adat.pathname);
				list_flags(adat.adtype);
				found=dfaAcqGT(&adat, &adat.adnum, 0);
			}
			dfaRstBlk();
			rstmbk();
			return(1);
		}
	}
	if ( sameas( margv[0], editstring ) ) {
		if ( !haskey(editkey) )	{       /* has no access to this */
			setmbk(annombk);
			prfmsg(SORRY);
			outprf(usrnum);
			rstmbk();
			return(1);
		}
		if ( ( margc == 2 ) && sameas( margv[1], "?" ) ) {       /* user wants help */
			setmbk(annombk);
			prfmsg(HELP, liststring, editstring, editstring);
			outprf(usrnum);
			rstmbk();
			return(1);
		}
		if ( margc == 2 )	{       /* check for existance of ad */
			setmbk(annombk);
			dfaSetBlk(annobb);
			if ( dfaAcqEQ(&adat, margv[1], 1) )	{       /* remove ad from use */
				num_ads--;
				dfaDelete();
				while ( dfaAcqGT(&adat, &adat.adnum, 0) )	{       /* make sure ads are contiguous */
					dfaDelete();
					adat.adnum--;
					dfaInsert(&adat);
				}
				prfmsg(RECDEL, margv[1]);
			} else {       /* add ad to database */
				if ( rsvnam(margv[1]) )	{       /* cannot use this file */
					prfmsg(RSVNAM, margv[1]);
				} else {       /* all is well */
					adat.adnum=num_ads;
					adat.adtype=0;
					strcpy(adat.pathname, margv[1]);
					num_ads++;
					dfaInsert(&adat);
					prfmsg(RECADD, margv[1]);
				}
			}
			outprf(usrnum);
			dfaRstBlk();
			rstmbk();
			return(1);
		}
		if ( margc > 2 ) {
			setmbk(annombk);
			dfaSetBlk(annobb);
			if ( dfaAcqEQ(&adat, margv[1], 1) ) {       /* edit existing ad */
				adat.adtype=get_flags();
				dfaUpdate(&adat);
				prfmsg(RECADD, margv[1]);
				list_flags(adat.adtype);
			} else {       /* add ad to database */
				if ( rsvnam(margv[1]) )	{       /* cannot use this file */
					prfmsg(RSVNAM, margv[1]);
					outprf(usrnum);
				} else {       /* all is well */
					adat.adnum=num_ads;
					strcpy(adat.pathname, margv[1]);
					adat.adtype=get_flags();
					num_ads++;
					dfaInsert(&adat);
					prfmsg(RECADD, margv[1]);
					list_flags(adat.adtype);
				}
			}
			rstmbk();
			dfaRstBlk();
			return(1);
		}
	}
	return(0);
}

VOID mark_seen( CHAR *pn )                      /* marks ad pn as seen by user today */
{       /* assumes othuap has been set and is user to mark as seeing ad */
	struct adseen as;       /* show that ad was seen today */
	INT found=0;            /* 1 if record already exists, 0 if not */
	INT records;            /* 1 if records found, 0 if not */

	dfaSetBlk(seenbb);
	records=dfaAcqEQ(&as, othuap->userid, 0);
	while ( records && !found ) {
		found=sameas(as.ad, pn);
		if ( found ) {
			dfaDelete();
		}
	   records=dfaAcqNX(&as);
	}
	strcpy(as.ad, pn);
	strcpy(as.userid, othuap->userid);
	as.dateseen=today();
	dfaInsertDup(&as);
	dfaRstBlk();
}

INT show_ad( CHAR *pn, CHAR flags )             /* returns 1 if show othuap ad pn, 0 if don't */
{       /* flags is flags of ad; requires that othuap, othusn, and othusp have been set */
	if ( othkey(pesterkey) ) {       /* has key to not see ad */
		return(0);
	}
	if ( (flags&ADULT) && (othuap->age < 18) ) {       /* show to adults only */
		return(0);
	}
	if ( (flags&MINOR) && (othuap->age > 17) ) {       /* show to minors only */
		return(0);
	}
	if ( (flags&MALE) && !(flags&FEMALE) && (othuap->sex == 'F' ) ) {       /* show to males only */
		return(0);
	}
	if ( (flags&FEMALE) && !(flags&MALE) && (othuap->sex == 'M' ) ) {       /* show to females only */
		return(0);
	}
	if ( (flags&PRIME) && !primetime() ) {       /* show during primetime only */
		return(0);
	}
	if ( (flags&CHANNEL) && !channel_ok() ) {       /* display only to certain channels */
		return(0);
	}
	if ( (flags&ONCE) && seen_ad(pn) ) {       /* display only once, already seen */
		return(0);
	}
	return(1);
}

INT primetime( VOID )                           /* return 1 if during "prime time", 0 if not */
{
	USHORT timenow;                         /* the time as returned by now() */

	timenow=now();
	if ( endtime < starttime )	{       /* "prime time" extends until next morning, so end less than start */
		if ( ( timenow >= starttime ) || ( timenow <= endtime ) ) {       /* "prime time" is in effect */
			return(1);
		} else {       /* not currently "prime time" */
			return(0);
		}
	} else {       /* "prime time" ends same day, so end greater than start */
		if ( ( timenow >= starttime ) && ( timenow <= endtime ) ) {       /* "prime time" is in effect */
			return(1);
		} else {       /* not currently "prime time" */
			return(0);
		}
	}
}

INT channel_ok( VOID )                          /* return 1 if user's channel is to be displayed to, 0 if not */
{
	INT othchan;            /* othusn's channel number */

	othchan=channel[othusn];
	if ( ( othchan < firstline ) || ( othchan > lastline ) ) {       /* channel is not in allowed range */
		return(0);
	}
   return(1);
}

INT seen_ad( CHAR *pn )                         /* returns 1 if othuap->userid has seen ad pn, 0 if not */
{       /* requires that othuap be set */
	struct adseen as;       /* ad data */
	INT found=0;            /* 1 if record already exists, 0 if not */
	USHORT ourdate;         /* date as returned by today() */
	INT records;            /* 1 if records found, 0 if not */

	ourdate=today();
	dfaSetBlk(seenbb);
	records=dfaAcqEQ(&as, othuap->userid, 0);
	while ( records && !found ) {       /* the ad has been seen, check for when */
		found=sameas(as.ad, pn);
		if ( !found ) {       /* check for next record */
			records=dfaAcqNX(&as);
		}
	}
	dfaRstBlk();
	if ( found && ( ourdate == as.dateseen ) ) {       /* has been seen today already */
		return(1);
	}
	return(0);
}

VOID list_flags( CHAR f )                       /* displays flags in f */
{
	INT showcount;          /* for looping thru each flag */

	if ( f == 0 ) {       /* nothing to show */
		outprf(usrnum);
		return;
	}
	setmbk(annombk);
	prfmsg(FLAGHDR);
	for ( showcount=0; showcount < NUMFLAGS; showcount++ ) {
		if ( (f&flag_values[showcount]) != 0 ) {       /* this flag is set */
			prfmsg(FLAG, flag_names[showcount]);
		}
	}
	rstmbk();
	prf("\n");
	outprf(usrnum);
}

CHAR get_flags( VOID )                          /* returns flags of ad as CHAR from input */
{       /* assumes input contains flags desired */
	INT arg_loop;           /* loop from 2 to margc-1 */
	INT flags_loop;         /* loop from 0 to NUMFLAGS-1 */
	INT found;              /* 1 if found a flag, 0 if not */
	CHAR returnflag=0;      /* flags value to return */

	for ( arg_loop=2; arg_loop < margc; arg_loop++ ) {       /* check each word for being a flag */
		found=0;
		flags_loop=0;
		while ( ( flags_loop < NUMFLAGS ) && !found ) {       /* check each flag */
			if ( sameas( margv[arg_loop], flag_names[flags_loop] ) )	{       /* found a flag */
				found=1;
				if ( (returnflag&flag_values[flags_loop]) == 0 ) {       /* make sure flag hasn't already been added */
					returnflag+=flag_values[flags_loop];
				}
			}
			flags_loop++;
		}
	}
	return(returnflag);
}

INT user_to( VOID )                             /* returns a usrnum in the range 0 to nterms-1 to display an ad to */
{
	INT userto;     /* usrnum to show ad to */

#ifdef __BUILDV10MODULE
	userto = (rand() % nterms);         // same idea as random(n) which is 0..n-1, but for Visual Studio
#else
	userto = random(nterms);            // returns 0 through nterms-1, but a Borland macro
#endif
	return(userto);
}

INT valid_viewer( VOID )                        /* returns 1 if othusn is a potential viewer of ad, 0 if not */
{       /* requires that othusn, othusp, and othuap be set prior to calling */
	INT fsestate;           /* usrptr->state for user in The Editor */

	fsestate=findmod(editorname);
	if ( ( ( othusp->usrcls == ACTUSR ) || ( othusp->usrcls == BBSPRV ) ) &&
		 isuidc(othuap->userid[0]) && ( othuap->userid[0] != '(' ) && ( ( othusp->state != fsestate ) || sendedit ) ) {
		return(1);
	}
	return(0);
}

VOID show_displays( CHAR *pn )                  /* shows ad displays in the Audit Trail and/or a text file */
{
	FILE *adfile;                         /* the pointer to the text file to show ad displays */
   CHAR path_buffer[GCSTRFNM];           /* the file name of the text file we couldn't open */

	if ( showaudit ) {       /* show ad displayed in Audit Trail */
		shocst("ELW Advertiser displayed ad", "Ad: %s", pn);
	}
	if ( showtext ) {       /* show ad displayed in text file */
		if ( (adfile=fopen(pathname, FOPAA)) != NULL ) {       /* file was opened, proceed */
			fputs(spr("%s %s Displayed ad: %s\n", ncdate(today()), nctime(now()), pn), adfile);
			fclose(adfile);
		} else {
         memset(path_buffer, 0, GCSTRFNM);
			shocst("ELW Advertiser Error", "Cannot open %s as a text file", fileparts(GCPART_FNAM, pathname, path_buffer, GCSTRFNM));
		}
	}
}
