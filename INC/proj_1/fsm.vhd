-- fsm.vhd: Finite State Machine
-- Author(s): 
--
library ieee;
use ieee.std_logic_1164.all;
-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity fsm is
port(
   CLK         : in  std_logic;
   RESET       : in  std_logic;

   -- Input signals
   KEY         : in  std_logic_vector(15 downto 0);
   CNT_OF      : in  std_logic;

   -- Output signals
   FSM_CNT_CE  : out std_logic;
   FSM_MX_MEM  : out std_logic;
   FSM_MX_LCD  : out std_logic;
   FSM_LCD_WR  : out std_logic;
   FSM_LCD_CLR : out std_logic
);
end entity fsm;

-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of fsm is
   type t_state is (IDLE, ST0, ST1, ST2, ST3, ST4, ST5, ST6, ST7_1, ST7_2, ST8_1, 
      ST8_2, ST9_1, ST9_2, ST10_1, ST10_2, ST11, SKIP, PRINT_OK, PRINT_WRONG);
   signal present_state, next_state : t_state;

begin
-- -------------------------------------------------------
sync_logic : process(RESET, CLK)
begin
   if (RESET = '1') then
      present_state <= ST0;
   elsif (CLK'event AND CLK = '1') then
      present_state <= next_state;
   end if;
end process sync_logic;

-- -------------------------------------------------------
next_state_logic : process(present_state, KEY, CNT_OF)
begin
   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when IDLE =>
      next_state <= IDLE;
      if (KEY(15) = '1') then
         next_state <= ST0;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST0 =>
      next_state <= ST0;
      if (KEY(1) = '1') then
         next_state <= ST1;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST1 =>
      next_state <= ST1;
      if (KEY(4) = '1') then
         next_state <= ST2;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST2 =>
      next_state <= ST2;
      if (KEY(6) = '1') then
         next_state <= ST3;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST3 =>
      next_state <= ST3;
      if (KEY(0) = '1') then
         next_state <= ST4;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST4 =>
      next_state <= ST4;
      if (KEY(6) = '1') then
         next_state <= ST5;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST5 =>
      next_state <= ST5;
      if (KEY(4) = '1') then
         next_state <= ST6;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST6 =>
      next_state <= ST6;
      if (KEY(7) = '1') then
         next_state <= ST7_1;
      elsif (KEY(8) = '1') then
        next_state <= ST7_2;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST7_1 =>
      next_state <= ST7_1;
      if (KEY(2) = '1') then
         next_state <= ST8_1;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST7_2 =>
      next_state <= ST7_2;
      if (KEY(9) = '1') then
         next_state <= ST8_2;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST8_1 =>
      next_state <= ST8_1;
      if (KEY(6) = '1') then
         next_state <= ST9_1;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST8_2 =>
      next_state <= ST8_2;
      if (KEY(4) = '1') then
         next_state <= ST9_2;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST9_1 =>
      next_state <= ST9_1;
      if (KEY(2) = '1') then
         next_state <= ST10_1;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST9_2 =>
      next_state <= ST9_2;
      if (KEY(6) = '1') then
         next_state <= ST10_2;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST10_1 =>
      next_state <= ST10_1;
      if (KEY(4) = '1') then
         next_state <= ST11;
      elsif (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST10_2 =>
      next_state <= ST10_2;
      if (KEY(15) = '1') then
        next_state <= PRINT_OK;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when ST11 =>
      next_state <= ST11;
      if (KEY(15) = '1') then
        next_state <= PRINT_OK;
      elsif (KEY(14 downto 0) /= "000000000000000") then
         next_state <= SKIP;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when SKIP =>
      next_state <= SKIP;
      if (KEY(15) = '1') then
        next_state <= PRINT_WRONG;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_OK =>
      next_state <= PRINT_OK;
      if (CNT_OF = '1') then
         next_state <= IDLE;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_WRONG =>
      next_state <= PRINT_WRONG;
      if (CNT_OF = '1') then
         next_state <= IDLE;
      end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      next_state <= IDLE;
   end case;
end process next_state_logic;

-- -------------------------------------------------------
output_logic : process(present_state, KEY)
begin
   FSM_CNT_CE     <= '0';
   FSM_MX_MEM     <= '0';
   FSM_MX_LCD     <= '0';
   FSM_LCD_WR     <= '0';
   FSM_LCD_CLR    <= '0';

   case (present_state) is
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_OK =>
      FSM_MX_MEM <= '1';
      FSM_CNT_CE <= '1';
      FSM_MX_LCD <= '1';
      FSM_LCD_WR <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when PRINT_WRONG =>
      FSM_CNT_CE <= '1';
      FSM_MX_LCD <= '1';
      FSM_LCD_WR <= '1';
   -- - - - - - - - - - - - - - - - - - - - - - -
   when IDLE =>
    if (KEY(15) = '1') then
        FSM_LCD_CLR <= '1';
    end if;
   -- - - - - - - - - - - - - - - - - - - - - - -
   when others =>
      if (KEY(14 downto 0) /= "000000000000000") then
         FSM_LCD_WR <= '1';
      end if;
      if (KEY(15) = '1') then
         FSM_LCD_CLR <= '1';
      end if;
   end case;
end process output_logic;

end architecture behavioral;

