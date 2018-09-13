#include <Windows.h>
#include <injector/injector.hpp>

DWORD mainModuleBase;

// TODO: REVERSE EACH API WITH DEBUGGING AND USE ORIGINAL I/O FOR PROPER RETURN VALUES OF REST OF THE STUFF

char __cdecl ac_io_begin_get_status(int a1) // JGT
{
	return 1;
}

bool ac_io_end() // JGT
{
	return true;
}

int ac_io_end_get_status() // JGT
{
	return 1;
}

char __cdecl ac_io_get_version(int a1, int a2) // JGT
{
	return 1;
}

int __cdecl ac_io_hbhi_control_lamp_bright(char a1, unsigned __int8 a2) // JGT
{
	return 1;
}

int __cdecl ac_io_hbhi_control_lamp_mode(int a1) // JGT
{
	return 1;
}

int __cdecl ac_io_hbhi_control_lamp_off(int a1) // JGT
{
	return 1;
}

int __cdecl ac_io_hbhi_control_lamp_on(char a1) // JGT
{
	return 1;
}

int __cdecl ac_io_hbhi_control_parallel_off(char a1)
{
	return 1;
}

int __cdecl ac_io_hbhi_control_parallel_on(char a1)
{
	return 1;
}

int ac_io_hbhi_control_reset()
{
	return 1;
}

char ac_io_hbhi_create_get_status_thread()
{
	return 1;
}

int ac_io_hbhi_destroy_get_status_thread()
{
	return 1;
}

char __cdecl ac_io_hbhi_get_coin_input_wave_buffer(int *a1)
{
	return 1;
}

bool ac_io_hbhi_get_watchdog_status() // JGT
{
	return true;
}

char __cdecl ac_io_hbhi_lock_coincounter(signed int a1) // JGT
{
	return 1;
}

signed int ac_io_hbhi_req_carddispenser_disburse()
{
	return 1;
}

bool ac_io_hbhi_req_carddispenser_get_status()
{
	return true;
}

signed int ac_io_hbhi_req_carddispenser_init()
{
	return 1;
}

char ac_io_hbhi_req_coin_input_wave()
{
	return 1;
}

char __cdecl ac_io_hbhi_req_get_control_status(DWORD *a1)
{
	return 1;
}

char __cdecl ac_io_hbhi_req_secplug_check(const char *a1)
{
	return 1;
}

bool __cdecl ac_io_hbhi_req_secplug_check_isfinished(signed int *a1) // JGT
{
	return 1;
}

char __cdecl ac_io_hbhi_req_secplug_check_softwareplug(const char *a1)
{
	return 1;
}

char ac_io_hbhi_req_secplug_check_systemplug()
{
	return 1;
}

char ac_io_hbhi_req_secplug_missing_check()
{
	return 1;
}

int __cdecl ac_io_hbhi_reset_coin_slot_noise_flag(int a1)
{
	return 1;
}

bool __cdecl ac_io_hbhi_req_secplug_missing_check_isfinished(signed int *a1)
{
	return true;
}

bool __cdecl ac_io_hbhi_req_volume_control(char a1, unsigned __int8 a2)
{
	return true;
}

bool __cdecl ac_io_hbhi_req_volume_control_isfinished(signed int *a1)
{
	return true;
}

int __cdecl ac_io_hbhi_set_framing_err_packet_send_interval(int a1)
{
	return 1;
}

bool __cdecl ac_io_hbhi_set_watchdog_time(__int16 a1)
{
	return true;
}

char __cdecl ac_io_hbhi_unlock_coincounter(signed int a1) // JGT
{
	return 1;
}

char ac_io_hbhi_update_control_status_buffer() // JGT
{
	return 1;
}

char ac_io_hgth_update_recvdata()
{
	return 1;
}

int __cdecl ac_io_set_soft_watch_dog(int a1)
{
	return 1;
}

int ac_io_soft_watch_dog_off() // JGT
{
	return 1;
}

int ac_io_soft_watch_dog_on() // JGT
{
	return 1;
}

int ac_io_update() // JGT
{
	return 1;
}

char __cdecl ac_io_hdxs_update_control_status_buffer(signed int a1)
{
	return 1;
}

char ac_io_icca_is_felica()
{
	return 1;
}

int __cdecl ac_io_secplug_set_encodedpasswd(int a1, int a2)
{
	return 1;
}

char ac_io_get_firmware_update_device_index()
{
	return 0xFF;
}

const char *ac_io_get_version_string()
{
	return "1.25.0";
}

__int16 ac_io_hbhi_get_watchdog_time_min()
{
	return 0;
}

__int16 ac_io_hbhi_get_watchdog_time_now()
{
	return 0;
}

int __cdecl ac_io_hdxs_led_scroll(signed int a1, char a2, char a3, char a4, char a5, char a6, char a7, char a8, char a9, char a10, char a11, char a12, char a13)
{
	return 0;
}

int __cdecl ac_io_hdxs_led_set_pattern(signed int a1, char a2, char a3, char a4, __int64 a5)
{
	return 0;
}

int __cdecl ac_io_hdxs_led_set_rgb_mask(signed int a1, char a2, char a3, __int64 a4)
{
	return 0;
}

int __cdecl ac_io_icca_send_keep_alive_packet(int a1, unsigned __int64 a2)
{
	return 0;
}

void ac_io_hbhi_watchdog_off()
{
	return;
}

void __cdecl ac_io_hgth_set_senddata(int a1)
{
	return;
}

char __cdecl ac_io_begin(char a1, int a2, DWORD *a3, int a4) // JGT
{
	return 1;
}

char __cdecl ac_io_get_node_no(int a1)
{
	// TODO
	// UNK RET
	return 1;
}

DWORD *__cdecl ac_io_get_rs232c_status(DWORD *a1) // JGT
{
	memset(a1, 0, 0x54);
	return a1;
}

DWORD coinDw = 0;

int __cdecl ac_io_hbhi_add_coin(int a1, int a2)
{
	coinDw++;
	return 1;
}

char __cdecl ac_io_hbhi_consume_coinstock(signed int a1, int a2) // JGT
{
	const DWORD val = coinDw;
	coinDw = 0;
	return val;
}

DWORD coinBlockerVal = 0;

int __cdecl ac_io_hbhi_control_coin_blocker_close(int a1) // JGT
{
	coinBlockerVal = 1;
	return 1;
}

int __cdecl ac_io_hbhi_control_coin_blocker_open(int a1) // JGT
{
	coinBlockerVal = 0;
	return 1;
}

char __cdecl ac_io_hbhi_current_coinstock(signed int a1, DWORD *a2) // JGT
{
	*a2 = coinDw;
	return 1;
}

int __cdecl ac_io_hbhi_get_control_status_buffer(int a1) // JGT
{
	// Apparently a1 is button bits ?
	a1 = 0;
	return 0;
}

int __cdecl ac_io_hbhi_get_softwareid(DWORD *a1) // JGT
{
	*a1 = 0x46464646;
	a1[1] = 0x46464646;
	a1[2] = 0x46464646;
	a1[3] = 0x46464646;
	return 1;
}

int __cdecl ac_io_hbhi_get_systemid(DWORD *a1) // JGT
{
	*a1 = 0x46464646;
	a1[1] = 0x46464646;
	a1[2] = 0x46464646;
	a1[3] = 0x46464646;
	return 1;
}

bool __cdecl ac_io_hbhi_req_carddispenser_disburse_isfinished(DWORD *a1)
{
	++*a1;
	return 1;
}

int __cdecl ac_io_hbhi_req_carddispenser_get_status_isfinished(DWORD *a1)
{
	++*a1;
	return 2;
}

bool __cdecl ac_io_hbhi_req_carddispenser_init_isfinished(DWORD *a1)
{
	++*a1;
	return 1;
}

int __cdecl ac_io_hdxs_get_control_status_buffer(signed int a1, DWORD *a2)
{
	// TODO
	// TODO UNK RET
	return 1;
}

int __cdecl ac_io_hdxs_set_framing_err_packet_send_interval(int a1)
{
	return a1;
}

char __cdecl ac_io_hgth_directreq_set_handle_limit(char a1, DWORD *a2)
{
	*a2 = 1;
	return 1;
}

bool __cdecl ac_io_hgth_directreq_set_handle_limit_isfinished(DWORD *a1)
{
	*a1 = 2;
	return 1;
}

void __cdecl ac_io_hgth_get_recvdata(void *a1)
{
	// TODO
}

char icCardBuf[256];

bool __cdecl ac_io_icca_cardunit_init(int a1) // JGT
{
	icCardBuf[36 * (a1 & 1)] = 1;
	if (a1 == 2)
		icCardBuf[8] = -1;
	return 1;
}

char __cdecl ac_io_icca_cardunit_init_isfinished(int a1, DWORD *a2) // JGT
{
	if (a1 == 2)
		icCardBuf[8] = -1;
	*a2 = 4;
	return 1;
}

char __cdecl ac_io_icca_device_control_iccard_power_supply_off(int a1)
{
	if (a1 == 2)
		icCardBuf[8] = -1;
	return 1;
}

char __cdecl ac_io_icca_device_control_iccard_power_supply_on(int a1)
{
	if (a1 == 2)
		icCardBuf[8] = -1;
	return 1;
}

bool __cdecl ac_io_icca_device_control_isfinished(int a1, DWORD *a2)
{
	if (a1 == 2)
		icCardBuf[8] = -1;
	return 1;
}

bool __cdecl ac_io_icca_get_keep_alive_error(int a1, DWORD *a2)
{
	if (a1 == 2)
		icCardBuf[8] = -1;
	*a2 = 0;
	return 0;
}

char __cdecl ac_io_icca_get_status(int a1, DWORD *a2) // JGT
{
	// ALSO FE ?
	*a2 = 0x0000000A;
	return 1;
}

char __cdecl ac_io_icca_get_uid_felica(int a1, int a2) // JGT
{
	// TODO
	// TODO UNK RET
	return 1;
}

char __cdecl ac_io_icca_req_uid(int a1) // JGT
{
	// TODO
	// TODO UNK RET
	return 1;
}

bool __cdecl ac_io_icca_req_uid_isfinished(int a1, DWORD *a2) // JGT
{
	// TODO
	// TODO UNK RET
	return true;
}

int __cdecl ac_io_icca_workflow(int a1, int a2) // JGT
{
	// This requires some smarts, not sure if required for JGT
	if(a1 == 0 && a1 == 1)
	{
		return 0;
	}
	// TODO
	// TODO UNK RET
	return 1;
}

DWORD ac_io_unk = 0;

char __cdecl ac_io_is_active(int a1)
{
	// TODO: UNK NOT SURE
	ac_io_unk++;
	return ac_io_unk != 259;
}

bool __cdecl ac_io_is_active2(int a1, int *a2) // JGT
{
	ac_io_unk = 0x259;
	*a2 = 6;
	return true;
}

char __cdecl ac_io_is_active_device(int a1) // JGT
{
	return a1 != 5;
}

int ac_io_reset(int a1)
{
	return a1;
}

int __cdecl ac_io_set_get_status_device(int a1)
{
	return a1;
}

//const char *ac_io_version()
//{
//	// TODO
//	// TODO UNK RET
//	return "1337";
//}

int init_libacioHooks()
{
	// Hook ton of APIs
	LoadLibrary("libacio.dll");
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_begin_get_status"), ac_io_begin_get_status);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_end"), ac_io_end);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_end_get_status"), ac_io_end_get_status);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_get_version"), ac_io_get_version);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_control_lamp_bright"), ac_io_hbhi_control_lamp_bright);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_control_lamp_mode"), ac_io_hbhi_control_lamp_mode);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_control_lamp_off"), ac_io_hbhi_control_lamp_off);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_control_lamp_on"), ac_io_hbhi_control_lamp_on);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_control_parallel_off"), ac_io_hbhi_control_parallel_off);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_control_parallel_on"), ac_io_hbhi_control_parallel_on);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_control_reset"), ac_io_hbhi_control_reset);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_create_get_status_thread"), ac_io_hbhi_create_get_status_thread);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_destroy_get_status_thread"), ac_io_hbhi_destroy_get_status_thread);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_get_coin_input_wave_buffer"), ac_io_hbhi_get_coin_input_wave_buffer);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_get_watchdog_status"), ac_io_hbhi_get_watchdog_status);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_lock_coincounter"), ac_io_hbhi_lock_coincounter);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_carddispenser_disburse"), ac_io_hbhi_req_carddispenser_disburse);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_carddispenser_get_status"), ac_io_hbhi_req_carddispenser_get_status);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_carddispenser_init"), ac_io_hbhi_req_carddispenser_init);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_coin_input_wave"), ac_io_hbhi_req_coin_input_wave);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_get_control_status"), ac_io_hbhi_req_get_control_status);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_secplug_check"), ac_io_hbhi_req_secplug_check);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_secplug_check_isfinished"), ac_io_hbhi_req_secplug_check_isfinished);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_secplug_check_softwareplug"), ac_io_hbhi_req_secplug_check_softwareplug);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_secplug_check_systemplug"), ac_io_hbhi_req_secplug_check_systemplug);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_secplug_missing_check"), ac_io_hbhi_req_secplug_missing_check);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_reset_coin_slot_noise_flag"), ac_io_hbhi_reset_coin_slot_noise_flag);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_secplug_missing_check_isfinished"), ac_io_hbhi_req_secplug_missing_check_isfinished);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_volume_control"), ac_io_hbhi_req_volume_control);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_volume_control_isfinished"), ac_io_hbhi_req_volume_control_isfinished);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_set_framing_err_packet_send_interval"), ac_io_hbhi_set_framing_err_packet_send_interval);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_set_watchdog_time"), ac_io_hbhi_set_watchdog_time);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_unlock_coincounter"), ac_io_hbhi_unlock_coincounter);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_update_control_status_buffer"), ac_io_hbhi_update_control_status_buffer);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hgth_update_recvdata"), ac_io_hgth_update_recvdata);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_set_soft_watch_dog"), ac_io_set_soft_watch_dog);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_soft_watch_dog_off"), ac_io_soft_watch_dog_off);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_soft_watch_dog_on"), ac_io_soft_watch_dog_on);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_update"), ac_io_update);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hdxs_update_control_status_buffer"), ac_io_hdxs_update_control_status_buffer);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_is_felica"), ac_io_icca_is_felica);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_secplug_set_encodedpasswd"), ac_io_secplug_set_encodedpasswd);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_get_firmware_update_device_index"), ac_io_get_firmware_update_device_index);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_get_version_string"), ac_io_get_version_string);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_get_watchdog_time_min"), ac_io_hbhi_get_watchdog_time_min);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_get_watchdog_time_now"), ac_io_hbhi_get_watchdog_time_now);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hdxs_led_scroll"), ac_io_hdxs_led_scroll);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hdxs_led_set_pattern"), ac_io_hdxs_led_set_pattern);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hdxs_led_set_rgb_mask"), ac_io_hdxs_led_set_rgb_mask);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_send_keep_alive_packet"), ac_io_icca_send_keep_alive_packet);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_watchdog_off"), ac_io_hbhi_watchdog_off);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hgth_set_senddata"), ac_io_hgth_set_senddata);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_begin"), ac_io_begin);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_get_node_no"), ac_io_get_node_no);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_get_rs232c_status"), ac_io_get_rs232c_status);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_add_coin"), ac_io_hbhi_add_coin);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_consume_coinstock"), ac_io_hbhi_consume_coinstock);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_control_coin_blocker_close"), ac_io_hbhi_control_coin_blocker_close);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_control_coin_blocker_open"), ac_io_hbhi_control_coin_blocker_open);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_current_coinstock"), ac_io_hbhi_current_coinstock);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_get_control_status_buffer"), ac_io_hbhi_get_control_status_buffer);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_get_softwareid"), ac_io_hbhi_get_softwareid);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_get_systemid"), ac_io_hbhi_get_systemid);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_carddispenser_disburse_isfinished"), ac_io_hbhi_req_carddispenser_disburse_isfinished);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_carddispenser_get_status_isfinished"), ac_io_hbhi_req_carddispenser_get_status_isfinished);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hbhi_req_carddispenser_init_isfinished"), ac_io_hbhi_req_carddispenser_init_isfinished);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hdxs_get_control_status_buffer"), ac_io_hdxs_get_control_status_buffer);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hdxs_set_framing_err_packet_send_interval"), ac_io_hdxs_set_framing_err_packet_send_interval);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hgth_directreq_set_handle_limit"), ac_io_hgth_directreq_set_handle_limit);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hgth_directreq_set_handle_limit_isfinished"), ac_io_hgth_directreq_set_handle_limit_isfinished);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_hgth_get_recvdata"), ac_io_hgth_get_recvdata);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_cardunit_init"), ac_io_icca_cardunit_init);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_cardunit_init_isfinished"), ac_io_icca_cardunit_init_isfinished);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_device_control_iccard_power_supply_off"), ac_io_icca_device_control_iccard_power_supply_off);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_device_control_iccard_power_supply_on"), ac_io_icca_device_control_iccard_power_supply_on);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_device_control_isfinished"), ac_io_icca_device_control_isfinished);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_get_keep_alive_error"), ac_io_icca_get_keep_alive_error);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_get_status"), ac_io_icca_get_status);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_get_uid_felica"), ac_io_icca_get_uid_felica);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_req_uid"), ac_io_icca_req_uid);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_req_uid_isfinished"), ac_io_icca_req_uid_isfinished);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_icca_workflow"), ac_io_icca_workflow);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_is_active"), ac_io_is_active);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_is_active2"), ac_io_is_active2);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_is_active_device"), ac_io_is_active_device);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_reset"), ac_io_reset);
	injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_set_get_status_device"), ac_io_set_get_status_device);
	//injector::MakeJMP(GetProcAddress(GetModuleHandleA("libacio.dll"), "ac_io_version"), ac_io_version);

	return 1;
}
