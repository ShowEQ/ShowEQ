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
  void setEnabledTypes(uint64_t types) { m_enabledTypes = types; }
  const QString& dateTimeFormat() const { return m_dateTimeFormat; }
  void setDateTimeForamt(const QString& dateTime);
  const QString& eqDateTimeFormat() const { return m_eqDateTimeFormat; }
  void setEQDateTimeFormat(const QString& dateTime);
  bool displayType() const { return m_displayType; }
  void setDisplayType(bool enable) { m_displayType = enable; }
  bool displayDateTime() const { return m_displayDateTime; } 
  void setDisplayDateTime(bool enable) { m_displayDateTime = enable; }
  bool displayEQDateTime() const { return m_displayEQDateTime; }
  void setDisplayEQDateTime(bool enable) { m_displayEQDateTime = enable; }
  bool useColor() const { return m_useColor; }
  void setUseColor(bool enable) { m_useColor = enable; }
  
 public slots:
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
