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

#include "message.h"
#include "messagefilter.h"

#include <stdint.h>

#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qvaluevector.h>

//----------------------------------------------------------------------
// forward declarations
class DateTimeMgr;

//----------------------------------------------------------------------
// MessageList
typedef QValueList<MessageEntry> MessageList;

//----------------------------------------------------------------------
// Messages
class Messages : public QObject
{
  Q_OBJECT

 public:
  Messages(DateTimeMgr* dateTimeMgr, MessageFilters* messageFilters,
	   QObject* parent = 0, const char* name = 0);
  ~Messages();
  
  const MessageList messageList() const;
  const QString& messageTypeString(MessageType type);

 public slots:
  void addMessage(MessageType type, const QString& text, 
		  uint32_t color = ME_InvalidColor);
  void clear(void);

 protected slots:
  void removedFilter(uint32_t mask, uint8_t filter);
  void addedFilter(uint32_t mask, uint8_t filter, const MessageFilter& filter);
   
 signals:
  void newMessage(const MessageEntry& message);
  void cleared(void);

 protected:
  DateTimeMgr* m_dateTimeMgr;
  MessageFilters* m_messageFilters;
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
