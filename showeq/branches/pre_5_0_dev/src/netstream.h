/*
 * netstream.h
 *
 *  ShowEQ Distributed under GPL
 *  http://www.sourceforge.net/projects/seq
 *
 *  Copyright 2004 Zaphod (dohpaz@users.sourceforge.net). 
 *
 */

#ifndef _NETSTREAM_H_
#define _NETSTREAM_H_

#include <stdint.h>
#include <qstring.h>

class NetStream
{
 public:
  NetStream(const uint8_t* data, size_t length);
  ~NetStream();

  const uint8_t* data() { return m_data; }
  size_t length() { return m_length; }
  void reset();
  bool endOfStream() { return (m_pos == m_lastPos); }

  uint8_t uint8();
  int8_t int8();
  uint16_t uint16();
  int16_t int16();
  uint32_t uint32();
  int32_t int32();
  QString text();

 protected:
  const uint8_t* m_data;
  size_t m_length;
  const uint8_t* m_lastPos;
  const uint8_t* m_pos;
};

#endif // _NETSTREAM_H_


