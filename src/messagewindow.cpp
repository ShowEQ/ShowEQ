/*
 * messagewindow.cpp
 *
 * ShowEQ Distributed under GPL
 * http://seq.sf.net/
 */

#include "messagewindow.h"
#include "messages.h"

#include <qtextedit.h>
#include <qpopupmenu.h>

MessageWindow::MessageWindow(Messages* messages, 
			     const QString& prefName,
			     const QString& caption,
			     QWidget* parent, const char* name)
  : SEQWindow(prefName, caption, parent, name),
    m_messages(messages),
    m_menu(0),
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

  m_menu = new QPopupMenu;
  m_typeFilterMenu = new QPopupMenu;
  m_menu->insertItem("Show Message Type", m_typeFilterMenu);

  QString typeName;
  for (int i = MT_Guild; i <= MT_Max; i++)
  {
    typeName = messages->messageTypeString((MessageType)i);
    if (!typeName.isEmpty())
    {
      m_typeFilterMenu->insertItem(typeName, i);
      m_typeFilterMenu->setItemChecked(i, (((1 << i) & m_enabledTypes) != 0));
    }
  }
  
  connect(m_typeFilterMenu, SIGNAL(activated(int)),
	  this, SLOT(toggleTypeFilter(int)));

  QPopupMenu* userFilterMenu = new QPopupMenu;
  m_menu->insertItem("User Message Filters", userFilterMenu);

  m_menu->insertSeparator(-1);
  int x;
  x = m_menu->insertItem("Display Type", this, SLOT(toggleDisplayType(int)));
  m_menu->setItemChecked(x, m_displayType);
  x = m_menu->insertItem("Display Time/Date", this, SLOT(toggleDisplayTime(int)));
  m_menu->setItemChecked(x, m_displayDateTime);
  x = m_menu->insertItem("Display EQ Date/Time", this, SLOT(toggleEQDisplayTime(int)));
  m_menu->setItemChecked(x, m_displayEQDateTime);
  x = m_menu->insertItem("Use Color", this, SLOT(toggleUseColor(int)));
  m_menu->setItemChecked(x, m_useColor);
  m_menu->insertSeparator(-1);
  x = m_menu->insertItem("Refresh Messages...", this, SLOT(refreshMessages()));


  refreshMessages();
}

MessageWindow::~MessageWindow()
{
}

void MessageWindow::mousePressEvent(QMouseEvent* e)
{
  if (e->button() == RightButton)
    m_menu->popup(mapToGlobal(e->pos()));
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

  // if displaying the messages eq date/time, append it
  if (m_displayEQDateTime && (message.eqDateTime().isValid()))
    text += message.eqDateTime().toString(m_eqDateTimeFormat) + " - ";

  // append the actual message text
  text += message.text();

  // now append the message text to the buffer
  m_messageWindow->append(text);
}

void MessageWindow::toggleTypeFilter(int id)
{
  printf("toggleTypeFilter(%d)\n", id);
  if (((1 << id) & m_enabledTypes) != 0)
    m_enabledTypes &= ~(1 << id);
  else
    m_enabledTypes |= (1 << id);
 
  m_typeFilterMenu->setItemChecked(id, ((m_enabledTypes & (1 << id)) != 0));
}

void MessageWindow::toggleDisplayType(int id)
{
  m_displayType = !m_displayType;
  m_menu->setItemChecked(id, m_displayType);
}

void MessageWindow::toggleDisplayTime(int id)
{
  m_displayDateTime = !m_displayDateTime;
  m_menu->setItemChecked(id, m_displayDateTime);
}

void MessageWindow::toggleEQDisplayTime(int id)
{
  m_displayEQDateTime = ! m_displayEQDateTime;
  m_menu->setItemChecked(id, m_displayEQDateTime);
}

void MessageWindow::toggleUseColor(int id)
{
  m_useColor = !m_useColor;
  m_menu->setItemChecked(id, m_useColor);
}

void MessageWindow::refreshMessages()
{
  // clear the document
  m_messageWindow->clear();

  // move the cursor to the end of the document
  m_messageWindow->moveCursor(QTextEdit::MoveEnd, false);

  // get the list of messages
  const MessageList& messages = m_messages->messageList();
 
  // iterate over the message list and add the messages
  MessageList::const_iterator it;
  for (it = messages.begin(); it != messages.end(); ++it)
    newMessage(*it);

  // move the cursor to the end of the document
  m_messageWindow->moveCursor(QTextEdit::MoveEnd, false);
}

