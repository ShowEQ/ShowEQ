/*
 * messagewindow.cpp
 *
 * ShowEQ Distributed under GPL
 * http://seq.sf.net/
 */

#include "messagewindow.h"
#include "messages.h"

#include <qtextedit.h>

MessageWindow::MessageWindow(Messages* messages, 
			     const QString& prefName,
			     const QString& caption,
			     QWidget* parent, const char* name)
  : SEQWindow(prefName, caption, parent, name),
    m_messages(messages),
    m_messageWindow(0),
    m_enabledTypes(0xFFFFFFFF),
    m_defaultColor(black),
    m_dateTimeFormat("hh:mm"),
    m_eqDateTimeFormat("ddd m/d/yyyy h:mm"),
    m_displayType(true),
    m_displayDateTime(false),
    m_displayEQDateTime(false),
    m_useColor(false)
{
  // create the window for text display
  m_messageWindow = new QTextEdit(this, "messageText");

  // make the message window the main widget of the SEQWindow
  setWidget(m_messageWindow);

  // we'll start out trying to use the LogText format (for its speed)
  m_messageWindow->setTextFormat(LogText);

  // connect to the Messages signal(s)
  connect(m_messages, SIGNAL(newMessage(const MessageEntry&)),
	  this, SLOT(newMessage(const MessageEntry&)));
}

MessageWindow::~MessageWindow()
{
}

void MessageWindow::newMessage(const MessageEntry& message)
{
  // if the message type isn't enabled, nothing to do
  if ((m_enabledTypes & ( 1 << message.type())) == 0)
    return;
  
  // if using color, then use it... ;-)
  if (m_useColor)
  {
    // if the message has a specific color, then use it
    if (message.color() != ME_InvalidColor)
      m_messageWindow->setColor(QColor(message.color()));
    else // otherwise use the default color
      m_messageWindow->setColor(m_defaultColor);
  }
 
  QString text;

  // if displaying the type, add it
  if (m_displayType)
    text = m_messages->messageTypeString(message.type()) + ": ";
    
  // if displaying the message date/time append it
  if (m_displayDateTime)
    text += message.dateTime().toString(m_dateTimeFormat) + " - ";

  // if displayint the messages eq date/time, append it
  if (m_displayEQDateTime)
    text += message.eqDateTime().toString(m_eqDateTimeFormat);

  // append the actual message text
  text += message.text();

  // now append the message text to the buffer
  m_messageWindow->append(text);
}


