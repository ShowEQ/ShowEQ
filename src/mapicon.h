/*
 * mapicon.h
 *
 *  ShowEQ Distributed under GPL
 *  http://seq.sourceforge.net/
 */

#ifndef _EQMAPICON_H_
#define _EQMAPICON_H_

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif 

#ifdef __FreeBSD__
#include <sys/types.h>
#else
#include <stdint.h>
#endif

#include "spawn.h"

#include <qobject.h>
#include <qcolor.h>
#include <qpen.h>
#include <qbrush.h>
#include <qstring.h>

//----------------------------------------------------------------------
// forward declarations
class QPainter;
class QPoint;
class QTimer;

class MapParameters;
class SpawnPoint;
class Player;

//----------------------------------------------------------------------
// enumerated types
enum MapIconSize
{
  tIconSizeNone = 0,
  tIconSizeTiny = 1,
  tIconSizeSmall = 2,
  tIconSizeRegular = 3,
  tIconSizeLarge = 4,
  tIconSizeXLarge = 5,
  tIconSizeXXLarge = 6,
  tIconSizeMax = tIconSizeXXLarge,
  tIconSizeNumSizes = 7,
};

enum MapIconStyle
{
  tIconStyleNone = 0,
  tIconStyleCircle = 1,
  tIconStyleSquare = 2, 
  tIconStylePlus = 3,
  tIconStyleX = 4,
  tIconStyleUpTriangle = 5,
  tIconStyleRightTriangle = 6,
  tIconStyleDownTriangle = 7,
  tIconStyleLeftTriangle = 8,
  tIconStyleStar = 9,
  tIconStyleDiamond = 10,
  tIconStyleMax = tIconStyleDiamond,
};

enum MapIconType
{
  tIconTypeUnknown,
  tIconTypeDrop,
  tIconTypeDoor, 
  tIconTypeSpawnNPC,
  tIconTypeSpawnNPCCorpse,
  tIconTypeSpawnPlayer, 
  tIconTypeSpawnPlayerCorpse, 
  tIconTypeSpawnUnknown,
  tIconTypeSpawnConsidered,
  tIconTypeSpawnPlayerTeamBase,
  tIconTypeSpawnPlayerTeam1 = tIconTypeSpawnPlayerTeamBase,
  tIconTypeSpawnPlayerTeam2,
  tIconTypeSpawnPlayerTeam3,
  tIconTypeSpawnPlayerTeam4,
  tIconTypeSpawnPlayerTeam5,
  tIconTypeSpawnPlayerTeamOtherDeity,
  tIconTypeSpawnPlayerTeamOtherRace,
  tIconTypeSpawnPlayerTeamOtherRacePet,
  tIconTypeSpawnPlayerTeamOtherDeityPet,
  tIconTypeSpawnPlayerOld,
  tIconTypeItemSelected,
  tIconTypeFilterFlagBase,
  tIconTypeFilterFlagHunt = tIconTypeFilterFlagBase,
  tIconTypeFilterFlagCaution,
  tIconTypeFilterFlagDanger,
  tIconTypeFilterFlagLocate,
  tIconTypeFilterFlagAlert,
  tIconTypeFilterFlagFiltered,
  tIconTypeFilterFlagTracer,
  tIconTypeRuntimeFiltered,
  tIconTypeSpawnPoint,
  tIconTypeSpawnPointSelected,
  tIconTypeMax = tIconTypeSpawnPointSelected,
  tIconTypeNumTypes,
};

//----------------------------------------------------------------------
// MapIcon
struct MapIcon 
{
 public:
  MapIcon& operator=(const MapIcon& mapIcon);
  void combine(const MapIcon& mapIcon);
  void load(const QString& prefBase, const QString& section);
  void save(const QString& prefBase, const QString& section);
  
  const QBrush& imageBrush() const { return m_imageBrush; }
  const QBrush& highlightBrush() const { return m_highlightBrush; }
  const QPen& imagePen() const { return m_imagePen; }
  const QPen& highlightPen() const { return m_highlightPen; }
  const QPen& line0Pen() const { return m_line0Pen; }
  const QPen& line1Pen() const { return m_line1Pen; }
  const QPen& line2Pen() const { return m_line2Pen; }
  const QPen& walkPathPen() const { return m_walkPathPen; }
  uint32_t line1Distance() const { return m_line1Distance; }
  uint32_t line2Distance() const { return m_line2Distance; }
  MapIconStyle imageStyle() const { return m_imageStyle; }
  MapIconSize imageSize() const { return m_imageSize; }
  MapIconStyle highlightStyle() const { return m_highlightStyle; }
  MapIconSize highlightSize() const { return m_highlightSize; }
  bool image() const { return m_image; }
  bool imageUseSpawnColorPen() const { return m_imageUseSpawnColorPen; }
  bool imageUseSpawnColorBrush() const { return m_imageUseSpawnColorBrush; }
  bool imageFlash() const { return m_imageFlash; }
  bool highlight() const { return m_highlight; }
  bool highlightUseSpawnColorPen() const { return m_highlightUseSpawnColorPen; }
  bool highlightUseSpawnColorBrush() const { return m_highlightUseSpawnColorBrush; }
  bool highlightFlash() const { return m_highlightFlash; }
  bool showLine0() const { return m_showLine0; }
  bool useWalkPathPen() const { return m_useWalkPathPen; }
  bool showWalkPath() const { return m_showWalkPath; }
  bool showName() const { return m_showName; }

  void setImageBrush(const QBrush& val) { m_imageBrush = val; }
  void setHighlightBrush(const QBrush& val) { m_highlightBrush = val; }
  void setImagePen(const QPen& val) { m_imagePen = val; }
  void setHighlightPen(const QPen& val) { m_highlightPen = val; }
  void setLine0Pen(const QPen& val) { m_line0Pen = val; }
  void setLine1Pen(const QPen& val) { m_line1Pen = val; }
  void setLine2Pen(const QPen& val) { m_line2Pen = val; }
  void setWalkPathPen(const QPen& val) { m_walkPathPen = val; }
  void setLine1Distance(const uint32_t val) { m_line1Distance = val; }
  void setLine2Distance(const uint32_t val) { m_line2Distance = val; }
  void setImageStyle(const MapIconStyle val) { m_imageStyle = val; }
  void setImageSize(const MapIconSize val) { m_imageSize = val; }
  void setHighlightStyle(const MapIconStyle val) { m_highlightStyle = val; }
  void setHighlightSize(const MapIconSize val) { m_highlightSize = val; }
  void setImage(const bool val) { m_image = val; }
  void setImageUseSpawnColorPen(const bool val) { m_imageUseSpawnColorPen = val; }
  void setImageUseSpawnColorBrush(const bool val) { m_imageUseSpawnColorBrush = val; }
  void setImageFlash(const bool val) { m_imageFlash = val; }
  void setHighlight(const bool val) { m_highlight = val; }
  void setHighlightUseSpawnColorPen(const bool val) { m_highlightUseSpawnColorPen = val; }
  void setHighlightUseSpawnColorBrush(const bool val) { m_highlightUseSpawnColorBrush = val; }
  void setHighlightFlash(const bool val) { m_highlightFlash = val; }
  void setShowLine0(const bool val) { m_showLine0 = val; }
  void setUseWalkPathPen(const bool val) { m_useWalkPathPen = val; }
  void setShowWalkPath(const bool val) { m_showWalkPath = val; }
  void setShowName(const bool val) { m_showName = val; }


  void paintIconImage(MapIconStyle style, QPainter&p, const QPoint& point, 
		      int size, int sizeWH) const;

  typedef void (*IconImageFunction)(QPainter&p, const QPoint& point,
				    int size, int size);
  static void paintNone(QPainter&p, const QPoint& point, 
			int size, int sizeWH);
  static void paintCircle(QPainter&p, const QPoint& point, 
			  int size, int sizeWH);
  static void paintSquare(QPainter&p, const QPoint& point, 
			  int size, int sizeWH);
  static void paintPlus(QPainter&p, const QPoint& point, int size, int sizeWH);
  static void paintX(QPainter&p, const QPoint& point, int size, int sizeWH);
  static void paintUpTriangle(QPainter&p, const QPoint& point, 
			      int size, int sizeWH);
  static void paintRightTriangle(QPainter&p, const QPoint& point,
				 int size, int sizeWH);
  static void paintDownTriangle(QPainter&p, const QPoint& point, 
				int size, int sizeWH);
  static void paintLeftTriangle(QPainter&p, const QPoint& point, 
				int size, int sizeWH);
  static void paintStar(QPainter&p, const QPoint& point, int size, int sizeWH);
  static void paintDiamond(QPainter&p, const QPoint& point, 
			   int size, int sizeWH);

  public:
  QBrush m_imageBrush;
  QBrush m_highlightBrush;
  QPen m_imagePen;
  QPen m_highlightPen;
  QPen m_line0Pen;
  QPen m_line1Pen;
  QPen m_line2Pen;
  QPen m_walkPathPen;
  uint32_t m_line1Distance;
  uint32_t m_line2Distance;
  MapIconStyle m_imageStyle;
  MapIconSize m_imageSize;
  MapIconStyle m_highlightStyle;
  MapIconSize m_highlightSize;
  bool m_image;
  bool m_imageUseSpawnColorPen;
  bool m_imageUseSpawnColorBrush;
  bool m_imageFlash;
  bool m_highlight;
  bool m_highlightUseSpawnColorPen;
  bool m_highlightUseSpawnColorBrush;
  bool m_highlightFlash;
  bool m_showLine0;
  bool m_useWalkPathPen;
  bool m_showWalkPath;
  bool m_showName;
  private:
  static IconImageFunction s_iconImageFunctions[tIconStyleMax+1];
};

inline void MapIcon::paintIconImage(MapIconStyle style, QPainter& p, 
				    const QPoint& point, 
				    int size, int sizeWH) const
{
  (*s_iconImageFunctions[style])(p, point, size, sizeWH);
}

class MapIcons : public QObject
{
  Q_OBJECT
 public:
  MapIcons(Player* player, const QString& preferenceName,
	   QObject* parent = 0, const char* name = 0);
  ~MapIcons();

  void load(void);
  void save(void);

  const QString& preferenceName(void) const { return m_preferenceName; }
  int drawSize() const { return m_drawSize; }
  void setDrawSize(int val);
  bool showNPCWalkPaths() const { return m_showNPCWalkPaths; }
  void setShowNPCWalkPaths(bool val);
  bool showSpawnNames() const { return m_showSpawnNames; }
  void setShowSpawnNames(bool val);
  uint16_t fovDistance() const { return m_fovDistance; }
  void setFOVDistance(int val);

  // dump debug info
  void dumpInfo(QTextStream& out);

  const MapIcon& mapIcon(MapIconType iconType);
  const MapIcon& operator[](int iconType);

  void paintIcon(MapParameters& param, 
 		 QPainter& p, 
 		 const MapIcon& mapIcon,
 		 const Item* item, 
 		 const QPoint& point);
  void paintSpawnIcon(MapParameters& param, 
 		      QPainter& p, 
 		      const MapIcon& mapIcon,
 		      const Spawn* spawn, 
 		      const EQPoint& location,
 		      const QPoint& point);
  void paintSpawnPointIcon(MapParameters& param, 
			   QPainter& p, 
			   const MapIcon& mapIcon,
			   const SpawnPoint* spawnpoint, 
			   const QPoint& point);

 protected slots:
  void flashTick();

 protected:
  QColor pickSpawnPointColor(const SpawnPoint* sp, 
			     const QColor& defColor);
  Player* m_player; 
  QString m_preferenceName;
  MapIcon m_mapIcons[tIconTypeMax+1];
  int* m_mapIconSizes[tIconSizeMax+1];
  int* m_mapIconSizesWH[tIconSizeMax+1];
  int m_drawSize;
  int m_drawSizeWH; // 2 * m_drawSize
  int m_markerNSize; // m_drawSize - 2 (Tiny)
  int m_markerNSizeWH; // 2 * m_markerNSizeWH
  int m_marker0Size; // m_drawSize - 1
  int m_marker0SizeWH; // 2 * m_marker0SizeWH
  int m_marker1Size; // m_drawSize + 1
  int m_marker1SizeWH; // 2 * m_marker1SizeWH
  int m_marker2Size; // m_drawSize + 2
  int m_marker2SizeWH; // 2 * m_marker2SizeWH
  int m_marker3Size; // m_drawSize + 3
  int m_marker3SizeWH; // 2 * m_marker3SizeWH

  uint16_t m_fovDistance;

  QTimer* m_flashTimer;
  bool m_flash;

  bool m_showNPCWalkPaths;
  bool m_showSpawnNames;
};

inline const MapIcon& MapIcons::mapIcon(MapIconType iconType)
{
  // if a valid map icon was passed in, use it
  if ((iconType < tIconTypeMax) && (iconType > tIconTypeUnknown))
    return m_mapIcons[iconType];

  // otherwise return the unknown icon type
  return m_mapIcons[tIconTypeUnknown];
}

inline const MapIcon& MapIcons::operator[](int iconType) 
{ 
  return mapIcon((MapIconType)iconType); 
}

#endif // _EQMAPICON_H_
