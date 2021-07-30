#include "deviceregistry.h"

#include "flipperzero/flipperzero.h"
#include "usbdevice.h"
#include "macros.h"

using namespace Flipper;

DeviceRegistry::DeviceRegistry(QObject *parent):
    QAbstractListModel(parent)
{
    connect(USBDeviceDetector::instance(), &USBDeviceDetector::devicePluggedIn, this, &DeviceRegistry::insertDevice);
    connect(USBDeviceDetector::instance(), &USBDeviceDetector::deviceUnplugged, this, &DeviceRegistry::removeDevice);

    USBDeviceDetector::instance()->setWantedDevices({
        // Flipper Zero in DFU mode
        USBDeviceInfo(0x0483, 0xdf11),
        // Flipper Zero in VCP mode
        USBDeviceInfo(0x483, 0x5740)
            .withManufacturer("Flipper Devices Inc.")
            .withProductDescription("Flipper Control Virtual ComPort")
    });
}

int DeviceRegistry::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.size();
}

QVariant DeviceRegistry::data(const QModelIndex &index, int role) const
{
    return (role == DeviceRole) ? QVariant::fromValue(m_data.at(index.row())) : QVariant();
}

QHash<int, QByteArray> DeviceRegistry::roleNames() const
{
    return { { DeviceRole, "device" } };
}

void DeviceRegistry::insertDevice(const USBDeviceInfo &info)
{
    auto *newDevice = new Flipper::FlipperZero(info, this);

    if(newDevice->isError()) {
        error_msg("A new device has been detected, but it has an error, skipping...");
        return;
    }

    const auto it = std::find_if(m_data.begin(), m_data.end(), [&](Flipper::FlipperZero *dev) {
        return newDevice->name() == dev->name();
    });

    if(it != m_data.end()) {
        // Preserving the old instance
        (*it)->setDeviceInfo(info);
        newDevice->deleteLater();

    } else {
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
        m_data.append(newDevice);
        endInsertRows();

        emit deviceConnected(newDevice);
    }
}

void DeviceRegistry::removeDevice(const USBDeviceInfo &info)
{
    const auto it = std::find_if(m_data.begin(), m_data.end(), [&](Flipper::FlipperZero *dev) {
        return dev->info().backendData() == info.backendData();
    });

    if(it != m_data.end()) {
        const auto idx = std::distance(m_data.begin(), it);
        auto *device = m_data.at(idx);

        if(!device->isPersistent()) {
            beginRemoveRows(QModelIndex(), idx, idx);
            m_data.takeAt(idx)->deleteLater();
            endRemoveRows();

        } else {
            device->setConnected(false);
        }
    }
}