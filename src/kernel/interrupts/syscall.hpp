/*
 * Copyright (c) 2022 Eliyahu Gluschove-Koppel.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _HUGOS_SYSCALL_H
#define _HUGOS_SYSCALL_H

#include <stdint.h>
#include "handle_table.hpp"

extern "C" void syscall_long_mode_handler();

namespace syscall_vectors {
	enum syscall_vectors : uint64_t {
		get_current_task = 0x000, /* syscall_handle_t get_current_task() */
		yield = 0x001, /* int64_t yield() */
		exit = 0x002, /* void exit() */
		sleep = 0x003, /* int64_t sleep(uint64_t) */
		suspend = 0x004, /* int64_t suspend() */
		resume = 0x005, /* int64_t resume(syscall_handle_t) */
		spawn = 0x006, /* syscall_handle_t spawn(char*, void(*)(uint64_t, uint64_t, uint64_t), uint64_t, uint64_t, uint64_t) */
		spawn2 = 0x007, /* syscall_handle_t spawn(char*, char**, char**) */
		get_time_used = 0x008, /* int64_t get_time_used() */

		mailbox_new = 0x200,
		mailbox_send = 0x201,
		mailbox_recv = 0x202,
		mailbox_destroy = 0x203,

		region_new = 0x300,
		region_new_anon = 0x301,
		region_new_dma = 0x302,
		set_flags = 0x303,
		map_region = 0x304,
		share_region = 0x306,

		mutex_lock,
		mutex_try_lock,
		mutex_unlock,
		mutex_new,
		mutex_destroy,

		sem_post,
		sem_wait,
		sem_get_count,
		sem_new,
		sem_destroy
	};
}

namespace vm_flags {
	enum {
		PROT_READ = 1ull << 1,
		PROT_WRITE = 1ull << 2,
		PROT_EXEC = 1ull << 3,

		HINT_FAIL = 1ull << 4,
		COW = 1ull << 5,
		TRANSFER = 1ull << 6,
		NO_CACHE = 1ull << 7
	};
}

int64_t syscall_handler(uint64_t syscall_number, int64_t arg1, int64_t arg2, int64_t arg3, int64_t arg4, int64_t arg5);

#endif
