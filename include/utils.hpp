#pragma once

#define THROW_ERR(_STRING, ...) throw std::runtime_error(QString(_STRING).arg(__VA_ARGS__).toStdString())
