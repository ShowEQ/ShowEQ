/*
 * messages.cpp
 * 
 * ShowEQ Distributed under GPL
 * http://seq.sourceforge.net/
 *
 * Copyright 2002-2003 Zaphod (dohpaz@users.sourceforge.net)
 *
 */

#include "messages.h"
#include "datetimemgr.h"

//----------------------------------------------------------------------
// Messages
Messages::Messages(DateTimeMgr* dateTimeMgr, MessageFilters* messageFilters,
		   QObject* parent, const char* name)
  : QObject(parent, name),
    m_dateTimeMgr(dateTimeMgr),
    m_messageFilters(messageFilters),
    m_messageTypeStrings(MT_Max+1)
{
  m_messageTypeStrings[MT_Guild] = "Guild";
  m_messageTypeStrings[MT_Group] = "Group";
  m_messageTypeStrings[MT_Shout] = "Shout";
  m_messageTypeStrings[MT_Auction] = "Auction";
  m_messageTypeStrings[MT_OOC] = "OOC";
  m_messageTypeStrings[MT_Tell] = "Tell";
  m_messageTypeStrings[MT_Say] = "Say";
  m_messageTypeStrings[MT_GMSay] = "GMSay";
  m_messageTypeStrings[MT_GMTell] = "GMTell";
  m_messageTypeStrings[MT_Raid] = "Raid";
  m_messageTypeStrings[MT_Debug] = "Debug";
  m_messageTypeStrings[MT_Info] = "Info";
  m_messageTypeStrings[MT_General] = "General";
  m_messageTypeStrings[MT_Motd] = "MOTD";
  m_messageTypeStrings[MT_System] = "System";
  m_messageTypeStrings[MT_Money] = "Money";
  m_messageTypeStrings[MT_Random] = "Random";
  m_messageTypeStrings[MT_Emote] = "Emote";
  m_messageTypeStrings[MT_Time] = "Time";
  m_messageTypeStrings[MT_Spell] = "Spell";
  m_messageTypeStrings[MT_Zone] = "Zone";
  m_messageTypeStrings[MT_Inspect] = "Inspect";
  m_messageTypeStrings[MT_Player] = "Player";
  m_messageTypeStrings[MT_Consider] = "Consider";
  m_messageTypeStrings[MT_Alert] = "Alert";
  m_messageTypeStrings[MT_Danger] = "Danger";
  m_messageTypeStrings[MT_Caution] = "Caution";
  m_messageTypeStrings[MT_Hunt] = "Hunt";
  m_messageTypeStrings[MT_Locate] = "Locate";

  connect(m_messageFilters, SIGNAL(removed(uint32_t, uint8_t)),
	  this, SLOT(removedFilter(uint32_t, uint8_t)));
  connect(m_messageFilters, SIGNAL(added(uint32_t, uint8_t, 
					 const MessageFilter&)),
	  this, SLOT(addedFilter(uint32_t, uint8_t, const MessageFilter&)));
}

Messages::~Messages()
{
}

void Messages::addMessage(MessageType type, const QString& text, 
			  uint32_t color)
{
  // filter the message
  uint32_t filterFlags = m_messageFilters->filterMessage(type, text);
  
  // create a message entry
  MessageEntry message(type, QDateTime::currentDateTime(),
		       m_dateTimeMgr->updatedDateTime(),
		       text, color, filterFlags);

  // create the message and append it to the end of the list
  m_messages.append(message);

  // signal that a new message exists
  emit newMessage(message);
}

void Messages::clear(void)
{
  // clear the messages
  m_messages.clear();

  // signal that the messages have been cleared
  emit cleared();
}

void Messages::removedFilter(uint32_t mask, uint8_t filter)
{
  // filter has been removed, remove its mask from all the messages
  MessageList::iterator it;
  for (it = m_messages.begin(); it != m_messages.end(); ++it)
    (*it).setFilterFlags((*it).filterFlags() & ~mask);
}

void Messages::addedFilter(uint32_t mask, uint8_t filterid, 
			   const MessageFilter& filter)
{
  // filter has been added, filter all messages against it
  MessageList::iterator it;
  for (it = m_messages.begin(); it != m_messages.end(); ++it)
    if (filter.isFiltered(*it))
      (*it).setFilterFlags((*it).filterFlags() | mask);
}


