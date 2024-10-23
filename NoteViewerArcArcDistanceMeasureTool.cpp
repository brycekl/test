#include "note/NoteViewer.h"
#include "ui_NoteViewer.h"
#include "NoteViewerArcArcDistanceMeasureTool.h"
#include "note/detail/item/NoteViewerDotItem.h"
#include "note/detail/item/NoteViewerArcItem.h"


NoteViewerArcArcDistanceMeasureTool::NoteViewerArcArcDistanceMeasureTool(NoteViewer *viewer)
    : NoteViewerDotDotLengthMeasureTool(viewer)
{
    initActor();
}


QString NoteViewerArcArcDistanceMeasureTool::toolName() const
{
    return tr("圆弧距");
}

void NoteViewerArcArcDistanceMeasureTool::toolFocused()
{
    connect(m_viewer, &NoteViewer::sigItemSelectionChanged, this, &NoteViewerArcArcDistanceMeasureTool::slotItemSelectionChanged);
    slotItemSelectionChanged();
}

void NoteViewerArcArcDistanceMeasureTool::toolUnfocused()
{
    disconnect(m_viewer, &NoteViewer::sigItemSelectionChanged, this, &NoteViewerArcArcDistanceMeasureTool::slotItemSelectionChanged);
}


void NoteViewerArcArcDistanceMeasureTool::slotItemSelectionChanged()
{
    auto items = m_viewer->getNoteViewerUi()->itemList->getSelectedItems();
    if(items.length() != 2){
        emit m_viewer->sigStatusOperation(tr("请选择两点"));
        m_item1 = nullptr;
        m_item2 = nullptr;
        m_viewer->getRenderer()->RemoveActor(m_actor);
        return;
    }

    NoteViewerArcItem * acr1 = dynamic_cast<NoteViewerArcItem*>(items.first());
    NoteViewerArcItem * arc2 = dynamic_cast<NoteViewerArcItem*>(items.last());

    if(!acr1 || !arc2){
        emit m_viewer->sigStatusOperation(tr("请选择两个椭圆"));
        m_viewer->getRenderer()->RemoveActor(m_actor);
        m_viewer->updateRender();
    }else{
        Double3 vertex1 = calculateArcVertex(acr1->m_arc);
        Double3 vertex2 = calculateArcVertex(arc2->m_arc);

        m_item1 = new NoteViewerDotItem(m_viewer, vertex1);
        m_item2 = new NoteViewerDotItem(m_viewer, vertex2);
        emit m_viewer->sigStatusOperation(tr("请点击标注位置"));
    }
}

Double3 NoteViewerArcArcDistanceMeasureTool::calculateArcVertex(Double7 arc)
{
    auto [center_x, center_y, start_x, start_y, end_x, end_y, angle] = arc;
    double r = std::sqrt(std::pow(start_x - center_x, 2) + std::pow(start_y - center_y, 2));
    double start_angle = std::atan2(start_y - center_y, start_x - center_x);
    double vertex_angle = start_angle + angle / 2.0;

    double vertex_x = center_x + r * std::cos(vertex_angle);
    double vertex_y = center_y + r * std::sin(vertex_angle);
    return Double3{vertex_x, vertex_y, 0.001};
}
