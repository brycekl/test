#include "NoteViewerPlugin.h"
#include "AlgoMainWindow.h"
#include "algo/TreeWidget.h"
#include "algo/TreeBaseItem.h"
#include "algo/VtkImageTreeItem.h"
#include "dialogs/Toast.h"
#include "dialogs/WaitDialog.h"
#include <itkMeanImageFilter.h>
#include "ContourViewerDialog.h"
#include "util/ItkD3Util.h"
#include "algo/VtkActorTreeItem.h"
#include "Note3VolumeViewer.h"
#include "util/ImageUtil.h"
#include <vtkActor.h>

NoteViewerPlugin::NoteViewerPlugin(AlgoMainWindow * window)
    : AlgoPluginBase(window)
{
}

QString NoteViewerPlugin::pluginName()
{
    return tr("测量模块");
}

QStringList NoteViewerPlugin::pluginMenus()
{
    return {tr("3D测量")};
}

void NoteViewerPlugin::invokePlugin()
{
    auto tree = m_window->m_tree->getTree();
    auto item = tree->currentItem();
    if(item == nullptr){
        Toast::showTip(Toast::ErrorToast, tr("当前没有选中图像"));
        return;
    }

    auto ptr = dynamic_cast<VtkActorTreeItem*>(tree->itemWidget(item, 0));
    if(!ptr){
        Toast::showTip(Toast::ErrorToast, tr("当前的对象"));
    }

    auto actor = deepCopyActor(ptr->m_actor);
    auto viewer = new Note3VolumeViewer(actor);
    viewer->show();
}

void NoteViewerPlugin::afterInvoke()
{
}

void NoteViewerPlugin::slotFinished()
{
}

vtkSmartPointer<vtkActor> NoteViewerPlugin::deepCopyActor(vtkSmartPointer<vtkActor>actor)
{
    auto newActor = vtkSmartPointer<vtkActor>::New();
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(actor->GetMapper()->GetInputConnection(0, 0));
    newActor->SetMapper(mapper);

    if(actor->GetProperty()){
        auto property = vtkSmartPointer<vtkProperty>::New();
        property->DeepCopy(actor->GetProperty());
        newActor->SetProperty(property);
    }

    newActor->SetPosition(actor->GetPosition());
    newActor->SetScale(actor->GetScale());
    newActor->SetOrientation(actor->GetOrientation());
    newActor->SetOrigin(actor->GetOrigin());

    return newActor;
}
