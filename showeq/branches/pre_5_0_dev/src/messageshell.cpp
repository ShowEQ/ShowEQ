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
			   QString("Special: '%1' - %2")
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

  tempStr.sprintf("Request random number between %d and %d",
		  randr->bottom,
		  randr->top);
  
  m_messages->addMessage(MT_Random, tempStr);
}

void MessageShell::random(const uint8_t* data)
{
  const randomStruct* randr = (const randomStruct*)data;
  QString tempStr;

  tempStr.sprintf("Random number %d rolled between %d and %d by %s",
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
    tempStr.sprintf("He has %s (icn:%i)", inspt->itemNames[inp], inspt->icons[inp]);
    m_messages->addMessage(MT_Zone, tempStr);
  }

  tempStr.sprintf("His info: %s", inspt->mytext);
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

  tempStr = "EntryCode: Server, Zone: ";
  tempStr += m_zoneMgr->zoneNameFromID(zsentry->zoneId);
  m_messages->addMessage(MT_Zone, tempStr);
}

void MessageShell::zoneChanged(const zoneChangeStruct* zoneChange, size_t, uint8_t dir)
{
  QString tempStr;

  if (dir == DIR_Client)
  {
    tempStr = "ChangeCode: Client, Zone: ";
    tempStr += m_zoneMgr->zoneNameFromID(zoneChange->zoneId);
  }
  else
  {
    tempStr = "ChangeCode: Server, Zone:";
    tempStr += m_zoneMgr->zoneNameFromID(zoneChange->zoneId);
  }
  
  m_messages->addMessage(MT_Zone, tempStr);
}

void MessageShell::zoneNew(const uint8_t* data, size_t, uint8_t dir)
{
  const newZoneStruct* zoneNew = (const newZoneStruct*)data;
  QString tempStr;
  tempStr = "NewCode: Zone: ";
  tempStr += QString(zoneNew->shortName) + " ("
    + zoneNew->longName + ")";
  m_messages->addMessage(MT_Zone, tempStr);
}

void MessageShell::zoneBegin(const QString& shortZoneName)
{
  QString tempStr;
  tempStr = QString("Zoning, Please Wait...\t(Zone: '")
    + shortZoneName + "')";
  m_messages->addMessage(MT_Zone, tempStr);
}

void MessageShell::zoneEnd(const QString& shortZoneName, 
			   const QString& longZoneName)
{
  QString tempStr;
  tempStr = QString("Entered: ShortName = '") + shortZoneName +
                    "' LongName = " + longZoneName;

  m_messages->addMessage(MT_Zone, tempStr);
}

void MessageShell::zoneChanged(const QString& shortZoneName)
{
  QString tempStr;
  tempStr = QString("Zoning, Please Wait...\t(Zone: '")
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
    tempStr.sprintf("%s(%d): %s", 
		    (const char*)item->name(), icast->spawnId, icast->message);
  else
    tempStr.sprintf("spawn(%d): %s", 
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
  tempStr.sprintf ("Member: %s - %s", 
		   gmem->yourname, gmem->membername);
  m_messages->addMessage(MT_Group, tempStr);
}

void MessageShell::groupInvite(const uint8_t* data)
{
  const groupInviteStruct* gmem = (const groupInviteStruct*)data;
  QString tempStr;
  tempStr.sprintf ("Invite: %s invites %s", 
		   gmem->membername, gmem->yourname);
  m_messages->addMessage(MT_Group, tempStr);
}

void MessageShell::groupDecline(const uint8_t* data)
{
  const groupDeclineStruct* gmem = (const groupDeclineStruct*)data;
  QString tempStr;
  tempStr.sprintf ("Invite: %s declines invite from %s (%i)", 
		   gmem->membername, gmem->yourname, gmem->reason);
  m_messages->addMessage(MT_Group, tempStr);
}

void MessageShell::groupAccept(const uint8_t* data)
{
  const groupAcceptStruct* gmem = (const groupAcceptStruct*)data;
  QString tempStr;
  tempStr.sprintf ("Invite: %s accepts invite from %s", 
		   gmem->membername, gmem->yourname);
  m_messages->addMessage(MT_Group, tempStr);
}

void MessageShell::groupDelete(const uint8_t* data)
{
  const groupDeleteStruct* gmem = (const groupDeleteStruct*)data;
  QString tempStr;
  tempStr.sprintf ("Delete: %s - %s", 
		   gmem->membername, gmem->yourname);
  m_messages->addMessage(MT_Group, tempStr);
}

void MessageShell::player(const uint8_t* data)
{
  const charProfileStruct* player = (const charProfileStruct*)data;
  QString message;

  message.sprintf("Name: '%s' Last: '%s'\n", 
		  player->name, player->lastName);
  m_messages->addMessage(MT_Player, message);
  
  message.sprintf("Level: %d\n", player->level);
  m_messages->addMessage(MT_Player, message);
  
  message.sprintf("PlayerMoney: P=%d G=%d S=%d C=%d\n",
		 player->platinum, player->gold, 
		 player->silver, player->copper);
  m_messages->addMessage(MT_Player, message);
  
  message.sprintf("BankMoney: P=%d G=%d S=%d C=%d\n",
		  player->platinum_bank, player->gold_bank, 
		  player->silver_bank, player->copper_bank);
  m_messages->addMessage(MT_Player, message);

  message.sprintf("CursorMoney: P=%d G=%d S=%d C=%d\n",
		  player->platinum_cursor, player->gold_cursor, 
		  player->silver_cursor, player->copper_cursor);
  m_messages->addMessage(MT_Player, message);

  message.sprintf("SharedMoney: P=%d\n",
		  player->platinum_shared);
  m_messages->addMessage(MT_Player, message);

  message = "Player: Exp =" + Commanate(player->exp);
  m_messages->addMessage(MT_Player, message);

  message = "Player: ExpAA =" + Commanate(player->altexp);
  m_messages->addMessage(MT_Player, message);

  int buffnumber;
  for (buffnumber=0;buffnumber<15;buffnumber++)
  {
    if (player->buffs[buffnumber].spellid && player->buffs[buffnumber].duration)
    {
      message.sprintf("You have buff %s duration left is %d in ticks.\n",
		      (const char*)spell_name(player->buffs[buffnumber].spellid),
		      player->buffs[buffnumber].duration);
      m_messages->addMessage(MT_Player, message);
    }
  }
}

void MessageShell::increaseSkill(const uint8_t* data)
{
  const skillIncStruct* skilli = (const skillIncStruct*)data;
  QString tempStr;
  tempStr.sprintf("Skill: %s has increased (%d)",
		  (const char*)skill_name(skilli->skillId),
		  skilli->value);
  m_messages->addMessage(MT_Player, tempStr);
}

void MessageShell::updateLevel(const uint8_t* data)
{
  const levelUpUpdateStruct *levelup = (const levelUpUpdateStruct *)data;
  QString tempStr;
  tempStr.sprintf("NewLevel: %d\n", levelup->level);
  m_messages->addMessage(MT_Player, tempStr);
}
  


void MessageShell::consMessage(const uint8_t* data, size_t, uint8_t dir) 
{
  const considerStruct * con = (const considerStruct*)data;
  const Item* item;

  QString lvl("");
  QString hps("");
  QString cn("");
  QString deity;

  QString msg("Your faction standing with ");

  // is it you that you've conned?
  if (con->playerid == con->targetid) 
  {
    deity = m_player->deityName();
    
    // well, this is You
    msg += m_player->name();
  }
  else 
  {
    // find the spawn if it exists
    item = m_spawnShell->findID(tSpawn, con->targetid);
    
    // has the spawn been seen before?
    if (item != NULL)
    {
      Spawn* spawn = (Spawn*)item;

      // yes
      deity = spawn->deityName();

      msg += item->name();
    } // end if spawn found
    else
      msg += "Spawn:" + QString::number(con->targetid, 16);
  } // else not yourself
  
  switch (con->level) 
  {
  case 0:
    msg += " (even)";
    break;
  case 2:
    msg += " (green)";
    break;
  case 4:
    msg += " (blue)";
    break;
  case 13:
    msg += " (red)";
    break;
  case 15:
    msg += " (yellow)";
    break;
  case 18:
    msg += " (cyan)";
    break;
  default:
    msg += " (unknown: " + QString::number(con->level) + ")";
    break;
  }

  if (!deity.isEmpty())
    msg += QString(" [") + deity + "]";

  if (con->maxHp || con->curHp)
  {
    lvl.sprintf(" (%i/%i HP)", con->curHp, con->maxHp);
    msg += lvl;
  }
  
  msg += QString(" is: ") + print_faction(con->faction) + " (" 
    + QString::number(con->faction) + ")!";

  m_messages->addMessage(MT_Consider, msg);
} // end consMessage()


void MessageShell::setExp(uint32_t totalExp, uint32_t totalTick,
			  uint32_t minExpLevel, uint32_t maxExpLevel, 
			  uint32_t tickExpLevel)
{
    QString tempStr;
    tempStr.sprintf("Exp: Set: %u total, with %u (%u/330) into level with %u left, where 1/330 = %u",
		    totalExp, (totalExp - minExpLevel), totalTick, 
		    (maxExpLevel - totalExp), tickExpLevel);
    m_messages->addMessage(MT_Player, tempStr);
}

void MessageShell::newExp(uint32_t newExp, uint32_t totalExp, 
			  uint32_t totalTick,
			  uint32_t minExpLevel, uint32_t maxExpLevel, 
			  uint32_t tickExpLevel)
{
  QString tempStr;
  uint32_t leftExp = maxExpLevel - totalExp;

  // only can display certain things if new experience is greater then 0,
  // ie. a > 1/330'th experience increment.
  if (newExp)
  {
    // calculate the number of this type of kill needed to level.
    uint32_t needKills = leftExp / newExp;

    tempStr.sprintf("Exp: New: %u, %u (%u/330) into level with %u left [~%u kills]",
		    newExp, (totalExp - minExpLevel), totalTick, 
		    leftExp, needKills);
  }
  else
    tempStr.sprintf("Exp: New: < %u, %u (%u/330) into level with %u left",
		    tickExpLevel, (totalExp - minExpLevel), totalTick, 
		    leftExp);
  
  m_messages->addMessage(MT_Player, tempStr);
}

void MessageShell::setAltExp(uint32_t totalExp,
			     uint32_t maxExp, uint32_t tickExp, 
			     uint32_t aaPoints)
{
  QString tempStr;
  tempStr.sprintf("ExpAA: Set: %u total, with %u aapoints",
		  totalExp, aaPoints);

  m_messages->addMessage(MT_Player, tempStr);
}

void MessageShell::newAltExp(uint32_t newExp, uint32_t totalExp, 
			     uint32_t totalTick, 
			     uint32_t maxExp, uint32_t tickExp, 
			     uint32_t aapoints)
{
  QString tempStr;
  
  // only can display certain things if new experience is greater then 0,
  // ie. a > 1/330'th experience increment.
  if (newExp)
    tempStr.sprintf("ExpAA: %u, %u (%u/330) with %u left",
		    newExp, totalExp, totalTick, maxExp - totalExp);
  else
    tempStr.sprintf("ExpAA: < %u, %u (%u/330) with %u left",
		    tickExp, totalExp, totalTick, maxExp - totalExp);

  m_messages->addMessage(MT_Player, tempStr);
}


