/*
 * messages.h
 * 
 * ShowEQ Distributed under GPL
 * http://seq.sourceforge.net/
 *
 * Copyright 2002-2003 Zaphod (dohpaz@users.sourceforge.net)
 *
 */

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <stdint.h>

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qvaluelist.h>
#include <qvaluevector.h>

//----------------------------------------------------------------------
// forward declarations
class DateTimeMgr;

//----------------------------------------------------------------------
// constants
const uint32_t ME_InvalidColor = 0x000000FF;

//----------------------------------------------------------------------
// enumerated types
enum MessageType
{
  MT_Guild = 0, 
  MT_Group = 2,
  MT_Shout = 3,
  MT_Auction = 4,
  MT_OOC = 5,
  MT_Tell = 7,
  MT_Say = 8,
  MT_GMSay = 11,
  MT_GMTell = 14,
  MT_Raid = 15,
  MT_Debug,
  MT_Info,
  MT_General,
  MT_Motd,
  MT_System,
  MT_Money,
  MT_Random,
  MT_Emote,
  MT_Time,
  MT_Spell,
  MT_Zone,
  MT_Inspect,
  MT_Player,
  MT_Consider,
  MT_Alert,
  MT_Danger,
  MT_Caution,
  MT_Hunt,
  MT_Locate,
  MT_Max = MT_Locate,
};

//----------------------------------------------------------------------
// MessageEntry
class MessageEntry
{
 public:
  MessageEntry(MessageType type, 
	       const QDateTime& dateTime,
	       const QDateTime& eqDateTime,
	       const QString& text,
	       uint32_t color = ME_InvalidColor);
  MessageEntry();
  ~MessageEntry();

  MessageType type() const;
  const QDateTime& dateTime() const;
  const QDateTime& eqDateTime() const;
  const QString& text() const;
  const uint32_t color() const;

 protected:
  MessageType m_type;
  QDateTime m_dateTime;
  QDateTime m_eqDateTime;
  QString m_text;
  uint32_t m_color;
};

inline MessageType MessageEntry::type() const
{
  return m_type;
}

inline const QDateTime& MessageEntry::dateTime() const
{
  return m_dateTime;
}

inline const QDateTime& MessageEntry::eqDateTime() const
{
  return m_eqDateTime;
}

inline const QString& MessageEntry::text() const
{
  return m_text;
}

inline const uint32_t MessageEntry::color() const
{ 
  return m_color;
}

//----------------------------------------------------------------------
// MessageList
typedef QValueList<MessageEntry> MessageList;

//----------------------------------------------------------------------
// Messages
class Messages : public QObject
{
  Q_OBJECT

 public:
  Messages(DateTimeMgr* dateTimeMgr, QObject* parent = 0, const char* name = 0);
  ~Messages();
  
  const MessageList messageList() const;
  const QString& messageTypeString(MessageType type);

 public slots:
  void addMessage(MessageType type, const QString& text, 
		  uint32_t color = ME_InvalidColor);
  void clear(void);

 signals:
  void newMessage(const MessageEntry& message);
  void cleared(void);

 protected:
  DateTimeMgr* m_dateTimeMgr;
  MessageList m_messages;
  QValueVector<QString> m_messageTypeStrings;
};

inline const MessageList Messages::messageList() const
{
  return m_messages;
}

inline const QString& Messages::messageTypeString(MessageType type)
{
  static QString dummy;

  if (QValueVector<QString>::size_type(type) < m_messageTypeStrings.count())
    return m_messageTypeStrings[type];
  
  return dummy;
}

#endif // _MESSAGES_H_
