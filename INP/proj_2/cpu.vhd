-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2013 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Zdenek Vasicek <vasicek AT fit.vutbr.cz>
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti
   
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

   signal PC      : std_logic_vector(11 downto 0) := (others => '0');
   signal PC_EN   : std_logic := '0';
   signal PTR     : std_logic_vector(9 downto 0) := (others => '0');
   signal PTR_EN  : std_logic := '0';
   signal PTR_DIR : std_logic := '0';
   signal CNT     : std_logic_vector(7 downto 0) := (others => '0');
   signal CNT_EN  : std_logic := '0';
   signal CNT_DIR : std_logic := '0';
   signal CNT_ONE : std_logic := '0';
   signal RAS     : std_logic_vector(191 downto 0) := (others => '0');
   signal RAS_EN  : std_logic := '0';
   signal RAS_DIR : std_logic := '0';
   signal RAS_TOP : std_logic := '0';
   signal sel     : std_logic_vector(1 downto 0) := (others => '0');

   type state is (IDLE, FETCH_SET, FETCH_GET, DECODE, INC_PC, PUTC_WAIT, PUTC, GETC, SKIP_SET, SKIP_GET, SKIP_CHECK, SKIP_DECODE, SKIP_INC_PC, WRITE_BACK, WRITE_BACK2, WHILE_BEGIN_CHECK, WHILE_END_CHECK, HALT);
   signal present_state, next_state: state;

   begin
   PC_cnt: process (CLK, RESET)
   begin
      if (RESET = '1') then
         PC <= (others => '0');
      elsif (CLK'event and CLK = '1') then
         if (RAS_EN = '1') then
            if (RAS_DIR = '1') then
               RAS <= RAS(179 downto 0) & PC(11 downto 0);
            else
               if (RAS_TOP = '1') then
                  PC <= RAS(11 downto 0);
               else 
                  RAS <= "000000000000" & RAS(191 downto 12);
               end if;
            end if;
         elsif (PC_EN = '1') then
		 PC <= PC + 1;
         end if;
      end if;
   end process;

   CODE_ADDR <= PC;

   PTR_cnt: process (CLK, RESET)
   begin
      if (RESET = '1') then
         PTR <= (others => '0');
      elsif (CLK'event and CLK = '1' and PTR_EN = '1') then
         if (PTR_DIR = '1') then
            PTR <= PTR + 1;
         else
            PTR <= PTR - 1;
         end if;
      end if;
   end process;

   DATA_ADDR <= PTR;

   CNT_cnt: process (CLK, RESET)
   begin
      if (RESET = '1') then
         CNT <= (others => '0');
      elsif (CLK'event and CLK = '1' and CNT_EN = '1') then
         if (CNT_ONE = '1') then
            CNT <= "00000001";
         elsif (CNT_DIR = '1') then
            CNT <= CNT + 1;
         else
            CNT <= CNT - 1;
         end if;
      end if;
   end process;  

   data_inc_dec: process (CLK, sel, DATA_RDATA, IN_DATA)
   begin
      if (sel = "01") then
         DATA_WDATA <= DATA_RDATA + 1;
      elsif (sel = "10") then
         DATA_WDATA <= DATA_RDATA - 1;
      elsif (sel = "11") then
         DATA_WDATA <= IN_DATA;
      end if;
   end process;
         

   OUT_DATA <= DATA_RDATA;

   present_state_logic: process(CLK, RESET, next_state)
   begin
      if (RESET = '1') then
         present_state <= IDLE;
      elsif (CLK'event and CLK = '1' and EN = '1') then
         present_state <= next_state;
      end if;
   end process;

 -- zde dopiste potrebne deklarace signalu
   next_state_logic: process(present_state, IN_VLD, OUT_BUSY, DATA_RDATA, CODE_DATA, CNT)
   begin
	   PC_EN <= '0';
	   PTR_EN <= '0';
	   PTR_DIR <= '0';
	   CNT_EN <= '0';
	   CNT_DIR <= '0';
	   CNT_ONE <= '0';
	   RAS_EN <= '0';
	   RAS_DIR <= '0';
	   RAS_TOP <= '0';
	   sel <= "00";
	   CODE_EN <= '0';
	   DATA_EN <= '0';
	   DATA_RDWR <= '1';
	   OUT_WE <= '0';
	   IN_REQ <= '0';

	   case present_state is
		when IDLE =>
		   next_state <= FETCH_SET;
		-----------------------------
		-- set instruction address --
		when FETCH_SET =>
			CODE_EN <= '1';
			next_state <= FETCH_GET;
		-----------------------------
		-- get instruction ----------
		when FETCH_GET =>
			CODE_EN <= '1';
			next_state <= DECODE;
		-----------------------------
		-- decode instruction -------
		when DECODE =>
			case CODE_DATA is
				------------------------
				-- increment pointer ---
				when X"3E" =>
					PTR_EN <= '1';
					PTR_DIR <= '1';
					next_state <= INC_PC;
				------------------------
				-- decrement pointer ---
				when X"3C" =>
					PTR_EN <= '1';
					next_state <= INC_PC;
				------------------------
				-- increment data ------
				when X"2B" =>
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					sel <= "01";
					next_state <= WRITE_BACK;
				------------------------
				-- decrement data ------
				when X"2D" =>
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					sel <= "10";
					next_state <= WRITE_BACK;
				------------------------
				-- while begin ---------
				when X"5B" =>
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					next_state <= WHILE_BEGIN_CHECK;
				------------------------
				-- while end -----------
				when X"5D" =>
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					next_state <= WHILE_END_CHECK;
				------------------------
				-- put char ------------
				when X"2E" =>
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					next_state <= PUTC_WAIT;
				------------------------
				-- get char ------------
				when X"2C" =>
					sel <= "11";
					IN_REQ <= '1';
					next_state <= GETC;
				------------------------
				-- return --------------
				when X"00" =>
					next_state <= HALT;
				------------------------
				when others =>
					next_state <= INC_PC;
			end case;
		------------------------------
		-- increment instr. pointer --
		when INC_PC =>
			PC_EN <= '1';
			DATA_EN <= '1';
			DATA_RDWR <= '1';
			next_state <= FETCH_SET;
		------------------------------
		-- load data -----------------
		when WRITE_BACK =>
			DATA_EN <= '1';
			DATA_RDWR <= '1';
			next_state <= WRITE_BACK2;
		------------------------------
		-- save modified data --------
		when WRITE_BACK2 =>
			DATA_EN <= '1';
			DATA_RDWR <= '0';
			next_state <= INC_PC;
		------------------------------
		-- check DATA_RDATA value ----
		when WHILE_BEGIN_CHECK =>
			if (DATA_RDATA = 0) then
				CNT_EN <= '1';
				CNT_ONE <= '1';
				next_state <= SKIP_INC_PC;
			else
				RAS_EN <= '1';
				RAS_DIR <= '1';
				next_state <= INC_PC;
			end if;
		-----------------------------
		-- check DATA_RDATA value ---
		when WHILE_END_CHECK =>
			RAS_EN <= '1';
			if (DATA_RDATA = 0) then
				next_state <= INC_PC;
			else
				RAS_TOP <= '1';
				next_state <= INC_PC;
			end if;
		-----------------------------
		-- wait on OUT_BUSY ---------
		when PUTC_WAIT =>
			if (OUT_BUSY = '1') then
				next_state <= PUTC_WAIT;
			else
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				next_state <= PUTC;
			end if;
		-----------------------------
		-- put char -----------------
		when PUTC =>
			OUT_WE <= '1';
			next_state <= INC_PC;
		-----------------------------
		-- get char -----------------
		when GETC =>
			if (IN_VLD = '1') then
				sel <= "11";
				next_state <= WRITE_BACK;
			else
				IN_REQ <= '1';
				next_state <= GETC;
			end if;
		-----------------------------
		-- INC_PC and skip instr. ---
		when SKIP_INC_PC =>
			PC_EN <= '1';
			next_state <= SKIP_SET;
		-----------------------------
		-- set code addr ------------
		when SKIP_SET =>
			CODE_EN <= '1';
			next_state <= SKIP_GET;
		-----------------------------
		-- get instruction ----------
		when SKIP_GET =>
			CODE_EN <= '1';
			next_state <= SKIP_DECODE;
		-----------------------------
		-- decode instruction -------
		when SKIP_DECODE =>
			case CODE_DATA is
				--------------------
				-- while begin -----
				when X"5B" =>
					CNT_EN <= '1';
					CNT_DIR <= '1';
					next_state <= SKIP_INC_PC;
				--------------------
				-- while end -------
				when X"5D" =>
					CNT_EN <= '1';
					next_state <= SKIP_CHECK;
				--------------------
				when others =>
					next_state <= SKIP_INC_PC;
			end case;
		-----------------------------
		-- check CNT value ----------
		when SKIP_CHECK =>
			if (CNT = 0) then
				next_state <= INC_PC;
			else
				next_state <= SKIP_INC_PC;
			end if;
		-----------------------------
		-- stop program -------------
		when HALT =>
			next_state <= HALT;
		-----------------------------
		when others =>
			next_state <= INC_PC;
	end case;
end process;
end architecture;





										
