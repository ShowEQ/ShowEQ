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
// MessageEntry
MessageEntry::MessageEntry(MessageType type, 
			   const QDateTime& dateTime,
			   const QDateTime& eqDateTime,
			   const QString& text,
			   uint32_t color)
  : m_type(type),
    m_dateTime(dateTime),
    m_eqDateTime(eqDateTime),
    m_text(text),
    m_color(color)
{
}

MessageEntry::MessageEntry()
  : m_type(MT_Debug),
    m_color(0x000000FF)
{
}

MessageEntry::~MessageEntry()
{
}

//----------------------------------------------------------------------
// Messages
Messages::Messages(DateTimeMgr* dateTimeMgr, QObject* parent,
		   const char* name)
  : QObject(parent, name),
    m_dateTimeMgr(dateTimeMgr),
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
  m_messageTypeStrings[MT_Random] = "Random";
  m_messageTypeStrings[MT_Emote] = "Emote";
  m_messageTypeStrings[MT_Time] = "Time";
  m_messageTypeStrings[MT_Spell] = "Spell";
  m_messageTypeStrings[MT_Zone] = "Zone";
  m_messageTypeStrings[MT_Inspect] = "Inspect";
  m_messageTypeStrings[MT_Alert] = "Alert";
  m_messageTypeStrings[MT_Player] = "Player";
}

Messages::~Messages()
{
}

void Messages::addMessage(MessageType type, const QString& text, 
			  uint32_t color)
{
  MessageEntry message(type, QDateTime::currentDateTime(),
		       m_dateTimeMgr->updatedDateTime(),
		       text, color);

  // create the message and append it to the end of the list
  m_messages.append(message);

  emit newMessage(message);
}
