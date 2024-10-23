#pragma once
#include "note/detail/measureTool/NoteViewerDotDotLengthMeasureTool.h"


class NoteViewerArcArcDistanceMeasureTool : public NoteViewerDotDotLengthMeasureTool
{
    Q_OBJECT
public:
    NoteViewerArcArcDistanceMeasureTool(NoteViewer* viewer);

protected:
    virtual QString toolName() const final ;
    virtual void toolFocused() final ;
    virtual void toolUnfocused() final ;
    Double3 calculateArcVertex(Double7 arc);

public slots:
    void slotItemSelectionChanged();

};
