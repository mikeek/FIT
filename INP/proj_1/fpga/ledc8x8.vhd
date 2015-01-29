library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port(
	SMCLK: in std_logic;
	RESET: in std_logic;
	ROW: out std_logic_vector(7 downto 0);
	LED: out std_logic_vector(7 downto 0)
);
end entity ledc8x8;

architecture main of ledc8x8 is
	signal ce: std_logic := '1';
	signal switch: std_logic := '0';
	signal ctrl_cnt: std_logic_vector(21 downto 0) := (others => '0');
	signal row_cnt: std_logic_vector(7 downto 0) := "10000000";
	signal led_state: std_logic_vector(7 downto 0) := "01110111";
begin
	
	ctrl_proc: process(SMCLK, RESET)
	begin
		if (RESET = '1') then
			ctrl_cnt <= (others => '0');
		elsif ((SMCLK'event) and (SMCLK = '1')) then
			ctrl_cnt <= ctrl_cnt + 1;
			if (ctrl_cnt(7 downto 0) = "11111111") then
				ce <= '1';
			else
				ce <= '0';
			end if;
		end if;
		switch <= ctrl_cnt(21);
	end process;

	row_proc: process(SMCLK, RESET)
	begin
		if (RESET = '1') then
			row_cnt <= "10000000";
		elsif ((SMCLK'event) and (SMCLK = '1') and (ce = '1')) then
			row_cnt <= row_cnt(0) & row_cnt(7 downto 1);			
		end if;
		ROW <= row_cnt;
	end process;

	dec_proc: process(SMCLK)
	begin
		case row_cnt is
			when "10000000" => led_state <= "01110111";
			when "01000000" => led_state <= "00100111";
			when "00100000" => led_state <= "01010111";
			when "00010000" => led_state <= "01110110";
			when "00001000" => led_state <= "01110101";
			when "00000100" => led_state <= "11110011";
			when "00000010" => led_state <= "11110101";
			when "00000001" => led_state <= "11110110";
			when others => led_state <= (others => '1');
		end case;
	end process;

	blink_proc: process(SMCLK)
	begin
		if (switch = '0') then
			LED <= led_state;
		else
			LED <= (others => '1');
		end if;
	end process;

end architecture main; 