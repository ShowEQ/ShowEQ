/*
 * guildshell.h
 *
 *  ShowEQ Distributed under GPL
 *  http://www.sourceforge.net/projects/seq
 *
 *  Copyright 2004 Zaphod (dohpaz@users.sourceforge.net). 
 *
 */

#include "guildshell.h"
#include "netstream.h"
#include "util.h"
#include "zonemgr.h"
#include "everquest.h"
#include "diagnosticmessages.h"

#include <qdatetime.h>

GuildMember::GuildMember(NetStream& netStream)
{
  m_name = netStream.readText();
  m_level = uint8_t(netStream.readUInt32());
  m_class = uint8_t(netStream.readUInt32());
  m_guildRank = netStream.readUInt32();
  m_lastOn = time_t(netStream.readUInt32());
  m_publicNote = netStream.readText();
  m_zoneInstance = netStream.readUInt16();
  m_zoneId = netStream.readUInt16();
}

GuildMember::~GuildMember()
{
}

void GuildMember::update(const GuildMemberUpdate* gmu)
{
  if (gmu->type == 0xe3)
  {
    m_zoneId = gmu->zoneId;
    m_zoneInstance = gmu->zoneInstance;
    m_lastOn = gmu->lastOn;
  }
}

GuildShell::GuildShell(ZoneMgr* zoneMgr, QObject* parent, const char* name)
  : QObject(parent, name),
    m_zoneMgr(zoneMgr)
{
  m_members.setAutoDelete(true);
}

GuildShell::~GuildShell()
{
}

void GuildShell::guildMemberList(const uint8_t* data, size_t len)
{
  // clear out any existing member data
  emit cleared();
  m_members.clear();

  // construct a netstream object on the data
  NetStream gml(data, len);
  
  // read the player name from the front of the stream
  QString player = gml.readText();

  // read the player count from the stream
  uint32_t count = gml.readUInt32();

  seqDebug("Guild has %d members:", count);

  GuildMember* member;

  QDateTime dt;

  // iterate over the data 
  while (!gml.end())
  {
    member = new GuildMember(gml);
    m_members.insert(member->name(), member);
    emit added(member);

    dt.setTime_t(member->lastOn());
    seqDebug("%-64s\t%d\t%s\t%d\t%s\t'%s'\t%s:%d",
	     (const char*)member->name(),
	     member->level(),
	     (const char*)classString(member->classVal()),
	     member->guildRank(), 
	     (const char*)dt.toString(),
	     (const char*)member->publicNote(),
	     (const char*)m_zoneMgr->zoneNameFromID(member->zoneId()),
	     member->zoneInstance());
	     
  }

  seqDebug("Finished processing guildmates. (%d in members dict)", 
	   m_members.count());
}

void GuildShell::guildMemberUpdate(const uint8_t* data, size_t len)
{
  const GuildMemberUpdate* gmu = (const GuildMemberUpdate*)data;

  QString memberName = QString::fromUtf8(gmu->name);

  // find the member
  GuildMember* member = m_members[memberName];

  // update the guild members information
  if (member)
  {
    member->update(gmu);
    emit updated(member);
    QDateTime dt;
    dt.setTime_t(member->lastOn());
    seqDebug("%s is now in zone %s (lastOn: %s).",
	     (const char*)member->name(), 
	     (const char*)m_zoneMgr->zoneNameFromID(member->zoneId()),
	     (const char*)dt.toString());
  }
  else
  {
    seqDebug("GuildShell::guildMemberUpdate(): Failed to find '%s'(%d)!",
	     (const char*)memberName, memberName.length());
    seqDebug("%d in members dict.", m_members.count());
#if 1
    QDictIterator<GuildMember> it(m_members); // See QDictIterator
    for( ; it.current(); ++it )
	seqDebug("'%s'(%d): '%s'(%d)", 
		 (const char*)it.currentKey(), it.currentKey().length(),
		 (const char*)it.current()->name(), it.current()->name().length());
#endif
  }
}




