#include "NoteViewerDotDotLengthMeasureTool.h"
#include "note/NoteViewer.h"
#include "ui_NoteViewer.h"
#include "note/detail/gui/NoteViewerItemList.h"
#include "note/detail/item/NoteViewerDistanceItem.h"
#include "note/detail/item/NoteViewerDotItem.h"

NoteViewerDotDotLengthMeasureTool::NoteViewerDotDotLengthMeasureTool(NoteViewer *viewer)
    : NoteViewerMeasureMarkToolInterface(viewer)
{
    initActor();
}

QString NoteViewerDotDotLengthMeasureTool::toolName() const
{
    return tr("点距");
}

void NoteViewerDotDotLengthMeasureTool::toolFocused()
{
    connect(m_viewer, &NoteViewer::sigItemSelectionChanged, this, &NoteViewerDotDotLengthMeasureTool::slotItemSelectionChanged);
    slotItemSelectionChanged();
}

void NoteViewerDotDotLengthMeasureTool::toolUnfocused()
{
    disconnect(m_viewer, &NoteViewer::sigItemSelectionChanged, this, &NoteViewerDotDotLengthMeasureTool::slotItemSelectionChanged);
}

void NoteViewerDotDotLengthMeasureTool::initActor()
{
    m_source = vtkSmartPointer<vtkLineSource>::New();
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(m_source->GetOutputPort());
    m_actor = vtkSmartPointer<vtkActor>::New();
    m_actor->SetMapper(mapper);
}

void NoteViewerDotDotLengthMeasureTool::slotLeftButtonClicked(Double3 pos, int specialKey)
{
    Q_UNUSED(specialKey)
    if(!m_item1 || !m_item2){
        return;
    }

    auto item = new NoteViewerDistanceItem(m_viewer, m_item1->m_point, m_item2->m_point, pos);
    m_viewer->getNoteViewerUi()->itemList->addItem(item);
}

void NoteViewerDotDotLengthMeasureTool::slotLeftButtonDoubleClicked(Double3 pos, int specialKey)
{
    slotLeftButtonClicked(pos, specialKey);
}

static Double3 projection(Double3 p, Double3 center, Double3 direction)
{
    Double3 projection;

    Double3 vectorPl;
    vtkMath::Subtract(p.data(), center.data(), vectorPl.data());
    double t = vtkMath::Dot(vectorPl.data(), direction.data()) / vtkMath::Dot(direction.data(), direction.data());

    for(int i=0; i<3; i++){
        projection[i] = center[i] + t*direction[i];
    }

    return projection;
}

void NoteViewerDotDotLengthMeasureTool::slotMouseMoved(Double3 pos, int specialKey)
{
    Q_UNUSED(specialKey)
    if(!m_item1 || !m_item2){
        return;
    }

    Double3 direction;
    vtkMath::Subtract(m_item2->m_point.data(), m_item1->m_point.data(), direction.data());
    vtkMath::Normalize(direction.data());
    auto p4 = projection(m_item1->m_point, pos, direction);
    auto p5 = projection(m_item2->m_point, pos, direction);
    m_source->SetPoint1(p4[0], p4[1], 0.001);
    m_source->SetPoint2(p5[0], p5[1], 0.001);

    m_viewer->getRenderer()->AddActor(m_actor);
    m_viewer->updateRender();
}

void NoteViewerDotDotLengthMeasureTool::slotItemSelectionChanged()
{
    auto items = m_viewer->getNoteViewerUi()->itemList->getSelectedItems();
    if(items.length() != 2){
        emit m_viewer->sigStatusOperation(tr("请选择两点"));
        m_item1 = nullptr;
        m_item2 = nullptr;
        m_viewer->getRenderer()->RemoveActor(m_actor);
        return;
    }

    m_item1 = dynamic_cast<NoteViewerDotItem*>(items.first());
    m_item2 = dynamic_cast<NoteViewerDotItem*>(items.last());

    if(!m_item1 || !m_item2){
        emit m_viewer->sigStatusOperation(tr("请选择两点"));
        m_viewer->getRenderer()->RemoveActor(m_actor);
        m_viewer->updateRender();
    }else{
        emit m_viewer->sigStatusOperation(tr("请点击标注位置"));
    }
}

bool NoteViewerDotDotLengthMeasureTool::eventFilter(QObject *watched, QEvent *event)
{
    if(m_item1 && m_item2){
        if(event->type() == QEvent::Enter){
            m_viewer->getRenderer()->AddActor(m_actor);
            m_viewer->updateRender();
        }
        if(event->type() == QEvent::Leave){
            m_viewer->getRenderer()->RemoveActor(m_actor);
            m_viewer->updateRender();
        }
    }

    return QObject::eventFilter(watched, event);
}
