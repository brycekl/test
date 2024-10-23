#pragma once

#include "note/detail/NoteViewerMeasureMarkToolInterface.h"
#include <vtkLineSource.h>

class NoteViewerDotItem;
class NoteViewerItemInterface;
class NoteViewerDotDotLengthMeasureTool : public NoteViewerMeasureMarkToolInterface
{
    Q_OBJECT
public:
    NoteViewerDotDotLengthMeasureTool(NoteViewer* viewer);

protected:
    virtual QString toolName() const final;
    virtual void toolFocused() final;
    virtual void toolUnfocused() final;
    virtual void initActor() final;

public slots:
    virtual void slotLeftButtonClicked(Double3 pos, int specialKey) final;
    virtual void slotLeftButtonDoubleClicked(Double3 pos, int specialKey) final;
    virtual void slotMouseMoved(Double3 pos, int specialKey) final;
    void slotItemSelectionChanged();

public:
    virtual bool eventFilter(QObject *watched, QEvent *event) final;

private:
    vtkSmartPointer<vtkLineSource> m_source;
    vtkSmartPointer<vtkActor> m_actor;

private:
    NoteViewerDotItem* m_item1{};
    NoteViewerDotItem* m_item2{};
};

