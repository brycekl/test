#include "NoteViewerLineLineAngleMeasureTool.h"
#include "note/NoteViewer.h"
#include "ui_NoteViewer.h"
#include "note/detail/gui/NoteViewerItemList.h"
#include "note/detail/item/NoteViewerLineItem.h"
#include "note/detail/item/NoteViewerAngleItem.h"
#include "dialogs/Toast.h"

// TODO: 之后处理角度是 0 度的问题
NoteViewerLineLineAngleMeasureTool::NoteViewerLineLineAngleMeasureTool(NoteViewer* viewer)
    : NoteViewerMeasureMarkToolInterface(viewer)
{
    initActor();
}

QString NoteViewerLineLineAngleMeasureTool::toolName() const
{
    return tr("线角");
}

void NoteViewerLineLineAngleMeasureTool::toolFocused()
{
    connect(m_viewer, &NoteViewer::sigItemSelectionChanged, this, &NoteViewerLineLineAngleMeasureTool::slotItemSelectionChanged);
    slotItemSelectionChanged();
}

void NoteViewerLineLineAngleMeasureTool::toolUnfocused()
{
    disconnect(m_viewer, &NoteViewer::sigItemSelectionChanged, this, &NoteViewerLineLineAngleMeasureTool::slotItemSelectionChanged);
}

void NoteViewerLineLineAngleMeasureTool::initActor()
{
    m_source = vtkSmartPointer<vtkArcSource>::New();
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(m_source->GetOutputPort());

    m_actor = vtkSmartPointer<vtkActor>::New();
    m_actor->SetMapper(mapper);
}

void NoteViewerLineLineAngleMeasureTool::slotLeftButtonClicked(Double3 pos, int specialKey)
{
    if(!m_item1 || !m_item2){
        return;
    }

    auto item = new NoteViewerAngleItem(m_viewer, m_center, m_point1, m_point2, m_angle, m_isNagitive);
    m_viewer->getNoteViewerUi()->itemList->addItem(item);
}

void NoteViewerLineLineAngleMeasureTool::slotLeftButtonDoubleClicked(Double3 pos, int specialKey)
{
    slotLeftButtonClicked(pos, specialKey);
}

void NoteViewerLineLineAngleMeasureTool::slotMouseMoved(Double3 pos, int specialKey)
{
    if(!m_item1 || !m_item2){
        return;
    }

    m_viewer->getRenderer()->AddActor(m_actor);
    computeAngle();

    Double3 v1, v2;
    vtkMath::Subtract(m_item1->m_point2.data(), m_center.data(), v1.data());
    vtkMath::Subtract(m_item2->m_point2.data(), m_center.data(), v2.data());
    vtkMath::Normalize(v1.data());
    vtkMath::Normalize(v2.data());
    auto len = std::sqrt(vtkMath::Distance2BetweenPoints(m_center.data(), pos.data()));
    for(int i=0;i<2; i++){
        m_point1[i] = len * v1[i] + m_center[i];
        m_point2[i] = len * v2[i] + m_center[i];
    }
    m_point1[2] = 0.001;
    m_point2[2] = 0.001;

    m_isNagitive = !isSharpAngle(pos);
    m_source->SetPoint1(m_point1.data());
    m_source->SetPoint2(m_point2.data());
    m_source->SetCenter(m_center.data());
    m_source->SetNegative(!isSharpAngle(pos));

    m_source->SetResolution(100);
    m_viewer->updateRender();

}

void NoteViewerLineLineAngleMeasureTool::slotItemSelectionChanged()
{
    auto items = m_viewer->getNoteViewerUi()->itemList->getSelectedItems();
    if(items.size() !=2){
        m_item1 = nullptr;
        m_item2 = nullptr;
        emit m_viewer->sigStatusOperation(tr("请选择两条直线"));
        m_viewer->getRenderer()->RemoveActor(m_actor);
        return;
    }

    m_item1 = dynamic_cast<NoteViewerLineItem*>(items.first());
    m_item2 = dynamic_cast<NoteViewerLineItem*>(items.last());

    if(m_item1 && m_item2){
        computeAngle();
//        if(m_angle == 180.0){
//            Toast::showTip(Toast::ErrorToast, tr("两直线平行"));
//            m_item1 = nullptr;
//            m_item2 = nullptr;
//        }else{
            emit m_viewer->sigStatusOperation("请选择标记位置");
            m_viewer->getRenderer()->RemoveActor(m_actor);
            m_viewer->updateRender();
//        }
    }else{
        emit m_viewer->sigStatusOperation(tr("请选择两条直线"));
    }
}

bool NoteViewerLineLineAngleMeasureTool::eventFilter(QObject *watched, QEvent *event)
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

void NoteViewerLineLineAngleMeasureTool::computeAngle()
{
    Double3 v1, v2;
    vtkMath::Subtract(m_item1->m_point2.data(), m_item1->m_point1.data(), v1.data());
    vtkMath::Subtract(m_item2->m_point2.data(), m_item2->m_point1.data(), v2.data());
    double dotProduct = vtkMath::Dot(v1.data(), v2.data());
    double len1 = vtkMath::Norm(v1.data());
    double len2 = vtkMath::Norm(v2.data());
    m_angle = vtkMath::DegreesFromRadians(std::acos(dotProduct / (len1*len2)));

    if(m_angle < 0){
        m_angle = -m_angle;
    }
    if(m_angle > 180){
        m_angle = 360-m_angle;
    }

    if(m_angle == 0 || m_angle == 180){
        m_center[0] = std::numeric_limits<double>::max();
        m_center[1] = std::numeric_limits<double>::max();
        m_center[2] = 0.001;
        return;
    }

    // center
    m_item1->m_point1[2] = 0.001;
    m_item1->m_point2[2] = 0.001;
    m_item2->m_point1[2] = 0.001;
    m_item2->m_point2[2] = 0.001;
    double t, s;
    vtkLine::Intersection(m_item1->m_point1.data(),
                             m_item1->m_point2.data(),
                             m_item2->m_point1.data(),
                             m_item2->m_point2.data(), t, s);
    for(int i=0; i<3; i++){
        m_center[i] = m_item1->m_point1.data()[i] + t * v1[i];
    }
    m_center[2] = 0.001;
}

bool NoteViewerLineLineAngleMeasureTool::isSharpAngle(Double3 pos)
{
    Double3 v1, v2, v3;
    vtkMath::Subtract(m_item1->m_point2.data(), m_center.data(), v1.data());
    vtkMath::Subtract(m_item2->m_point2.data(), m_center.data(), v2.data());
    vtkMath::Subtract(pos.data(), m_center.data(), v3.data());

    double angle12, angle13, angle23;
    {
        double dotProduct = vtkMath::Dot(v1.data(), v2.data());
        double len1 = vtkMath::Norm(v1.data());
        double len2 = vtkMath::Norm(v2.data());
        angle12 = vtkMath::DegreesFromRadians(std::acos(dotProduct / (len1*len2)));
    }

    {
        double dotProduct = vtkMath::Dot(v1.data(), v3.data());
        double len1 = vtkMath::Norm(v1.data());
        double len2 = vtkMath::Norm(v3.data());
        angle13 = vtkMath::DegreesFromRadians(std::acos(dotProduct / (len1*len2)));
    }

    {
        double dotProduct = vtkMath::Dot(v2.data(), v3.data());
        double len1 = vtkMath::Norm(v2.data());
        double len2 = vtkMath::Norm(v3.data());
        angle23 = vtkMath::DegreesFromRadians(std::acos(dotProduct / (len1*len2)));
    }

    if(angle13 < angle12 && angle23 < angle12){
        return true;
    }
    return false;
}
