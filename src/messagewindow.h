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

#include <qtextedit.h>
#include <qregexp.h>
#include <qdialog.h>

//----------------------------------------------------------------------
// forward declarations
class MessageEntry;
class Messages;

class QPopupMenu;
class QLineEdit;
class QCheckBox;

//----------------------------------------------------------------------
// MessageBrowser
class MessageBrowser : public QTextEdit
{
  Q_OBJECT
 public:
  MessageBrowser(QWidget* parent = 0, const char* name = 0);

 signals:
  void rightClickedMouse(QMouseEvent* e);

 protected:
  bool eventFilter(QObject *o, QEvent *e);
};

//----------------------------------------------------------------------
// MessageFindDialog
class MessageFindDialog : public QDialog
{
  Q_OBJECT
 public:
  MessageFindDialog(MessageBrowser*, const QString& caption,
		    QWidget* parent=0, const char* name = 0);

 public slots:
  void find();
  void close();

 protected slots:
  void textChanged(const QString& newText);

 protected:
  MessageBrowser* m_messageWindow;
  QLineEdit* m_findText;
  QCheckBox* m_matchCase;
  QCheckBox* m_wholeWords;
  QCheckBox* m_findBackwards;
  QPushButton* m_find;
  int m_lastParagraph;
  int m_lastIndex;
};

//----------------------------------------------------------------------
// MessageWindow
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
  void refreshMessages();
  void findDialog();

 protected slots:
  void toggleTypeFilter(int);
  void toggleLockedText(int);
  void toggleDisplayType(int);
  void toggleDisplayTime(int);
  void toggleEQDisplayTime(int);
  void toggleUseColor(int);
  void toggleWrapText(int);
  void setTypeColor(int);
  void setTypeBGColor(int);
  void setColor();
  void setBGColor();
  void setFont();
  void setCaption();
  virtual void restoreFont();

  void mousePressEvent(QMouseEvent* e);
  void clicked(int para, int pos);

 protected:
  Messages* m_messages;
  MessageBrowser* m_messageWindow;
  QPopupMenu* m_menu;
  QPopupMenu* m_typeFilterMenu;
  MessageFindDialog* m_findDialog;
  uint32_t m_enabledTypes;
  QColor m_defaultColor;
  QColor m_defaultBGColor;
  QString m_dateTimeFormat;
  QString m_eqDateTimeFormat;
  QColor* m_typeColors;
  QColor* m_typeBGColors;
  QRegExp m_itemPattern;
  bool m_lockedText;
  bool m_displayType;
  bool m_displayDateTime;
  bool m_displayEQDateTime;
  bool m_useColor;
  bool m_wrapText;
};

#endif // _MESSAGEWINDOW_H_

