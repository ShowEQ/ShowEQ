/*
 * messagewindow.cpp
 *
 * ShowEQ Distributed under GPL
 * http://seq.sf.net/
 */

#include "messagewindow.h"
#include "messages.h"
#include "main.h"

#include <qpopupmenu.h>
#include <qinputdialog.h>
#include <qfontdialog.h>
#include <qcolordialog.h>
#include <qregexp.h>

//---------------------------------------------------------------------- 
// MessageBrowser
MessageBrowser::MessageBrowser(QWidget* parent, const char* name)
  : QTextEdit(parent, name)
{
}

bool MessageBrowser::eventFilter(QObject *o, QEvent *e)
{
  if (e->type() != QEvent::MouseButtonPress)
    return QTextEdit::eventFilter(o, e);

  QMouseEvent* m = (QMouseEvent*)e;

  if (m->button() == RightButton)
  {
    emit rightClickedMouse(m);

    return true;
  }

  return QTextEdit::eventFilter(o, e);
}

//----------------------------------------------------------------------
// MessageWindow
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
    m_defaultBGColor(white),
    m_dateTimeFormat("hh:mm"),
    m_eqDateTimeFormat("ddd M/d/yyyy h:mm"),
    m_lockedText(false),
    m_displayType(true),
    m_displayDateTime(false),
    m_displayEQDateTime(false),
    m_useColor(false),
    m_wrapText(true),
    m_typeColors(0),
    m_typeBGColors(0),
    m_itemPattern("\022(\\d{5,7})\\w*-\\d+-\\d+-\\d+-\\d+-.{13}([^\022]+)\022")
{
  m_enabledTypes = pSEQPrefs->getPrefInt("EnabledTypes", preferenceName(), 
					 m_enabledTypes);
  m_defaultColor = pSEQPrefs->getPrefColor("DefaultColor", preferenceName(),
					   m_defaultColor);
  m_defaultBGColor = pSEQPrefs->getPrefColor("DefaultBGColor", 
					     preferenceName(), 
					     m_defaultBGColor);
  m_dateTimeFormat = pSEQPrefs->getPrefString("DateTimeFormat",
					      preferenceName(), 
					      m_dateTimeFormat);
  m_eqDateTimeFormat = pSEQPrefs->getPrefString("EQDateTimeForamt",
						preferenceName(),
						m_eqDateTimeFormat);
  m_displayType = pSEQPrefs->getPrefBool("DisplayType", preferenceName(),
					 m_displayType);
  m_displayDateTime = pSEQPrefs->getPrefBool("DisplayDateTime",
					     preferenceName(), 
					     m_displayDateTime);
  m_displayEQDateTime = pSEQPrefs->getPrefBool("DisplayEQDateTime",
					       preferenceName(),
					       m_displayEQDateTime);
  m_useColor = pSEQPrefs->getPrefBool("UseColor", preferenceName(),
				      m_useColor);
  m_wrapText = pSEQPrefs->getPrefBool("WrapText", preferenceName(),
				      m_wrapText);

  m_typeColors = new QColor[MT_Max+1];
  m_typeBGColors = new QColor[MT_Max+1];

  // create the window for text display
  m_messageWindow = new MessageBrowser(this, "messageText");

  // make the message window the main widget of the SEQWindow
  setWidget(m_messageWindow);
  
  // set the message window frame style
  m_messageWindow->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  // set the current font
  m_messageWindow->setCurrentFont(font());

  // set the colors
  m_messageWindow->setColor(m_defaultColor);
  m_messageWindow->setPaper(m_defaultBGColor);

  // make sure history isn't kept
  m_messageWindow->setUndoDepth(0);
  m_messageWindow->setUndoRedoEnabled(false);

  m_messageWindow->setTextFormat(PlainText);

  // set it to read only
  m_messageWindow->setReadOnly(true);

  // set the word wrap 
  m_messageWindow->setWordWrap(m_wrapText ? 
			       QTextEdit::WidgetWidth : QTextEdit::NoWrap);

  // set the wrap policy to break at space
  m_messageWindow->setWrapPolicy(QTextEdit::AtWhiteSpace);

  // connect to the Messages signal(s)
  connect(m_messages, SIGNAL(newMessage(const MessageEntry&)),
	  this, SLOT(newMessage(const MessageEntry&)));

  connect(m_messageWindow, SIGNAL(rightClickedMouse(QMouseEvent*)),
	  this, SLOT(mousePressEvent(QMouseEvent*)));
  connect(m_messageWindow, SIGNAL(clicked(int, int)),
	  this, SLOT(clicked(int, int)));

  m_menu = new QPopupMenu;
  QPopupMenu* typeColorMenu = new QPopupMenu;
  QPopupMenu* typeBGColorMenu = new QPopupMenu;

  m_typeFilterMenu = new QPopupMenu;
  m_menu->insertItem("Show Message Type", m_typeFilterMenu);

  QColor typeColor;
  QString typeName;
  // iterate over the message types, filling in various menus and getting 
  // font color preferences
  for (int i = MT_Guild; i <= MT_Max; i++)
  {
    typeName = messages->messageTypeString((MessageType)i);
    if (!typeName.isEmpty())
    {
      m_typeFilterMenu->insertItem(typeName, i);
      m_typeFilterMenu->setItemChecked(i, (((1 << i) & m_enabledTypes) != 0));
      typeColorMenu->insertItem(typeName + "...", i);
      typeBGColorMenu->insertItem(typeName + "...", i);

      typeColor = pSEQPrefs->getPrefColor(typeName + "Color", preferenceName(),
					  m_typeColors[i]);
      if (typeColor.isValid())
	m_typeColors[i] = typeColor;

      typeColor = pSEQPrefs->getPrefColor(typeName + "BGColor", 
					  preferenceName(),
					  m_typeBGColors[i]);
      if (typeColor.isValid())
	m_typeBGColors[i] = typeColor;
    }
  }
  
  connect(m_typeFilterMenu, SIGNAL(activated(int)),
	  this, SLOT(toggleTypeFilter(int)));
  connect(typeColorMenu, SIGNAL(activated(int)),
	  this, SLOT(setTypeColor(int)));
  connect(typeBGColorMenu, SIGNAL(activated(int)),
	  this, SLOT(setTypeBGColor(int)));

  QPopupMenu* userFilterMenu = new QPopupMenu;
  m_menu->insertItem("User Message Filters", userFilterMenu);

  m_menu->insertSeparator(-1);
  int x;
  x = m_menu->insertItem("&Lock Text", this, SLOT(toggleLockedText(int)));
  m_menu->setItemChecked(x, m_lockedText);
  m_menu->insertSeparator(-1);
  x = m_menu->insertItem("Display &Type", this, SLOT(toggleDisplayType(int)));
  m_menu->setItemChecked(x, m_displayType);
  x = m_menu->insertItem("Display Time/&Date", this, SLOT(toggleDisplayTime(int)));
  m_menu->setItemChecked(x, m_displayDateTime);
  x = m_menu->insertItem("Display &EQ Date/Time", this, SLOT(toggleEQDisplayTime(int)));
  m_menu->setItemChecked(x, m_displayEQDateTime);
  x = m_menu->insertItem("&Use Color", this, SLOT(toggleUseColor(int)));
  m_menu->setItemChecked(x, m_useColor);
  x = m_menu->insertItem("&Wrap Text", this, SLOT(toggleWrapText(int)));
  m_menu->setItemChecked(x, m_wrapText);
  m_menu->insertItem("&Font...", this, SLOT(setFont()));
  m_menu->insertItem("&Caption...", this, SLOT(setCaption()));
  m_menu->insertItem("Text Colo&r...", this, SLOT(setColor()));
  m_menu->insertItem("Text Back&ground Color...", this, SLOT(setBGColor()));
  m_menu->insertItem("Type C&olor", typeColorMenu);
  m_menu->insertItem("Type &Background Color", typeBGColorMenu);
  m_menu->insertSeparator(-1);
  x = m_menu->insertItem("Refresh Messages...", this, SLOT(refreshMessages()));


  refreshMessages();
}

MessageWindow::~MessageWindow()
{
  delete m_typeColors;
  delete m_typeBGColors;
}

void MessageWindow::mousePressEvent(QMouseEvent* e)
{
  if (e->button() == RightButton)
    m_menu->popup(mapToGlobal(e->pos()));
  if (e->button() == LeftButton)
  {
    QString anchor = m_messageWindow->anchorAt(e->pos(), Qt::AnchorHref);
    if (!anchor.isNull())
      printf("Clicked Anchor '%s'!", (const char*)anchor);
  }
}

void MessageWindow::clicked(int para, int pos)
{
  //printf("MessageWindow::clicked(%d, %d)\n", para, pos);
}

void MessageWindow::newMessage(const MessageEntry& message)
{
  MessageType type = message.type();

  // if the message type isn't enabled, nothing to do
  if (((m_enabledTypes & ( 1 << type)) == 0) || m_lockedText)
    return;
  
  // if using color, then use it... ;-)
  if (m_useColor)
  {
    // if the message has a specific color, then use it
    if (message.color() != ME_InvalidColor)
      m_messageWindow->setColor(QColor(message.color()));
    else if (m_typeColors[type].isValid()) // or use the types color
      m_messageWindow->setColor(m_typeColors[type]);
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

  text.replace(m_itemPattern, "\\2 (#\\1)");

  // now append the message text to the buffer
  m_messageWindow->append(text);

  if (m_useColor)
  {
    int para = m_messageWindow->paragraphs() - 1;
    if (m_typeBGColors[type].isValid())
      m_messageWindow->setParagraphBackgroundColor(para, m_typeBGColors[type]);
    else
      m_messageWindow->setParagraphBackgroundColor(para, m_defaultBGColor);
  }
}

void MessageWindow::refreshMessages()
{
  // set the IBeam Cursor for easier text selection
  setCursor(Qt::WaitCursor);
  m_messageWindow->setCursor(Qt::WaitCursor);

  // clear the document
  m_messageWindow->clear();

  // turn off updates while doing this mass update
  m_messageWindow->setUpdatesEnabled(false);
  setUpdatesEnabled(false);

  m_messageWindow->append(" ");

  // set the cursor to the beginning of the document
  m_messageWindow->setCursorPosition(0, 0);

  // move the cursor to the end of the document
  m_messageWindow->moveCursor(QTextEdit::MoveEnd, false);

  // get the list of messages
  const MessageList& messages = m_messages->messageList();
 
  // iterate over the message list and add the messages
  MessageList::const_iterator it;
  for (it = messages.begin(); it != messages.end(); ++it)
    newMessage(*it);

  // move the cursor to the end of the document
  m_messageWindow->scrollToBottom();

  // turn updates back on 
  m_messageWindow->setUpdatesEnabled(true);
  setUpdatesEnabled(true);

  // repain window now that updates have been re-enabled
  repaint();

  // set the IBeam Cursor for easier text selection
  unsetCursor();
  m_messageWindow->unsetCursor();
}

void MessageWindow::toggleTypeFilter(int id)
{
  printf("toggleTypeFilter(%d)\n", id);
  if (((1 << id) & m_enabledTypes) != 0)
    m_enabledTypes &= ~(1 << id);
  else
    m_enabledTypes |= (1 << id);
 
  m_typeFilterMenu->setItemChecked(id, ((m_enabledTypes & (1 << id)) != 0));

  pSEQPrefs->setPrefInt("EnabledTypes", preferenceName(), m_enabledTypes);
}

void MessageWindow::toggleLockedText(int id)
{
  m_lockedText = !m_lockedText;
  m_menu->setItemChecked(id, m_lockedText);

  // if the text had been locked, refresh the messages
  if (!m_lockedText)
    refreshMessages();
}

void MessageWindow::toggleDisplayType(int id)
{
  m_displayType = !m_displayType;
  m_menu->setItemChecked(id, m_displayType);

  pSEQPrefs->setPrefBool("DisplayType", preferenceName(), m_displayType);
}

void MessageWindow::toggleDisplayTime(int id)
{
  m_displayDateTime = !m_displayDateTime;
  m_menu->setItemChecked(id, m_displayDateTime);
  pSEQPrefs->setPrefBool("DisplayDateTime", preferenceName(),
			 m_displayDateTime);
}

void MessageWindow::toggleEQDisplayTime(int id)
{
  m_displayEQDateTime = ! m_displayEQDateTime;
  m_menu->setItemChecked(id, m_displayEQDateTime);
  pSEQPrefs->setPrefBool("DisplayEQDateTime", preferenceName(),
			 m_displayEQDateTime);
}

void MessageWindow::toggleUseColor(int id)
{
  m_useColor = !m_useColor;
  m_menu->setItemChecked(id, m_useColor);
  pSEQPrefs->setPrefBool("UseColor", preferenceName(), m_useColor);
}

void MessageWindow::toggleWrapText(int id)
{
  m_wrapText = !m_wrapText;
  m_menu->setItemChecked(id, m_wrapText);

  pSEQPrefs->setPrefBool("WrapText", preferenceName(), m_wrapText);

  // set the wrap policy according to the setting
  m_messageWindow->setWordWrap(m_wrapText ? 
			       QTextEdit::WidgetWidth : QTextEdit::NoWrap);
}

void MessageWindow::setTypeColor(int id)
{
  QString typeName = m_messages->messageTypeString((MessageType)id);
  QString clrCaption = caption() + " " + typeName + " Color";
  QColor color = QColorDialog::getColor(m_typeColors[id], this, clrCaption);

  if (color.isValid())
  {
    m_typeColors[id] = color;

    pSEQPrefs->setPrefColor(typeName + "Color", preferenceName(), 
			    m_typeColors[id]);
  }
}

void MessageWindow::setTypeBGColor(int id)
{
  QString typeName = m_messages->messageTypeString((MessageType)id);
  QString clrCaption = caption() + " " + typeName + " Background Color";
  QColor color = QColorDialog::getColor(m_typeColors[id], this, clrCaption);

  if (color.isValid())
  {
    m_typeColors[id] = color;

    pSEQPrefs->setPrefColor(typeName + "BGColor", preferenceName(), 
			    m_typeColors[id]);
  }
}

void MessageWindow::setColor()
{
  QString clrCaption = caption() + " Default Text Color";
  QColor color = QColorDialog::getColor(m_defaultColor, this, clrCaption);

  if (color.isValid())
  {
    m_defaultColor = color;

    pSEQPrefs->setPrefColor("DefaultColor", preferenceName(), 
			    m_defaultColor);
  }
}

void MessageWindow::setBGColor()
{
  QString clrCaption = caption() + " Default Text Background Color";
  QColor color = QColorDialog::getColor(m_defaultBGColor, this, clrCaption);

  if (color.isValid())
  {
    m_defaultBGColor = color;
    m_messageWindow->setPaper(m_defaultBGColor);

    pSEQPrefs->setPrefColor("DefaultBGColor", preferenceName(), 
			    m_defaultBGColor);
  }
}

void MessageWindow::setFont()
{
  QFont newFont;
  bool ok = false;
  // get a new font
  newFont = QFontDialog::getFont(&ok, m_messageWindow->currentFont(),
				 this, caption() + " Font");
  
  
  // if the user entered a font and clicked ok, set the windows font
  if (ok)
    setWindowFont(newFont);
}

void MessageWindow::setCaption()
{
  bool ok = false;

  QString captionText = 
    QInputDialog::getText("ShowEQ Message Window Caption",
			  "Enter caption for this Message Window:",
			  QLineEdit::Normal, caption(),
			  &ok, this);
  
  // if the user entered a caption and clicked ok, set the windows caption
  if (ok)
    SEQWindow::setCaption(captionText);
}

void MessageWindow::restoreFont()
{
  SEQWindow::restoreFont();

  if (m_messageWindow)
    m_messageWindow->setCurrentFont(font());
}

