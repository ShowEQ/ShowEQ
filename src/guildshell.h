/*
 * guildshell.h
 *
 *  ShowEQ Distributed under GPL
 *  http://www.sourceforge.net/projects/seq
 *
 *  Copyright 2004 Zaphod (dohpaz@users.sourceforge.net). 
 *
 */

#include <stdint.h>
#include <qstring.h>
#include <qobject.h>
#include <time.h>
#include <qdict.h>

class NetStream;
class ZoneMgr;

struct GuildMemberUpdate;

class GuildMember
{
 public:
  GuildMember(NetStream& netStream);
  ~GuildMember();

  void update(const GuildMemberUpdate* gmu);

  const QString& name() const { return m_name; }
  uint8_t level() const { return m_level; }
  uint8_t classVal() const { return m_class; }
  uint32_t guildRank() const { return m_guildRank; }
  time_t lastOn() const { return m_lastOn; }
  const QString& publicNote() const { return m_publicNote; }
  uint16_t zoneId() const { return m_zoneId; }
  uint16_t zoneInstance() const { return m_zoneInstance; }

 protected:
  QString m_name;
  uint8_t m_level;
  uint8_t m_class;
  uint32_t m_guildRank;
  time_t m_lastOn;
  QString m_publicNote;
  uint16_t m_zoneId;
  uint16_t m_zoneInstance;
};

class GuildShell : public QObject
{
  Q_OBJECT
 public:
  GuildShell(ZoneMgr* zoneMgr, QObject* parent = 0, const char* name = 0);
  ~GuildShell();

  public slots:
    void guildMemberList(const uint8_t* data, size_t len);
    void guildMemberUpdate(const uint8_t* data, size_t len);

 signals:
    void cleared();
    void added(const GuildMember* gm);
    void removed(const GuildMember* gm);
    void updated(const GuildMember* gm);

 protected:
    QDict<GuildMember> m_members;
    ZoneMgr* m_zoneMgr;
};

