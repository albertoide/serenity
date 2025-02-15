/*
 * Copyright (c) 2021, the SerenityOS developers.
 * Copyright (c) 2021, Kyle Pereira <hey@xylepereira.me>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Badge.h>
#include <Kernel/Bus/VirtIO/Device.h>
#include <Kernel/Devices/CharacterDevice.h>
#include <Kernel/FileSystem/OpenFileDescription.h>
#include <Kernel/Memory/RingBuffer.h>

namespace Kernel::VirtIO {

class Console;

#define VIRTIO_CONSOLE_F_SIZE (1 << 0)
#define VIRTIO_CONSOLE_F_MULTIPORT (1 << 1)
#define VIRTIO_CONSOLE_F_EMERG_WRITE (1 << 2)

class ConsolePort
    : public CharacterDevice {
public:
    explicit ConsolePort(unsigned port, VirtIO::Console&);
    void handle_queue_update(Badge<VirtIO::Console>, u16 queue_index);

    void set_open(Badge<VirtIO::Console>, bool state) { m_open = state; }
    bool is_open() const { return m_open; }

    void init_receive_buffer(Badge<Console>);

private:
    constexpr static size_t RINGBUFFER_SIZE = 2 * PAGE_SIZE;

    virtual StringView class_name() const override { return "VirtIOConsolePort"; }

    virtual bool can_read(const OpenFileDescription&, size_t) const override;
    virtual KResultOr<size_t> read(OpenFileDescription&, u64, UserOrKernelBuffer&, size_t) override;
    virtual bool can_write(const OpenFileDescription&, size_t) const override;
    virtual KResultOr<size_t> write(OpenFileDescription&, u64, const UserOrKernelBuffer&, size_t) override;
    virtual KResultOr<NonnullRefPtr<OpenFileDescription>> open(int options) override;

    static unsigned next_device_id;
    u16 m_receive_queue {};
    u16 m_transmit_queue {};

    OwnPtr<Memory::RingBuffer> m_receive_buffer;
    OwnPtr<Memory::RingBuffer> m_transmit_buffer;

    VirtIO::Console& m_console;
    unsigned m_port;

    bool m_open { false };
    Atomic<bool> m_receive_buffer_exhausted;
};

}
