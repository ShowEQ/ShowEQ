/*
 * mapicon.cpp
 *
 *  ShowEQ Distributed under GPL
 *  http://seq.sourceforge.net/
 */

#include "mapicon.h"
#include "mapcore.h"
#include "spawn.h"
#include "spawnmonitor.h"
#include "player.h"
#include "main.h"

#include <qpainter.h>
#include <qpoint.h>
#include <qtimer.h>

//----------------------------------------------------------------------
// constants
static const char* iconTypePrefBaseNames[] = 
{
  "Unknown",
  "Drop",
  "Doors",
  "SpawnNPC",
  "SpawnNPCCorpse",
  "SpawnPlayer",
  "SpawnPlayerCorpse",
  "SpawnUnknown",
  "SpawnConsidered",
  "SpawnPlayerTeam1",
  "SpawnPlayerTeam2",
  "SpawnPlayerTeam3",
  "SpawnPlayerTeam4",
  "SpawnPlayerTeam5",
  "SpawnPlayerTeamOtherDeity",
  "SpawnPlayerTeamOtherRace",
  "SpawnPlayerTeamOtherDeityPet",
  "SpawnPlayerTeamOtherRacePet",
  "SpawnPlayerOld",
  "SpawnItemSelected",
  "FilterFlagHunt",
  "FilterFlagCaution",
  "FilterFlagDanger",
  "FilterFlagLocate",
  "FilterFlagAlert",
  "FilterFlagFiltered",
  "FilterFlagTracer",
  "RuntimeFiltered",
  "SpawnPoint",
  "SpawnPointSelected",
};

//----------------------------------------------------------------------
// MapIcon
MapIcon::IconImageFunction MapIcon::s_iconImageFunctions[] = 
  {
    &MapIcon::paintNone,
    &MapIcon::paintCircle,
    &MapIcon::paintSquare,
    &MapIcon::paintPlus,
    &MapIcon::paintX,
    &MapIcon::paintUpTriangle,
    &MapIcon::paintRightTriangle,
    &MapIcon::paintDownTriangle,
    &MapIcon::paintLeftTriangle,
    &MapIcon::paintStar,
    &MapIcon::paintDiamond,
  };

MapIcon& MapIcon::operator=(const MapIcon& mapIcon)
{
  m_imageBrush = mapIcon.m_imageBrush;
  m_highlightBrush = mapIcon.m_highlightBrush;
  m_imagePen = mapIcon.m_imagePen;
  m_highlightPen = mapIcon.m_highlightPen;
  m_line0Pen = mapIcon.m_line0Pen;
  m_line1Pen = mapIcon.m_line1Pen;
  m_line2Pen = mapIcon.m_line2Pen;
  m_walkPathPen = mapIcon.m_walkPathPen;
  m_line1Distance = mapIcon.m_line1Distance;
  m_line2Distance = mapIcon.m_line2Distance;
  m_imageStyle = mapIcon.m_imageStyle;
  m_imageSize = mapIcon.m_imageSize;
  m_highlightStyle = mapIcon.m_highlightStyle;
  m_highlightSize = mapIcon.m_highlightSize;
  m_image = mapIcon.m_image;
  m_imageUseSpawnColorPen = mapIcon.m_imageUseSpawnColorPen;
  m_imageUseSpawnColorBrush = mapIcon.m_imageUseSpawnColorBrush;
  m_imageFlash = mapIcon.m_imageFlash;
  m_highlight = mapIcon.m_highlight;
  m_highlightUseSpawnColorPen = mapIcon.m_highlightUseSpawnColorPen;
  m_highlightUseSpawnColorBrush = mapIcon.m_highlightUseSpawnColorPen;
  m_highlightFlash = mapIcon.m_highlightFlash;
  m_showLine0 = mapIcon.m_showLine0;
  m_useWalkPathPen = mapIcon.m_useWalkPathPen;
  m_showWalkPath = mapIcon.m_showWalkPath;
  m_showName = mapIcon.m_showName;

  return *this;
}

void MapIcon::combine(const MapIcon& mapIcon)
{
  // try our best to generate the combined result of the two MapIcons

  // use image information
  if (mapIcon.m_image)
  {
    m_image = mapIcon.m_image;
    if (mapIcon.m_imageStyle != tIconStyleNone)
      m_imageStyle = mapIcon.m_imageStyle;
    if (mapIcon.m_imageSize != tIconSizeNone)
      m_imageSize = mapIcon.m_imageSize;
    m_imageBrush = mapIcon.m_imageBrush;
    m_imagePen = mapIcon.m_imagePen;
    m_imageUseSpawnColorPen = mapIcon.m_imageUseSpawnColorPen;
    m_imageUseSpawnColorBrush = mapIcon.m_imageUseSpawnColorBrush;
    if (mapIcon.m_imageFlash)
      m_imageFlash = mapIcon.m_imageFlash;
  }

  // use highlight information
  if (mapIcon.m_highlight)
  {
    m_highlight = mapIcon.m_highlight;
    if (mapIcon.m_highlightStyle != tIconStyleNone)
      m_highlightStyle = mapIcon.m_highlightStyle;
    if (mapIcon.m_highlightSize != tIconSizeNone)
      m_highlightSize = mapIcon.m_highlightSize;
    m_highlightBrush = mapIcon.m_highlightBrush;
    m_highlightPen = mapIcon.m_highlightPen;
    m_highlightUseSpawnColorPen = mapIcon.m_highlightUseSpawnColorPen;
    m_highlightUseSpawnColorBrush = mapIcon.m_highlightUseSpawnColorBrush;
    if (mapIcon.m_highlightFlash)
      m_highlightFlash = mapIcon.m_highlightFlash;
  }

  // use walk path pen info iff set
  if (mapIcon.m_useWalkPathPen)
  {
    m_useWalkPathPen = mapIcon.m_useWalkPathPen;
    m_walkPathPen = mapIcon.m_walkPathPen;
  }

  // use showWalkPath info iff set
  if (mapIcon.m_showWalkPath)
    m_showWalkPath = mapIcon.m_showWalkPath;

  // use showLine0 info iff set
  if (mapIcon.m_showLine0)
  {
    m_showLine0 = mapIcon.m_showLine0;
    m_line0Pen = mapIcon.m_line0Pen;
  }
  
  // use line1 info iff set and larger then current setting
  if ((mapIcon.m_line1Distance) && (m_line1Distance < mapIcon.m_line1Distance))
  {
    m_line1Distance = mapIcon.m_line1Distance;
    m_line1Pen = mapIcon.m_line1Pen;
  }

  // use line2 info iff set and larger then current setting
  if ((mapIcon.m_line2Distance) && (m_line2Distance < mapIcon.m_line2Distance))
  {
    m_line2Distance = mapIcon.m_line2Distance;
    m_line2Pen = mapIcon.m_line2Pen;
  }

  // use showName info iff set
  if (mapIcon.m_showName)
    m_showName = mapIcon.m_showName;
}

void MapIcon::load(const QString& prefBase, const QString& section)
{
  // Initialize the image related members
  m_imageBrush = pSEQPrefs->getPrefBrush(prefBase + "ImageBrush", section, m_imageBrush);
  m_imagePen = pSEQPrefs->getPrefPen(prefBase + "ImagePen", section, m_imagePen);
  m_imageStyle = (MapIconStyle)pSEQPrefs->getPrefInt(prefBase + "ImageStyle", 
						     section, m_imageStyle);
  m_imageSize = (MapIconSize)pSEQPrefs->getPrefInt(prefBase + "ImageSize", 
						   section, m_imageSize);
  m_image = pSEQPrefs->getPrefBool(prefBase + "UseImage", section, m_image);
  m_imageUseSpawnColorPen =
    pSEQPrefs->getPrefBool(prefBase + "ImageUseSpawnColorPen", section, 
			   m_imageUseSpawnColorPen);
  m_imageUseSpawnColorBrush =
    pSEQPrefs->getPrefBool(prefBase + "ImageUseSpawnColorBrush", section,
			   m_imageUseSpawnColorBrush);
  m_imageFlash =
    pSEQPrefs->getPrefBool(prefBase + "ImageFlash", section, m_imageFlash);

  // Initialize the Highlight related members
  m_highlightBrush = 
    pSEQPrefs->getPrefBrush(prefBase + "HighlightBrush", section, m_highlightBrush);
  m_highlightPen = pSEQPrefs->getPrefPen(prefBase + "HighlightPen", section, m_highlightPen);
  m_highlightStyle = 
    (MapIconStyle)pSEQPrefs->getPrefInt(prefBase + "HighlightStyle", section,
					m_highlightStyle);
  m_highlightSize =
    (MapIconSize)pSEQPrefs->getPrefInt(prefBase + "HighlightSize", section,
				       m_highlightSize);
  m_highlight = 
    pSEQPrefs->getPrefBool(prefBase + "UseHighlight", section, m_highlight);
  m_highlightUseSpawnColorPen =
    pSEQPrefs->getPrefBool(prefBase + "HighlightUseSpawnColorPen", section, 
			   m_highlightUseSpawnColorPen);
  m_highlightUseSpawnColorBrush =
    pSEQPrefs->getPrefBool(prefBase + "HighlightUseSpawnColorBrush", section,
			   m_highlightUseSpawnColorBrush);
  m_highlightFlash =
    pSEQPrefs->getPrefBool(prefBase + "HighlightFlash", section, 
			   m_highlightFlash);

  // Initialize the line stuff
  m_line0Pen = pSEQPrefs->getPrefPen(prefBase + "Line0Pen", section, m_line0Pen);
  m_showLine0 = pSEQPrefs->getPrefBool(prefBase + "ShowLine0", section,
				       m_showLine0);
  m_line1Pen = pSEQPrefs->getPrefPen(prefBase + "Line1Pen", section, m_line1Pen);
  m_line1Distance = pSEQPrefs->getPrefInt(prefBase + "Line1Distance", section, 
					  m_line1Distance);
  m_line2Pen = pSEQPrefs->getPrefPen(prefBase + "Line2Pen", section, m_line2Pen);
  m_line2Distance = pSEQPrefs->getPrefInt(prefBase + "Line2Distance", section,
					  m_line2Distance);

  // Initialize the Walk Path related member variables
  m_walkPathPen = pSEQPrefs->getPrefPen(prefBase + "WalkPathPen", section, m_walkPathPen);
  m_useWalkPathPen = pSEQPrefs->getPrefBool(prefBase + "UseWalkPathPen", 
					    section, m_useWalkPathPen);
  m_showWalkPath = pSEQPrefs->getPrefBool(prefBase + "ShowWalkPath", section,
					  m_showWalkPath);

  // Initialize whatever's left
  m_showName = pSEQPrefs->getPrefBool(prefBase + "ShowName", section,
				      m_showName);
}


void MapIcon::save(const QString& prefBase, const QString& section)
{
  // Save the image related members
  pSEQPrefs->setPrefBrush(prefBase + "ImageBrush", section, m_imageBrush);
  pSEQPrefs->setPrefPen(prefBase + "ImagePen", section, m_imagePen);
  pSEQPrefs->setPrefInt(prefBase + "ImageStyle", section, m_imageStyle);
  pSEQPrefs->setPrefInt(prefBase + "ImageSize", section, m_imageSize);
  pSEQPrefs->setPrefBool(prefBase + "UseImage", section, m_image);
  pSEQPrefs->setPrefBool(prefBase + "ImageUseSpawnColorPen", section, 
			 m_imageUseSpawnColorPen);
  pSEQPrefs->setPrefBool(prefBase + "ImageUseSpawnColorBrush", section,
			 m_imageUseSpawnColorBrush);
  pSEQPrefs->setPrefBool(prefBase + "ImageFlash", section, m_imageFlash);

  // Save the Highlight related members
  pSEQPrefs->setPrefBrush(prefBase + "HighlightBrush", section, 
			  m_highlightBrush);
  pSEQPrefs->setPrefPen(prefBase + "HighlightPen", section, m_highlightPen);
  pSEQPrefs->setPrefInt(prefBase + "HighlightStyle", section, 
			m_highlightStyle);
  pSEQPrefs->setPrefInt(prefBase + "HighlightSize", section, m_highlightSize);
  pSEQPrefs->setPrefBool(prefBase + "UseHighlight", section, m_highlight);
  pSEQPrefs->setPrefBool(prefBase + "HighlightUseSpawnColorPen", section, 
			 m_highlightUseSpawnColorPen);
  pSEQPrefs->setPrefBool(prefBase + "HighlightUseSpawnColorBrush", section,
			 m_highlightUseSpawnColorBrush);
  pSEQPrefs->setPrefBool(prefBase + "HighlightFlash", section, 
			 m_highlightFlash);

  // Save the line stuff
  pSEQPrefs->setPrefPen(prefBase + "Line0Pen", section, m_line0Pen);
  pSEQPrefs->setPrefBool(prefBase + "ShowLine0", section, m_showLine0);
  pSEQPrefs->setPrefPen(prefBase + "Line1Pen", section, m_line1Pen);
  pSEQPrefs->setPrefInt(prefBase + "Line1Distance", section, m_line1Distance);
  pSEQPrefs->setPrefPen(prefBase + "Line2Pen", section, m_line2Pen);
  pSEQPrefs->setPrefInt(prefBase + "Line2Distance", section, m_line2Distance);

  // Save the Walk Path related member variables
  pSEQPrefs->setPrefPen(prefBase + "WalkPathPen", section, m_walkPathPen);
  pSEQPrefs->setPrefBool(prefBase + "UseWalkPathPen", section, 
			 m_useWalkPathPen);
  pSEQPrefs->setPrefBool(prefBase + "ShowWalkPath", section, m_showWalkPath);

  // Save whatever's left
  pSEQPrefs->setPrefBool(prefBase + "ShowName", section, m_showName);
}

void MapIcon::paintNone(QPainter&p, const QPoint& point, 
			int size, int sizeWH)
{
}

void MapIcon::paintCircle(QPainter&p, const QPoint& point, 
			  int size, int sizeWH)
{
  p.drawEllipse(point.x() - size, point.y() - size, sizeWH, sizeWH);
}

void MapIcon::paintSquare(QPainter&p, const QPoint& point, 
			  int size, int sizeWH)
{
  p.drawRect(point.x() - size, point.y() - size, sizeWH, sizeWH);
}

void MapIcon::paintPlus(QPainter&p, const QPoint& point, int size, int sizeWH)
{
    p.drawLine(point.x(), point.y() - size, point.x(), point.y() + size );
    p.drawLine(point.x() - size, point.y(), point.x() + size, point.y() );
}

void MapIcon::paintX(QPainter&p, const QPoint& point, int size, int sizeWH)
{
    p.drawLine(point.x() - size, point.y() - size,
	       point.x() + size, point.y() + size);
    p.drawLine(point.x() - size, point.y() + size,
	       point.x() + size, point.y() - size);
}

void MapIcon::paintUpTriangle(QPainter&p, const QPoint& point, 
			      int size, int sizeWH)
{
  QPointArray atri(3);
  atri.setPoint(0, point.x(), point.y() - sizeWH);
  atri.setPoint(1, point.x() + size, point.y() + size);
  atri.setPoint(2, point.x() - size, point.y() + size);
  p.drawPolygon(atri);
}

void MapIcon::paintRightTriangle(QPainter&p, const QPoint& point,
				 int size, int sizeWH)
{
  QPointArray atri(3);
  atri.setPoint(0, point.x() + sizeWH, point.y());
  atri.setPoint(1, point.x() - size,  point.y() + size);
  atri.setPoint(2, point.x() - size,  point.y() - size);
  p.drawPolygon(atri);
}

void MapIcon::paintDownTriangle(QPainter&p, const QPoint& point, 
				int size, int sizeWH)
{
  QPointArray atri(3);
  atri.setPoint(0, point.x(), point.y() + sizeWH);
  atri.setPoint(1, point.x() + size, point.y() - size);
  atri.setPoint(2, point.x() - size, point.y() - size);
  p.drawPolygon(atri);
}

void MapIcon::paintLeftTriangle(QPainter&p, const QPoint& point, 
				int size, int sizeWH)
{
  QPointArray atri(3);
  atri.setPoint(0, point.x() - sizeWH, point.y());
  atri.setPoint(1, point.x() + size, point.y() + size);
  atri.setPoint(2, point.x() + size, point.y() - size);
  p.drawPolygon(atri);
}

void MapIcon::paintStar(QPainter&p, const QPoint& point, int size, int sizeWH)
{
  p.drawLine(point.x(), point.y() - size, point.x(), point.y() + size);
  p.drawLine(point.x() - size, point.y(), point.x() + size, point.y());
  p.drawLine(point.x() - size, point.y() - size,
	     point.x() + size, point.y() + size);
  p.drawLine(point.x() - size, point.y() + size,
	     point.x() + size, point.y() - size);
}

void MapIcon::paintDiamond(QPainter&p, const QPoint& point, 
			   int size, int sizeWH)
{
  QPointArray diamond(4);
  diamond.setPoint(0, point.x(), point.y() +  size);
  diamond.setPoint(1, point.x() + size, point.y());
  diamond.setPoint(2, point.x(), point.y() - size);
  diamond.setPoint(3, point.x() - size, point.y());
  p.drawPolygon(diamond);
}

//----------------------------------------------------------------------
// MapIcons
MapIcons::MapIcons(Player* player, const QString& preferenceName,
		   QObject* parent, const char* name)
  : QObject(parent, name),
    m_player(player),
    m_preferenceName(preferenceName),
    m_flash(false)
{
  // Declare the default icon type characteristics
  // NOTE: They are only declared here instead of in a file scope global
  //       const because QBrush, QPen, QColor, etc. can't be used until after
  //       the QApplication instance is created.
  PenCapStyle cap = SquareCap;
  PenJoinStyle join = BevelJoin;

  // see MapIcon class definition in map.h to see ordering.
  const MapIcon mapIconDefs[tIconTypeNumTypes] =
  {
    // tIconTypeUnknown
    { QBrush(), QBrush(), 
      QPen(gray, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(), 
      0, 0,
      tIconStyleCircle, tIconSizeSmall,
      tIconStyleNone, tIconSizeNone,
      true, false, true, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeDrop
    { QBrush(), QBrush(),
      QPen(yellow, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleX, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, false, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeDoor
    { QBrush(NoBrush), QBrush(),
      QPen(QColor(110, 60, 0), 0, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleSquare, tIconSizeTiny,
      tIconStyleNone, tIconSizeNone,
      true, false, false, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnNPC
    { QBrush(SolidPattern), QBrush(),
      QPen(black, 0, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleCircle, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, true, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnNPCCorpse
    { QBrush(SolidPattern), QBrush(),
      QPen(cyan, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStylePlus, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, false, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnPlayer
    { QBrush(SolidPattern), QBrush(),
      QPen(magenta, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleSquare, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, true, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnPlayerCorpse
    { QBrush(), QBrush(),
      QPen(yellow, 2, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleSquare, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, false, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnUnknown
    { QBrush(gray), QBrush(),
      QPen(NoPen, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleCircle, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, false, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnConsidered
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(red, 1, SolidLine, cap, join),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleSquare, tIconSizeLarge,
      false, false, false, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnPlayerTeam1
    { QBrush(SolidPattern), QBrush(),
      QPen(magenta, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleUpTriangle, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, true, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnPlayerTeam2
    { QBrush(SolidPattern), QBrush(),
      QPen(magenta, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleRightTriangle, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, true, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnPlayerTeam3
    { QBrush(SolidPattern), QBrush(),
      QPen(magenta, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleDownTriangle, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, true, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnPlayerTeam4
    { QBrush(SolidPattern), QBrush(),
      QPen(magenta, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleLeftTriangle, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, true, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnPlayerTeam5
    { QBrush(SolidPattern), QBrush(),
      QPen(magenta, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleSquare, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, true, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnPlayerTeamOtherRace
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(gray, 1, SolidLine, cap, join),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleSquare, tIconSizeXLarge,
      false, false, false, false,
      true, false, false, true,
      false, false, false, false },
    // tIconTypeSpawnPlayerTeamOtherDeity
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(gray, 1, SolidLine, cap, join),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleSquare, tIconSizeXLarge,
      false, false, false, false,
      true, false, false, true,
      false, false, false, false },
    // tIconTypeSpawnPlayerTeamOtherRacePet
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(SolidLine, 1, SolidLine, cap, join),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleCircle, tIconSizeXLarge,
      false, false, false, false,
      true, true, false, true,
      false, false, false, false },
    // tIconTypeSpawnPlayerTeamOtherDeityPet
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(SolidLine, 1, SolidLine, cap, join),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleCircle, tIconSizeXLarge,
      false, false, false, false,
      true, true, false, true,
      false, false, false, false },
    // tIconTypeSpawnPlayerOld
    { QBrush(), QBrush(),
      QPen(magenta, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStylePlus, tIconSizeRegular,
      tIconStyleNone, tIconSizeNone,
      true, false, false, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeItemSelected
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(magenta, 1, SolidLine, cap, join),
      QPen(magenta), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleCircle, tIconSizeXXLarge,
      false, false, false, false,
      true, false, false, true,
      true, false, true, false },
    // tIconTypeFilterFlagHunt
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(gray, 1, SolidLine, cap, join),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleCircle, tIconSizeLarge,
      false, false, false, false,
      true, false, false, true,
      false, false, false, false },
    // tIconTypeFilterFlagCaution
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(yellow, 1, SolidLine, cap, join),
      QPen(), QPen(yellow, 1, SolidLine, cap, join), QPen(), QPen(),
      500, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleCircle, tIconSizeLarge,
      false, false, false, false,
      true, false, false, true,
      false, false, false, false },
    // tIconTypeFilterFlagDanger
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(red, 1, SolidLine, cap, join),
      QPen(), QPen(red, 1, SolidLine, cap, join), QPen(yellow, 1, SolidLine, cap, join), QPen(),
      500, 1000,
      tIconStyleNone, tIconSizeNone,
      tIconStyleCircle, tIconSizeLarge,
      false, false, false, false,
      true, false, false, true,
      false, false, false, false },
    // tIconTypeFilterLocate
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(white, 1, SolidLine, cap, join),
      QPen(white, 1, SolidLine, cap, join), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleCircle, tIconSizeLarge,
      false, false, false, false,
      true, false, false, true,
      true, false, false, false },
    // tIconTypeFilterAlert
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleCircle, tIconSizeLarge,
      false, false, false, false,
      false, false, false, true,
      false, false, false, false },
    // tIconTypeFilterFiltered
    { QBrush(Dense2Pattern), QBrush(),
      QPen(gray, 0, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleNone, tIconSizeNone,
      true, false, true, false,
      false, false, false, true,
      false, false, false, false },
    // tIconTypeFilterTracer
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(yellow, 1, SolidLine, cap, join),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleCircle, tIconSizeLarge,
      false, false, false, false,
      true, false, false, false,
      false, false, true, false },
    // tIconTypeRuntimeFiltered
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(white, 1, SolidLine, cap, join),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleCircle, tIconSizeLarge,
      false, false, false, false,
      true, false, false, true,
      false, false, false, false },
    // tIconTypeSpawnPoint
    { QBrush(SolidPattern), QBrush(),
      QPen(darkGray, 1, SolidLine, cap, join), QPen(),
      QPen(), QPen(), QPen(), QPen(),
      0, 0,
      tIconStylePlus, tIconSizeSmall,
      tIconStyleNone, tIconSizeNone,
      true, true, false, false,
      false, false, false, false,
      false, false, false, false },
    // tIconTypeSpawnPointSelected
    { QBrush(), QBrush(NoBrush),
      QPen(), QPen(blue, 1, SolidLine, cap, join),
      QPen(blue), QPen(), QPen(), QPen(),
      0, 0,
      tIconStyleNone, tIconSizeNone,
      tIconStyleCircle, tIconSizeLarge,
      false, false, false, false,
      true, false, false, true,
      true, false, false, false },
  };

  // setup icon size maps
  m_mapIconSizes[tIconSizeNone] = &m_markerNSize; // none should never be drawn
  m_mapIconSizesWH[tIconSizeNone] = &m_markerNSizeWH; // but just in case...
  m_mapIconSizes[tIconSizeTiny] = &m_markerNSize;
  m_mapIconSizesWH[tIconSizeTiny] = &m_markerNSizeWH;
  m_mapIconSizes[tIconSizeSmall] = &m_marker0Size;
  m_mapIconSizesWH[tIconSizeSmall] = &m_marker0SizeWH;
  m_mapIconSizes[tIconSizeRegular] = &m_drawSize;
  m_mapIconSizesWH[tIconSizeRegular] = &m_drawSizeWH;
  m_mapIconSizes[tIconSizeLarge] = &m_marker1Size;
  m_mapIconSizesWH[tIconSizeLarge] = &m_marker1SizeWH;
  m_mapIconSizes[tIconSizeXLarge] = &m_marker2Size;
  m_mapIconSizesWH[tIconSizeXLarge] = &m_marker2SizeWH;
  m_mapIconSizes[tIconSizeXXLarge] = &m_marker2Size;
  m_mapIconSizesWH[tIconSizeXXLarge] = &m_marker2SizeWH;

  // setup the icons
  for (int k = 0; k <= tIconTypeMax; k++)
    m_mapIcons[k] = mapIconDefs[k];

  // setup the flash timer
  m_flashTimer = new QTimer(this);
  connect(m_flashTimer, SIGNAL(timeout()), this, SLOT(flashTick()));
  m_flashTimer->start(166, false);
}

MapIcons::~MapIcons()
{
}

void MapIcons::load()
{
  m_showNPCWalkPaths = pSEQPrefs->getPrefBool("ShowNPCWalkPaths", 
					      preferenceName(), false);
  m_showSpawnNames = pSEQPrefs->getPrefBool("ShowSpawnNames", preferenceName(),
					    false);
  m_fovDistance = pSEQPrefs->getPrefInt("FOVDistance", preferenceName(), 
					200);



  int val = pSEQPrefs->getPrefInt("DrawSize", preferenceName(), 3);
  m_drawSize = val; 
  m_drawSizeWH = val << 1; // 2 x size
  m_marker1Size = val + 1;
  m_marker1SizeWH = m_marker1Size << 1; // 2 x size
  m_marker2Size = val + 2;
  m_marker2SizeWH = m_marker2Size << 1; // 2 x size
  m_marker3Size = val + 3;
  m_marker3SizeWH = m_marker2Size << 1; // 2 x size
  if (val > 1)
    m_marker0Size = val - 1;
  else 
    m_marker0Size = val;
  m_marker0SizeWH = m_marker0Size << 1; // 2 x size
  if (val > 2)
    m_markerNSize = val - 2;
  else
    m_markerNSize = 1;
  m_markerNSizeWH = m_markerNSize << 1; // 2 x size

  // setup map icons
  for (int k = 0; k <= tIconTypeMax; k++)
    m_mapIcons[k].load(iconTypePrefBaseNames[k], preferenceName());
}

void MapIcons::save()
{
  // save map icons
  for (int k = 0; k <= tIconTypeMax; k++)
    m_mapIcons[k].save(iconTypePrefBaseNames[k], preferenceName());
}

void MapIcons::dumpInfo(QTextStream& out)
{
  out << "[" << preferenceName() << " MapIcons]" << endl;
  out << "ShowSpawnNames: " << m_showSpawnNames << endl;
  out << "FOVDistance: " << m_fovDistance << endl;
  out << "DrawSize: " << m_drawSize << endl;
  out << endl;
}

void MapIcons::setDrawSize(int val)
{
  if ((val < 1) || (val > 6))
    return;

  m_drawSize = val; 
  m_drawSizeWH = val << 1; // 2 x size
  m_marker1Size = val + 1;
  m_marker1SizeWH = m_marker1Size << 1; // 2 x size
  m_marker2Size = val + 2;
  m_marker2SizeWH = m_marker2Size << 1; // 2 x size
  m_marker3Size = val + 3;
  m_marker3SizeWH = m_marker2Size << 1; // 2 x size
  if (val > 1)
    m_marker0Size = val - 1;
  else 
    m_marker0Size = val;
  m_marker0SizeWH = m_marker0Size << 1; // 2 x size
  if (val > 2)
    m_markerNSize = val - 2;
  else
    m_markerNSize = 1;
  m_markerNSizeWH = m_markerNSize << 1; // 2 x size

  pSEQPrefs->setPrefInt("DrawSize", preferenceName(), m_drawSize);
}

void MapIcons::setShowNPCWalkPaths(bool val) 
{ 
  m_showNPCWalkPaths = val; 

  pSEQPrefs->setPrefBool("ShowNPCWalkPaths", preferenceName(), 
			 m_showNPCWalkPaths);
}


void MapIcons::setShowSpawnNames(bool val) 
{ 
  m_showSpawnNames = val; 

  QString tmpPrefString = "ShowSpawnNames";
  pSEQPrefs->setPrefBool(tmpPrefString, preferenceName(), m_showSpawnNames);
}

void MapIcons::setFOVDistance(int val) 
{ 
  if ((val < 1) || (val > 1200))
    return;

  m_fovDistance = val; 

  QString tmpPrefString = "FOVDistance";
  pSEQPrefs->setPrefInt(tmpPrefString, preferenceName(), m_fovDistance);
}

void MapIcons::paintIcon(MapParameters& param, 
			 QPainter& p, 
			 const MapIcon& mapIcon,
			 const Item* item, 
			 const QPoint& point)
{
  // Draw Line
  if (mapIcon.m_showLine0)
  {
    p.setPen(mapIcon.m_line0Pen);
    p.drawLine(param.playerXOffset(), 
	       param.playerYOffset(),
	       point.x(), point.y());
  }

  // Calculate distance and draw distance related lines
  uint32_t distance = UINT32_MAX;
  if (mapIcon.m_line1Distance || mapIcon.m_line2Distance)
  {
    if (!showeq_params->fast_machine)
      distance = item->calcDist2DInt(param.player());
    else
      distance = (int)item->calcDist(param.player());

    if (mapIcon.m_line1Distance > distance)
    {
      p.setPen(mapIcon.m_line1Pen);
      p.drawLine(param.playerXOffset(), 
		 param.playerYOffset(),
		 point.x(), point.y());
    }

    if (mapIcon.m_line2Distance > distance)
    {
      p.setPen(mapIcon.m_line2Pen);
      p.drawLine(param.playerXOffset(), 
		 param.playerYOffset(),
		 point.x(), point.y());
    }
  }

  // Draw Item Name
  if (mapIcon.m_showName)
  {
    QString spawnNameText = item->name();
    
    QFontMetrics fm(param.font());
    int width = fm.width(spawnNameText);
    p.setPen(darkGray);
    p.drawText(point.x() - (width / 2),
	       point.y() + 10, spawnNameText);
  }

  // Draw Icon Image
  if (mapIcon.m_image && 
      (!mapIcon.m_imageFlash || m_flash) &&
      (mapIcon.m_imageStyle != tIconStyleNone))
  {
    p.setPen(mapIcon.m_imagePen);
    p.setBrush(mapIcon.m_imageBrush);

    mapIcon.paintIconImage(mapIcon.m_imageStyle, p, point, 
			   *m_mapIconSizes[mapIcon.m_imageSize],
			   *m_mapIconSizesWH[mapIcon.m_imageSize]);
  }

  // Draw Highlight
  if (mapIcon.m_highlight && 
      (!mapIcon.m_highlightFlash || m_flash) &&
      (mapIcon.m_highlightStyle != tIconStyleNone))
  {
    p.setPen(mapIcon.m_highlightPen);
    p.setBrush(mapIcon.m_highlightBrush);

    mapIcon.paintIconImage(mapIcon.m_highlightStyle, p, point, 
			   *m_mapIconSizes[mapIcon.m_highlightSize],
			   *m_mapIconSizesWH[mapIcon.m_highlightSize]);
  }
}

void MapIcons::paintSpawnIcon(MapParameters& param, 
			      QPainter& p, 
			      const MapIcon& mapIcon,
			      const Spawn* spawn, 
			      const EQPoint& location,
			      const QPoint& point)
{
  // ------------------------
  // Draw Walk Path
  if (mapIcon.m_showWalkPath ||
      (m_showNPCWalkPaths && spawn->isNPC()))
  {
    SpawnTrackListIterator trackIt(spawn->trackList());
    
    const SpawnTrackPoint* trackPoint = trackIt.current();
    if (trackPoint)
    {
      if (!mapIcon.m_useWalkPathPen)
	p.setPen(blue);
      else
	p.setPen(mapIcon.m_walkPathPen);

      p.moveTo (param.calcXOffsetI(trackPoint->x()), 
		param.calcYOffsetI(trackPoint->y()));
      
      while ((trackPoint = ++trackIt) != NULL)
	p.lineTo (param.calcXOffsetI (trackPoint->x()), 
		  param.calcYOffsetI (trackPoint->y()));
      
      p.lineTo (point.x(), point.y());
    }
  }

  // Draw Line
  if (mapIcon.m_showLine0)
  {
    p.setPen(mapIcon.m_line0Pen);
    p.drawLine(param.playerXOffset(), 
	       param.playerYOffset(),
	       point.x(), point.y());
  }

  // calculate distance and draw distance related lines
  uint32_t distance = UINT32_MAX;
  if (mapIcon.m_line1Distance || mapIcon.m_line2Distance || 
      m_showSpawnNames)
  {
    if (!showeq_params->fast_machine)
      distance = location.calcDist2DInt(param.player());
    else
      distance = (int)location.calcDist(param.player());
    
    if (mapIcon.m_line1Distance > distance)
    {
      p.setPen(mapIcon.m_line1Pen);
      p.drawLine(param.playerXOffset(), 
		 param.playerYOffset(),
		 point.x(), point.y());
    }

    if (mapIcon.m_line2Distance > distance)
    {
      p.setPen(mapIcon.m_line2Pen);
      p.drawLine(param.playerXOffset(), 
		 param.playerYOffset(),
		 point.x(), point.y());
    }
  }

  // Draw Spawn Names
  if (mapIcon.m_showName || 
      (m_showSpawnNames && (distance < m_fovDistance)))
  {
    QString spawnNameText;
    
    spawnNameText.sprintf("%2d: %s",
			  spawn->level(),
			  (const char*)spawn->name());
    
    QFontMetrics fm(param.font());
    int width = fm.width(spawnNameText);
    p.setPen(darkGray);
    p.drawText(point.x() - (width / 2),
	       point.y() + 10, spawnNameText);
  }
  
  // Draw the Icon
  if (mapIcon.m_image && 
      (!mapIcon.m_imageFlash || m_flash) &&
      (mapIcon.m_imageStyle != tIconStyleNone))
  {
    if (mapIcon.m_imageUseSpawnColorPen)
    {
      QPen pen = mapIcon.m_imagePen;
      pen.setColor(m_player->pickConColor(spawn->level()));
      p.setPen(pen);
    }
    else
      p.setPen(mapIcon.m_imagePen);

    if (mapIcon.m_imageUseSpawnColorBrush)
    {
      QBrush brush = mapIcon.m_imageBrush;
      brush.setColor(m_player->pickConColor(spawn->level()));
      p.setBrush(brush);
    }
    else
      p.setBrush(mapIcon.m_imageBrush);

    mapIcon.paintIconImage(mapIcon.m_imageStyle, p, point, 
			   *m_mapIconSizes[mapIcon.m_imageSize],
			   *m_mapIconSizesWH[mapIcon.m_imageSize]);
  }

  // Draw the highlight
  if (mapIcon.m_highlight && 
      (!mapIcon.m_highlightFlash || m_flash) &&
      (mapIcon.m_highlightStyle != tIconStyleNone))
  {
    if (mapIcon.m_highlightUseSpawnColorPen)
    {
      QPen pen = mapIcon.m_highlightPen;
      pen.setColor(m_player->pickConColor(spawn->level()));
      p.setPen(pen);
    }
    else
      p.setPen(mapIcon.m_highlightPen);

    if (mapIcon.m_highlightUseSpawnColorBrush)
    {
      QBrush brush = mapIcon.m_highlightBrush;
      brush.setColor(m_player->pickConColor(spawn->level()));
      p.setBrush(brush);
    }
    else
      p.setBrush(mapIcon.m_highlightBrush);

    mapIcon.paintIconImage(mapIcon.m_highlightStyle,p, point, 
			   *m_mapIconSizes[mapIcon.m_highlightSize],
			   *m_mapIconSizesWH[mapIcon.m_highlightSize]);
  }
}

void MapIcons::paintSpawnPointIcon(MapParameters& param, 
				   QPainter& p, 
				   const MapIcon& mapIcon,
				   const SpawnPoint* sp, 
				   const QPoint& point)
{
  // Draw Line
  if (mapIcon.m_showLine0)
  {
    p.setPen(mapIcon.m_line0Pen);
    p.drawLine(param.playerXOffset(), 
	       param.playerYOffset(),
	       point.x(), point.y());
  }

  // calculate distance and draw distance related lines
  uint32_t distance = UINT32_MAX;
  if (mapIcon.m_line1Distance || mapIcon.m_line2Distance)
  {
    if (!showeq_params->fast_machine)
      distance = sp->calcDist2DInt(param.player());
    else
      distance = (int)sp->calcDist(param.player());
    
    if (mapIcon.m_line1Distance > distance)
    {
      p.setPen(mapIcon.m_line1Pen);
      p.drawLine(param.playerXOffset(), 
		 param.playerYOffset(),
		 point.x(), point.y());
    }

    if (mapIcon.m_line2Distance > distance)
    {
      p.setPen(mapIcon.m_line2Pen);
      p.drawLine(param.playerXOffset(), 
		 param.playerYOffset(),
		 point.x(), point.y());
    }
  }

  // Draw Spawn Names
  if (mapIcon.m_showName)
  {
    QString spawnNameText;
    
    spawnNameText.sprintf("sp:%s %s (%d)",
			  (const char*)sp->name(),
			  (const char*)sp->last(),
			  sp->count());
    
    QFontMetrics fm(param.font());
    int width = fm.width(spawnNameText);
    p.setPen(darkGray);
    p.drawText(point.x() - (width / 2),
	       point.y() + 10, spawnNameText);
  }
  
  // Draw the Icon
  if (mapIcon.m_image && 
      (!mapIcon.m_imageFlash || m_flash) &&
      (mapIcon.m_imageStyle != tIconStyleNone))
  {
    if (mapIcon.m_imageUseSpawnColorPen)
    {
      QPen pen = mapIcon.m_imagePen;
      pen.setColor(pickSpawnPointColor(sp, pen.color()));
      p.setPen(pen);
    }
    else
      p.setPen(mapIcon.m_imagePen);

    if (mapIcon.m_imageUseSpawnColorBrush)
    {
      QBrush brush = mapIcon.m_imageBrush;
      brush.setColor(pickSpawnPointColor(sp, brush.color()));
      p.setBrush(brush);
    }
    else
      p.setBrush(mapIcon.m_imageBrush);

    mapIcon.paintIconImage(mapIcon.m_imageStyle, p, point, 
			   *m_mapIconSizes[mapIcon.m_imageSize],
			   *m_mapIconSizesWH[mapIcon.m_imageSize]);
  }

  // Draw the highlight
  if (mapIcon.m_highlight && 
      (!mapIcon.m_highlightFlash || m_flash) &&
      (mapIcon.m_highlightStyle != tIconStyleNone))
  {
    if (mapIcon.m_highlightUseSpawnColorPen)
    {
      QPen pen = mapIcon.m_highlightPen;
      pen.setColor(pickSpawnPointColor(sp, pen.color()));
      p.setPen(pen);
    }
    else
      p.setPen(mapIcon.m_highlightPen);

    if (mapIcon.m_highlightUseSpawnColorBrush)
    {
      QBrush brush = mapIcon.m_highlightBrush;
      brush.setColor(pickSpawnPointColor(sp, brush.color()));
      p.setBrush(brush);
    }
    else
      p.setBrush(mapIcon.m_highlightBrush);

    mapIcon.paintIconImage(mapIcon.m_highlightStyle,p, point, 
			   *m_mapIconSizes[mapIcon.m_highlightSize],
			   *m_mapIconSizesWH[mapIcon.m_highlightSize]);
  }
}

void MapIcons::flashTick()
{
  m_flash = !m_flash;
}

QColor MapIcons::pickSpawnPointColor(const SpawnPoint* sp, 
				     const QColor& defColor)
{
  QColor color;
  // calculate the pen color
  if ((sp->diffTime() == 0) || (sp->deathTime() == 0))
    color = defColor;
  else
  {
    unsigned char age = sp->age();
    
    if ( age == 255 )
      color = darkRed;
    else if ( age > 220 )
    {
      if (m_flash)
	color = red;
    }
    else
      color = QColor(age, age, 0);
  }

  return color;
}

