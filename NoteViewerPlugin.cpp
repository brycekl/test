#include "NoteViewerPlugin.h"
#include "AlgoMainWindow.h"
#include "algo/TreeWidget.h"
#include "algo/TreeBaseItem.h"
#include "algo/VtkPointCloudTreeItem.h"
#include "dialogs/Toast.h"
#include "dialogs/WaitDialog.h"
#include <itkMeanImageFilter.h>
#include "ContourViewerDialog.h"
#include "util/ItkD3Util.h"
#include "algo/VtkActorTreeItem.h"
#include "Note3VolumeViewer.h"
#include "util/ImageUtil.h"
#include <vtkActor.h>
#include <vtkVertexGlyphFilter.h>

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
    auto items = tree->selectedItems();

    if(items.isEmpty()){
        Toast::showTip(Toast::ErrorToast, tr("当前没有选中图像"));
        return;
    }

    QList<Note3Actor> actors;
    for(auto item : items){
        auto ptrActor = dynamic_cast<VtkActorTreeItem*>(tree->itemWidget(item, 0));
        if(ptrActor){
            Note3Actor actor;
            actor.m_actor = deepCopyMeshActor(ptrActor->m_data);
            actor.m_name = ptrActor->m_name;
            actors.append(actor);
            continue;
        }

        auto ptrClound = dynamic_cast<VtkPointCloudTreeItem*>(tree->itemWidget(item, 0));
        if(ptrClound){
            Note3Actor actor;
            actor.m_actor = deepCopyPointCloudActor(ptrClound->m_data);
            actor.m_name = ptrClound->m_name;
            actors.append(actor);
            continue;
        }

        Toast::showTip(Toast::ErrorToast, tr("只能选择点云或Mesh数据"));
        return;
    }

    auto viewer = new Note3VolumeViewer(actors);
    viewer->show();
}

void NoteViewerPlugin::afterInvoke()
{
}

void NoteViewerPlugin::slotFinished()
{
}

vtkSmartPointer<vtkActor> NoteViewerPlugin::deepCopyMeshActor(vtkSmartPointer<vtkPolyData> data)
{
    auto newActor = vtkSmartPointer<vtkActor>::New();
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(data);
    newActor->SetMapper(mapper);

    return newActor;
}

vtkSmartPointer<vtkActor> NoteViewerPlugin::deepCopyPointCloudActor(vtkSmartPointer<vtkPolyData> data)
{
    auto filter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    filter->SetInputData(data);
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(filter->GetOutputPort());
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    return actor;
}
