/*
 * terminal.h
 * 
 * ShowEQ Distributed under GPL
 * http://seq.sourceforge.net/
 *
 * Copyright 2003 Zaphod (dohpaz@users.sourceforge.net)
 *
 */

#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include "messages.h"

#include <stdint.h>

#include <qobject.h>
#include <qregexp.h>

//----------------------------------------------------------------------
// forward declarations
class QString;

//----------------------------------------------------------------------
// Terminal
class Terminal : public QObject
{
  Q_OBJECT
 public:
  Terminal(Messages* messages, 
	   QObject* parent = 0, const char* name = 0);
  ~Terminal();

  // accessors
  uint64_t enabledTypes() const { return m_enabledTypes; }
  const QString& dateTimeFormat() const { return m_dateTimeFormat; }
  const QString& eqDateTimeFormat() const { return m_eqDateTimeFormat; }
  bool displayType() const { return m_displayType; }
  bool displayDateTime() const { return m_displayDateTime; } 
  bool displayEQDateTime() const { return m_displayEQDateTime; }
  bool useColor() const { return m_useColor; }
  
 public slots:
  void setEnabledTypes(uint64_t types);
  void setDateTimeForamt(const QString& dateTime);
  void setEQDateTimeFormat(const QString& dateTime);
  void setDisplayType(bool enable);
  void setDisplayDateTime(bool enable);
  void setDisplayEQDateTime(bool enable);
  void setUseColor(bool enable);

 protected slots:
  void newMessage(const MessageEntry& message);

 protected:
  Messages* m_messages;
  uint64_t m_enabledTypes;
  QRegExp m_itemPattern;
  QString m_dateTimeFormat;
  QString m_eqDateTimeFormat;
  bool m_displayType;
  bool m_displayDateTime;
  bool m_displayEQDateTime;
  bool m_useColor;
};

#endif // _TERMINAL_H_
