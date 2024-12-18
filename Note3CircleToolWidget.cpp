#include "Note3CircleToolWidget.h"
#include "ui_Note3CircleToolWidget.h"
#include "dialogs/Toast.h"
#include "Note3VolumeViewer.h"
#include <vtkVertexGlyphFilter.h>
#include "util/SpecialKey.h"
#include "ui_Note3VolumeViewer.h"
#include "Note3ItemListWidget.h"
#include "items/Note3CircleItem.h"
#include <vtkTransformPolyDataFilter.h>

Note3CircleToolWidget::Note3CircleToolWidget(Note3VolumeViewer* viewer) :
    QWidget(viewer), ui(new Ui::Note3CircleToolWidget), m_viewer(viewer)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    initGui();
    initConnection();
}

Note3CircleToolWidget::~Note3CircleToolWidget()
{
    if(m_pointActor){
        m_viewer->removeActor(m_pointActor);
    }
    if(m_diskActor){
        m_viewer->removeActor(m_diskActor);
    }

    delete ui;
}

void Note3CircleToolWidget::slotOkBtnClicked()
{
    if(!m_diskActor){
        Toast::showTip(Toast::ErrorToast, tr("圆未就绪"));
        return;
    }

    m_viewer->removeActor(m_diskActor);
    auto item = new Note3CircleItem(m_viewer, tr("圆"), m_diskActor, m_circleSource);
    m_diskActor = nullptr;
    m_viewer->ui->itemListWidget->addItem(item);

    this->close();
}

void Note3CircleToolWidget::slotCancelBtnClicked()
{
    this->close();
}

vtkSmartPointer<vtkActor> createDiskActor(const Double3& position, Double3 normal, double radius)
{
    auto diskSource = vtkSmartPointer<vtkDiskSource>::New();
    diskSource->SetOuterRadius(radius);
    diskSource->SetInnerRadius(0);
    diskSource->SetRadialResolution(200);
    diskSource->SetCircumferentialResolution(200);
    diskSource->SetOutputPointsPrecision(200);

    auto transform = vtkSmartPointer<vtkTransform>::New();
    transform->Identity();

    Double3 defaultNormal {0, 0, 1};
    Double3 rotateAxis;
    double angle;

    vtkMath::Cross(defaultNormal.data(), normal.data(), rotateAxis.data());
    angle = vtkMath::DegreesFromRadians(acos(vtkMath::Dot(defaultNormal.data(), normal.data())));
    if(vtkMath::Norm(rotateAxis.data()) < 1e-6){
        if(normal[2] < 0){
            rotateAxis[0] = 1.0;
            angle = 180.0;
        }else{
            rotateAxis[0] = 1.0;
            angle = 0.0;
        }
    }

    transform->Translate(position.data());
    transform->RotateWXYZ(angle, rotateAxis.data());

    auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputConnection(diskSource->GetOutputPort());
    transformFilter->SetTransform(transform);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(transformFilter->GetOutputPort());

    auto diskActor = vtkSmartPointer<vtkActor>::New();
    diskActor->SetMapper(mapper);
    diskActor->SetPickable(false);
    diskActor->GetProperty()->SetOpacity(0.5);

    return diskActor;
}

void Note3CircleToolWidget::slotGenerateBtnClicked()
{
    if(m_diskActor){
        m_viewer->removeActor(m_diskActor);
        m_diskActor = nullptr;
    }

    m_circleSource = computeCircleSource(m_points);
    m_diskActor = createDiskActor(m_circleSource.position, m_circleSource.Direction, m_circleSource.radius);
    m_viewer->addActor(m_diskActor);
    m_viewer->updateRender();
}

void Note3CircleToolWidget::slotLeftBtnReleased(int specialKey)
{
    if(!SpecialKeyUtil::isOnlyAlt(specialKey)){
        return;
    }

    auto point = m_viewer->getPickPoint();
    if(point.actor){
        m_points.append(point.pos);
        pointSetChanged();
    }
}

void Note3CircleToolWidget::slotListWidgetContextMenuRequested(const QPoint & point)
{
    auto items = ui->pointListWidget->selectedItems();
    if(items.isEmpty()){
        auto item = ui->pointListWidget->itemAt(point);
        if(item){
            items.append(item);
        }else{
            return;
        }
    }
    QMenu menu;
    menu.addAction(tr("删除"), [=](){
        QList<int> rows;
        for(QListWidgetItem* item : items){
            rows.append(ui->pointListWidget->row(item));
        }
        std::sort(rows.begin(), rows.end(), [](int a, int b){
            return a > b;
        });
        for(auto row : rows){
            m_points.removeAt(row);
        }
        pointSetChanged();
    });
    menu.exec(QCursor::pos());
}

void Note3CircleToolWidget::pointSetChanged()
{
    QStringList items;
    auto points = vtkSmartPointer<vtkPoints>::New();
    for(const auto& val : m_points){
        items << QString::number(val[0]) + ", " + QString::number(val[1]) + ", " + QString::number(val[2]);
        points->InsertNextPoint(val[0], val[1], val[2]);
    }

    ui->pointListWidget->clear();
    ui->pointListWidget->addItems(items);
    m_pointPoly->SetPoints(points);
    m_viewer->updateRender();
}

void Note3CircleToolWidget::initGui()
{

    {
        m_pointPoly = vtkSmartPointer<vtkPolyData>::New();
        auto filter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
        filter->SetInputData(m_pointPoly);

        auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(filter->GetOutputPort());

        m_pointActor = vtkSmartPointer<vtkActor>::New();
        m_pointActor->SetMapper(mapper);
        m_pointActor->GetProperty()->SetPointSize(3);
        m_viewer->addActor(m_pointActor);
    }
}

void Note3CircleToolWidget::initConnection()
{
    connect(ui->okBtn, &QPushButton::clicked, this, &Note3CircleToolWidget::slotOkBtnClicked);
    connect(ui->cancelBtn, &QPushButton::clicked, this, &Note3CircleToolWidget::slotCancelBtnClicked);
    connect(ui->generateBtn, &QPushButton::clicked, this, &Note3CircleToolWidget::slotGenerateBtnClicked);

    connect(ui->pointListWidget, &QListWidget::customContextMenuRequested, this, &Note3CircleToolWidget::slotListWidgetContextMenuRequested);
    connect(m_viewer, &Note3VolumeViewer::sigLeftButtonReleased, this, &Note3CircleToolWidget::slotLeftBtnReleased);
}

void Note3CircleToolWidget::addOriPolydata()
{
    auto points = vtkSmartPointer<vtkPoints>::New();
    for(const auto& val : m_points){
        points->InsertNextPoint(val[0], val[1], val[2]);
    }

    auto m_pointPoly = vtkSmartPointer<vtkPolyData>::New();
    m_pointPoly->SetPoints(points);

    auto filter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    filter->SetInputData(m_pointPoly);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(filter->GetOutputPort());

    m_pointActor = vtkSmartPointer<vtkActor>::New();
    m_pointActor->SetMapper(mapper);

    Note3Actor actor;
    actor.m_actor = m_pointActor;
    actor.m_name = "圆";
    m_viewer->m_actors.append(actor);
    m_viewer->ui->eleListWidget->addItem(actor.m_name);
}
