/*
 * main.cpp
 *
 *  ShowEQ Distributed under GPL
 *  http://seq.sourceforge.net/
 */

/* main.cpp is the entrypoint to ShowEQ, it parses the commandline
 * options and initializes the application
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <qapplication.h>
#include <sys/stat.h>
#include <qwindowsstyle.h>
#include <getopt.h>            // for long GNU-style options

#include <qaccel.h>

#include "interface.h"
#include "main.h"
#include "xmlpreferences.h"      // prefrence file class
#include "conf.h"              // autoconf/configure definitions
#include "itemdb.h"

/* **********************************
   defines used for option processing
   ********************************** */
#define OPTION_LIST "i:rf:g::j:::s:aeo:CncFAKSVPNbtL:xWX:Y:Z:"

/* For long options without any short (single letter) equivalent, we'll
   assign single char nonprinting character equivalents, as is common
   for many GNU utilities.  */

#define   VERSION_OPTION                10
#define   IPADDR_OPTION                 11
#define   MACADDR_OPTION                12
#define   GLOBAL_LOG_FILENAME_OPTION    13
#define   ZONE_LOG_FILENAME_OPTION      14
#define   UNKNOWN_LOG_FILENAME_OPTION   15
#define   GLOBAL_LOG_OPTION             16
#define   ZONE_LOG_OPTION               17
#define   UNKNOWN_ZONE_LOG_OPTION       18
#define   PLAYBACK_SPEED_OPTION         19
#define   SHOW_MAP_POINTS_OPTION        20
#define   ENCRYPTED_LOG_FILE_OPTION     21
#define   ENCRYPTED_LOG_OPTION          22
#define   SYSTIME_SPAWNTIME_OPTION      23
#define   SPAWNLOG_FILENAME_OPTION      24
#define   DISABLE_SPAWNLOG_OPTION       25
#define   NO_BANK_INFO                  26
#define   ITEMDB_LORE_FILENAME_OPTION   27
#define   ITEMDB_NAME_FILENAME_OPTION   28
#define   ITEMDB_DATA_FILENAME_OPTION   29
#define   ITEMDB_RAW_FILENAME_OPTION    30
#define   ITEMDB_DATABASES_ENABLED      31
#define   ITEMDB_DISABLE                3
#define   STATUS_FONT_SIZE              4
#define   RESTORE_DECODE_KEY            5
#define   RESTORE_PLAYER_STATE          6
#define   RESTORE_ZONE_STATE            7
#define   RESTORE_SPAWNS                8
#define   RESTORE_ALL                   9

/* Note that ASCII 32 is a space, best to stop at 31 and pick up again
   at 128 or higher
*/


/* 
   Add your options here.  Format:

   <long option name>,

   <no_argument | optional_argument | required_argument>,

   NULL,

   <single option char or unprintable number like VERSION_OPTION>
*/

static struct option option_list[] = {
  {"net-interface",                required_argument,  NULL,  'i'},
  {"realtime",                     no_argument,        NULL,  'r'},
  {"filter-file",                  required_argument,  NULL,  'f'},
  {"playback-filename",            optional_argument,  NULL,  'j'},
  {"playback-speed",               required_argument,  NULL,  PLAYBACK_SPEED_OPTION},
  {"record-filename",              optional_argument,  NULL,  'g'},
  {"enlightenment-audio",          no_argument,        NULL,  'a'},
  {"filter-case-sensitive",        no_argument,        NULL,  'C'},
  {"use-retarded-coords",          no_argument,        NULL,  'c'},
  {"fast-machine",                 no_argument,        NULL,  'F'},
  {"spawn-alert",                  no_argument,        NULL,  'A'},
  {"create-unknown-spawns",        no_argument,        NULL,  'K'},
  {"show-unknown-spawns",          no_argument,        NULL,  'K'},
  {"select-on-consider",           no_argument,        NULL,  'S'},
  {"select-on-target",             no_argument,        NULL,  'e'},
  {"no-keep-selected-visible",     no_argument,        NULL,  'V'},
  {"no-promiscuous",               no_argument,        NULL,  'P'},
  {"show-packet-numbers",          no_argument,        NULL,  'N'},
  {"broken-decode",                no_argument,        NULL,  'b'},
  {"show-selected",                no_argument,        NULL,  't'},
  {"spawn-path-length",            required_argument,  NULL,  'L'},
  {"log-spawns",                   no_argument,        NULL,  'x'},
  {"dont-autodetectcharsettings",  no_argument,        NULL,  'W'},
  {"default-level",                required_argument,  NULL,  'X'},
  {"default-race",                 required_argument,  NULL,  'Y'},
  {"default-class",                required_argument,  NULL,  'Z'},
  {"help",                         no_argument,        NULL,  'h'},
  {"version",                      no_argument,        NULL,  VERSION_OPTION},
  {"ip-address",                   required_argument,  NULL,  IPADDR_OPTION},
  {"mac-address",                  required_argument,  NULL,  MACADDR_OPTION},
  {"global-log-filename",          required_argument,  NULL,  GLOBAL_LOG_FILENAME_OPTION},
  {"zone-log-filename",            required_argument,  NULL,  ZONE_LOG_FILENAME_OPTION},
  {"unknown-zone-log-filename",    required_argument,  NULL,  UNKNOWN_LOG_FILENAME_OPTION},
  {"log-all",                      no_argument,        NULL,  GLOBAL_LOG_OPTION},
  {"log-zone",                     no_argument,        NULL,  ZONE_LOG_OPTION},
  {"log-unknown-zone",             no_argument,        NULL,  UNKNOWN_ZONE_LOG_OPTION},
  {"encrypted-log-filebase",       required_argument,  NULL,  ENCRYPTED_LOG_FILE_OPTION},
  {"log-encrypted",                no_argument,        NULL,  ENCRYPTED_LOG_OPTION},
  {"systime-spawntime",            no_argument,        NULL,  SYSTIME_SPAWNTIME_OPTION},
  {"spawnlog-filename",            required_argument,  NULL,  SPAWNLOG_FILENAME_OPTION},
  {"disable-spawnlog",             no_argument,        NULL,  DISABLE_SPAWNLOG_OPTION},
  {"no-bank",                      no_argument,        NULL,  NO_BANK_INFO},    
  {"itemdb-lore-filename",         required_argument,  NULL, ITEMDB_LORE_FILENAME_OPTION},
  {"itemdb-name-filename",         required_argument,  NULL, ITEMDB_NAME_FILENAME_OPTION},
  {"itemdb-data-filename",         required_argument,  NULL, ITEMDB_DATA_FILENAME_OPTION},
  {"itemdb-raw-data-filename",     required_argument,  NULL, ITEMDB_RAW_FILENAME_OPTION},
  {"itemdb-databases-enabled",     required_argument,  NULL, ITEMDB_DATABASES_ENABLED},
  {"itemdb-disable",               no_argument,        NULL, ITEMDB_DISABLE},
  {"restore-decode-key",           no_argument,        NULL, RESTORE_DECODE_KEY},
  {"restore-player-state",         no_argument,        NULL, RESTORE_PLAYER_STATE},
  {"restore-zone",                 no_argument,        NULL, RESTORE_ZONE_STATE},
  {"restore-spawns",               no_argument,        NULL, RESTORE_SPAWNS},
  {"restore-all",                  no_argument,        NULL, RESTORE_ALL},
  {0,                              0,                  0,     0}
};

/* Global parameters, so all parts of ShowEQ has access to it */
struct ShowEQParams *showeq_params;
XMLPreferences      *pSEQPrefs; 
class EQItemDB* pItemDB;

int main (int argc, char **argv)
{
   int           opt;
   int           temp_int;
   int           option_index = 0;
   
   bool          bOptionHelp = false;
        
   /* Print the version number */
   printf ("ShowEQ %s, released under the GPL.\n", VERSION);
   printf ("All ShowEQ source code is Copyright (C) 2000, 2001, 2002 by the respective ShowEQ Developers\n");
   printf ("Binary distribution without source code and resale are explictily NOT authorized by ANY party.\n");
   printf ("If you have paid for this software in any way, shape, or form, the person selling the\n");
   printf ("software is doing so in violation of the express wishes and intents of the authors of this product.\n\n");
   printf ("Please see http://seq.sourceforge.net for further information\n\n");

   /* Create application instance */
   QApplication::setStyle( new QWindowsStyle );
   QApplication qapp (argc, argv);

   /* Initialize the parameters with default values */
   char *configfile = LOGDIR "/showeq.xml";

   // scan command line arguments for a specified config file
   int i = 1;
   while (i < argc)
   {
      if ((argv[i][0] == '-') && (argv[i][1] == 'o'))
         configfile = strdup(argv[i + 1]);

      i ++;
   }

   /* NOTE: See preferencefile.cpp for info on how to use prefrences class */
   printf("Using config file '%s'\n", configfile);
   pSEQPrefs = new XMLPreferences(LOGDIR "/seqdef.xml", configfile);

   showeq_params = new ShowEQParams;

   QString section;

   /* TODO: Add some sanity checks to the MAC address option.  cpphack */
   section = "Network";
   showeq_params->device = pSEQPrefs->getPrefString("Device", section, "eth0");
   showeq_params->ip = strdup(pSEQPrefs->getPrefString("IP", section,
						       AUTOMATIC_CLIENT_IP));
   showeq_params->mac_address = strdup(pSEQPrefs->getPrefString("MAC", section, "0"));
   showeq_params->realtime = pSEQPrefs->getPrefBool("RealTimeThread", section,   false);
   showeq_params->promisc = pSEQPrefs->getPrefBool("NoPromiscuous", section, true);
   showeq_params->arqSeqGiveUp = pSEQPrefs->getPrefInt("ArqSeqGiveUp", section, 96);
   showeq_params->session_tracking = pSEQPrefs->getPrefBool("SessionTracking", section, 0);
#if HAVE_LIBEQ
   showeq_params->broken_decode = pSEQPrefs->getPrefBool("BrokenDecode", section, 0);

   if (showeq_params->broken_decode)
      printf("Disabling decoder due to showeq.xml preferences\n");
#else
   /* Default to broken decoder if libEQ not present */
   showeq_params->broken_decode = 1;
   printf("Disabling decoder due to missing libEQ.a\n");
#endif

   section = "Interface";
   /* Allow map depth filtering */
   showeq_params->retarded_coords  = pSEQPrefs->getPrefBool("RetardedCoords", section, 0);
   showeq_params->con_select = pSEQPrefs->getPrefBool("SelectOnCon", section, false);
   showeq_params->tar_select = pSEQPrefs->getPrefBool("SelectOnTarget", section, false);
   showeq_params->net_stats = pSEQPrefs->getPrefBool("NetStats", section, false);
   showeq_params->systime_spawntime = pSEQPrefs->getPrefBool("SystimeSpawntime", section, false);
   showeq_params->pvp = pSEQPrefs->getPrefBool("PvPTeamColoring", section, false);
   showeq_params->deitypvp = pSEQPrefs->getPrefBool("DeityPvPTeamColoring", section, false);
   showeq_params->keep_selected_visible = pSEQPrefs->getPrefBool("KeepSelected", section, true);

   showeq_params->no_bank = pSEQPrefs->getPrefBool("NoBank", section, true);

   section = "Interface_StatusBar";
   showeq_params->showEQTime = pSEQPrefs->getPrefBool("ShowEQTime",section,false);
   section = "Misc";
   showeq_params->fast_machine = pSEQPrefs->getPrefBool("FastMachine", section, true);
   showeq_params->createUnknownSpawns = pSEQPrefs->getPrefBool("CreateUnknownSpawns", section, false);

   showeq_params->walkpathrecord = pSEQPrefs->getPrefBool("WalkPathRecording", section, false);
   showeq_params->walkpathlength = pSEQPrefs->getPrefInt("WalkPathLength", section, 25);
   showeq_params->logSpawns = pSEQPrefs->getPrefBool("LogSpawns", section, false);
   showeq_params->logItems = pSEQPrefs->getPrefBool("LogItems",  section, false);
   /* Tells SEQ whether or not to display casting messages (Turn this off if you're on a big raid) */
   showeq_params->showSpellMsgs = pSEQPrefs->getPrefBool("ShowSpellMessages", section, true);
   /* Spawn logging preferences */
   showeq_params->SpawnLogFilename = pSEQPrefs->getPrefString("SpawnLogFilename", section, LOGDIR "/spawnlog.txt");
   showeq_params->spawnlog_enabled = pSEQPrefs->getPrefBool("SpawnLogEnabled", section,  true);
/* Decoder override for coping with encryption changes */

   section = "Filters";
   showeq_params->filterfile = pSEQPrefs->getPrefString("FilterFile", section, LOGDIR "/filters.conf");
   showeq_params->spawnfilter_audio = pSEQPrefs->getPrefBool("Audio", section, false);
   showeq_params->spawnfilter_loglocates = pSEQPrefs->getPrefBool("LogLocates", section, 0);
   showeq_params->spawnfilter_logcautions = pSEQPrefs->getPrefBool("LogCautions", section, 0);
   showeq_params->spawnfilter_loghunts = pSEQPrefs->getPrefBool("LogHunts", section, 0);
   showeq_params->spawnfilter_logdangers = pSEQPrefs->getPrefBool("LogDangers", section, 0);
   showeq_params->spawnfilter_case = pSEQPrefs->getPrefBool("IsCaseSensitive", section, 0);
   showeq_params->spawn_alert_plus_plus = pSEQPrefs->getPrefBool("AlertInfo", section, 0);

   /* Default Level / Race / Class preferences */
   section = "Defaults";
   showeq_params->AutoDetectCharSettings = pSEQPrefs->getPrefBool("AutoDetectCharSettings", section, 1);
   showeq_params->defaultName = pSEQPrefs->getPrefString("DefaultName", section, "You");
   showeq_params->defaultLastName = pSEQPrefs->getPrefString("DefaultLastName", section, "");
   showeq_params->defaultLevel = pSEQPrefs->getPrefInt("DefaultLevel", section, 1);
   showeq_params->defaultRace = pSEQPrefs->getPrefInt("DefaultRace", section, 1);
   showeq_params->defaultClass = pSEQPrefs->getPrefInt("DefaultClass", section, 1);
   showeq_params->defaultDeity = pSEQPrefs->getPrefInt("DefaultDeity", section, DEITY_AGNOSTIC);

   /* VPacket (Packet Recording / Playback) */
   section = "VPacket";
   showeq_params->playbackpackets = pSEQPrefs->getPrefBool("Playback", section,  false);
   showeq_params->recordpackets = pSEQPrefs->getPrefBool("Record", section, false);
   showeq_params->playbackspeed = pSEQPrefs->getPrefBool("PlaybackRate", section, false);

   section = "SpawnList";
   showeq_params->showRealName = pSEQPrefs->getPrefBool("ShowRealName", section, false);

   /* OpCode monitoring preferences */
   section = "OpCodeMonitoring";
   showeq_params->monitorOpCode_Usage = 
     pSEQPrefs->getPrefBool("Enable", section, false);   /*  Disabled  */
   showeq_params->monitorOpCode_List = 
     pSEQPrefs->getPrefString("OpCodeList", section, "");  /*    NONE    */
   showeq_params->monitorOpCode_Log = 
     pSEQPrefs->getPrefBool("Log", section, false);
   showeq_params->monitorOpCode_Filename =
     pSEQPrefs->getPrefString("LogFilename", section, 
			      LOGDIR "/opcodemonitor.log");

   /* Packet logging preferences */
   section = "PacketLogging";
   showeq_params->logAllPackets = pSEQPrefs->getPrefBool("LogAllPackets", section, 0);
   showeq_params->logZonePackets = pSEQPrefs->getPrefBool("LogZonePackets", section, 0);
   showeq_params->logUnknownZonePackets  = pSEQPrefs->getPrefBool("LogUnknownZonePackets", section,  0);
   showeq_params->GlobalLogFilename = pSEQPrefs->getPrefString("GlobalLogFilename", section, LOGDIR "/global.log") ;
   showeq_params->ZoneLogFilename = pSEQPrefs->getPrefString("ZoneLogFilename", section, LOGDIR "/zone.log");
   showeq_params->UnknownZoneLogFilename = pSEQPrefs->getPrefString("UnknownZoneLogFilename", section, LOGDIR "/unknownzone.log") ;
   /* Different files for different kinds of encrypted data */
   showeq_params->logEncrypted = pSEQPrefs->getPrefBool("LogEncrypted", section, 0);
   showeq_params->EncryptedLogFilenameBase = pSEQPrefs->getPrefString("EncryptedLogFilenameBase", section, LOGDIR "/encrypted") ;
   showeq_params->PktLoggerMask = pSEQPrefs->getPrefString("PktLoggerMask", section, "");
   showeq_params->PktLoggerFilename = pSEQPrefs->getPrefString("PktLoggerFilename", section, LOGDIR "/packet.log") ;

   // item database parameters
   section = "ItemDB";
   showeq_params->ItemLoreDBFilename = pSEQPrefs->getPrefString("LoreDBFilename", section, LOGDIR "/itemlore");
   showeq_params->ItemNameDBFilename = pSEQPrefs->getPrefString("NameDBFilename", section, LOGDIR "/itemname");
   showeq_params->ItemDataDBFilename = pSEQPrefs->getPrefString("DataDBFilename", section, LOGDIR "/itemdata");
   showeq_params->ItemRawDataDBFileName = pSEQPrefs->getPrefString("RawDataDBFilename", section, LOGDIR "/itemrawdata");
   showeq_params->ItemDBTypes = pSEQPrefs->getPrefInt("DatabasesEnabled", section, (EQItemDB::LORE_DB | EQItemDB::NAME_DB | EQItemDB::DATA_DB));
   showeq_params->ItemDBEnabled = pSEQPrefs->getPrefBool("Enabled", section, 1);

   section = "SaveState";
   showeq_params->saveDecodeKey = 
     pSEQPrefs->getPrefBool("DecodeKey", section, 1);
   showeq_params->saveZoneState = 
     pSEQPrefs->getPrefBool("ZoneState", section, 1);
   showeq_params->savePlayerState = 
     pSEQPrefs->getPrefBool("PlayerState", section, 1);
   showeq_params->saveSpawns = pSEQPrefs->getPrefBool("Spawns", section, false);
   showeq_params->saveSpawnsFrequency = 
     pSEQPrefs->getPrefInt("SpawnsFrequency", section, (120 * 1000));
   showeq_params->restoreDecodeKey = false;
   showeq_params->restorePlayerState = false;
   showeq_params->restoreZoneState = false;
   showeq_params->restoreSpawns = false;
   showeq_params->saveRestoreBaseFilename = pSEQPrefs->getPrefString("BaseFilename", section, LOGDIR "/last");

   /* Parse the commandline for commandline parameters */
   while ((opt = getopt_long( argc,
                              argv,
                              OPTION_LIST,
                              option_list,
                              &option_index
                             ))               != -1
          )
   {
      switch (opt)
      {
         /* Set the request to use a despawn list based off the spawn alert list. */

         /* Set the interface */
         case 'i':
         {            
            showeq_params->device = optarg;
            break;
         }


         /* Set pcap thread to realtime */
         case 'r':
         {  
            showeq_params->realtime = 1;
            break;
         }


         /* Set the spawn filter file */
         case 'f':
         {
            showeq_params->filterfile             = optarg;
            
            break;
         }


         /* Packet playback mode */
         case 'j':
         {            
            if (optarg)
               pSEQPrefs->setPrefString("Filename", "VPacket", optarg, XMLPreferences::Runtime);

            showeq_params->playbackpackets = 1;
            showeq_params->recordpackets   = 0;
            
            break;
         }

         /* Packet record mode */
         case 'g':
         {
            if (optarg)
               pSEQPrefs->setPrefString("Filename", "VPacket", optarg, XMLPreferences::Runtime);

            showeq_params->recordpackets   = 1;
            showeq_params->playbackpackets = 0;
            
            break;
         }


         /* Config file was already taken care of, ignore */
         case 'o':
            break;


         /* Enable use of enlightenment audio */
         case 'a':
         {
            showeq_params->spawnfilter_audio = 1;
            break;
         }


         /* Make filter case sensitive */
         case 'C':
         {
            showeq_params->spawnfilter_case = 1;
            break;
         }

         /* Use retarded coordinate system yxz */
         case 'c':
         {
            showeq_params->retarded_coords = 1;
            break;
         }


         /* Fast machine updates.. framerate vs packet based */
         case 'F':
         {
            showeq_params->fast_machine = 1;
            break;
         }

         /* Cool spawn alert */
         case 'A':
         {
            showeq_params->spawn_alert_plus_plus = 1;
            break;
         }


         /* Show unknown spawns */
         case 'K':
         {
            showeq_params->createUnknownSpawns = 1;
            break;
         }


         /* Select spawn on 'Consider' */
         case 'S':
         {
            showeq_params->con_select = 1;
            break;
         }


         /* Select spawn on 'Target' */
         case 'e':
         {
            showeq_params->tar_select = 1;
            break;
         }


         /* Don't force the selected spawn to be visible in scrollbox */
         case 'V':
         {
            showeq_params->keep_selected_visible = 0;
            break;
         }


         /* Don't use Promiscuous mode on sniffing */
         case 'P':
         {
            showeq_params->promisc = 0;
            break;
         }


         /* Show net info */
         case 'N':
         {
            showeq_params->net_stats = 1;
            break;
         }


         /* 'b'roken decode -- don't deal with spawn packets */
         case 'b':
         {
            showeq_params->broken_decode = 1;
            
            printf("Disabling decoder due to command-line parameter\n");
            break;
         }


         /* 't'rack pathing for mobs */
         case 't':
         {
            showeq_params->walkpathrecord = 1;
            break;
         }


         /* Maximum spawn path tracking length  */
         case 'L':
         {
            showeq_params->walkpathlength = atoi(optarg);
            break;
         }

         
         /* Log spawns! */
         case 'x':
         {
            showeq_params->logSpawns = 1;
            break;
         }


         /* Don't autodetect character settings */
         case 'W':
         {
            showeq_params->AutoDetectCharSettings = 0;
            break;
         }


         /* Set default player level */
         case 'X':
         {
            temp_int = atoi(optarg);
            
            if (temp_int < 1 || temp_int > 60)
            {
               printf ("Invalid default level.  Valid range is 1 - 60.\n");
               exit(0);
            }
            
            showeq_params->defaultLevel = temp_int;           
            break;
         }


         /* Set default player race */
         case 'Y':
         {
            temp_int = atoi(optarg);
            
            if ((temp_int < 1 || temp_int > 12) && temp_int != 128)
            {
               printf ("Invalid default race, please use showeq -h to list valid race options.\n");
               exit(0);
            }
            
            showeq_params->defaultRace = temp_int;
            break;
         }


         /* Set default player class */
         case 'Z':
         {
            temp_int = atoi(optarg);
            
            if (temp_int < 1 || temp_int > 14)
            {
               printf ("Invalid default class, please use showeq -h to list valid class options.\n");
               exit(0);
            }

            showeq_params->defaultClass = temp_int;
            break;
         }


         /* Display the version info... */
         case VERSION_OPTION:
         {
            printf ("ShowEQ %s\n",VERSION);
            printf ("Copyright (C) 1999-2001 ShowEQ Contributors\n\n");
            
            printf ("ShowEQ comes with NO WARRANTY.\n\n");
            
            printf ("You may redistribute copies of ShowEQ under the terms of\n");
            printf ("The GNU General Public License.\n");
            printf ("See: http://www.gnu.org/copyleft/gpl.html for more details...\n\n");
            
            printf ("For updates and information, please visit http://seq.sourceforge.net/\n");
            
            exit(0);
            break;
         }


         /* IP address to track */
         case IPADDR_OPTION:
         {
            showeq_params->ip = strdup(optarg);
            break;
         }


         /* MAC address to track for those on DHCP */
         case MACADDR_OPTION:
         {
            showeq_params->mac_address = optarg;
            break;
         }


         /* Filename for logging all packets */
         case GLOBAL_LOG_FILENAME_OPTION:
         {
            showeq_params->GlobalLogFilename = optarg;
            break;
         }


         /* Filename for logging zone change packets */
         case ZONE_LOG_FILENAME_OPTION:
         {
            showeq_params->ZoneLogFilename = optarg;
            break;
         }


         /* Filename for logging unknown zone change packets */
         case UNKNOWN_LOG_FILENAME_OPTION:
         {
            showeq_params->UnknownZoneLogFilename = optarg;
            break;
         }


         /* Log everything */
         case GLOBAL_LOG_OPTION:
         {
            showeq_params->logAllPackets = 1;
            break;
         }


         /* Log all zone change packets */
         case ZONE_LOG_OPTION:
         {
            showeq_params->logZonePackets = 1;
            break;
         }


         /* Log only unfamiliar zone change packets */
         case UNKNOWN_ZONE_LOG_OPTION:
         {
            showeq_params->logUnknownZonePackets = 1;
            break;
         }


         case PLAYBACK_SPEED_OPTION:
         {
            showeq_params->playbackspeed = atoi(optarg);
            break;
         }


         /* Enable logging of encrypted packets... */
         case ENCRYPTED_LOG_OPTION:
         {
            showeq_params->logEncrypted = 1;
            break;
         }

         
         /* Log encrypted packets to this file... */
         case ENCRYPTED_LOG_FILE_OPTION:
         {
            showeq_params->EncryptedLogFilenameBase = optarg;
            break;
         }


         /* Display spawntime in UNIX time (time_t) instead of hh:mm format */
         case SYSTIME_SPAWNTIME_OPTION:
         {
            showeq_params->systime_spawntime = 1;
            break;
         }

         
         case SPAWNLOG_FILENAME_OPTION:
         {
            showeq_params->SpawnLogFilename = optarg;
            break;
         }


         case DISABLE_SPAWNLOG_OPTION:
         {
            showeq_params->spawnlog_enabled = FALSE;
            break;
         }
         
         case NO_BANK_INFO:
         {
         	showeq_params->no_bank = TRUE;
         	break;
         }
         
         case ITEMDB_LORE_FILENAME_OPTION:
         {
            showeq_params->ItemLoreDBFilename = optarg;
            break;
         }
         
         case ITEMDB_NAME_FILENAME_OPTION:
         {
            showeq_params->ItemNameDBFilename = optarg;
            break;
         }
         
         case ITEMDB_DATA_FILENAME_OPTION:
         {
            showeq_params->ItemDataDBFilename = optarg;
            break;
         }
         
         case ITEMDB_RAW_FILENAME_OPTION:
         {
            showeq_params->ItemRawDataDBFileName = optarg;
            break;
         }

         case ITEMDB_DATABASES_ENABLED:
         {
            showeq_params->ItemDBTypes = atoi(optarg);
            break;
         }
	 
         case ITEMDB_DISABLE:
	 {
	   showeq_params->ItemDBEnabled = false;
	   break;
	 }

         case RESTORE_DECODE_KEY:
	 {
	   showeq_params->restoreDecodeKey = true;
	   break;
	 }
         case RESTORE_PLAYER_STATE:
	 {
	   showeq_params->restorePlayerState = true;
	   break;
	 }
         case RESTORE_ZONE_STATE:
	 {
	   showeq_params->restoreZoneState = true;
	   break;
	 }
         case RESTORE_SPAWNS:
	 {
	   showeq_params->restoreSpawns = true;
	   break;
	 }
         case RESTORE_ALL:
	 {
	   showeq_params->restoreDecodeKey = true;
	   showeq_params->restorePlayerState = true;
	   showeq_params->restoreZoneState = true;
	   showeq_params->restoreSpawns = true;
	   break;
	 }


         /* Spit out the help */
         case 'h': /* Fall through */
         default:
         {
            bOptionHelp = true;
            break;
         }
      }
   }

   if (bOptionHelp)
   {
      /* The default help text */
      printf ("Usage:\n  %s [<options>] [<client IP address>]\n\n", argv[0]);
      
      printf ("  -h, --help                            Shows this help\n");
      printf ("  -o CONFIGFILE                         Alternate showeq.xml pathname\n");
      printf ("      --version                         Prints ShowEQ version number\n");
      printf ("  -i, --net-interface=DEVICE            Specify which network device to bind to\n");
      printf ("  -r, --realtime                        Set the network thread realtime\n");
      printf ("  -f, --filter-file=FILENAME            Sets spawn filter file\n");
      printf ("  -s, --spawn-file=FILENAME             Sets spawn alert file\n");
      printf ("  -C, --filter-case-sensitive           Spawn alert and filter is case sensitive\n");
      printf ("  -a, --enlightenment-audio             Use ESD to play alert during spawn alert\n");
      printf ("  -c, --use-retarded-coords             Use \"retarded\" YXZ coordinates\n");
      printf ("  -F, --fast-machine                    Fast machine - perform more accurate vs. \n");
      printf ("                                        less accurate calculations.\n");
      printf ("  -A, --spawn-alert                     Use name/race/class/light/equipment for \n");
      printf ("                                        spawn matching\n");
      printf ("  -K, --create-unknown-spawns           create unknown spawns\n");
      printf ("  -S, --select-on-consider              Select the spawn considered\n");
      printf ("  -e, --select-on-target                Select the spawn targetted\n");
      printf ("  -V, --no-keep-selected-visible        Don't force the listbox to keep selected\n");
      printf ("                                        spawn visible\n");
      printf ("  -P, --no-promiscuous                  Don't sniff the network in promiscuous\n");
      printf ("                                        mode (Don't process packets bound for\n");
      printf ("                                        other machines).\n");
      printf ("  -N, --show-packet-numbers             Show network info dialog\n");
      printf ("  -j, --playback-file=FILENAME          Playback packets in FILENAME, previously\n");
      printf ("                                        recorded with -g option\n");
      printf ("      --playback-speed=SPEED            -1 = Paused, 0 = Max, 1 = Slow, 9 = Fast\n");
      printf ("  -g, --record-file=FILENAME            Record packets to FILENAME to playback\n");
      printf ("                                        with the -j option\n");
      printf ("  -b, --broken-decode                   Broken decode -- Don't attempt to decode\n");
      printf ("                                        the spawn packets (i.e. Your CPU is VERY\n");
      printf ("                                        slow)\n");
      printf ("  -t, --show-selected                   Track spawn movements (no path trace)\n");
      printf ("  -L, --spawn-path-length=###           Track spawn maximum track length (min:3)\n");
      printf ("  -x, --log-spawns                      Log spawns into spawns.db\n");
      printf ("      --systime-spawntime               Show spawn time using UNIX systtem time\n");
      printf ("      --ip-address=IP                   Client IP address\n");
      printf ("      --mac-address=MAC                 Client MAC address as 00:00:00:00:00:00\n");
      printf ("      --log-all                         Log all packets to global logfile\n");
      printf ("      --global-log-filename=FILE        Use FILE for above packet logging\n");
      printf ("      --log-zone                        Like --log-all, but only zone data\n");
      printf ("      --zone-log-filename=FILE          Use FILE for above packet logging\n");
      printf ("      --log-unknown-zone                Log only unrecognized zone data\n");
      printf ("      --unknown-zone-log-filename=FILE  Use FILE for above packet logging\n");
      printf ("      --log-encrypted                   Log some unprocessed encrypted data\n");
      printf ("      --encrypted-log-filebase=FILE     Use FILE as base for above logging\n");
      printf ("      --disable-spawnlog                Disable spawn logging to spawnlog.txt\n");
      printf ("      --spawnlog-filename=FILE          Use FILE instead of spawnlog.txt\n");
      printf ("      --itemdb-lore-filename=FILE       Use FILE instead of itemlore\n");
      printf ("      --itemdb-name-filename=FILE       Use FILE instead of itemname\n");
      printf ("      --itemdb-data-filename=FILE       Use FILE instead of itemdata\n");
      printf ("      --itemdb-raw-data-filename=FILE   Use FILE instead of itemrawdata\n");
      printf ("      --itemdb-databases-enabled=DBS    Use DBS to enable different item\n");
      printf ("                                        databases.\n");
      printf ("      --itemdb-disable                  Disable use of the item DB.\n");
      printf ("  -W, --dont-autodetectcharsettings     Don't auto-detect your character's\n");
      printf ("                                        Level/Race/Class.\n");
      printf ("  -X, --default-level=##                Default player level. (1-60)\n");
      printf ("  -Y, --default-race=##                 Default player race:");
      printf ("\n\t                                    HUM 1,  BAR 2,  ERU 3");
      printf ("\n\t                                    ELF 4,  HIE 5,  DEF 6");
      printf ("\n\t                                    HEF 7,  DWF 8,  TRL 9");
      printf ("\n\t                                    OGR 10, HFL 11, GNM 12");
      printf ("\n\t                                    GNM 12, IKS 128\n");
      printf ("  -Z, --default-class=##                Default player class:");
      printf ("\n\t                                    WAR 1,  CLR 2,  PAL 3");
      printf ("\n\t                                    RNG 4,  SHD 5,  DRU 6");
      printf ("\n\t                                    MNK 7,  BRD 8,  ROG 9");
      printf ("\n\t                                    SHM 10, NEC 11, WIZ 12");
      printf ("\n\t                                    MAG 13, ENC 14\n");
      
      printf ("                                                                   \n");
      printf ("                                                                   \n");
      printf (" The following four options should be used with extreme care!         \n");
      printf ("      --restore-decode-key              Restores the decode key from\n");
      printf ("                                        a previous session    \n");
      printf ("      --restore-player-state            Restores the player state\n");
      printf ("                                        from a previous session    \n");
      printf ("      --restore-zone-state              Restores the zone state\n");
      printf ("                                        from a previous session    \n");
      printf ("      --restore-spawns                  Restores the spawns\n");
      printf ("                                        from a previous session    \n");
      printf ("      --restore-all                     Restores decode key, \n");
      printf ("                                        player state, and spawns   \n");
      printf ("                                        from a previous session    \n");
      exit (0);
   }

   /* Set up individual files for logging selected packet types based on
      a common filename base.   The types to log were found by following
      where pre_worked was a precondition for further analysis.
   */

   /* NewSpawnCode */
   showeq_params->NewSpawnCodeFilename = showeq_params->EncryptedLogFilenameBase + QString("_NewSpawnCode.log");
   
   
   /* ZoneSpawnsCode */
   showeq_params->ZoneSpawnsCodeFilename = showeq_params->EncryptedLogFilenameBase + QString("_ZoneSpawnsCode.log");
  
   /* CharProfileCode */
   showeq_params->CharProfileCodeFilename = showeq_params->EncryptedLogFilenameBase + QString ("CharProfileCode.log");

   if (showeq_params->ItemDBEnabled)
   {
     // Create an instance of the ItemDB
     pItemDB = new EQItemDB;

     // make it's parameters match those set via the config file and 
     // command line
     pItemDB->SetDBFile(EQItemDB::LORE_DB, showeq_params->ItemLoreDBFilename);
     pItemDB->SetDBFile(EQItemDB::NAME_DB, showeq_params->ItemNameDBFilename);
     pItemDB->SetDBFile(EQItemDB::DATA_DB, showeq_params->ItemDataDBFilename);
     pItemDB->SetDBFile(EQItemDB::RAW_DATA_DB, showeq_params->ItemRawDataDBFileName);
     pItemDB->SetEnabledDBTypes(showeq_params->ItemDBTypes);

     // Make sure the databases are upgraded to the current format
     pItemDB->Upgrade();
   }
   else
     pItemDB = NULL;

   if (showeq_params->logEncrypted)
   {
      printf("Logging CharProfileCode packets to: %s\n",
	     (const char*)showeq_params->CharProfileCodeFilename);
      printf("Logging ZoneSpawnsCode packets to: %s\n",
	     (const char*)showeq_params->ZoneSpawnsCodeFilename);
      printf("Logging NewSpawnCode packets to: %s\n",
	     (const char*)showeq_params->NewSpawnCodeFilename);
   }

   if (showeq_params->broken_decode)   
    printf( "***DECRYPTION DISABLED***\n\n"
    
            "Decoder has been manually disabled by either -b, the BrokenDecode option,\n"
            "or a missing libEQ.a library.\n\n"
            
            "(There should be a more detailed message above)\n"
          );

   /* Verify OpCode Monitor settings... */
   if (showeq_params->monitorOpCode_Usage)
   {
     if (!(showeq_params->monitorOpCode_List.isEmpty()))
         printf( "\nOpCode monitoring ENABLED...\n"
                 "Using list:\t%s\n\n",
                 
                 (const char*)showeq_params->monitorOpCode_List
               );

      else
      {
         showeq_params->monitorOpCode_Usage = false;
         printf( "\nOpCode monitoring COULD NOT BE ENABLED!\n"
                 ">> Please check your ShowEQ.xml file for a list entry under [OpCodeMonitoring]\n\n"
               );
      }
   }

   /* The main interface widget */
   EQInterface intf (0, "interface");
   qapp.setMainWidget (&intf);
   
   /* Start the main loop */
   int ret = qapp.exec ();

   // Shutdown the Item DB before any other cleanup
   if (pItemDB != NULL)
     pItemDB->Shutdown();

   // delete the ItemDB before application exit
   delete pItemDB;

   // Causes segv on exit all of a sudden.  why?  no changes to cprefs classes. -- cybertech
   delete pSEQPrefs;

   return ret;
}
