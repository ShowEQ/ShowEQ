/*
 * guildlist.h
 *
 *  ShowEQ Distributed under GPL
 *  http://www.sourceforge.net/projects/seq
 *
 *  Copyright 2004 Zaphod (dohpaz@users.sourceforge.net). 
 *
 */

#ifndef _GUILDLIST_H_
#define _GUILDLIST_H_

#include "seqwindow.h"
#include "seqlistview.h" 

#include <qlistview.h>
#include <qptrdict.h>
#include <qstring.h>

//----------------------------------------------------------------------
// forward declarations
class Player;
class ZoneMgr;
class GuildMember;
class GuildShell;

//--------------------------------------------------
// constants
const int tGuildListColName = 0;
const int tGuildListColLevel = 1;
const int tGuildListColClass = 2;
const int tGuildListColRank = 3;
const int tGuildListColLastOn = 4;
const int tGuildListColZone = 5;
const int tGuildListColPublicNote = 6;
const int tGuildListColMaxCols = 7;

//----------------------------------------------------------------------
// GuildListItem
class GuildListItem : public QListViewItem
{
 public:
  GuildListItem(QListView* parent,
		const GuildMember* member, 
		const GuildShell* guildShell);
  virtual ~GuildListItem();

   virtual void paintCell( QPainter *p, const QColorGroup &cg,
                           int column, int width, int alignment );

   virtual int compare(QListViewItem *i, int col, bool ascending) const;

   void update(const GuildShell* guildShell);

   const GuildMember* guildMember() { return m_member; }
   void setGuildMember(const GuildMember* member);

   virtual int rtti() const;

 protected:
   const GuildMember* m_member;
};

//----------------------------------------------------------------------
// GuildListWindow
class GuildListWindow : public SEQWindow
{
  Q_OBJECT
    
 public:
  GuildListWindow(Player* player, GuildShell* guildShell,
		  QWidget* parent = 0, const char* name = 0);
  ~GuildListWindow();

  virtual QPopupMenu* menu();

 public slots: 
  void cleared();
  void loaded();
  void updated(const GuildMember* gm);

 protected slots:
  void init_Menu(void);
  void toggle_showOffline(int id);
  void toggle_keepSorted(int id);
  void toggle_guildListCol( int id );
  void set_font(int id);
  void set_caption(int id);

 protected:
  void clear(void);
  void populate(void);

  Player* m_player;
  GuildShell* m_guildShell;
  
  SEQListView* m_guildList;
  QPtrDict<GuildListItem> m_guildListItemDict;
  QPopupMenu* m_menu;
  int m_id_guildList_Cols[tGuildListColMaxCols];

  bool m_showOffline;
  bool m_keepSorted;
};

#endif // _GUILDLIST_H_

