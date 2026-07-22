#pragma once
#include <string>

// Абстрактный интерфейс для View (не зависит от Qt)
class IViewModelListener
{
public:
    virtual void onPropertyChanged(const std::string &propertyName) = 0;
    virtual ~IViewModelListener() = default;
};