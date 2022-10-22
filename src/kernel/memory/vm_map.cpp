
/*
 * Copyright (c) 2022 Eliyahu Gluschove-Koppel.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "vm_map.hpp"
#include "paging.h"
#include "../main/main.hpp"
#include <utility>
#include <utils.h>

alignas(alignof(SyscallHandleTable<VmMapping, syscall_handle_type::syscall_handle_type::VM>)) static char vm_map_handles_list_[sizeof(SyscallHandleTable<VmMapping, syscall_handle_type::syscall_handle_type::VM>)]; // memory for the stream object
auto& vm_map_handles_list = reinterpret_cast<SyscallHandleTable<VmMapping, syscall_handle_type::syscall_handle_type::VM>&>(vm_map_handles_list_);

void vm_map_init() {
	new(&vm_map_handles_list) SyscallHandleTable<VmMapping, syscall_handle_type::syscall_handle_type::VM>();
}

syscall_handle_t new_vm_mapping_anon(uint64_t size) {
	auto backing_frames = std::vector<uint64_t>(IDIV_ROUND_UP(size, 0x1000));
	for (uint64_t i = 0; i < size; i += 0x1000) backing_frames.push_back(allocator_allocate(global_frame_allocator));

	auto handle = vm_map_handles_list.new_handle(VmMapping(std::move(backing_frames)));
	auto data = vm_map_handles_list.get_data_from_handle_ptr(handle);
	data->set_handle(handle);
	return handle;
}

syscall_handle_t new_vm_mapping(uint64_t phys_addr, uint64_t size) {
	if (phys_addr & (~0x1000)) return 0;

	auto backing_frames = std::vector<uint64_t>(IDIV_ROUND_UP(size, 0x1000));
	for (uint64_t i = phys_addr; i < size; i += 0x1000) {
		auto frame = allocator_allocate_at(global_frame_allocator, i);
		if (!frame) return 0;
		backing_frames.push_back(frame);
	}

	auto handle = vm_map_handles_list.new_handle(VmMapping(std::move(backing_frames)));
	auto data = vm_map_handles_list.get_data_from_handle_ptr(handle);
	data->set_handle(handle);
	return handle;
}
