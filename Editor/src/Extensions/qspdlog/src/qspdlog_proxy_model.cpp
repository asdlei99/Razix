// clang-format off
#include "rzepch.h"
// clang-format on
#include "qspdlog_model.hpp"
#include "qspdlog_proxy_model.hpp"

QSpdLogProxyModel::QSpdLogProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setFilterKeyColumn(-1);
}
