#ifndef IVW_ORDINALMINMAXTEXTTROPERTYWIDGETQT_H
#define IVW_ORDINALMINMAXTEXTTROPERTYWIDGETQT_H

#include <inviwo/qt/widgets/inviwoqtwidgetsdefine.h>
#include <inviwo/qt/widgets/properties/propertywidgetqt.h>
#include <inviwo/qt/widgets/editablelabelqt.h>
#include <inviwo/qt/widgets/ordinaleditorwidget.h>
#include <inviwo/core/properties/minmaxproperty.h>
#include <inviwo/core/util/stringconversion.h>

namespace inviwo {

class IVW_QTWIDGETS_API BaseOrdinalMinMaxTextPropertyWidgetQt : public PropertyWidgetQt {

    Q_OBJECT

public:
    BaseOrdinalMinMaxTextPropertyWidgetQt(Property* property);
    virtual ~BaseOrdinalMinMaxTextPropertyWidgetQt();
    
    virtual void updateFromProperty() = 0;
    
public slots:
    virtual void updateFromMin() = 0;
    virtual void updateFromMax() = 0;
    
    void setPropertyDisplayName();

protected:
    void generateWidget();
    
    virtual void makeEditorWidgets() = 0;
    
    BaseOrdinalEditorWidget* min_;
    BaseOrdinalEditorWidget* max_;
    EditableLabelQt* label_;
};



template <typename T>
class OrdinalMinMaxTextPropertyWidgetQt : public BaseOrdinalMinMaxTextPropertyWidgetQt {

public:
    OrdinalMinMaxTextPropertyWidgetQt(MinMaxProperty<T>* property);
    virtual ~OrdinalMinMaxTextPropertyWidgetQt();

    virtual void updateFromProperty();
    virtual void updateFromMin();
    virtual void updateFromMax();

    typedef glm::detail::tvec2<T, glm::defaultp> V;
    
protected:
    virtual void makeEditorWidgets();
    virtual std::string getToolTipText();
    
    MinMaxProperty<T>* minMaxProperty_;
    
    OrdinalEditorWidget<T>* min_;
    OrdinalEditorWidget<T>* max_;
};

typedef OrdinalMinMaxTextPropertyWidgetQt<double> DoubleMinMaxTextPropertyWidgetQt;
typedef OrdinalMinMaxTextPropertyWidgetQt<float> FloatMinMaxTextPropertyWidgetQt;
typedef OrdinalMinMaxTextPropertyWidgetQt<int> IntMinMaxTextPropertyWidgetQt;


template<typename T>
OrdinalMinMaxTextPropertyWidgetQt<T>::OrdinalMinMaxTextPropertyWidgetQt(MinMaxProperty<T>* property)
    : BaseOrdinalMinMaxTextPropertyWidgetQt(property)
    , minMaxProperty_(property) {
    
    BaseOrdinalMinMaxTextPropertyWidgetQt::generateWidget();
    updateFromProperty();
}

template<typename T>
OrdinalMinMaxTextPropertyWidgetQt<T>::~OrdinalMinMaxTextPropertyWidgetQt() {
}

template<typename T>
void OrdinalMinMaxTextPropertyWidgetQt<T>::makeEditorWidgets(){
    min_ = new OrdinalEditorWidget<T>();
    max_ = new OrdinalEditorWidget<T>();
    
    BaseOrdinalMinMaxTextPropertyWidgetQt::min_ = min_;
    BaseOrdinalMinMaxTextPropertyWidgetQt::max_ = max_;
}

template<typename T>
void OrdinalMinMaxTextPropertyWidgetQt<T>::updateFromProperty() {
    V val = minMaxProperty_->get();
    V range = minMaxProperty_->getRange();
    T inc = minMaxProperty_->getIncrement();
    T sep = minMaxProperty_->getMinSeparation();
    
    min_->blockSignals(true);
    max_->blockSignals(true);

    min_->setRange(range.x, range.y-sep);
    max_->setRange(range.x+sep, range.y);
    
    min_->initValue(val.x);
    max_->initValue(val.y);

    min_->setIncrement(inc);
    max_->setIncrement(inc);

    min_->blockSignals(false);
    max_->blockSignals(false);

    min_->setDisabled(minMaxProperty_->getReadOnly());
    max_->setDisabled(minMaxProperty_->getReadOnly());
}


template<typename T>
void OrdinalMinMaxTextPropertyWidgetQt<T>::updateFromMin() {
    T min = min_->getValue();
    T sep = minMaxProperty_->getMinSeparation();
    V range = minMaxProperty_->get();
    
    if (std::abs(min - range.x) > glm::epsilon<T>()) {
        range.x = min;
        
        if (range.y-range.x < sep) {
            range.y = range.x + sep;
            max_->blockSignals(true);
            max_->setValue(range.y);
            max_->blockSignals(false);
        }
        
        minMaxProperty_->setInitiatingWidget(this);
        minMaxProperty_->set(range);
        minMaxProperty_->clearInitiatingWidget();
    }
}

template<typename T>
void OrdinalMinMaxTextPropertyWidgetQt<T>::updateFromMax() {
    T max = max_->getValue();
    T sep = minMaxProperty_->getMinSeparation();
    V range = minMaxProperty_->get();
    
    if (std::abs(max - range.y) > glm::epsilon<T>()) {
        range.y = max;
        
        if (range.y - range.x < sep) {
            range.x = range.y - sep;
            min_->blockSignals(true);
            min_->setValue(range.x);
            min_->blockSignals(false);
        }
        
        minMaxProperty_->setInitiatingWidget(this);
        minMaxProperty_->set(range);
        minMaxProperty_->clearInitiatingWidget();
    }
}

template <typename T>
std::string OrdinalMinMaxTextPropertyWidgetQt<T>::getToolTipText() {
    std::stringstream ss;

    ss << this->makeToolTipTop(this->minMaxProperty_->getDisplayName());
    ss << this->makeToolTipTableTop();
    ss << this->makeToolTipRow("Identifier", this->minMaxProperty_->getIdentifier());
    ss << this->makeToolTipRow("Path", joinString(property_->getPath(),"."));
    ss << this->makeToolTipRow("Semantics", this->minMaxProperty_->getSemantics().getString());
    ss << makeToolTipRow("Validation Level", PropertyOwner::invalidationLevelToString(
                                             this->minMaxProperty_->getInvalidationLevel()));

    ss << this->makeToolTipRow("Minimum", toString(minMaxProperty_->get().x));
    ss << this->makeToolTipRow("Maximum", toString(minMaxProperty_->get().y));
    ss << this->makeToolTipRow("Range min", toString(minMaxProperty_->getRangeMin()));
    ss << this->makeToolTipRow("Range max", toString(minMaxProperty_->getRangeMax()));
    ss << this->makeToolTipRow("Increment", toString(minMaxProperty_->getIncrement()));
    ss << this->makeToolTipRow("Separation", toString(minMaxProperty_->getMinSeparation()));

    ss << this->makeToolTipTableBottom();
    ss << this->makeToolTipBottom();
    return ss.str();
}

} // namespace

#endif // IVW_ORDINALMINMAXTEXTTROPERTYWIDGETQT_H

