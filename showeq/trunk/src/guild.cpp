/*
 * group.cpp
 *
 * ShowEQ Distributed under GPL
 * http://seq.sourceforge.net/
 */

#include <qfile.h>
#include <qdatastream.h>
#include <qtextstream.h>

#include "guild.h"
#include "packet.h"

GuildMgr::GuildMgr(QString fn, EQPacket *packet, QObject* parent, const char* name)
                   : QObject(parent, name)
{
  guildsFileName = fn;

   connect(packet, SIGNAL(worldGuildList(const char*, uint32_t)),
           this, SLOT(worldGuildList(const char*, uint32_t)));
   connect(parent, SIGNAL(guildList2text(QString)),
           this, SLOT(guildList2text(QString)));

   readGuildList();
}

GuildMgr::~GuildMgr()
{
}

QString GuildMgr::guildIdToName(uint16_t guildID)
{
  return m_guildMap[guildID];
}

void GuildMgr::worldGuildList(const char* guildData, uint32_t len)
{
  writeGuildList(guildData, len);
  readGuildList();
}

void GuildMgr::writeGuildList(const char* data, uint32_t len)
{
  QFile guildsfile(guildsFileName);

  if (guildsfile.exists()) {
     if (!guildsfile.remove()) {
        fprintf(stderr, "WARNING: could not remove old %s, unable to replace with server data!\n"
,
                guildsFileName.latin1());
        return;
     }
  }

  if(!guildsfile.open(IO_WriteOnly))
     fprintf(stderr, "WARNING: could not open %s for writing, unable to replace with server data!\n",
             guildsFileName.latin1());

  QDataStream guildDataStream(&guildsfile);

  guildDataStream.writeRawBytes(data, len);

  guildsfile.close();
  printf("GuildMgr: new guildsfile written\n");
}

void GuildMgr::readGuildList()
{
  QFile guildsfile(guildsFileName);

  if (guildsfile.open(IO_ReadOnly))
  {
     if (guildsfile.size() != sizeof(worldGuildListStruct))
     {
	fprintf(stderr, "WARNING: guildsfile not loaded, expected size %d got %d\n",
                sizeof(worldGuildListStruct), guildsfile.size()); 
	return;
     }

     struct guildListStruct gl;
     uint32_t offset;
     
     guildsfile.at(sizeof(offset));
     while (!guildsfile.atEnd())
     {
         guildsfile.readBlock(reinterpret_cast<char*>(&gl), sizeof(gl));
         if (gl.valid)
            m_guildMap.insert(gl.guildID, gl.guildName);
     }
     
    guildsfile.close();
    printf("GuildMgr: guildsfile loaded\n");
  }
  else
    printf("GuildMgr: WARNING - could not load guildsfile, %s\n", (const char*)guildsFileName);
}

void GuildMgr::guildList2text(QString fn)
{
  QFile guildsfile(fn);
  QTextStream guildtext(&guildsfile);

    if (guildsfile.exists()) {
         if (!guildsfile.remove()) {
             fprintf(stderr, "WARNING: could not remove old %s, unable to process request!\n",
                   fn.latin1());
           return;
        }
   }

   if (!guildsfile.open(IO_WriteOnly)) {
      fprintf(stderr, "WARNING: could not open %s for writing, unable to process request!\n",
              fn.latin1());
      return;
   }


   for (int i =0 ; i < MAXGUILDS; i++)
   {
       if (m_guildMap[i])
          guildtext << i << "\t" << m_guildMap[i] << "\n";
   }

   guildsfile.close();

   return;
}


void GuildMgr::listGuildInfo()
{
   for (int i = 0; i < MAXGUILDS; i++)
   {
       if (m_guildMap[i])
           printf("%d	%s\n", i, (const char*)m_guildMap[i]);
   }
}
