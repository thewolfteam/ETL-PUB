/*
 * name:		cg_consolecmds.c
 *
 * desc:		text commands typed in at the local console, or executed by a key binding
 *
*/


#include "cg_local.h"

void CG_TargetCommand_f( void ) {
	int		targetNum;
	char	test[4];

	targetNum = CG_CrosshairPlayer();
	if (!targetNum ) {
		return;
	}

	trap_Argv( 1, test, 4 );
	trap_SendConsoleCommand( va( "gc %i %i", targetNum, atoi( test ) ) );
}

/*
=============
CG_Viewpos_f

Debugging command to print the current position
=============
*/
static void CG_Viewpos_f(void)
{
	CG_Printf ("(%i %i %i) : %i\n", (int)cg.refdef.vieworg[0],
		(int)cg.refdef.vieworg[1], (int)cg.refdef.vieworg[2], 
		(int)cg.refdefViewAngles[YAW]);
}

/**
 * @brief CG_LimboMenu_f
 */
void CG_LimboMenu_f(void)
{
	if (cg.showGameView)
	{
		CG_EventHandling( CGAME_EVENT_NONE, qfalse );
	}
	else
	{
		CG_EventHandling( CGAME_EVENT_GAMEVIEW, qfalse );
	}
}

/**
 * @brief CG_StatsDown_f
 */
static void CG_StatsDown_f(void)
{
	if (!cg.demoPlayback)
	{
		if (
#ifdef FEATURE_MULTIVIEW
			cg.mvTotalClients < 1 &&
#endif
			cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR)
		{
			Pri("You must be a player or following a player to use +stats\n");
			return;
		}

		if (cgs.gamestats.show == SHOW_SHUTDOWN && cg.time < cgs.gamestats.fadeTime)
		{
			cgs.gamestats.fadeTime = 2 * cg.time + STATS_FADE_TIME - cgs.gamestats.fadeTime;
		}
		else if (cgs.gamestats.show != SHOW_ON)
		{
			cgs.gamestats.fadeTime = cg.time + STATS_FADE_TIME;
		}

		cgs.gamestats.show = SHOW_ON;

		if (cgs.gamestats.requestTime < cg.time)
		{
			int i =
#ifdef FEATURE_MULTIVIEW
				(cg.mvTotalClients > 0) ? (cg.mvCurrentActive->mvInfo & MV_PID) :
#endif
				cg.snap->ps.clientNum;

			cgs.gamestats.requestTime = cg.time + 2000;
			trap_SendClientCommand(va("sgstats %d", i));
		}
	}
}

/**
 * @brief CG_StatsUp_f
 */
static void CG_StatsUp_f(void)
{
	if (cgs.gamestats.show == SHOW_ON)
	{
		cgs.gamestats.show = SHOW_SHUTDOWN;
		if (cg.time < cgs.gamestats.fadeTime)
		{
			cgs.gamestats.fadeTime = 2 * cg.time + STATS_FADE_TIME - cgs.gamestats.fadeTime;
		}
		else
		{
			cgs.gamestats.fadeTime = cg.time + STATS_FADE_TIME;
		}
	}
}

/**
 * @brief CG_topshotsDown_f
 */
void CG_topshotsDown_f(void)
{
	if (!cg.demoPlayback)
	{
		if (cgs.topshots.show == SHOW_SHUTDOWN && cg.time < cgs.topshots.fadeTime)
		{
			cgs.topshots.fadeTime = 2 * cg.time + STATS_FADE_TIME - cgs.topshots.fadeTime;
		}
		else if (cgs.topshots.show != SHOW_ON)
		{
			cgs.topshots.fadeTime = cg.time + STATS_FADE_TIME;
		}

		cgs.topshots.show = SHOW_ON;

		if (cgs.topshots.requestTime < cg.time)
		{
			cgs.topshots.requestTime = cg.time + 2000;
			trap_SendClientCommand("stshots");
		}
	}
}

/**
 * @brief CG_topshotsUp_f
 */
void CG_topshotsUp_f(void)
{
	if (cgs.topshots.show == SHOW_ON)
	{
		cgs.topshots.show = SHOW_SHUTDOWN;
		if (cg.time < cgs.topshots.fadeTime)
		{
			cgs.topshots.fadeTime = 2 * cg.time + STATS_FADE_TIME - cgs.topshots.fadeTime;
		}
		else
		{
			cgs.topshots.fadeTime = cg.time + STATS_FADE_TIME;
		}
	}
}

/**
 * @brief CG_objectivesDown_f
 */
void CG_objectivesDown_f(void)
{
	if (!cg.demoPlayback)
	{
		if (cgs.objectives.show == SHOW_SHUTDOWN && cg.time < cgs.objectives.fadeTime)
		{
			cgs.objectives.fadeTime = 2 * cg.time + STATS_FADE_TIME - cgs.objectives.fadeTime;
		}
		else if (cgs.objectives.show != SHOW_ON)
		{
			cgs.objectives.fadeTime = cg.time + STATS_FADE_TIME;
		}
		cgs.objectives.show = SHOW_ON;
	}
}

/**
 * @brief CG_objectivesUp_f
 */
void CG_objectivesUp_f(void)
{
	if (cgs.objectives.show == SHOW_ON)
	{
		cgs.objectives.show = SHOW_SHUTDOWN;
		if (cg.time < cgs.objectives.fadeTime)
		{
			cgs.objectives.fadeTime = 2 * cg.time + STATS_FADE_TIME - cgs.objectives.fadeTime;
		}
		else
		{
			cgs.objectives.fadeTime = cg.time + STATS_FADE_TIME;
		}
	}
}


static void CG_Scoreboard_f(void) 
{
	int sb = cg_scoreboard.integer + 1;
	int i = 0;
	int allowed = SCOREBOARD_XP;
		
	if(sb < SCOREBOARD_XP)
		sb = SCOREBOARD_XP;

	// tjw: cycle through scoreboards until we find one that's allowed
	//      on this server
	for(i = SCOREBOARD_XP; i <= SCOREBOARD_KR; i++) {
		if(sb > SCOREBOARD_KR)
			sb = SCOREBOARD_XP;
		if(sb == SCOREBOARD_XP) {
			allowed = SCOREBOARD_XP;
			break;
		}
		if(sb == SCOREBOARD_PR && cgs.playerRating) {
			allowed = SCOREBOARD_PR;
			break;
		}
		if(sb == SCOREBOARD_KR && cgs.killRating) {
			allowed = SCOREBOARD_KR;
			break;
		}
		sb++;
	}
	trap_Cvar_Set("cg_scoreboard", va("%i", allowed));
}

void CG_ScoresDown_f( void ) {

	if(cg_scoresDoubleTap.integer &&
		!cg.showScores &&
		cg.scoresDownTime + 250 > cg.time &&
		cg.scoreToggleTime < (cg.time - 500)) {

		// tjw: cycle scoreboard type with a quick tap of +scores
		CG_Scoreboard_f();
		cg.scoreToggleTime = cg.time;
	}
	cg.scoresDownTime = cg.time;

	if (cg.scoresRequestTime + 2000 < cg.time)
	{
		// the scores are more than two seconds out of data,
		// so request new ones
		cg.scoresRequestTime = cg.time;

		// OSP - we get periodic score updates if we are merging clients
		if(!cg.demoPlayback && cg.mvTotalClients < 1) trap_SendClientCommand( "score" );

		// leave the current scores up if they were already
		// displayed, but if this is the first hit, clear them out
		if ( !cg.showScores ) {
			cg.showScores = qtrue;
			// quad - do not clear the scores if we're a spectator connected on the ETTV
			if(!cg.demoPlayback && cg.mvTotalClients < 1 && !cgs.clientinfo[cg.clientNum].ettv) 
				cg.numScores = 0;
		}
		}
	}
	else
	{
		// show the cached contents even if they just pressed if it
		// is within two seconds
		cg.showScores = qtrue;
	}
}

void CG_ScoresUp_f( void ) {
	if ( cg.showScores ) {
		cg.showScores = qfalse;
		cg.scoreFadeTime = cg.time;
	}
}

// quad - show list of huds
// chaplja - fixed the function, it was printing some strange stuff
char *CG_getHudList()
{
	static char hudText[0x2000] = "";
	char hudList[0x2000];
	char *hudName;
	int numHuds, c = 0, len;

	memset( hudList, 0, sizeof( hudList ) );
	memset( hudText, 0, sizeof( hudText ) );
	numHuds = trap_FS_GetFileList("hud", ".hud", hudList, sizeof(hudList));

	hudName = &hudList[0];

	if ( numHuds ) {
		for ( ; c < numHuds ; c++ ) {
			len = strlen( hudName );

			// Sanity check
			if ( len <= 4 ) {
				hudName += (len + 1);
				continue;
			}

			hudName[len-4] = ' ';
			hudName[len-3] = '\0';

			Q_strcat( hudText, sizeof( hudText ), hudName );

			hudName += (len + 1);
		}
	}
	
	return hudText;
}

static void CG_LoadHud_f( void ) {
	char filename[MAX_TOKEN_CHARS];

	if ( trap_Argc() < 2 ) {
		CG_LoadDefaultHud();
		CG_Printf("^2Loaded default HUD. Type ^3/loadhud ? ^2to show a list of available huds.\n");
	} else {
		trap_Argv(1, filename, sizeof(filename) );
		if (filename[0] == '?') {
			CG_Printf("^3Available HUDs are:^2 %s\n", CG_getHudList());
		} else {
			trap_Cvar_Set("cg_hud", filename);
			if (CG_LoadHud( filename ))
				CG_Printf("^2Loaded HUD settings from '%s'\n", filename);
			else
				CG_Printf("^1Failed ^2to load HUD settings from '%s', loaded default HUD\nType ^3/loadHud ?^2 to show a list.", filename);
		}
	}
//	String_Init();
//	Menu_Reset();
//	CG_LoadMenus("ui/hud.txt");
}

static void CG_EditHud_f( void ) 
{
	CG_EditHud();
}

static void CG_DumpHud_f( void )
{
	CG_DumpHud();
}

static void CG_LoadWeapons_f( void ) {
	int i;

	for( i = WP_KNIFE; i < WP_NUM_WEAPONS; i++ ) {
		// DHM - Nerve :: Only register weapons we use in WolfMP
		if( BG_WeaponInWolfMP( i ) )
			CG_RegisterWeapon( i, qtrue );
	}	
}

/*
static void CG_InventoryDown_f( void ) {
	cg.showItems = qtrue;
}

static void CG_InventoryUp_f( void ) {
	cg.showItems = qfalse;
	cg.itemFadeTime = cg.time;
}
*/

static void CG_TellTarget_f( void ) {
	int		clientNum;
	char	command[128];
	char	message[128];

	clientNum = CG_CrosshairPlayer();
	if ( clientNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "tell %i %s", clientNum, message );
	trap_SendClientCommand( command );
}

static void CG_TellAttacker_f( void ) {
	int		clientNum;
	char	command[128];
	char	message[128];

	clientNum = CG_LastAttacker();
	if ( clientNum == -1 ) {
		return;
	}

	trap_Args( message, 128 );
	Com_sprintf( command, 128, "tell %i %s", clientNum, message );
	trap_SendClientCommand( command );
}

/////////// cameras

#define MAX_CAMERAS 64	// matches define in splines.cpp
qboolean cameraInuse[MAX_CAMERAS];

int CG_LoadCamera(const char *name) {
	int i;
	for(i=1;i<MAX_CAMERAS;i++) {	// start at '1' since '0' is always taken by the cutscene camera
		if(!cameraInuse[i]) {
			if(trap_loadCamera(i, name)) {
				cameraInuse[i] = qtrue;
				return i;
			}
		}
	}
	return -1;
}

void CG_FreeCamera(int camNum){
	cameraInuse[camNum] = qfalse;
}

// @TEST.  See if we can get an initial camera started at the first frame.
char g_initialCamera[256] = "";
qboolean g_initialCameraStartBlack = qfalse;

/*
==============
CG_SetInitialCamera
==============
*/
void CG_SetInitialCamera( const char *name, qboolean startBlack )  {
	// Store this info to get reset after first snapshot inited
	Q_strncpyz(g_initialCamera, name, sizeof(g_initialCamera));
	g_initialCameraStartBlack = startBlack;
}


/*
==============
CG_StartCamera
==============
*/
void CG_StartCamera( const char *name, qboolean startBlack ) {
	char lname[MAX_QPATH];

	//if ( cg.predictedPlayerState.stats[STAT_HEALTH] <= 0 )	// don't allow camera to start if you're dead
	//	return;

	COM_StripExtensionSafe(name, lname, sizeof(lname));
	strcat( lname, ".camera" );

	if (trap_loadCamera(CAM_PRIMARY, va("cameras/%s", lname))) {
		cg.cameraMode = qtrue;					// camera on in cgame
		if(startBlack)
			CG_Fade(0, 0, 0, 255, cg.time, 0);	// go black
		trap_Cvar_Set( "cg_letterbox", "1" );	// go letterbox
		//trap_SendClientCommand("startCamera");	// camera on in game
		trap_startCamera(CAM_PRIMARY, cg.time);	// camera on in client
	} else {
//----(SA)	removed check for cams in main dir
		cg.cameraMode = qfalse;					// camera off in cgame
		trap_SendClientCommand("stopCamera");	// camera off in game
		trap_stopCamera(CAM_PRIMARY);			// camera off in client
		CG_Fade(0, 0, 0, 0, cg.time, 0);		// ensure fadeup
		trap_Cvar_Set( "cg_letterbox", "0" );
		CG_Printf ("Unable to load camera %s\n",lname);
	}
}

/*
==============
CG_StartInitialCamera
==============
*/
void CG_StartInitialCamera( )  {
	// See if we've got a camera name
	if (g_initialCamera[0] != 0) {
		// Start a camera with the initial data we stored.
		CG_StartCamera( g_initialCamera, g_initialCameraStartBlack );

		// Clear it now so we don't get re-entrance problems
		g_initialCamera[0] = 0;
		g_initialCameraStartBlack = qfalse;

	} // if (g_initialCamera[0] != 0)...
}

/*
==============
CG_StopCamera
==============
*/
void CG_StopCamera( void ) {
	cg.cameraMode = qfalse;					// camera off in cgame
	trap_SendClientCommand("stopCamera");	// camera off in game
	trap_stopCamera(CAM_PRIMARY);			// camera off in client
	trap_Cvar_Set( "cg_letterbox", "0" );

	// fade back into world
	CG_Fade(0, 0, 0, 255, 0, 0);
	CG_Fade(0, 0, 0, 0, cg.time + 500, 2000);

}


static void CG_Fade_f(void)
{
	int r, g, b, a, duration;

	if (trap_Argc() < 6)
	{
		return;
	}

	r = (int)atof(CG_Argv(1));
	g = (int)atof(CG_Argv(2));
	b = (int)atof(CG_Argv(3));
	a = (int)atof(CG_Argv(4));

	duration = (int)(atof(CG_Argv(5)) * 1000);

	CG_Fade(r, g, b, a, cg.time, duration);
}

// kw: TODO: make this menu dynamic, showing spawnpoint names
void CG_SpawnSelectMenu_f(void) {
	CG_EventHandling( CGAME_EVENT_NONE, qfalse );
	trap_UI_Popup( UIMENU_WM_SPAWNALT );
	trap_SendConsoleCommand( "vstr select_spawnpoint;");
}

void CG_TeamSelectMenu_f(void) {
	CG_EventHandling( CGAME_EVENT_NONE, qfalse );
	trap_UI_Popup( UIMENU_WM_TEAMALT );
	trap_SendConsoleCommand( "vstr select_team;");
}

void CG_ClassSelectMenu_f(void) {
	CG_EventHandling( CGAME_EVENT_NONE, qfalse );
	trap_UI_Popup( UIMENU_WM_CLASSALT );
	trap_SendConsoleCommand( "vstr select_class;");
}

void CG_QuickMessage_f( void ) {
	if( cgs.clientinfo[ cg.clientNum ].team == TEAM_SPECTATOR &&
		cgs.etlpub < ETLPUB_VERSION(0,1,0) ) {
		return;
	}

	CG_EventHandling( CGAME_EVENT_NONE, qfalse );

	if( cg_quickMessageAlt.integer ) {
		trap_UI_Popup( UIMENU_WM_QUICKMESSAGEALT );
	}
	else
	{
		trap_UI_Popup( UIMENU_WM_QUICKMESSAGE );
	}
}

/**
 * @brief CG_QuickFireteamMessage_f
 */
void CG_QuickFireteamMessage_f(void)
{
	if (cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR || cgs.clientinfo[cg.clientNum].team == TEAM_FREE)
	{
		return;
	}

	CG_EventHandling( CGAME_EVENT_NONE, qfalse );

	if (cg_quickMessageAlt.integer)
	{
		trap_UI_Popup( UIMENU_WM_FTQUICKMESSAGEALT );
	}
	else
	{
		trap_UI_Popup( UIMENU_WM_FTQUICKMESSAGE );
	}
}

/**
 * @brief CG_QuickFireteamAdmin_f
 */
void CG_QuickFireteamAdmin_f(void)
{
	trap_UI_Popup( UIMENU_NONE );
	
	if (cg.showFireteamMenu)
	{
		if (cgs.ftMenuMode == 1)
		{
			CG_EventHandling( CGAME_EVENT_NONE, qfalse );
		}
		else
		{
			cgs.ftMenuMode = 1;
		}
	}
	else if (cgs.clientinfo[cg.clientNum].team != TEAM_SPECTATOR)
	{
		CG_EventHandling( CGAME_EVENT_FIRETEAMMSG, qfalse );
		cgs.ftMenuMode = 1;
	}
}

/**
 * @brief CG_QuickFireteams_f
 */
static void CG_QuickFireteams_f(void)
{
	if (!CG_IsOnFireteam(cg.clientNum))
	{
		return;
	}

	if (cg.showFireteamMenu)
	{
		if (cgs.ftMenuMode == 0)
		{
			CG_EventHandling( CGAME_EVENT_NONE, qfalse );
		}
		else
		{
			cgs.ftMenuMode = 0;
		}
	}
 else if( CG_IsOnFireteam( cg.clientNum ) ) 
{
		CG_EventHandling( CGAME_EVENT_FIRETEAMMSG, qfalse );
		cgs.ftMenuMode = 0;
	}

/**
 * @brief CG_FTSayPlayerClass_f
 */
static void CG_FTSayPlayerClass_f(void)
{
	int        playerType = cgs.clientinfo[cg.clientNum].cls;
	const char *s;

	if ( playerType == PC_MEDIC )
	{
		s = "IamMedic";
	}
	else if ( playerType == PC_ENGINEER )
	{
		s = "IamEngineer";
	}
	else if ( playerType == PC_FIELDOPS )
	{
		s = "IamFieldOps";
	}
	else if ( playerType == PC_COVERTOPS )
	{
		s = "IamCovertOps";
	}
	else
	{
		s = "IamSoldier";
	}

	if ( cg.snap && cg.snap->ps.pm_type != PM_INTERMISSION ) {
		if ( cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR || 
				cgs.clientinfo[cg.clientNum].team == TEAM_FREE ) {
			CG_Printf ( CG_TranslateString( "Can't buddy voice chat as a spectator.\n" ) );
			return;
		}
	}

	trap_SendConsoleCommand( va( "cmd vsay_buddy -1 %s %s\n", CG_BuildSelectedFirteamString(), s ) );
}

/**
 * @brief CG_SayPlayerClass_f
 */
static void CG_SayPlayerClass_f( void )
{
	int        playerType = cgs.clientinfo[cg.clientNum].cls;
	const char *s;

	if ( playerType == PC_MEDIC )
	{
		s = "IamMedic";
	}
	else if ( playerType == PC_ENGINEER )
	{
		s = "IamEngineer";
	}
	else if ( playerType == PC_FIELDOPS )
	{
		s = "IamFieldOps";
	}
	else if ( playerType == PC_COVERTOPS )
	{
		s = "IamCovertOps";
	}
	else
	{
		s = "IamSoldier";
	}

	// kw: still keeping this disabled for spectators.
	if (cg.snap && (cg.snap->ps.pm_type != PM_INTERMISSION))
	{
		if (cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR || cgs.clientinfo[cg.clientNum].team == TEAM_FREE)
		{
			CG_Printf("%s", CG_TranslateString("Can't team voice chat as a spectator.\n"));
			return;
		}
	}

	trap_SendConsoleCommand( va( "cmd vsay_team %s\n", s ) );
}

/**
 * @brief CG_VoiceChat_f
 */
static void CG_VoiceChat_f(void)
{
	char chatCmd[64];

	if ( trap_Argc() != 2 )
	{
		return;
	}

	trap_Argv( 1, chatCmd, 64 );
	trap_SendConsoleCommand( va( "cmd vsay %s\n", chatCmd ) );
}

/**
 * @brief CG_TeamVoiceChat_f
 */
static void CG_TeamVoiceChat_f(void)
{
	char chatCmd[64];

	if (trap_Argc() != 2)
	{
		return;
	}

	// NERVE - SMF - don't let spectators voice chat
	// NOTE - This cg.snap will be the person you are following, but its just for intermission test
	if (cg.snap && (cg.snap->ps.pm_type != PM_INTERMISSION))
	{
		if (cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR || cgs.clientinfo[cg.clientNum].team == TEAM_FREE)
		{
			CG_Printf("%s", CG_TranslateString("Can't team voice chat as a spectator.\n")); // FIXME? find a way to print this on screen
			return;
		}
	}

	trap_Argv( 1, chatCmd, 64 );

	trap_SendConsoleCommand( va( "cmd vsay_team %s\n", chatCmd ) );
}

/**
 * @brief CG_BuddyVoiceChat_f
 */
static void CG_BuddyVoiceChat_f(void)
{
	char chatCmd[64];

	if (trap_Argc() != 2)
	{
		return;
	}

	// NERVE - SMF - don't let spectators voice chat
	// NOTE - This cg.snap will be the person you are following, but its just for intermission test
	if (cg.snap && (cg.snap->ps.pm_type != PM_INTERMISSION))
	{
		if (cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR || cgs.clientinfo[cg.clientNum].team == TEAM_FREE)
		{
			CG_Printf("%s", CG_TranslateString("Can't buddy voice chat as a spectator.\n"));
			return;
		}
	}

	trap_Argv( 1, chatCmd, 64 );

	trap_SendConsoleCommand( va( "cmd vsay_buddy -1 %s %s\n", CG_BuildSelectedFirteamString(), chatCmd ) );
}

/**
 * @brief CG_MessageMode_f
 * @details say, team say, etc
 */
static void CG_MessageMode_f(void)
{
	char	cmd[ 64 ];	

	if (cgs.eventHandling != CGAME_EVENT_NONE)
	{
		return;
	}
	
	// get the actual command
	trap_Argv( 0, cmd, 64 );
	
	// team say
	if( !Q_stricmp( cmd, "messagemode2" ) )
	{
		trap_Cvar_Set( "cg_messageType", "2" );
	}
	// fireteam say
	else if (!Q_stricmp(cmd, "messagemode3") && CG_IsOnFireteam(cg.clientNum))
	{
		trap_Cvar_Set( "cg_messageType", "3" );
	}
	// (normal) say
	else
	{
		trap_Cvar_Set( "cg_messageType", "1" );
	}
	
	// clear the chat text
	trap_Cvar_Set( "cg_messageText", "" );
	
	// open the menu
	trap_UI_Popup( UIMENU_INGAME_MESSAGEMODE );
}

/**
 * @brief CG_MessageSend_f
 */
static void CG_MessageSend_f( void )
{
	char	messageText[ 256 ];
	int		messageType;
	
	// get values
	trap_Cvar_VariableStringBuffer( "cg_messageType", messageText, 256 );
	messageType = atoi( messageText );
	trap_Cvar_VariableStringBuffer( "cg_messageText", messageText, 256 );
	
	// reset values
	trap_Cvar_Set( "cg_messageText", "" );
	trap_Cvar_Set( "cg_messageType", "" );
	 
	// don't send empty messages
	if( messageText[ 0 ] == '\0' )
	{
		return;
	}
	
	if (messageType == 2) // team say
	{
		trap_SendConsoleCommand( va( "say_team \"%s\"\n", messageText ) );
	}
	else if (messageType == 3) // fireteam say
	{
		trap_SendConsoleCommand( va( "say_buddy \"%s\"\n", messageText ) );
	}
	else // normal say
	{
		trap_SendConsoleCommand( va( "say \"%s\"\n", messageText ) );
	}
}

/**
 * @brief CG_SetWeaponCrosshair_f
 */
static void CG_SetWeaponCrosshair_f(void)
{
	char crosshair[64];

	trap_Argv( 1, crosshair, 64 );
	cg.newCrosshairIndex = atoi( crosshair ) + 1;
}

/**
 * @brief CG_SelectBuddy_f
 */
static void CG_SelectBuddy_f(void)
{
	int pos = atoi( CG_Argv( 1 ) );
	int i;
	clientInfo_t* ci;

	// 0 - 7 = select that person
	// -1 = none
	// -2 = all
	switch (pos)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		if (!CG_IsOnFireteam(cg.clientNum))
		{
			break;     // we aren't a leader, so dont allow selection
		}

		ci = CG_SortedFireTeamPlayerForPosition(pos);
		if (!ci)
		{
			break;     // there was no-one in this position
		}

		ci->selected ^= qtrue;
		break;

		case -1:
		if (!CG_IsOnFireteam(cg.clientNum))
		{
			break;     // we aren't a leader, so dont allow selection
		}

		for (i = 0; i < MAX_FIRETEAM_MEMBERS; i++)
		{
				ci = CG_SortedFireTeamPlayerForPosition( i );
			if (!ci)
			{
				break;     // there was no-one in this position
				}

				ci->selected = qfalse;
			}
		break;

		case -2:
		if (!CG_IsOnFireteam(cg.clientNum))
		{
			break;     // we aren't a leader, so dont allow selection
		}

		for (i = 0; i < MAX_FIRETEAM_MEMBERS; i++)
		{
				ci = CG_SortedFireTeamPlayerForPosition( i );
			if (!ci)
			{
				break;     // there was no-one in this position
				}

				ci->selected = qtrue;
			}
		break;
	}
}

extern void CG_AdjustAutomapZoom(int zoomIn);

/**
 * @brief CG_AutomapZoomIn_f
 */
static void CG_AutomapZoomIn_f(void)
{
	if (!cgs.autoMapOff)
	{
		CG_AdjustAutomapZoom(qtrue);
	}
}

/**
 * @brief CG_AutomapZoomOut_f
 */
static void CG_AutomapZoomOut_f(void)
{
	if (!cgs.autoMapOff)
	{
		CG_AdjustAutomapZoom(qfalse);
	}
}

/**
 * @brief CG_AutomapExpandDown_f
 */
static void CG_AutomapExpandDown_f(void)
{
	if (!cgs.autoMapExpanded)
	{
		cgs.autoMapExpanded = qtrue;
		if( cg.time - cgs.autoMapExpandTime < 250.f ) 
{
			cgs.autoMapExpandTime = cg.time - ( 250.f - ( cg.time - cgs.autoMapExpandTime ) );
		}
        else 
        {
			cgs.autoMapExpandTime = cg.time;
		}
	}
}

/**
 * @brief CG_AutomapExpandUp_f
 */
static void CG_AutomapExpandUp_f(void)
{
	if (cgs.autoMapExpanded)
	{
		cgs.autoMapExpanded = qfalse;
		if( cg.time - cgs.autoMapExpandTime < 250.f ) {
			cgs.autoMapExpandTime = cg.time - ( 250.f - ( cg.time - cgs.autoMapExpandTime ) );
		} 
        else 
        {
			cgs.autoMapExpandTime = cg.time;
		}
	}
}

/**
 * @brief CG_ToggleAutomap_f
 */
static void CG_ToggleAutomap_f(void)
{
	cgs.autoMapOff = (qboolean) !cgs.autoMapOff;
}

const char *aMonths[12] =
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

/**
 * @brief CG_currentTime_f
 */
void CG_currentTime_f(void)
{
	qtime_t ct;

	trap_RealTime(&ct);
	CG_Printf("[cgnotify]Current time: ^3%02d:%02d:%02d (%02d %s %d)\n", ct.tm_hour, ct.tm_min, ct.tm_sec, ct.tm_mday, aMonths[ct.tm_mon], 1900 + ct.tm_year);
}

// Dynamically names a demo and sets up the recording
void CG_autoRecord_f( void ) {
	// kw: works for pre-2.60 patches too
	if( cg.demoRecording || cl_demorecording.integer ) {
		trap_SendConsoleCommand("stoprecord\n");
		cg.demoRecording = qfalse;
	} else {	
		trap_SendConsoleCommand(va("record %s\n", CG_generateFilename()));
		cg.demoRecording = qtrue;
	}
}

/**
 * @brief Dynamically names a screenshot[JPEG]
 */
void CG_autoScreenShot_f(void)
{
	trap_SendConsoleCommand(va("screenshot%s %s\n", ((cg_useScreenshotJPEG.integer)?"JPEG":""), CG_generateFilename()));
}

/**
 * @brief CG_vstrDown_f
 */
void CG_vstrDown_f(void)
{
	// The engine also passes back the key code and time of the key press
	if (trap_Argc() == 5)
	{
		trap_SendConsoleCommand(va("vstr %s;", CG_Argv(1)));
	}
	else
	{
		CG_Printf("[cgnotify]Usage: +vstr [down_vstr] [up_vstr]\n");
	}
}

/**
 * @brief CG_vstrUp_f
 */
void CG_vstrUp_f(void)
{
	// The engine also passes back the key code and time of the key press
	if (trap_Argc() == 5)
	{
		trap_SendConsoleCommand(va("vstr %s;", CG_Argv(2)));
	}
	else
	{
		CG_Printf("[cgnotify]Usage: +vstr [down_vstr] [up_vstr]\n");
	}
}

void CG_keyOn_f(void)
{
	if (!cg.demoPlayback)
	{
		CG_Printf("[cgnotify]^3*** NOT PLAYING A DEMO!!\n");
		return;
	}

	if (demo_infoWindow.integer > 0)
	{
		CG_ShowHelp_On(&cg.demohelpWindow);
	}

	CG_EventHandling(CGAME_EVENT_DEMO, qtrue);
}

/**
 * @brief CG_keyOff_f
 */
void CG_keyOff_f(void)
{
	if (!cg.demoPlayback)
	{
		return;
	}
	CG_EventHandling(CGAME_EVENT_NONE, qfalse);
}

void CG_dumpStats_f(void)
{
	if (cgs.dumpStatsTime < cg.time)
	{
		cgs.dumpStatsTime = cg.time + 2000;
		trap_SendClientCommand(
#ifdef FEATURE_MULTIVIEW
			(cg.mvTotalClients < 1) ?
#endif
			"weaponstats"
#ifdef FEATURE_MULTIVIEW
			: "statsall"
#endif
			);
	}
}

/*
 * @brief CG_wStatsDown_f
 * @note Unused
void CG_wStatsDown_f(void)
{
    if (
#ifdef FEATURE_MULTIVIEW
        cg.mvTotalClients < 1 &&
#endif
        cg.snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR)
    {
		Pri("You must be a player or following a player to use +wstats\n");
		return;
	}

    if (cg.statsRequestTime < cg.time)
    {
        int i =
#ifdef FEATURE_MULTIVIEW
            (cg.mvTotalClients > 0) ? (cg.mvCurrentActive->mvInfo & MV_PID) :
#endif
            cg.snap->ps.clientNum;

		cg.statsRequestTime = cg.time + 500;
		trap_SendClientCommand(va("wstats %d", i));
	}

	cg.showStats = qtrue;
}
*/

/*
 * @brief CG_wStatsUp_f
 * @note unused
void CG_wStatsUp_f(void)
{
	cg.showStats = qfalse;
	CG_windowFree(cg.statsWindow);
	cg.statsWindow = NULL;
}
*/

#ifdef FEATURE_MULTIVIEW
/**
 * @brief CG_toggleSpecHelp_f
 */
void CG_toggleSpecHelp_f(void)
{
	if (cg.mvTotalClients > 0 && !cg.demoPlayback)
	{
		if (cg.spechelpWindow != SHOW_ON && cg_specHelp.integer > 0)
		{
			CG_ShowHelp_On(&cg.spechelpWindow);
		}
		else if (cg.spechelpWindow == SHOW_ON)
		{
			CG_ShowHelp_Off(&cg.spechelpWindow);
		}
	}
}
#endif

/**
 * @brief CG_EditSpeakers_f
 */
// -OSP

void CG_Obj_f( void ) {
	// Gordon: short circuit this
}

static void CG_EditSpeakers_f( void )
{
	if (cg.editingSpeakers)
	{
		CG_DeActivateEditSoundMode();
	}
	else
	{
		const char *s = Info_ValueForKey( CG_ConfigString( CS_SYSTEMINFO ), "sv_cheats" );

		if (s[0] != '1')
		{
			CG_Printf( "editSpeakers is cheat protected.\n" );
			return;
		}
		CG_ActivateEditSoundMode();
	}
}

/**
 * @brief CG_DumpSpeaker_f
 */
static void CG_DumpSpeaker_f( void )
{
/*	char sscrfilename[MAX_QPATH];
	char soundfile[MAX_STRING_CHARS];
	int i, wait, random;
	char *extptr, *buffptr;
	fileHandle_t f;

		// Check for argument
	if( trap_Argc() < 2 || trap_Argc() > 4 )
	{
		CG_Printf( "Usage: dumpspeaker <soundfile> ( <wait=value>|<random=value> )\n" );
		return;
	}

	wait = random = 0;

	// parse the other parameters
	for( i = 2; i < trap_Argc(); i++ ) {
		char *valueptr = NULL;

		trap_Argv( i, soundfile, sizeof(soundfile) );

		for( buffptr = soundfile; *buffptr; buffptr++ ) {
			if( *buffptr == '=' ) {
				valueptr = buffptr + 1;
				break;
			}
		}

		Q_strncpyz( soundfile, soundfile, buffptr - soundfile + 1 );

		if( !Q_stricmp( soundfile, "wait" ) )
			wait = atoi( valueptr );
		else if( !Q_stricmp( soundfile, "random" ) )
			random = atoi( valueptr );
	}

	// parse soundfile
	trap_Argv( 1, soundfile, sizeof(soundfile) );

	// Open soundfile
	Q_strncpyz( sscrfilename, cgs.mapname, sizeof(sscrfilename) );
	extptr = sscrfilename + strlen( sscrfilename ) - 4;
	if( extptr < sscrfilename || Q_stricmp( extptr, ".bsp" ) )
	{
		CG_Printf( "Unable to dump, unknown map name?\n" );
		return;
	}
	Q_strncpyz( extptr, ".sscr", 5 );
	trap_FS_FOpenFile( sscrfilename, &f, FS_APPEND_SYNC );
	if( !f )
	{
		CG_Printf( "Failed to open '%s' for writing.\n", sscrfilename );
		return;
	}

		// Build the entity definition
	Com_sprintf( soundfile, sizeof(soundfile), "{\n\"classname\" \"target_speaker\"\n\"origin\" \"%i %i %i\"\n\"noise\" \"%s\"\n",
												(int) cg.snap->ps.origin[0], (int) cg.snap->ps.origin[1], (int) cg.snap->ps.origin[2], soundfile );

	if( wait ) {
		Q_strcat( soundfile, sizeof(soundfile), va( "\"wait\" \"%i\"\n", wait ) );
	}
	if( random ) {
		Q_strcat( soundfile, sizeof(soundfile), va( "\"random\" \"%i\"\n", random ) );
	}
	Q_strcat( soundfile, sizeof(soundfile), "}\n\n" );

		// And write out/acknowledge
	trap_FS_Write( soundfile, strlen( soundfile ), f );
	trap_FS_FCloseFile( f );
	CG_Printf( "Entity dumped to '%s' (%i %i %i).\n", sscrfilename,
			   (int) cg.snap->ps.origin[0], (int) cg.snap->ps.origin[1], (int) cg.snap->ps.origin[2] );*/
	bg_speaker_t	speaker;
	trace_t			tr;
	vec3_t			end;

	if (!cg.editingSpeakers)
	{
		CG_Printf( "Speaker Edit mode needs to be activated to dump speakers\n" );
		return;
	}

	memset( &speaker, 0, sizeof(speaker) );

	speaker.volume = 127;
	speaker.range = 1250;

	VectorMA( cg.refdef_current->vieworg, 32, cg.refdef_current->viewaxis[0], end );
	CG_Trace( &tr, cg.refdef_current->vieworg, NULL, NULL, end, -1, MASK_SOLID );

	if (tr.fraction < 1.f)
	{
		VectorCopy( tr.endpos, speaker.origin );
		VectorMA( speaker.origin, -4, cg.refdef_current->viewaxis[0], speaker.origin );
	}
	else
	{
		VectorCopy( tr.endpos, speaker.origin );
	}

	if (!BG_SS_StoreSpeaker(&speaker))
	{
		CG_Printf( S_COLOR_RED "ERROR: Failed to store speaker\n" );
	}
}

/**
 * @brief CG_ModifySpeaker_f
 */
static void CG_ModifySpeaker_f( void )
{
	if (cg.editingSpeakers)
	{
		CG_ModifyEditSpeaker();
	}
}

/**
 * @brief CG_UndoSpeaker_f
 */
static void CG_UndoSpeaker_f( void )
{
	if (cg.editingSpeakers)
	{
		CG_UndoEditSpeaker();
	}
}

/**
 * @brief CG_ForceTapOut_f
 */
void CG_ForceTapOut_f(void)
{
	trap_SendClientCommand( "forcetapout" );
}

static void CG_CPM_f( void ) {
	CG_AddPMItem( PM_MESSAGE, CG_Argv(1), cgs.media.voiceChatShader, NULL );
}

static void CG_Nop_f(void)
{
}

static void CG_HideVote_f(void)
{
	cgs.hidevote = (cgs.hidevote) ? 0 : cgs.voteTime;
}

qboolean resetmaxspeed = qfalse;

static void CG_ResetMaxSpeed_f(void)
{
	resetmaxspeed = qtrue;
}

static void CG_LoadFont_f(void)
{
	char buff[32] = {""};
	char name[255] = {""};
	int size = 0;
	int num = 0;

	if(trap_Argc() < 2) {
		CG_Printf("loadfont: usage "
			"/loadfont <1.2.3.4> <fontname> <size>\n"
			"/loadfont <1.2.3.4> default\n"
			"/loadfont defaults\n");
		return;
	}
	trap_Argv(1, buff, sizeof(buff));
	if(!Q_stricmp(buff, "defaults")) {
		CG_LoadFont(1, "ariblk", 27);
		CG_LoadFont(2, "ariblk", 16);
		CG_LoadFont(3, "courbd", 30);
		CG_LoadFont(4, "courbd", 21);
		return;
	}
	else {
		num = atoi(buff);
		if(num < 1 || num > 4) {
			CG_Printf("loadfont: font number must be 1, 2, 3, or 4\n");
			return;
		}
		trap_Argv(2, name, sizeof(name));
		if(!Q_stricmp(name, "default")) {
			switch(num) {
			case 1:
				Q_strncpyz(name, "ariblk", sizeof(name));
				size = 27;
				break;
			case 2:
				Q_strncpyz(name, "ariblk", sizeof(name));
				size = 16;
				break;
			case 3:
				Q_strncpyz(name, "courbd", sizeof(name));
				size = 30;
				break;
			default:
				Q_strncpyz(name, "courbd", sizeof(name));
				size = 21;
			}
		}
		else {
			buff[0] = '\0';
			trap_Argv(3, buff, sizeof(buff));
			size = atoi(buff);
		}
	}
	if(!CG_LoadFont(num, name, size)) {
		CG_Printf("loadfont: could not load "
			"fonts/%s_%i.dat\n", name, size);
	}
}

// quad: etpro style enemy spawntimer
void CG_TimerSet_f( void )
{
	int spawnPeriod, msec;
	char buff[32] = {""};

	if (cgs.gamestate != GS_PLAYING)
	{
		CG_Printf("You may only use this command during the match.\n");
		return;
	}

	if (trap_Argc() == 1)
	{
		trap_Cvar_Set("cg_spawnTimer_set", "-1");
	}
	else if (trap_Argc() == 2)
	{
		trap_Argv( 1, buff, sizeof(buff) );
		spawnPeriod = atoi(buff);
		if (spawnPeriod < 1 || spawnPeriod > 60)
			CG_Printf("Argument must be a number between 1 and 60.\n");
		else
		{
			msec = (cgs.timelimit * 60.f * 1000.f) - (cg.time - cgs.levelStartTime);
			trap_Cvar_Set("cg_spawnTimer_period", buff);
			trap_Cvar_Set("cg_spawnTimer_set", va("%d", msec/1000));
		}
	}
	else
	{
		CG_Printf("Usage: timerSet [seconds]\n");
}
}

// flms: etpro style timer resetting
void CG_ResetTimer_f(void)
{
	int msec;

	if (cgs.gamestate != GS_PLAYING)
	{
		CG_Printf("You may only use this command during the match.\n");
		return;
	}

	msec = (cgs.timelimit * 60.f * 1000.f) - (cg.time - cgs.levelStartTime);
	trap_Cvar_Set("cg_spawnTimer_set", va("%d", msec/1000));
}

typedef struct {
	char	*cmd;
	void	(*function)(void);
} consoleCommand_t;
/**
 * @brief CG_GetSecondaryWeapon
 * @param[in] weapon
 * @param[in] team
 * @param[in] playerclass
 * @return
 */
static int CG_GetSecondaryWeapon(int weapon, team_t team, int playerclass)
{
	int outputWeapon;

	if (cgs.clientinfo[cg.clientNum].skill[SK_HEAVY_WEAPONS] >= 4 && playerclass == PC_SOLDIER)
	{
		switch (weapon)
		{
		case 1:
			outputWeapon = (team == TEAM_AXIS) ? WP_LUGER : WP_COLT;
			break;
		case 2:
			if (cgs.clientinfo[cg.clientNum].skill[SK_LIGHT_WEAPONS] >= 4)
			{
				outputWeapon = (team == TEAM_AXIS) ? WP_AKIMBO_LUGER : WP_AKIMBO_COLT;
			}
			else
			{
				outputWeapon = (team == TEAM_AXIS) ? WP_MP40 : WP_THOMPSON;
			}
			break;
		case 3:
		default:
			outputWeapon = (team == TEAM_AXIS) ? WP_MP40 : WP_THOMPSON;
			break;
		}
	}
	else if (cgs.clientinfo[cg.clientNum].skill[SK_LIGHT_WEAPONS] >= 4)
	{
		switch (weapon)
		{
		case 1:
			goto single_pistol;
		case 2:
		default:
			goto akimbo_pistols;
		}
	}
	else
	{
		goto single_pistol;
	}

	return outputWeapon;

single_pistol:
	if (playerclass == PC_COVERTOPS)
	{
		outputWeapon = (team == TEAM_AXIS) ? WP_SILENCER : WP_SILENCED_COLT;
	}
	else
	{
		outputWeapon = (team == TEAM_AXIS) ? WP_LUGER : WP_COLT;
	}
	return outputWeapon;

akimbo_pistols:
	if (playerclass == PC_COVERTOPS)
	{
		outputWeapon = (team == TEAM_AXIS) ? WP_AKIMBO_SILENCEDLUGER : WP_AKIMBO_SILENCEDCOLT;
	}
	else
	{
		outputWeapon = (team == TEAM_AXIS) ? WP_AKIMBO_LUGER : WP_AKIMBO_COLT;
	}
	return outputWeapon;
}

/**
 * @brief class change menu
 */
void CG_ClassMenu_f(void)
{
	if(cgs.clientinfo[cg.clientNum].team == TEAM_SPECTATOR)
	{
		return;
	}

	CG_EventHandling(CGAME_EVENT_NONE, qfalse);

	if(cg_quickMessageAlt.integer)
	{
		trap_UI_Popup(UIMENU_WM_CLASSALT);
	}
	else
	{
		trap_UI_Popup(UIMENU_WM_CLASS);
	}
}

/**
 * @brief Sends a class setup message. Enables etpro like class scripts
 */
void CG_Class_f(void)
{
	char             cls[64];
	const char       *classtype, *teamstring;
	int              weapon1, weapon2, playerclass;
	bg_playerclass_t *classinfo;
	team_t           team;

	if (cg.demoPlayback)
	{
		return;
	}

	team = cgs.clientinfo[cg.clientNum].team;

	if (team == TEAM_SPECTATOR)
	{
		return;
	}

	if (trap_Argc() < 2)
	{
		CG_Printf("Invalid command format.\n");
		return;
	}

	// TODO: handle missing case ?
	switch (team)
	{
	case TEAM_AXIS:
		classtype  = "r";
		teamstring = CG_TranslateString("Axis");
		break;
	case TEAM_ALLIES:
		classtype  = "b";
		teamstring = CG_TranslateString("Allies");
		break;
	default:
		CG_Printf("Invalid team.\n");
		return;
	}

	trap_Argv(1, cls, 64);

	if (!Q_stricmp(cls, "s") || !Q_stricmp(cls, "0"))
	{
		playerclass = PC_SOLDIER;
	}
	else if (!Q_stricmp(cls, "m") || !Q_stricmp(cls, "1"))
	{
		playerclass = PC_MEDIC;
	}
	else if (!Q_stricmp(cls, "e") || !Q_stricmp(cls, "2"))
	{
		playerclass = PC_ENGINEER;
	}
	else if (!Q_stricmp(cls, "f") || !Q_stricmp(cls, "3"))
	{
		playerclass = PC_FIELDOPS;
	}
	else if (!Q_stricmp(cls, "c") || !Q_stricmp(cls, "4"))
	{
		playerclass = PC_COVERTOPS;
	}
	else
	{
		CG_Printf("Invalid class format.\n");
		return;
	}

	classinfo = BG_GetPlayerClassInfo(team, playerclass);

	if (trap_Argc() > 2)
	{
		trap_Argv(2, cls, 64);
		weapon1 = atoi(cls);
		if (weapon1 <= 0 || weapon1 > MAX_WEAPS_PER_CLASS)
		{
			weapon1 = classinfo->classWeapons[0];
		}
		else if (!classinfo->classWeapons[weapon1 - 1])
		{
			CG_Printf("Invalid command format for weapon.\n");
			return;
		}
		else
		{
			weapon1 = classinfo->classWeapons[weapon1 - 1];
		}
	}
	else
	{
		weapon1 = classinfo->classWeapons[0];
	}

	if (trap_Argc() > 3)
	{
		trap_Argv(3, cls, 64);
		weapon2 = atoi(cls);
		weapon2 = CG_GetSecondaryWeapon(weapon2, team, playerclass);
	}
	else
	{
		weapon2 = CG_GetSecondaryWeapon(-1, team, playerclass);
	}

	// Print out the selected class and weapon info
	if (cgs.clientinfo[cg.clientNum].skill[SK_HEAVY_WEAPONS] >= 4 && playerclass == PC_SOLDIER && !Q_stricmp(weaponTable[weapon1].desc, weaponTable[weapon2].desc))
	{
		CG_PriorityCenterPrint(va(CG_TranslateString("You will spawn as an %s %s with a %s."), teamstring, BG_ClassnameForNumber(playerclass), weaponTable[weapon1].desc), 400, cg_fontScaleCP.value, -1);
	}
	else
	{
		switch (weapon2)
		{
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDCOLT:
		case WP_AKIMBO_SILENCEDLUGER:
			CG_PriorityCenterPrint(va(CG_TranslateString("You will spawn as an %s %s with a %s and %s."), teamstring, BG_ClassnameForNumber(playerclass), weaponTable[weapon1].desc, weaponTable[weapon2].desc), 400, cg_fontScaleCP.value, -1);
			break;
		default:
			CG_PriorityCenterPrint(va(CG_TranslateString("You will spawn as an %s %s with a %s and a %s."), teamstring, BG_ClassnameForNumber(playerclass), weaponTable[weapon1].desc, weaponTable[weapon2].desc), 400, cg_fontScaleCP.value, -1);
			break;
		}
	}
	// Send the switch command to the server
	trap_SendClientCommand(va("team %s %i %i %i\n", classtype, playerclass, weapon1, weapon2));
}

/**
 * @brief CG_ReadHuds_f
 */
void CG_ReadHuds_f(void)
{
	CG_ReadHudScripts();
}

#ifdef FEATURE_EDV
/**
 * @brief CG_FreecamTurnLeftDown_f
 */
void CG_FreecamTurnLeftDown_f(void)
{
	cgs.demoCamera.turn |= 0x01;
}

/**
 * @brief CG_FreecamTurnLeftUp_f
 */
void CG_FreecamTurnLeftUp_f(void)
{
	cgs.demoCamera.turn &= ~0x01;
}

/**
 * @brief CG_FreecamTurnRightDown_f
 */
void CG_FreecamTurnRightDown_f(void)
{
	cgs.demoCamera.turn |= 0x02;
}

/**
 * @brief CG_FreecamTurnRightUp_f
 */
void CG_FreecamTurnRightUp_f(void)
{
	cgs.demoCamera.turn &= ~0x02;
}

/**
 * @brief CG_FreecamTurnDownDown_f
 */
void CG_FreecamTurnDownDown_f(void)
{
	cgs.demoCamera.turn |= 0x04;
}

/**
 * @brief CG_FreecamTurnDownUp_f
 */
void CG_FreecamTurnDownUp_f(void)
{
	cgs.demoCamera.turn &= ~0x04;
}

/**
 * @brief CG_FreecamTurnUpDown_f
 */
void CG_FreecamTurnUpDown_f(void)
{
	cgs.demoCamera.turn |= 0x08;
}

/**
 * @brief CG_FreecamTurnUpUp_f
 */
void CG_FreecamTurnUpUp_f(void)
{
	cgs.demoCamera.turn &= ~0x08;
}

/**
 * @brief CG_FreecamRollLeftDown_f
 */
void CG_FreecamRollLeftDown_f(void)
{
	cgs.demoCamera.turn |= 0x20;
}

/**
 * @brief CG_FreecamRollLeftUp_f
 */
void CG_FreecamRollLeftUp_f(void)
{
	cgs.demoCamera.turn &= ~0x20;
}

/**
 * @brief CG_FreecamRollRightDown_f
 */
void CG_FreecamRollRightDown_f(void)
{
	cgs.demoCamera.turn |= 0x10;
}

/**
 * @brief CG_FreecamRollRightUp_f
 */
void CG_FreecamRollRightUp_f(void)
{
	cgs.demoCamera.turn &= ~0x10;
}

/**
 * @brief CG_Freecam_f
 */
void CG_Freecam_f(void)
{
	char state[MAX_TOKEN_CHARS];

	if (!cg.demoPlayback)
	{
		CG_Printf("Not playing a demo.\n");
		return;
	}

	trap_Argv(1, state, sizeof(state));

	if (!Q_stricmp(state, "on"))
	{
		cgs.demoCamera.renderingFreeCam = qtrue;
	}
	else if (!Q_stricmp(state, "off"))
	{
		cgs.demoCamera.renderingFreeCam = qfalse;
	}
	else
	{
		cgs.demoCamera.renderingFreeCam ^= qtrue;
	}

	CG_Printf("freecam %s\n", cgs.demoCamera.renderingFreeCam ? "ON" : "OFF");

	if (cgs.demoCamera.renderingFreeCam)
	{
		int viewheight;

		if (cg.snap->ps.eFlags & EF_CROUCHING)
		{
			viewheight = CROUCH_VIEWHEIGHT;
		}
		else if (cg.snap->ps.eFlags & EF_PRONE || cg.snap->ps.eFlags & EF_PRONE_MOVING)
		{
			viewheight = PRONE_VIEWHEIGHT;
		}
		else
		{
			viewheight = DEFAULT_VIEWHEIGHT;
		}
		cgs.demoCamera.camOrigin[2] += viewheight;
	}
}

/**
 * @brief CG_FreecamGetPos_f
 */
void CG_FreecamGetPos_f(void)
{
	if (cg.demoPlayback)
	{
		CG_Printf("freecam origin: %.0f %.0f %.0f\n", (double)cgs.demoCamera.camOrigin[0], (double)cgs.demoCamera.camOrigin[1], (double)cgs.demoCamera.camOrigin[2]);
	}
	else
	{
		CG_Printf("freecam origin: %.0f %.0f %.0f\n", (double)cg.refdef_current->vieworg[0], (double)cg.refdef_current->vieworg[1], (double)cg.refdef_current->vieworg[2]);
	}
}

/**
 * @brief etpro_float_Argv
 * @param[in] argnum
 * @return
 */
float etpro_float_Argv(int argnum)
{
	char buffer[MAX_TOKEN_CHARS];

	trap_Argv(argnum, buffer, sizeof(buffer));
	return (float)atof(buffer);
}

/**
 * @brief CG_FreecamSetPos_f
 */
void CG_FreecamSetPos_f(void)
{
	int n;

	if (!cg.demoPlayback)
	{
		CG_Printf("Cheats must be enabled.\n");
		return;
	}

	n = trap_Argc();
	if (n < 4)
	{
		CG_Printf("^1Syntax: freecamSetPos x y z\n");
		return;
	}
	if (n > 4 && n < 7)
	{
		CG_Printf("^1Syntax: freecamSetPos x y z pitch yaw roll\n");
		return;
	}

	cgs.demoCamera.camOrigin[0] = etpro_float_Argv(1);
	cgs.demoCamera.camOrigin[1] = etpro_float_Argv(2);
	cgs.demoCamera.camOrigin[2] = etpro_float_Argv(3);

	if (n >= 7)
	{
		cgs.demoCamera.camAngle[0]  = etpro_float_Argv(4);
		cgs.demoCamera.camAngle[1]  = etpro_float_Argv(5);
		cgs.demoCamera.camAngle[2]  = etpro_float_Argv(6);
		cgs.demoCamera.setCamAngles = qtrue;
	}
	else
	{
		cgs.demoCamera.setCamAngles = qfalse;
	}

}

/**
 * @brief noclip in demos
 */
void CG_NoClip_f(void)
{
	char buffer[MAX_TOKEN_CHARS];
	char state[MAX_TOKEN_CHARS];

	trap_Argv(0, buffer, sizeof(buffer));
	trap_Args(state, sizeof(state));

	if (!cg.demoPlayback)
	{
		if (trap_Argc() > 1)
		{
			trap_SendClientCommand(va("noclip %s\n", state));
		}
		else
		{
			trap_SendClientCommand("noclip\n");
		}
	}
	else
	{
		if (!Q_stricmp(state, "on"))
		{
			cgs.demoCamera.noclip = qtrue;
		}
		else if (!Q_stricmp(state, "off"))
		{
			cgs.demoCamera.noclip = qfalse;
		}
		else
		{
			cgs.demoCamera.noclip ^= qtrue;
		}
		CG_Printf("noclip %s\n", cgs.demoCamera.noclip ? "ON" : "OFF");
	}
}
#endif

void CG_PrintObjectiveInfo()
{
	int i;

	CG_Printf("^2Objective Info\n");

	for (i = 0; i < MAX_OID_TRIGGERS; i++)
	{
		CG_Printf("[%2i] %-26s -> num: %3i - spawnflags: %3i - objflags: %3i\n", i , cgs.oidInfo[i].name, cgs.oidInfo[i].entityNum, cgs.oidInfo[i].spawnflags, cgs.oidInfo[i].objflags);
	}
}

static consoleCommand_t	commands[] =
 {
//	{ "obj", CG_Obj_f },
//	{ "setspawnpt", CG_Obj_f },
	{ "testgun", CG_TestGun_f },
	{ "testmodel", CG_TestModel_f },
	{ "nextframe", CG_TestModelNextFrame_f },
	{ "prevframe", CG_TestModelPrevFrame_f },
	{ "nextskin", CG_TestModelNextSkin_f },
	{ "prevskin", CG_TestModelPrevSkin_f },
	{ "viewpos", CG_Viewpos_f },
	{ "+scores", CG_ScoresDown_f },
	{ "-scores", CG_ScoresUp_f },
	{ "zoomin", CG_ZoomIn_f },
	{ "zoomout", CG_ZoomOut_f },
	{ "weaplastused", CG_LastWeaponUsed_f },
	{ "weapnextinbank", CG_NextWeaponInBank_f },
	{ "weapprevinbank", CG_PrevWeaponInBank_f },
	{ "weapnext", CG_NextWeapon_f },
	{ "weapprev", CG_PrevWeapon_f },
	{ "weapalt", CG_AltWeapon_f },
	{ "weapon", CG_Weapon_f },
	{ "weaponbank", CG_WeaponBank_f },
	{ "tell_target", CG_TellTarget_f },
	{ "tell_attacker", CG_TellAttacker_f },
	{ "tcmd", CG_TargetCommand_f },
	{ "fade", CG_Fade_f },	// duffy
	{ "loadhud", CG_LoadHud_f },
	{ "edithud", CG_EditHud_f },
	{ "dumphud", CG_DumpHud_f },
	{ "loadweapons", CG_LoadWeapons_f },

	{ "mp_QuickMessage",	CG_QuickMessage_f },
	{ "mp_fireteammsg",		CG_QuickFireteams_f },
	{ "mp_fireteamadmin",	CG_QuickFireteamAdmin_f },
	{ "wm_sayPlayerClass",	CG_SayPlayerClass_f },
	{ "wm_ftsayPlayerClass",CG_FTSayPlayerClass_f },

	{ "classmenu", 	CG_ClassSelectMenu_f },
	{ "teammenu", 		CG_TeamSelectMenu_f },
	{ "spawnmenu", 	CG_SpawnSelectMenu_f },

	{ "VoiceChat",		CG_VoiceChat_f },
	{ "VoiceTeamChat",	CG_TeamVoiceChat_f },
	{ "VoiceFireTeamChat", CG_BuddyVoiceChat_f },

	// ydnar: say, teamsay, etc
	{ "messageMode", CG_MessageMode_f },
	{ "messageMode2", CG_MessageMode_f },
	{ "messageMode3", CG_MessageMode_f },
	{ "messageSend", CG_MessageSend_f },
	
	{ "SetWeaponCrosshair", CG_SetWeaponCrosshair_f },
	// -NERVE - SMF

	{ "openlimbomenu",	CG_LimboMenu_f },

	{ "+stats",			CG_StatsDown_f },
	{ "-stats",			CG_StatsUp_f },
	{ "+topshots",		CG_topshotsDown_f },
	{ "-topshots",		CG_topshotsUp_f },
	{ "+obj",			CG_objectivesDown_f },
	{ "-obj",			CG_objectivesUp_f },

	// OSP
	{ "autoRecord",		CG_autoRecord_f },
	{ "autoScreenshot",	CG_autoScreenShot_f },
	{ "currentTime",	CG_currentTime_f },
	{ "keyoff",			CG_keyOff_f },
	{ "keyon",			CG_keyOn_f },
#ifdef FEATURE_MULTIVIEW
	{ "mvactivate",		CG_mvToggleAll_f },
	{ "mvdel",			CG_mvDelete_f },
	{ "mvhide",			CG_mvHideView_f },
	{ "mvnew",			CG_mvNew_f },
	{ "mvshow",			CG_mvShowView_f },
	{ "mvswap",			CG_mvSwapViews_f },
	{ "mvtoggle",		CG_mvToggleView_f },
	{ "spechelp",		CG_toggleSpecHelp_f },
#endif
	{ "statsdump",		CG_dumpStats_f },
	{ "+vstr",			CG_vstrDown_f },
	{ "-vstr",			CG_vstrUp_f },
	// OSP

	{ "selectbuddy", CG_SelectBuddy_f },

	{ "MapZoomIn", CG_AutomapZoomIn_f },
	{ "MapZoomOut", CG_AutomapZoomOut_f },
	{ "+mapexpand", CG_AutomapExpandDown_f },
	{ "-mapexpand", CG_AutomapExpandUp_f },

	{ "generateTracemap", CG_GenerateTracemap },
	// xkan, 11/27/2002, toggle automap on/off
	{ "ToggleAutoMap", CG_ToggleAutomap_f },

	{ "editSpeakers", CG_EditSpeakers_f },
	{ "dumpSpeaker", CG_DumpSpeaker_f },
	{ "modifySpeaker", CG_ModifySpeaker_f },
	{ "undoSpeaker", CG_UndoSpeaker_f },
	{ "cpm", CG_CPM_f },
	{ "forcetapout", CG_ForceTapOut_f },
	{ "etpub", CG_Nop_f },
	{ "etpubc", CG_Nop_f },
	{ "hidevote", CG_HideVote_f },
	{ "resetmaxspeed", CG_ResetMaxSpeed_f },
	{ "loadfont", CG_LoadFont_f },
	// forty - visual hud editor
	{ "hudeditor", CG_HudEditor_f },
	{ "savehud", CG_SaveHud_f },
	{ "savehudmenucmd", CG_SaveHudMenu_f },
	{ "loadhudmenucmd", CG_LoadHudMenu_f },
	{ "cancelhudmenucmd", CG_CancelHudMenu_f },

	{ "scoreboard", CG_Scoreboard_f },

	//quad: spawntimer
	{ "timerSet", CG_TimerSet_f },
	{ "timerReset",          CG_TimerReset_f           },
	{ "resetTimer",          CG_TimerReset_f           }, // keep ETPro compatibility
	{ "class",               CG_Class_f                },
	{ "classmenu",           CG_ClassMenu_f            },
	{ "readhuds",            CG_ReadHuds_f             },
#ifdef FEATURE_EDV
	{ "+freecam_turnleft",   CG_FreecamTurnLeftDown_f  },
	{ "-freecam_turnleft",   CG_FreecamTurnLeftUp_f    },
	{ "+freecam_turnright",  CG_FreecamTurnRightDown_f },
	{ "-freecam_turnright",  CG_FreecamTurnRightUp_f   },

	{ "+freecam_turnup",     CG_FreecamTurnUpDown_f    },
	{ "-freecam_turnup",     CG_FreecamTurnUpUp_f      },
	{ "+freecam_turndown",   CG_FreecamTurnDownDown_f  },
	{ "-freecam_turndown",   CG_FreecamTurnDownUp_f    },

	{ "+freecam_rollleft",   CG_FreecamRollLeftDown_f  },
	{ "-freecam_rollleft",   CG_FreecamRollLeftUp_f    },
	{ "+freecam_rollright",  CG_FreecamRollRightDown_f },
	{ "-freecam_rollright",  CG_FreecamRollRightUp_f   },
	{ "freecam",             CG_Freecam_f              },
	{ "freecamsetpos",       CG_FreecamSetPos_f        },
	{ "freecamgetpos",       CG_FreecamGetPos_f        },

	{ "noclip",              CG_NoClip_f               },
#endif
	// objective info list for mappers/scripters (and players? - we might extend it)
	{ "oinfo",               CG_PrintObjectiveInfo     }
};


/*
=================
CG_ConsoleCommand

The string has been tokenized and can be retrieved with
Cmd_Argc() / Cmd_Argv()
=================
*/
qboolean CG_ConsoleCommand( void ) {
	const char	*cmd;
	unsigned int i;

	// Arnout - don't allow console commands until a snapshot is present
	if ( !cg.snap ) {
		return qfalse;
	}

	cmd = CG_Argv(0);

	//mcwf
	if (!Q_stricmp(cmd,"m")) { 
		if (need_escape(ConcatArgs(1))) {
		trap_SendClientCommand(va("m \"%s\"\n",escape_string(ConcatArgs(1))));  // say private
		return -1;
		}
	}

	if (!Q_stricmp(cmd,"priv")) { 
		if (need_escape(ConcatArgs(1))) {
		trap_SendClientCommand(va("priv \"%s\"\n",escape_string(ConcatArgs(1))));  // say private clone
		return -1;
		}
	}

	if (!Q_stricmp(cmd,"mt")) { 
		if (need_escape(ConcatArgs(1))) {
		trap_SendClientCommand(va("mt \"%s\"\n",escape_string(ConcatArgs(1))));  // say private team
		return -1;
		}
	}

	if (!Q_stricmp(cmd,"ma")) { 
		if (need_escape(ConcatArgs(1))) {
		trap_SendClientCommand(va("ma \"%s\"\n",escape_string(ConcatArgs(1))));  // admin chat
		return -1;
		}
	}

	if (!Q_stricmp(cmd,"say")) {
		if (need_escape(ConcatArgs(1))) {
		trap_SendClientCommand(va("say \"%s\"\n",escape_string(ConcatArgs(1))));  // say normal
		return -1;
		}
	}

	if (!Q_stricmp(cmd,"say_team")) {
		if (need_escape(ConcatArgs(1))) {
		trap_SendClientCommand(va("say_team \"%s\"\n",escape_string(ConcatArgs(1)))); // say team
		return -1;
		}
	}

	if (!Q_stricmp(cmd,"say_teamnl")) {
		if (need_escape(ConcatArgs(1))) {
		trap_SendClientCommand(va("say_teamnl \"%s\"\n",escape_string(ConcatArgs(1)))); // say team no pos
		return -1;
		}
	}

	if (!Q_stricmp(cmd,"say_buddy")) {
		if (need_escape(ConcatArgs(1))) {
		trap_SendClientCommand(va("say_buddy \"%s\"\n",escape_string(ConcatArgs(1)))); // say fireteam
		return -1;
		}
	}
	//mcwf

	for ( i = 0 ; i < sizeof( commands ) / sizeof( commands[0] ) ; i++ ) {
		if ( !Q_stricmp( cmd, commands[i].cmd ) ) {
			commands[i].function();
			return qtrue;
		}
	}

	return qfalse;
}


/*
=================
CG_InitConsoleCommands

Let the client system know about all of our commands
so it can perform tab completion
=================
*/
void CG_InitConsoleCommands(void)
{
	unsigned int i;
	const char *s;

	for (i = 0 ; i < sizeof(commands) / sizeof(commands[0]) ; i++)
	{
		trap_AddCommand( commands[i].cmd );
	}

	//
	// the game server will interpret these commands, which will be automatically
	// forwarded to the server after they are not recognized locally
	//
	trap_AddCommand ("kill");
	trap_AddCommand("say");
	// CHRUKER: b011 - Doesn't exist
	//trap_AddCommand ("say_limbo");
	// CHRUKER: b011 - Doesn't exist
	//trap_AddCommand ("tell");
	trap_AddCommand ("listbotgoals");
	trap_AddCommand ("give");
	trap_AddCommand ("god");
	trap_AddCommand ("notarget");
	trap_AddCommand ("noclip");
	trap_AddCommand ("team");
	trap_AddCommand ("follow");
	// CHRUKER: b011 - Not activated
	//trap_AddCommand ("addbot");
	trap_AddCommand ("setviewpos");
	trap_AddCommand ("callvote");
	trap_AddCommand ("vote");

	// Rafael
	trap_AddCommand ("nofatigue");

	// NERVE - SMF
	trap_AddCommand ("follownext");
	trap_AddCommand ("followprev");

	trap_AddCommand ("start_match");
	trap_AddCommand ("reset_match");
	trap_AddCommand ("swap_teams");
	// -NERVE - SMF
	// OSP
	trap_AddCommand("?");
	trap_AddCommand("bottomshots");
	trap_AddCommand("commands");
	// CHRUKER: b011 - Duplicate, look further up
	//trap_AddCommand("follow");
	trap_AddCommand("lock");
#ifdef FEATURE_MULTIVIEW
	trap_AddCommand("mvadd");
	trap_AddCommand("mvaxis");
	trap_AddCommand("mvallies");
	trap_AddCommand("mvall");
	trap_AddCommand("mvnone");
#endif
	trap_AddCommand("notready");
	trap_AddCommand("pause");
	trap_AddCommand("players");
	trap_AddCommand("readyteam");
	trap_AddCommand("ready");
	trap_AddCommand("ref");
	trap_AddCommand("say_teamnl");
	trap_AddCommand("say_team");
	trap_AddCommand("say");
	trap_AddCommand("scores");
	trap_AddCommand("specinvite");
	trap_AddCommand("speclock");
	trap_AddCommand("specunlock");
	trap_AddCommand("statsall");
	trap_AddCommand("statsdump");
	trap_AddCommand("timein");
	trap_AddCommand("timeout");
	trap_AddCommand("topshots");
	trap_AddCommand("unlock");
	trap_AddCommand("unpause");
	trap_AddCommand("unready");
	trap_AddCommand("weaponstats");
	// OSP

	trap_AddCommand("fireteam");
	// CHRUKER: b011 - Doesn't exist
	//trap_AddCommand ("buddylist");
	trap_AddCommand("showstats");

	trap_AddCommand("ignore");
	trap_AddCommand("unignore");

	// CHRUKER: b011 - Doesn't exist
	//trap_AddCommand ("addtt");
	// CHRUKER: b011 - Duplicate of an existing client side command
	//trap_AddCommand ("selectbuddy"); 
	// CHRUKER: b011 - Doesn't exist
	//trap_AddCommand ("selectNextBuddy");	// xkan 9/26/2002
	
	// CHRUKER: b011 - Missing commands
	trap_AddCommand ("imready");
	trap_AddCommand ("say_buddy");
	trap_AddCommand ("setspawnpt");
	trap_AddCommand ("vsay");
	trap_AddCommand ("vsay_buddy");
	trap_AddCommand ("vsay_team");
	trap_AddCommand ("where");

	// kw: new etpub commands
	trap_AddCommand("hitsounds");
	trap_AddCommand("m");
	trap_AddCommand("mt");
	trap_AddCommand("damage");
	trap_AddCommand("killrating");
	trap_AddCommand("playerrating");
	trap_AddCommand("playdead");
	trap_AddCommand("throwknife");
	trap_AddCommand("adrenother");
	trap_AddCommand("knives");
	trap_AddCommand("class");
	trap_AddCommand("nextteam");

	trap_AddCommand("dropweapon"); // Terifire
	trap_AddCommand("sclogin");
	trap_AddCommand("sclogout");
	trap_AddCommand("shoutcastlogin");
	trap_AddCommand("shoutcastlogout");
#ifdef FEATURE_LUA
	trap_AddCommand("lua_status");
#endif
	trap_AddCommand("etlpub_version");

	// tjw: remove engine commands
	trap_RemoveCommand("+lookup");
	trap_RemoveCommand("-lookup");
	trap_RemoveCommand("+lookdown");
	trap_RemoveCommand("-lookdown");

	// kw : only allow configstrings command when cheats enabled
	s = Info_ValueForKey( CG_ConfigString( CS_SYSTEMINFO ),
							"sv_cheats" );
	if (s[0] != '1')
	{
		trap_RemoveCommand("configstrings");
	}
}

void CG_parseMapVoteListInfo()
{
	int i;
	cgs.dbNumMaps = (trap_Argc() - 2) / 4;
	
	if ( atoi(CG_Argv(1)) ) 
		cgs.dbMapMultiVote = qtrue;

	for ( i = 0; i < cgs.dbNumMaps; i++ ) {
		Q_strncpyz(cgs.dbMaps[i], CG_Argv((i*4)+2),
			sizeof(cgs.dbMaps[0]));
		cgs.dbMapVotes[i] = 0;
		cgs.dbMapID[i] = atoi(CG_Argv((i*4)+3));
		cgs.dbMapLastPlayed[i] = atoi(CG_Argv((i*4)+4));
		cgs.dbMapTotalVotes[i] = atoi(CG_Argv((i*4)+5));
		if ( CG_FindArenaInfo(va("scripts/%s.arena", cgs.dbMaps[i]), 
					cgs.dbMaps[i], &cgs.arenaData) ) {
			Q_strncpyz(cgs.dbMapDispName[i], 
				cgs.arenaData.longname, 
				sizeof(cgs.dbMaps[0]) );
		} else {
			Q_strncpyz(cgs.dbMapDispName[i], 
				cgs.dbMaps[i],
				sizeof(cgs.dbMaps[0]) );
		}
	}
	
	CG_LocateArena();

	cgs.dbMapListReceived = qtrue;

	return;
}

void CG_parseMapVoteTally()
{
	int i, numMaps;
	
	numMaps = (trap_Argc() - 1);
	for ( i = 0; i < numMaps; i++ ) {
		cgs.dbMapVotes[i] = atoi(CG_Argv(i+1));
	}
	return;
}
