/*
 * messagewindow.h
 *
 * ShowEQ Distributed under GPL
 * http://seq.sf.net/
 */

#ifndef _MESSAGEWINDOW_H_
#define _MESSAGEWINDOW_H_

#include "seqwindow.h"

#include <stdint.h>

class MessageEntry;
class Messages;
class QTextEdit;

class MessageWindow : public SEQWindow
{
  Q_OBJECT
 public:
  MessageWindow(Messages* messages, 
		const QString& prefName = "MessageWindow",
		const QString& caption = "Message Window",
		QWidget* parent = 0, const char* name = 0);
  ~MessageWindow();

 public slots:
  void newMessage(const MessageEntry& message);

 protected:
  Messages* m_messages;
  QTextEdit* m_messageWindow;
  uint32_t m_enabledTypes;
  QColor m_defaultColor;
  QString m_dateTimeFormat;
  QString m_eqDateTimeFormat;
  bool m_displayType;
  bool m_displayDateTime;
  bool m_displayEQDateTime;
  bool m_useColor;
};

#endif // _MESSAGEWINDOW_H_

