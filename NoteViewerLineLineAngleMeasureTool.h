#pragma once

#include "note/detail/NoteViewerMeasureMarkToolInterface.h"
#include <vtkArcSource.h>

class NoteViewerLineItem;
class NoteViewerLineLineAngleMeasureTool : public NoteViewerMeasureMarkToolInterface
{
    Q_OBJECT
public:
    NoteViewerLineLineAngleMeasureTool(NoteViewer* viewer);

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
    void computeAngle();
    bool isSharpAngle(Double3 pos);


private:
    vtkSmartPointer<vtkArcSource> m_source{};
    vtkSmartPointer<vtkActor> m_actor{};

private:
    NoteViewerLineItem* m_item1{};
    NoteViewerLineItem* m_item2{};

private:
    Double3 m_center;
    Double3 m_point1, m_point2;
    double m_angle;
    bool m_isNagitive{false};
};

