#include "ComponentInstance.h"
#include "Wire.h"
#include <algorithm>
#include <memory>

class Circuit
{
public:
    // Добавление провода
    bool addWire(PinInstance *from, PinInstance *to)
    {
        // Проверяем, существует ли уже такой провод
        if (hasWire(from, to)) {
            return false;
        }

        // Создаём новый провод и добавляем в вектор
        wires.push_back(std::make_unique<Wire>(from, to));
        return true;
    }

    // Проверка существования
    bool hasWire(PinInstance *from, PinInstance *to) const
    {
        return std::any_of(wires.begin(),
                           wires.end(),
                           [from, to](const std::unique_ptr<Wire> &wire) {
                               return wire
                                      && ((wire->from == from && wire->to == to)
                                          || (wire->from == to && wire->to == from));
                           });
    }

    // Удаление по пинам
    bool removeWire(PinInstance *from, PinInstance *to)
    {
        auto it = std::find_if(wires.begin(),
                               wires.end(),
                               [from, to](const std::unique_ptr<Wire> &wire) {
                                   return wire
                                          && ((wire->from == from && wire->to == to)
                                              || (wire->from == to && wire->to == from));
                               });

        if (it != wires.end()) {
            wires.erase(it); // unique_ptr автоматически удалит объект
            return true;
        }
        return false;
    }

    // Удаление всех проводов, связанных с пином
    void removeWiresConnectedTo(PinInstance *pin)
    {
        // Удаляем все провода, где участвует данный пин
        wires.erase(std::remove_if(wires.begin(),
                                   wires.end(),
                                   [pin](const std::unique_ptr<Wire> &wire) {
                                       return wire && (wire->from == pin || wire->to == pin);
                                   }),
                    wires.end());
    }

    // Получение всех проводов (только для чтения) - возвращаем вектор указателей
    const std::vector<std::unique_ptr<Wire>> &getWires() const { return wires; }

    // Альтернатива: получить сырые указатели для безопасного использования
    std::vector<Wire *> getRawWires() const
    {
        std::vector<Wire *> result;
        result.reserve(wires.size());
        for (const auto &wire : wires) {
            if (wire) {
                result.push_back(wire.get());
            }
        }
        return result;
    }

    // Очистка всех проводов
    void clear()
    {
        wires.clear(); // все unique_ptr будут автоматически удалены
    }

    // Количество проводов
    size_t wireCount() const { return wires.size(); }

    // Получить провод по индексу (с проверкой)
    Wire *getWire(size_t index) const
    {
        if (index < wires.size() && wires[index]) {
            return wires[index].get();
        }
        return nullptr;
    }

    // Компоненты схемы
    std::vector<std::unique_ptr<ComponentInstance>> components;

private:
    std::vector<std::unique_ptr<Wire>> wires;
};