#include <fitkitlib.h>

void print_user_help()
{
	term_send_str_crlf(" Nastaveni hodin pres klavesnici na FitKitu");
	term_send_str_crlf(" Ovladani:");
	term_send_str_crlf("  #  - vstup do editace hodin, potvrzeni editace hodin nebo budiku");
	term_send_str_crlf("  *  - vstup do editace budiku");
	term_send_str_crlf("  A  - zapnuti/vypnuti budiku");
	term_send_str_crlf(" 0-9 - nastaveni hodnoty zvyraznene pozice hodin/budiku");
}

unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{
	return (CMD_UNKNOWN);
}

void fpga_initialized()
{
}

int main()
{
	initialize_hardware();
	print_user_help();
	while (1) {
		terminal_idle();
	}
}
