#include "graphpreprocess.h"

void GraphPreprocess::generateWidget(const QList<QMap<QString, QVariant>> &widget_properties) {
    interface_widget = new QGroupBox(group_name);
    QVBoxLayout *widget_layout = new QVBoxLayout(interface_widget);
    widget_layout->setContentsMargins(0, 0, 0, 0);
    QFrame *body_frame = new QFrame(interface_widget);
    QFormLayout *layout = new QFormLayout(body_frame);

    widget_layout->addWidget(body_frame);

    FormGenerator::generateWidget(interface_widget, layout, widget_properties);
}

GraphPreprocess::GraphPreprocess(QObject *parent) : FormGenerator(parent), interface_widget(nullptr) {}

GraphPreprocess::~GraphPreprocess() {
    if (interface_widget != nullptr)
        delete interface_widget;
}



void VectorTransforms::generateWidget(const QList<QMap<QString, QVariant>> &widget_properties) {
    interface_widget = new QGroupBox(group_name);
    QVBoxLayout *widget_layout = new QVBoxLayout(interface_widget);
    widget_layout->setContentsMargins(0, 0, 0, 0);
    QFrame *tools_frame = new QFrame(interface_widget), *body_frame = new QFrame(interface_widget);
    QHBoxLayout *tools_layout = new QHBoxLayout(tools_frame);
    QFormLayout *layout = new QFormLayout(body_frame);

    widget_layout->addWidget(tools_frame);
    widget_layout->addWidget(body_frame);

    QCheckBox *use_checkbox = new QCheckBox("use");
    use_checkbox->setChecked(true);
    tools_layout->addWidget(use_checkbox);
    connect(use_checkbox, &QCheckBox::stateChanged, this, &VectorTransforms::useFilter);

    FormGenerator::generateWidget(interface_widget, layout, widget_properties);
}

VectorTransforms::VectorTransforms(QObject *parent) : GraphPreprocess(parent), use(true) {}

VectorTransforms::~VectorTransforms() {}

void VectorTransforms::useFilter(bool use) {
    this->use = use;
}

// process implementations
LinearVectorization::LinearVectorization(QObject *parent) : Vectorization(parent) {
    m_ratio = 3;

    group_name = "Linear vectorization";
    generateWidget(QList<QMap<QString, QVariant>>(
    {
        {
            std::pair<QString, QVariant>("name", "ratio"),
            std::pair<QString, QVariant>("min", 1),
            std::pair<QString, QVariant>("max", 255)
        }
    }));
}

QLinkedList<QPoint> LinearVectorization::vectorizeCurve(const QImage &image, bool **used_field, const QPoint &start) {
    QLinkedList<QPoint> result;

    auto nextStep {
        [&](int x, int y, QPoint &cur_pos) {
            if (image.pixelColor(cur_pos.x() + x, cur_pos.y() + y).value() > 0 && ! used_field[cur_pos.y() + y][cur_pos.x() + x]) {
                used_field[cur_pos.y()][cur_pos.x()] = true;
                cur_pos.setX(cur_pos.x() + x);
                cur_pos.setY(cur_pos.y() + y);
                return true;
            }
            return false;
        }
    };

    auto useNearest {
        [&](QPoint pos, int align) {
            if (align == 0) { // vertical
                if (pos.y() > 0) {
                    used_field[pos.y() - 1][pos.x()] = true;
                }
                if (pos.y() < image.height() - 1) {
                    used_field[pos.y() + 1][pos.x()] = true;
                }
            }
            else if (align == 1) { // horizontal
                if (pos.x() > 0) {
                    used_field[pos.y()][pos.x() - 1] = true;
                }
                if (pos.y() < image.width() - 1) {
                    used_field[pos.y()][pos.x() + 1] = true;
                }
            }
        }
    };

    QPoint cur_pos = start;
    QPoint prev_pos = cur_pos;
    int cur_step = 0;
    bool found = true, first = true;
    while (found) {
        if (cur_step % this->m_ratio == 0) {
            result.append(cur_pos);
        }

        found = false;

        if (nextStep(-1, 0, cur_pos)) {
            if (!first) useNearest(prev_pos, 0);
            found = true;
        }
        else if (nextStep(0, 1, cur_pos)) {
            if (!first) useNearest(prev_pos, 1);
            found = true;
        }
        else if (nextStep(1, 0, cur_pos)) {
            if (!first) useNearest(prev_pos, 0);
            found = true;
        }
        else if (nextStep(0, -1, cur_pos)) {
            if (!first) useNearest(prev_pos, 1);
            found = true;
        }
        else if (nextStep(-1, 1, cur_pos)) found = true;
        else if (nextStep(1, 1, cur_pos)) found = true;
        else if (nextStep(1, -1, cur_pos)) found = true;
        else if (nextStep(-1, -1, cur_pos)) found = true;

        if (! found) {
            if (cur_step % this->m_ratio != 0) {
                result.append(cur_pos);
            }
            break;
        }

        cur_step++;
        prev_pos = cur_pos;
        first = false;
    }


    prev_pos = cur_pos = start;
    cur_step = 0;
    found = true;
    while (found) {
        found = false;

        if (nextStep(-1, 0, cur_pos)) {
            useNearest(prev_pos, 0);
            found = true;
        }
        else if (nextStep(0, 1, cur_pos)) {
            useNearest(prev_pos, 1);
            found = true;
        }
        else if (nextStep(1, 0, cur_pos)) {
            useNearest(prev_pos, 0);
            found = true;
        }
        else if (nextStep(0, -1, cur_pos)) {
            useNearest(prev_pos, 1);
            found = true;
        }
        else if (nextStep(-1, 1, cur_pos)) found = true;
        else if (nextStep(1, 1, cur_pos)) found = true;
        else if (nextStep(1, -1, cur_pos)) found = true;
        else if (nextStep(-1, -1, cur_pos)) found = true;

        if (! found) {
            if (cur_step % this->m_ratio != 0) {
                result.prepend(cur_pos);
            }
            break;
        }

        cur_step++;

        if (cur_step % this->m_ratio == 0) {
            result.prepend(cur_pos);
        }

        prev_pos = cur_pos;
    }

    return result;
}

VectorizationProduct LinearVectorization::processData(const QImage &image) {
    bool **used_field = new bool*[image.height()];
    for (int i = 0; i < image.height(); i++) {
        used_field[i] = new bool[image.width()];
        for (int j = 0; j < image.width(); j++) {
            used_field[i][j] = false;
        }
    }

    VectorizationProduct vp;

    for (int y = 0; y < image.height(); y++) {
        for (int x = 0; x < image.width(); x++) {
            if (! used_field[y][x]) {
                if (image.pixelColor(x, y).value() > 0) {
                    vp.append(this->vectorizeCurve(image, used_field, QPoint(x, y)));
                }
            }
            used_field[y][x] = true;
        }
    }

    for (int i = 0; i < image.height(); i++) {
        delete [] used_field[i];
    }
    delete [] used_field;

    return vp;
}

LinearVectorization::~LinearVectorization() {};



VectorNoiseClearing::VectorNoiseClearing(int radius, QObject *parent) : VectorTransforms(parent) {
    m_radius = radius;

    group_name = "Noise clearing";
    generateWidget(QList<QMap<QString, QVariant>>(
    {
        {
            std::pair<QString, QVariant>("name", "radius"),
            std::pair<QString, QVariant>("min", 1),
            std::pair<QString, QVariant>("max", 1024)
        }
    }));
}

VectorizationProduct VectorNoiseClearing::processData(const VectorizationProduct &vp) {
    VectorizationProduct result;

    for (auto it = vp.begin(); it != vp.end(); it++) {
        QPoint min(it->first().x(), it->first().y());
        QPoint max = min;
        for (auto it2 = it->begin(); it2 != it->end(); it2++) {
            if (it2->x() < min.x()) min.setX(it2->x());
            if (it2->x() > max.x()) max.setX(it2->x());
            if (it2->y() < min.y()) min.setY(it2->y());
            if (it2->y() > max.y()) max.setY(it2->y());
        }
        QPoint size = max - min;
        if (size.x() >= m_radius || size.y() >= m_radius) {
            result.append(*it);
        }
    }

    return result;
}

VectorNoiseClearing::~VectorNoiseClearing() {}



VectorMerge::VectorMerge(int radius, QObject *parent) : VectorTransforms(parent) {
    m_radius = radius;

    group_name = "Merge";
    generateWidget(QList<QMap<QString, QVariant>>(
    {
        {
            std::pair<QString, QVariant>("name", "radius"),
            std::pair<QString, QVariant>("min", 1),
            std::pair<QString, QVariant>("max", 1024)
        }
    }));
}

VectorizationProduct VectorMerge::processData(const VectorizationProduct &vp) {
    VectorizationProduct result(vp);

    auto calcLength {
        [](QPoint vec) {
            return vec.x() * vec.x() + vec.y() * vec.y();
        }
    };

    int radius = m_radius * m_radius;

    // merge different contours
    bool found = true;
    int cur_length;
    VectorizationProduct::iterator it1, it2;
    while (found) {
        found = false;
        for (it1 = result.begin(); it1 != result.end(); it1++) {

            int first_end = -1, second_end = -1;
            int min_length = std::numeric_limits<int>::max();
            VectorizationProduct::iterator merge_elem_it;
            for (it2 = result.begin(); it2 != result.end(); it2++) {
                if (it1 != it2) {
                    // 1 case
                    cur_length = calcLength(it1->first() - it2->first());
                    if (cur_length < min_length) {
                        first_end = 0;
                        second_end = 0;
                        min_length = cur_length;
                        merge_elem_it = it2;
                    }
                    // 2 case
                    cur_length = calcLength(it1->first() - it2->last());
                    if (cur_length < min_length) {
                        first_end = 0;
                        second_end = 1;
                        min_length = cur_length;
                        merge_elem_it = it2;
                    }
                    // 3 case
                    cur_length = calcLength(it1->last() - it2->first());
                    if (cur_length < min_length) {
                        first_end = 1;
                        second_end = 0;
                        min_length = cur_length;
                        merge_elem_it = it2;
                    }
                    // 4 case
                    cur_length = calcLength(it1->last() - it2->last());
                    if (cur_length < min_length) {
                        first_end = 1;
                        second_end = 1;
                        min_length = cur_length;
                        merge_elem_it = it2;
                    }
                }
            }
            if (first_end == 0 && second_end == 0 && min_length <= radius) {
                for (auto it_copy = merge_elem_it->begin(); it_copy != merge_elem_it->end(); it_copy++) {
                    it1->prepend(*it_copy);
                }
                result.erase(merge_elem_it);
                found = true;
            }
            if (first_end == 0 && second_end == 1 && min_length <= radius) {
                for (auto it_copy = merge_elem_it->rbegin(); it_copy != merge_elem_it->rend(); it_copy++) {
                    it1->prepend(*it_copy);
                }
                result.erase(merge_elem_it);
                found = true;
            }
            if (first_end == 1 && second_end == 0 && min_length <= radius) {
                for (auto it_copy = merge_elem_it->begin(); it_copy != merge_elem_it->end(); it_copy++) {
                    it1->append(*it_copy);
                }
                result.erase(merge_elem_it);
                found = true;
            }
            if (first_end == 1 && second_end == 1 && min_length <= radius) {
                for (auto it_copy = merge_elem_it->rbegin(); it_copy != merge_elem_it->rend(); it_copy++) {
                    it1->append(*it_copy);
                }
                result.erase(merge_elem_it);
                found = true;
            }
        }
    }

    // merge closed contours
    for (auto it = result.begin(); it != result.end(); it++) {
        if (calcLength(it->first() - it->last()) <= radius) {
            it->append(it->first());
        }
    }

    return result;
}

VectorMerge::~VectorMerge() {}

// GraphProcessor

GraphProcessor::GraphProcessor(QWidget *prop_group_widget, QObject *parent) : QObject(parent) {
    this->prop_group_widget = prop_group_widget;
}

GraphProcessor::~GraphProcessor() {
    if (vectorization_filter != nullptr) {
        delete vectorization_filter;
    }
}

void GraphProcessor::setMiddleware(Vectorization *vectorization_filter, const QList<VectorTransforms*> &vector_trans_filters) {
    this->vectorization_filter = vectorization_filter;
    if (vectorization_filter->getWidget() != nullptr) {
        prop_group_widget->layout()->addWidget(vectorization_filter->getWidget());
    }

    this->vector_trans_filters = vector_trans_filters;
    for (auto it = vector_trans_filters.begin(); it != vector_trans_filters.end(); it++) {
        if ((*it)->getWidget() != nullptr) {
            prop_group_widget->layout()->addWidget((*it)->getWidget());
        }
    }
}

void GraphProcessor::processGraph(const QImage &image) {
    emit startCalculating(1 + vector_trans_filters.count(), "Processing graph...");
    VectorizationProduct vectorization_result;


    int f_ind = 0;
    emit currentFilter(f_ind, vectorization_filter->getGroupName());
    f_ind++;
    vectorization_result = vectorization_filter->processData(image);

    for (int i = 0; i < vector_trans_filters.count(); i++) {
        emit currentFilter(f_ind, vector_trans_filters[i]->getGroupName());
        f_ind++;
        if (! vector_trans_filters[i]->isUse())
            continue;
        vectorization_result = vector_trans_filters[i]->processData(vectorization_result);
    }

    emit finishCalculating();
    emit resultReady(vectorization_result);
}
