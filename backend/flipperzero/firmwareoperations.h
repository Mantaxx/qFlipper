#pragma once

#include "abstractfirmwareoperation.h"

class QIODevice;

namespace Flipper {
class FlipperZero;

namespace Zero {

class FirmwareDownloadOperation : public AbstractFirmwareOperation
{
public:
    FirmwareDownloadOperation(FlipperZero *device, QIODevice *file);
    ~FirmwareDownloadOperation();

    const QString name() const override;
    bool execute() override;

private:
    FlipperZero *m_device;
    QIODevice *m_file;
};

class WirelessStackDownloadOperation : public AbstractFirmwareOperation
{
public:
    WirelessStackDownloadOperation(FlipperZero *device, QIODevice *file, uint32_t targetAddress = 0);
    ~WirelessStackDownloadOperation();

    const QString name() const override;
    bool execute() override;

private:
    FlipperZero *m_device;
    QIODevice *m_file;
    uint32_t m_targetAddress;
};

class FixOptionBytesOperation : public AbstractFirmwareOperation
{
public:
    FixOptionBytesOperation(FlipperZero *device);
    ~FixOptionBytesOperation();

    const QString name() const override;
    bool execute() override;

private:
    FlipperZero *m_device;
};

}
}