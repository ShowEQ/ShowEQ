/*
 * netdiag.h
 *
 *  ShowEQ Distributed under GPL
 *  http://seq.sourceforge.net/
 *
 */

#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qaccel.h>

#include "main.h"
#include "netdiag.h"
#include "packet.h"
#include "util.h"

NetDiag::NetDiag(EQPacket* packet, QWidget* parent, const char* name = NULL)
  : SEQWindow("NetDiag", "ShowEQ - Network Diagnostics", parent, name),
    m_packet(packet),
    m_playbackSpeed(NULL)
{
  // get preferences
  QGridLayout* tmpGrid = new QGridLayout(boxLayout(), 26, 9);
  tmpGrid->addColSpacing(3, 5);
  tmpGrid->addColSpacing(6, 5);
  tmpGrid->addRowSpacing(1, 5);
  tmpGrid->addRowSpacing(3, 5);
  tmpGrid->addRowSpacing(6, 5);
  tmpGrid->addRowSpacing(8, 5);

  int row = 0;
  int col = 0;

  // create labels to display client & server info
  tmpGrid->addWidget(new QLabel("Network ", this), row, col++);
  tmpGrid->addWidget(new QLabel("Client: ", this), row, col++);
  m_clientLabel = new QLabel(this);
  tmpGrid->addWidget(m_clientLabel, row, col++);
  col++;
  tmpGrid->addWidget(new QLabel("ServerPort: ", this), row, col++);
  m_serverPortLabel = new QLabel(this);
  tmpGrid->addWidget(m_serverPortLabel, row, col++);
  col++;
  tmpGrid->addWidget(new QLabel("ClientPort: ", this), row, col++);
  m_clientPortLabel = new QLabel(this);
  tmpGrid->addWidget(m_clientPortLabel, row, col++);

  // second row of network info
  row++; col = 1;
  tmpGrid->addWidget(new QLabel("Device: ", this), row, col++);
  QLabel* tmpLabel = new QLabel(this);
  tmpLabel->setText(m_packet->device());
  tmpGrid->addWidget(tmpLabel, row, col++);
  col++;
  tmpGrid->addWidget(new QLabel("MAC: ", this), row, col++);
  tmpLabel = new QLabel(this);
  tmpLabel->setText(m_packet->mac());
  tmpGrid->addWidget(tmpLabel, row, col++);
  col++;
  tmpGrid->addWidget(new QLabel("Track: ", this), row, col++);
  m_sessionLabel = new QLabel(this);
  tmpGrid->addWidget(m_sessionLabel, row, col++);
  m_sessionLabel->setNum(m_packet->session_tracking_enabled());

  clientChanged(m_packet->clientAddr());
  serverPortLatched(m_packet->serverPort());
  clientPortLatched(m_packet->clientPort());

  // 3rd row of network info
  row++; col = 0;
  tmpGrid->addWidget(new QLabel("Pcap Thread: ", this), row, col++);
  tmpGrid->addWidget(new QLabel("Realtime: ", this), row, col++);
  tmpLabel = new QLabel(this);
  tmpLabel->setText(QString::number(m_packet->realtime()));
  tmpGrid->addWidget(tmpLabel, row, col++);
  row++; col = 1;
  tmpGrid->addWidget(new QLabel("Filter: ", this), row, col++);
  m_filterLabel = new QLabel(this);
  m_filterLabel->setText(m_packet->pcapFilter());
  tmpGrid->addWidget(m_filterLabel, row, col++);
  
  // stream specific statistics
  row++; row++; col = 0;

  QString eqStreams[] = {"client->world", "world->client", "client->zone", "zone->client"};

  for (int a = 0; a < MAXSTREAMS; a++)
  {
      tmpGrid->addWidget(new QLabel(eqStreams[a], this), row++, col);

     // packet throughput
     tmpGrid->addWidget(new QLabel("Packets ", this), row, col++);
     tmpGrid->addWidget(new QLabel("Total: ", this), row, col++);
     m_packetTotal[a] = new QLabel(this, "count");
     tmpGrid->addWidget(m_packetTotal[a], row, col++);
     col++;
     tmpGrid->addWidget(new QLabel("Recent: ", this), row, col++);
     m_packetRecent[a] = new QLabel(this, "recent");
     tmpGrid->addWidget(m_packetRecent[a], row, col++);
     col++;
     tmpGrid->addWidget(new QLabel("Rate: ", this), row, col++);
     m_packetAvg[a] = new QLabel(this, "avg");
     tmpGrid->addWidget(m_packetAvg[a], row, col++);
     resetPacket(m_packet->packetCount(a), a);

     row++; col = 0;

     // network status
     tmpGrid->addWidget(new QLabel("Status ", this), row, col++);
     tmpGrid->addWidget(new QLabel("Cached: ", this), row, col++);
     m_cache[a] = new QLabel(this, "cached");
     m_cache[a]->setNum((int)m_packet->currentCacheSize(a));
     tmpGrid->addWidget(m_cache[a], row, col++);
     col++;
     tmpGrid->addWidget(new QLabel("SeqExp: ", this), row, col++);
     m_seqExp[a] = new QLabel(this, "seqexp");
     tmpGrid->addWidget(m_seqExp[a], row, col++);
     col++;
     tmpGrid->addWidget(new QLabel("SeqCur: ", this), row, col++);
     m_seqCur[a] = new QLabel(this, "seqcur");
     tmpGrid->addWidget(m_seqCur[a], row, col++);
     row++; row++; col = 0;
     seqExpect(m_packet->serverSeqExp(a), a); 
     m_seqCur[a]->setText("????");
  }

  if (m_packet->playbackPackets())
  {
    tmpGrid->addWidget(new QLabel("Playback ", this), row, col++);
    tmpGrid->addWidget(new QLabel("Rate: ", this), row, col++);
    m_playbackSpeed = new QSpinBox(-1, 9, 1, this, "speed");
    m_playbackSpeed->setSuffix("x");
    m_playbackSpeed->setSpecialValueText("Puase");
    m_playbackSpeed->setWrapping(true);
    tmpGrid->addWidget(m_playbackSpeed, row, col++, Qt::AlignLeft);

    m_playbackSpeed->setValue(m_packet->playbackSpeed());

    QAccel* accel = new QAccel(this);
    int key;
    key = pSEQPrefs->getPrefKey("IncPlaybackSpeedKey", preferenceName(), "Ctrl+X");
    accel->connectItem(accel->insertItem(key), m_packet, SLOT(incPlayback()));
    key = pSEQPrefs->getPrefKey("IncPlaybackSpeedKey", preferenceName(), "Ctrl+Z");
    accel->connectItem(accel->insertItem(key), m_packet, SLOT(decPlayback()));
  }
  
  // supply the LCD's with signals
  connect (m_packet, SIGNAL(cacheSize(int, int)), 
	   this, SLOT(cacheSize(int, int)));
  connect (m_packet, SIGNAL(seqExpect (int, int)), 
	   this, SLOT(seqExpect(int, int)));
  connect (m_packet, SIGNAL(seqReceive (int, int)), 
	   this, SLOT(seqReceive(int, int)));
  connect (m_packet, SIGNAL(clientChanged(in_addr_t)),
	   this, SLOT(clientChanged(in_addr_t)));
  connect (m_packet, SIGNAL(clientPortLatched(in_port_t)),
	   this, SLOT(clientPortLatched(in_port_t)));
  connect (m_packet, SIGNAL(serverPortLatched(in_port_t)),
	   this, SLOT(serverPortLatched(in_port_t)));
  connect (m_packet, SIGNAL(sessionTrackingChanged(uint8_t)),
	   this, SLOT(sessionTrackingChanged(uint8_t)));
  connect (m_packet, SIGNAL(numPacket(int, int)),
	   this, SLOT(numPacket(int, int)));
  connect (m_packet, SIGNAL(resetPacket(int, int)),
	   this, SLOT(resetPacket(int, int)));
  connect (m_packet, SIGNAL(filterChanged()),
	   this, SLOT(filterChanged()));

  if (m_playbackSpeed)
  {
    connect (m_playbackSpeed, SIGNAL(valueChanged(int)),
	     m_packet, SLOT(setPlayback(int)));
    connect (m_packet, SIGNAL(playbackSpeedChanged(int)),
	     m_playbackSpeed, SLOT(setValue(int)));
  }
}

NetDiag::~NetDiag()
{
}

void NetDiag::seqReceive(int seq, int stream)
{
  QString disp;
  disp.sprintf("%4.4x", seq);
  m_seqCur[stream]->setText(disp);
}

void NetDiag::seqExpect(int seq, int stream)
{
  QString disp;
  disp.sprintf("%4.4x", seq);
  m_seqExp[stream]->setText(disp);
}

void NetDiag::clientChanged(uint32_t addr)
{
  QString disp, tmp;
  disp = print_addr(addr);

  uint8_t sessionState = m_packet->session_tracking_enabled();
  m_sessionLabel->setNum(sessionState);

  switch (sessionState)
  {
  case 2:
    tmp.sprintf(":%d", 
		m_packet->clientPort());
    disp += tmp;
    break;
  case 1:
    disp += QString(":?");
    break;
  default:
    break;
  }
  m_clientLabel->setText(disp);
}

void NetDiag::clientPortLatched(uint16_t clientPort)
{
  QString disp, tmp;
  uint32_t addr = m_packet->clientAddr();

  disp = print_addr(addr);
  tmp.sprintf(":%d", clientPort);

  disp += tmp;

  m_clientLabel->setText(disp);
}

void NetDiag::serverPortLatched(uint16_t port)
{
  m_serverPortLabel->setText(QString::number(port));
}

void NetDiag::sessionTrackingChanged(uint8_t sessionTrackState)
{
  QString disp, tmp;
  disp = print_addr(m_packet->clientAddr());

  m_sessionLabel->setNum(sessionTrackState);
  switch(sessionTrackState)
  {
  case 1:
    m_clientLabel->setText(disp + ":?");
    break;
  case 2:
    break;
  default:
    m_clientLabel->setText(disp);
    break;
  }
}

void NetDiag::filterChanged()
{
  m_filterLabel->setText(m_packet->pcapFilter());
}

void NetDiag::resetPacket(int num, int stream)
{
  // if passed 0 reset the average
  m_packetStartTime[stream] = mTime();
  m_initialcount[stream] = num;

  m_packetTotal[stream]->setText(QString::number(num));
  m_packetRecent[stream]->setText("0");
  m_packetAvg[stream]->setText("0.0");
}

void NetDiag::numPacket(int num, int stream)
{
  // start the timer of not started
  if (!m_packetStartTime)
    m_packetStartTime[stream] = mTime();

   QString tempStr;

   m_packetTotal[stream]->setText(QString::number(num));

  // update five times per sec
  static int lastupdate = 0;
  if ( (mTime() - lastupdate) < 1000)
    return;
  lastupdate = mTime();

   int numdelta = num - m_initialcount[stream];
   m_packetRecent[stream]->setText(QString::number(num));
   int delta = mTime() - m_packetStartTime[stream];
   if (numdelta && delta)
     tempStr.sprintf("%2.1f", 
		     (float) (numdelta<<10) / (float) delta);
   else   
     tempStr.sprintf("0.0");

   m_packetAvg[stream]->setText(tempStr);
}

void NetDiag::cacheSize(int size, int stream)
{
  m_cache[stream]->setNum(size);
}

QString NetDiag::print_addr(in_addr_t  addr)
{
#ifdef DEBUG_PACKET
   debug ("print_addr()");
#endif /* DEBUG_PACKET */
  QString paddr;

  paddr.sprintf ( "%d.%d.%d.%d",
		  addr & 0x000000ff,
		  (addr & 0x0000ff00) >> 8,
		  (addr & 0x00ff0000) >> 16,
		  (addr & 0xff000000) >> 24
                  );

   return paddr;
}
