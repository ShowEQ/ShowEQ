/*
 * messageshell.cpp
 * 
 * ShowEQ Distributed under GPL
 * http://seq.sourceforge.net/
 *
 * Copyright 2002-2003 Zaphod (dohpaz@users.sourceforge.net)
 *
 */

#include "messageshell.h"
#include "eqstr.h"
#include "messages.h"
#include "everquest.h"
#include "spells.h"
#include "zonemgr.h"
#include "spawnshell.h"
#include "player.h"
#include "packetcommon.h"
#include "util.h"

MessageShell::MessageShell(Messages* messages, EQStr* eqStrings,
			   Spells* spells, ZoneMgr* zoneMgr, 
			   SpawnShell* spawnShell, Player* player,
			   QObject* parent, const char* name)
  : QObject(parent, name),
    m_messages(messages),
    m_eqStrings(eqStrings),
    m_spells(spells),
    m_zoneMgr(zoneMgr),
    m_spawnShell(spawnShell),
    m_player(player)
{
}

void MessageShell::channelMessage(const uint8_t* data, size_t, uint8_t dir)
{
  // avoid client chatter and do nothing if not viewing channel messages
  if (dir == DIR_Client)
    return;

  const channelMessageStruct* cmsg = (const channelMessageStruct*)data;

  QString tempStr;

  bool target = false;
  if (cmsg->chanNum >= MT_Tell)
    target = true;

  if (cmsg->language)
  {
    if ((cmsg->target[0] != 0) && target)
    {
      tempStr.sprintf( "'%s' -> '%s' - %s {%s}",
		       cmsg->sender,
		       cmsg->target,
		       cmsg->message,
		       (const char*)language_name(cmsg->language)
		       );
    }
    else
    {
      tempStr.sprintf( "'%s' - %s {%s}",
		       cmsg->sender,
		       cmsg->message,
		       (const char*)language_name(cmsg->language)
		       );
    }
  }
  else // Don't show common, its obvious
  {
    if ((cmsg->target[0] != 0) && target)
    {
      tempStr.sprintf( "'%s' -> '%s' - %s",
		       cmsg->sender,
		       cmsg->target,
		       cmsg->message
		       );
    }
    else
    {
      tempStr.sprintf( "'%s' - %s",
		       cmsg->sender,
		       cmsg->message
		       );
    }
  }

  m_messages->addMessage((MessageType)cmsg->chanNum, tempStr);
}

void MessageShell::formattedMessage(const uint8_t* data, size_t len, uint8_t dir)
{
  // avoid client chatter and do nothing if not viewing channel messages
  if (dir == DIR_Client)
    return;

  const formattedMessageStruct* fmsg = (const formattedMessageStruct*)data;
  QString tempStr;

  size_t messagesLen = 
    len
    - ((uint8_t*)&fmsg->messages[0] - (uint8_t*)fmsg) 
    - sizeof(fmsg->unknownXXXX);

  m_messages->addMessage(MT_General, 
			 m_eqStrings->formatMessage(fmsg->messageFormat,
						    fmsg->messages, 
						    messagesLen));
}

void MessageShell::simpleMessage(const uint8_t* data, size_t len, uint8_t dir)
{
  // avoid client chatter and do nothing if not viewing channel messages
  if (dir == DIR_Client)
    return;

  const simpleMessageStruct* smsg = (const simpleMessageStruct*)data;
  QString tempStr;
 
  m_messages->addMessage(MT_General, 
			 m_eqStrings->message(smsg->messageFormat), 
			 smsg->color);
}

void MessageShell::specialMessage(const uint8_t* data, size_t, uint8_t dir)
{
  // avoid client chatter and do nothing if not viewing channel messages
  if (dir == DIR_Client)
    return;

  const specialMessageStruct* smsg = (const specialMessageStruct*)data;

  const Item* target = NULL;
  
  if (smsg->target)
    target = m_spawnShell->findID(tSpawn, smsg->target);

  // calculate the message position
  const char* message = smsg->source + strlen(smsg->source) + 1 
    + sizeof(smsg->unknown0xxx);
  
  if (target)
    m_messages->addMessage(MT_General, 
			   QString("Special: '%1' -> '%2' - %3")
			   .arg(smsg->source)
			   .arg(target->name())
			   .arg(message));
  else
    m_messages->addMessage(MT_General,
			   QString("Formatted:Special: '%1' - %2")
			   .arg(smsg->source)
			   .arg(message));
}

void MessageShell::guildMOTD(const uint8_t* data, size_t, uint8_t dir)
{
  // avoid client chatter and do nothing if not viewing channel messages
  if (dir == DIR_Client)
    return;

  const guildMOTDStruct* gmotd = (const guildMOTDStruct*)data;

  m_messages->addMessage(MT_Guild, 
			 QString("MOTD: %1 - %2")
			 .arg(QString::fromUtf8(gmotd->sender))
			 .arg(QString::fromUtf8(gmotd->message)));
}


void MessageShell::moneyOnCorpse(const uint8_t* data)
{
  const moneyOnCorpseStruct* money = (const moneyOnCorpseStruct*)data;

  QString tempStr;

  if( money->platinum || money->gold || money->silver || money->copper )
  {
    bool bneedComma = false;
    
    tempStr = "You receive ";
    
    if(money->platinum)
    {
      tempStr += QString::number(money->platinum) + " platinum";
      bneedComma = true;
    }
    
    if(money->gold)
    {
      if(bneedComma)
	tempStr += ", ";
      
      tempStr += QString::number(money->gold) + " gold";
      bneedComma = true;
    }
    
    if(money->silver)
    {
      if(bneedComma)
	tempStr += ", ";
      
      tempStr += QString::number(money->silver) + " silver";
      bneedComma = true;
    }
    
    if(money->copper)
      {
	if(bneedComma)
	  tempStr += ", ";
	
	tempStr += QString::number(money->copper) + " copper";
      }
    
    tempStr += " from the corpse";
    
    m_messages->addMessage(MT_Money, tempStr);
  }
}

void MessageShell::moneyUpdate(const uint8_t* data)
{
  //  const moneyUpdateStruct* money = (const moneyUpdateStruct*)data;
  m_messages->addMessage(MT_Money, "Update");
}

void MessageShell::moneyThing(const uint8_t* data)
{
  //  const moneyUpdateStruct* money = (const moneyUpdateStruct*)data;
  m_messages->addMessage(MT_Money, "Thing");
}

void MessageShell::randomRequest(const uint8_t* data)
{
  const randomReqStruct* randr = (const randomReqStruct*)data;
  QString tempStr;

  tempStr.sprintf("Request random number between %d and %d\n",
		  randr->bottom,
		  randr->top);
  
  m_messages->addMessage(MT_Random, tempStr);
}

void MessageShell::random(const uint8_t* data)
{
  const randomStruct* randr = (const randomStruct*)data;
  QString tempStr;

  tempStr.sprintf("Random number %d rolled between %d and %d by %s\n",
		  randr->result,
		  randr->bottom,
		  randr->top,
		  randr->name);
  
  m_messages->addMessage(MT_Random, tempStr);
}

void MessageShell::emoteText(const uint8_t* data)
{
  const emoteTextStruct* emotetext = (const emoteTextStruct*)data;
  QString tempStr;

  m_messages->addMessage(MT_Emote, emotetext->text);
}

void MessageShell::inspectData(const uint8_t* data)
{
  const inspectDataStruct *inspt = (const inspectDataStruct *)data;
  QString tempStr;

  for (int inp = 0; inp < 21; inp ++)
  {
    tempStr.sprintf("He has %s (icn:%i)\n", inspt->itemNames[inp], inspt->icons[inp]);
    m_messages->addMessage(MT_Zone, tempStr);
  }

  tempStr.sprintf("His info: %s\n", inspt->mytext);
  m_messages->addMessage(MT_Zone, tempStr);
}

void MessageShell::logOut(const uint8_t*, size_t, uint8_t)
{
  m_messages->addMessage(MT_Zone, "LogoutCode: Client logged out of server");
}

void MessageShell::zoneEntryClient(const ClientZoneEntryStruct* zsentry)
{
  m_messages->addMessage(MT_Zone, "EntryCode: Client");
}

void MessageShell::zoneEntryServer(const ServerZoneEntryStruct* zsentry)
{
  QString tempStr;

  tempStr = "Zone: EntryCode: Server, Zone: ";
  tempStr += m_zoneMgr->zoneNameFromID(zsentry->zoneId);
  m_messages->addMessage(MT_Zone, tempStr);
}

void MessageShell::zoneChanged(const zoneChangeStruct* zoneChange, size_t, uint8_t dir)
{
  QString tempStr;

  if (dir == DIR_Client)
  {
    tempStr = "Zone: ChangeCode: Client, Zone: ";
    tempStr += m_zoneMgr->zoneNameFromID(zoneChange->zoneId);
  }
  else
  {
    tempStr = "Zone: ChangeCode: Server, Zone:";
    tempStr += m_zoneMgr->zoneNameFromID(zoneChange->zoneId);
  }
  
  m_messages->addMessage(MT_Zone, tempStr);
}

void MessageShell::zoneNew(const uint8_t* data, size_t, uint8_t dir)
{
  const newZoneStruct* zoneNew = (const newZoneStruct*)data;
  QString tempStr;
  tempStr = "Zone: NewCode: Zone: ";
  tempStr += QString(zoneNew->shortName) + " ("
    + zoneNew->longName + ")";
  m_messages->addMessage(MT_Zone, tempStr);
}

void MessageShell::zoneBegin(const QString& shortZoneName)
{
  QString tempStr;
  tempStr = QString("Zone: Zoning, Please Wait...\t(Zone: '")
    + shortZoneName + "')";
  m_messages->addMessage(MT_Zone, tempStr);
}

void MessageShell::zoneEnd(const QString& shortZoneName, 
			   const QString& longZoneName)
{
  QString tempStr;
  tempStr = QString("Zone: Entered: ShortName = '") + shortZoneName +
                    "' LongName = " + longZoneName;

  m_messages->addMessage(MT_Zone, tempStr);
}

void MessageShell::zoneChanged(const QString& shortZoneName)
{
  QString tempStr;
  tempStr = QString("Zone: Zoning, Please Wait...\t(Zone: '")
    + shortZoneName + "')";
  m_messages->addMessage(MT_Zone, tempStr);
}


void MessageShell::worldMOTD(const uint8_t* data)
{ 
  const worldMOTDStruct* motd = (const worldMOTDStruct*)data;
  m_messages->addMessage(MT_Motd, QString::fromUtf8(motd->message));
}

void MessageShell::syncDateTime(const QDateTime& dt)
{
  QString dateString = dt.toString(pSEQPrefs->getPrefString("DateTimeFormat", "Interface", "ddd MMM dd,yyyy - hh:mm ap"));

  m_messages->addMessage(MT_Time, dateString);
}

void MessageShell::handleSpell(const uint8_t* data, size_t, uint8_t dir)
{
  const memSpellStruct* mem = (const memSpellStruct*)data;
  QString tempStr;

  bool client = (dir == DIR_Client);

  tempStr = "";
  
  switch (mem->param1)
  {
  case 0:
    {
      if (!client)
	tempStr = "You have finished scribing '";
      break;
    }
    
  case 1:
    {
      if (!client)
	tempStr = "You have finished memorizing '";
      break;
    }
    
  case 2:
    {
      if (!client)
	tempStr = "You forget '";
      break;
    }
    
  case 3:
    {
      if (!client)
	tempStr = "You finish casting '";
      break;
    }
    
  default:
    {
      tempStr.sprintf( "Unknown Spell Event ( %s ) - '",
		       client  ?
		     "Client --> Server"   :
		       "Server --> Client"
		       );
      break;
    }
  }
  
  
  if (!tempStr.isEmpty())
  {
    QString spellName;
    const Spell* spell = m_spells->spell(mem->spellId);
    
    if (spell)
      spellName = spell->name();
    else
      spellName = spell_name(mem->spellId);

    if (mem->param1 != 3)
      tempStr.sprintf("%s%s', slot %d.", 
		      tempStr.ascii(), 
		      (const char*)spellName, 
		      mem->slotId);
    else 
    {
      tempStr.sprintf("%s%s'.", 
		      tempStr.ascii(), 
		      (const char*)spellName);
    }

    m_messages->addMessage(MT_Spell, tempStr);
  }
}

void MessageShell::beginCast(const uint8_t* data)
{
  const beginCastStruct *bcast = (const beginCastStruct *)data;
  QString tempStr;

  if (!showeq_params->showSpellMsgs)
    return;
  
  tempStr = "";

  if (bcast->spawnId == m_player->id())
    tempStr = "You begin casting '";
  else
  {
    const Item* item = m_spawnShell->findID(tSpawn, bcast->spawnId);
    if (item != NULL)
      tempStr = item->name();
    
    if (tempStr == "" || tempStr.isEmpty())
      tempStr.sprintf("UNKNOWN (ID: %d)", bcast->spawnId);
    
    tempStr += " has begun casting '";
  }
  float casttime = ((float)bcast->param1 / 1000);
  
  QString spellName;
  const Spell* spell = m_spells->spell(bcast->spellId);
  
  if (spell)
    spellName = spell->name();
  else
    spellName = spell_name(bcast->spellId);
  
  tempStr.sprintf( "%s%s' - Casting time is %g Second%s", 
		   tempStr.ascii(),
		   (const char*)spellName, casttime,
		   casttime == 1 ? "" : "s");

  m_messages->addMessage(MT_Spell, tempStr);
}

void MessageShell::spellFaded(const uint8_t* data)
{
  const spellFadedStruct *sf = (const spellFadedStruct *)data;
  QString tempStr;

  if (!showeq_params->showSpellMsgs)
    return;
  
  tempStr.sprintf( "Faded: %s", 
		   sf->message);

  m_messages->addMessage(MT_Spell, tempStr);
}

void MessageShell::interruptSpellCast(const uint8_t* data)
{
  const badCastStruct *icast = (const badCastStruct *)data;
  const Item* item = m_spawnShell->findID(tSpawn, icast->spawnId);

  QString tempStr;
  if (item != NULL)
    tempStr.sprintf("%s(%d): %s\n", 
		    (const char*)item->name(), icast->spawnId, icast->message);
  else
    tempStr.sprintf("spawn(%d): %s\n", 
		    icast->spawnId, icast->message);

  m_messages->addMessage(MT_Spell, tempStr);
}

void MessageShell::startCast(const uint8_t* data)
{
  const startCastStruct* cast = (const startCastStruct*)data;
  QString spellName;
  const Spell* spell = m_spells->spell(cast->spellId);
  
  if (spell)
    spellName = spell->name();
  else
    spellName = spell_name(cast->spellId);

  const Item* item = m_spawnShell->findID(tSpawn, cast->targetId);

  QString targetName;

  if (item != NULL)
    targetName = item->name();
  else
    targetName = "";

  QString tempStr;

  tempStr.sprintf("You begin casting %s.  Current Target is %s(%d)", 
		  (const char*)spellName, (const char*)targetName, 
		  cast->targetId);

  m_messages->addMessage(MT_Spell, tempStr);
}


void MessageShell::groupInfo(const uint8_t* data)
{
  const groupInfoStruct* gmem = (const groupInfoStruct*)data;
  QString tempStr;
  tempStr.sprintf ("Member: %s - %s\n", 
		   gmem->yourname, gmem->membername);
  m_messages->addMessage(MT_Group, tempStr);
}

void MessageShell::groupInvite(const uint8_t* data)
{
  const groupInviteStruct* gmem = (const groupInviteStruct*)data;
  QString tempStr;
  tempStr.sprintf ("Invite: %s invites %s\n", 
		   gmem->membername, gmem->yourname);
  m_messages->addMessage(MT_Group, tempStr);
}

void MessageShell::groupDecline(const uint8_t* data)
{
  const groupDeclineStruct* gmem = (const groupDeclineStruct*)data;
  QString tempStr;
  tempStr.sprintf ("Invite: %s declines invite from %s (%i)\n", 
		   gmem->membername, gmem->yourname, gmem->reason);
  m_messages->addMessage(MT_Group, tempStr);
}

void MessageShell::groupAccept(const uint8_t* data)
{
  const groupAcceptStruct* gmem = (const groupAcceptStruct*)data;
  QString tempStr;
  tempStr.sprintf ("Invite: %s accepts invite from %s\n", 
		   gmem->membername, gmem->yourname);
  m_messages->addMessage(MT_Group, tempStr);
}

void MessageShell::groupDelete(const uint8_t* data)
{
  const groupDeleteStruct* gmem = (const groupDeleteStruct*)data;
  QString tempStr;
  tempStr.sprintf ("Delete: %s - %s\n", 
		   gmem->membername, gmem->yourname);
  m_messages->addMessage(MT_Group, tempStr);
}
