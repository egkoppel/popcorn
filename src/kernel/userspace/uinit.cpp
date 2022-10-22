/*
 * Copyright (c) 2022 Eliyahu Gluschove-Koppel.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define HUGOS_USERSPACE

#include "uinit.hpp"
#include "initramfs.hpp"
#include "../interrupts/syscall.hpp"
#include <stdint.h>
#include <stdarg.h>
#include "userspace_macros.hpp"
#include "fsd.hpp"

#define hugOS_ascii_logo \
" \n\
 _                  ____   _____ \n\
| |                / __ \\ / ____| \n\
| |__  _   _  __ _| |  | | (___ \n\
| '_ \\| | | |/ _` | |  | |\\___ \\ \n\
| | | | |_| | (_| | |__| |____) | \n\
|_| |_|\\__,_|\\__, |\\____/|_____/ \n\
              __/ | \n\
             |___/ \n\
"

[[noreturn]] int uinit_main(uint64_t ramfs_data, uint64_t ramfs_size) {
	// [TARGET PRE-FSD]
	void *fsd_online_sem = sem_init(1);
	auto fsd_mbox = mbox_new();
	auto fsd_task_handle = sys_spawn_2("fsd_bootstrap", fsd_start, fsd_online_sem, fsd_mbox);
	mbox_transfer(fsd_mbox, fsd_task_handle);

	sem_wait(fsd_online_sem);
	// [TARGET FSD]

	// [TARGET PRE-INITRAMFS]
	fsd_command_t fsd_command;

	fsd_command_mount_t mount_ramfs_command = {
			.driver_command_len = 0,
			.mountpoint = 'A',
			.driver_info = "initramfs \0"
	};
	char ramfs_data_str[32];
	char ramfs_size_str[32];
	utoa(ramfs_data, ramfs_data_str, 10);
	utoa(ramfs_size, ramfs_size_str, 10);
	strcat(mount_ramfs_command.driver_info, ramfs_data_str);
	strcat(mount_ramfs_command.driver_info, " ");
	strcat(mount_ramfs_command.driver_info, ramfs_size_str);
	mount_ramfs_command.driver_command_len = strlen(mount_ramfs_command.driver_info);

	fsd_command = {.command = fsd_command_t::MOUNT};
	memcpy(fsd_command.data, &mount_ramfs_command, sizeof(mount_ramfs_command));

	send_msg_with_reply(fsd_mbox, UINT64_MAX, &fsd_command);
	auto mount_command_response = reinterpret_cast<fsd_command_response_t *>(&fsd_command);

	// [TARGET INITRAMFS]

	fsd_command_open_t open_ramfs_command = {
			.path_len = 15,
			.path = "a:/.placeholder"
	};
	fsd_command = {.command = fsd_command_t::OPEN};
	memcpy(fsd_command.data, &open_ramfs_command, sizeof(open_ramfs_command));
	send_msg_with_reply(fsd_mbox, UINT64_MAX, &fsd_command);
	auto open_command_response = reinterpret_cast<fsd_command_response_t *>(&fsd_command);

	// [TARGET PRE-LOGINSHELL]
	// [TARGET LOGINSHELL]

	while (true) __asm__ volatile("");
}

[[noreturn, gnu::naked]] int uinit() {
	__asm__ volatile("pop %rsi"); // pop argument off stack
	__asm__ volatile("pop %rdi"); // pop argument off stack
	__asm__ volatile("andq $-16, %rsp"); // align stack
	__asm__ volatile("jmp %P0" : : "i"(uinit_main));
}
