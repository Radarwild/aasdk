
#include <thread>
#include <aasdk/USB/IUSBWrapper.hpp>
#include <aasdk/USB/USBHub.hpp>
#include <aasdk/USB/AccessoryModeQueryChain.hpp>
#include <aasdk/Error/Error.hpp>


namespace aasdk
{
namespace usb
{

USBHub::USBHub(IUSBWrapper& usbWrapper, asio::io_service& ioService, IAccessoryModeQueryChainFactory& queryChainFactory)
    : usbWrapper_(usbWrapper)
    , strand_(ioService)
    , queryChainFactory_(queryChainFactory)
{
}

void USBHub::start(Promise::Pointer promise)
{
    strand_.dispatch([this, self = this->shared_from_this(), promise = std::move(promise)]() {
        if(hotplugPromise_ != nullptr)
        {
            hotplugPromise_->reject(error::Error(error::ErrorCode::OPERATION_ABORTED));
            hotplugPromise_.reset();
        }

      hotplugPromise_ = promise;

        if(self_ == nullptr)
        {
            self_ = this->shared_from_this();
            /*hotplugHandle_ = usbWrapper_.hotplugRegisterCallback(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, (libusb_hotplug_flag)LIBUSB_HOTPLUG_NO_FLAGS, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
                                                                 LIBUSB_HOTPLUG_MATCH_ANY, reinterpret_cast<libusb_hotplug_callback_fn>(&USBHub::hotplugEventsHandler), reinterpret_cast<void*>(this)); */
			hotplugHandle_ = usbWrapper_.hotplugRegisterCallback(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_ENUMERATE, LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY,
                                                                 LIBUSB_HOTPLUG_MATCH_ANY, reinterpret_cast<libusb_hotplug_callback_fn>(&USBHub::hotplugEventsHandler), reinterpret_cast<void*>(this));													 
        }
    });
}

void USBHub::cancel()
{
    strand_.dispatch([this, self = this->shared_from_this()]() mutable {
        if(hotplugPromise_ != nullptr)
        {
            hotplugPromise_->reject(error::Error(error::ErrorCode::OPERATION_ABORTED));
            hotplugPromise_.reset();
        }

        std::for_each(queryChainQueue_.begin(), queryChainQueue_.end(), std::bind(&IAccessoryModeQueryChain::cancel, std::placeholders::_1));

        if(self_ != nullptr)
        {
            hotplugHandle_.reset();
            self_.reset();
        }
    });
}

int USBHub::hotplugEventsHandler(libusb_context* usbContext, libusb_device* device, libusb_hotplug_event event, void* userData)
{
    if(event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
    {
        auto self = reinterpret_cast<USBHub*>(userData)->shared_from_this();
        self->strand_.dispatch(std::bind(&USBHub::handleDevice, self, device));
    }
    
    return 0;
}

bool USBHub::isAOAPDevice(const libusb_device_descriptor& deviceDescriptor) const
{
    return deviceDescriptor.idVendor == cGoogleVendorId &&
            (deviceDescriptor.idProduct == cAOAPId || deviceDescriptor.idProduct == cAOAPWithAdbId);
}

void USBHub::handleDevice(libusb_device* device)
{
    if(hotplugPromise_ == nullptr)
    {
        return;
    }

    libusb_device_descriptor deviceDescriptor;
    if(usbWrapper_.getDeviceDescriptor(device, deviceDescriptor) != 0)
    {
        return;
    }

    DeviceHandle handle;
    auto openResult = usbWrapper_.open(device, handle);

    if(openResult != 0)
    {
        return;
    }

    if(this->isAOAPDevice(deviceDescriptor))
    {
        hotplugPromise_->resolve(std::move(handle));
        hotplugPromise_.reset();
    }
    else
    {
        ////////// Workaround for VMware
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //////////

        queryChainQueue_.emplace_back(queryChainFactory_.create());

        auto queueElementIter = std::prev(queryChainQueue_.end());
        auto queryChainPromise = IAccessoryModeQueryChain::Promise::defer(strand_);
        queryChainPromise->then([this, self = this->shared_from_this(), queueElementIter](DeviceHandle handle) mutable {
                queryChainQueue_.erase(queueElementIter);
            },
            [this, self = this->shared_from_this(), queueElementIter](const error::Error& e) mutable {
                queryChainQueue_.erase(queueElementIter);
            });

        queryChainQueue_.back()->start(std::move(handle), std::move(queryChainPromise));
    }
}

}
}
